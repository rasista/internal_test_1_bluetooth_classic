import sys
import os
import traceback
import yaml
import pytest

simpy_path = os.getenv("SIM_PATH")
firmware_path = os.getenv("BUILD_PATH")
sys.path.append(f"{firmware_path}/models/")
sys.path.append(f"{simpy_path}/models")
sys.path.append(f"{simpy_path}/")

from simpy import *
import inspect
from sim import udp_config
from socket import timeout
import queue
# Get the directory containing the current file
current_dir = os.path.dirname(os.path.abspath(__file__))

# Add the py_utils directory to the Python path
sys.path.append(os.path.join(current_dir, '../py_utils'))
from udp_framework.udp import udp
import copy
import json

class sim_models():
    def simpy_external_init(self):
        self.generate_send_command_function()

    '''Function to parse all methods present in the class. if a method is of the form <command>_handler, it is added to the dictionary. 
        The dictionary maps <command> to <command>_handler'''
    def parse_methods(self):
        for method in dir(self):
            if method.endswith("_handler"):
                #strip the _handler from the method name and add it to the dictionary
                self.py_handlers[method[:-8]] = method
    
    def __init__(self, **kwargs):
        if "env" not in kwargs.keys():
            self.model_name = str(kwargs['model_name'])
            self.py_handlers = {}
            self.parse_methods() 
            self.generate_send_command_function()
        else:
            self.env = kwargs['env']
            model_name = str(kwargs['model_name'])
            model_params_dict = kwargs['model_params_dict']
            self.model_name = kwargs['complete_name']
            index = kwargs["instance_index"]
            self.channel = udp('localhost', 5005, self.env)
            try:
                cmds = kwargs['cmds']
            except KeyError as e:
                pass
            self.timeout = 0
            try:
                if "secondary_init_function" in model_params_dict[model_name].keys():
                    getattr(self, model_params_dict[model_name]["secondary_init_function"])(index,model_params_dict[model_name])
                #self.channel = udp('localhost', 5005, env)
            except AttributeError as e:
                pass
            except TypeError as e:
                pass
            self.timers = dict()
            self.dict_cmds_vs_py_handlers = {}
            self.dict_cmd_vs_c_handlers = {}
            try:
                self.dict_cmds_vs_py_handlers = model_params_dict[model_name]['python_handlers']
            except:
                pass
            try:
                self.dict_cmd_vs_c_handlers = model_params_dict[model_name]['c_handlers']

            except:
                pass
            self.trigger_list = []
            self.dict_global_models_vs_object_lut = {}
            if self.env:
                self.user_trigger = self.env.event()
            self.scheduled_activities = {}
            #Implementing command queue for each model to hold incoming command strings
            self.command_queue = queue.Queue()
            #implementing a list of target models for which triggers are pending
            self.event_trigger_pending = []

    def init_trigger_events(self, model_params_dict):
        try:
            self.dict_model_vs_events = {i:None for i in model_params_dict.keys()}
        except:
            self.dict_model_vs_events = {}
            
        
        for model in self.dict_model_vs_events.keys():
            self.dict_model_vs_events[model] = self.env.event()
        self.user_trigger = self.env.event()
        
    def register_timeout(self, **kwargs):
        timeout = kwargs['timeout']
        timeout_handler = kwargs['handler']
        try:
            timeout_args = kwargs['args']
        except KeyError as e:
            timeout_args = None
            
        if timeout not in self.timers.keys():
            self.timers[timeout] = dict()
        self.timers[timeout].update({timeout_handler:timeout_args})
    
    def deregister_timeout(self, timeout_handler):
        timers_temp = copy.deepcopy(self.timers)
        try:
            for timeout, timeout_handler_dict in timers_temp.items():
                if timeout_handler in timers_temp[timeout]:
                    self.timers[timeout].pop(timeout_handler)
                if len(self.timers[timeout]) == 0 :
                    self.timers.pop(timeout)
        
        except Exception as e:
            self.sim_print(f"Deregistering timeout thrown error : {e}")
            pass            
    
    def is_timeout_registered(self, timeout, timeout_handler):
        return timeout_handler in self.timers[timeout]

    def model_process(self):
        command_response = ''
        self.init_trigger_events(self.dict_global_models_vs_object_lut)
        while True:
            self.trigger_target_models()
            del command_response
            command_response = ''
            self.trigger_list = []
            self.trigger_list = [events for events in self.dict_model_vs_events.values()]
            self.trigger_list.append(self.user_trigger)
            try:
                if self.timeout != 0:
                    if self.timers.keys():
                        assert(f"cannot use multiple timer feature with timeout support")
                    self.trigger_list.append(self.env.timeout(self.timeout - self.env.now , value=['timeout']))
                else:
                    if self.timers.keys():
                        timeout_tsf = min(self.timers.keys())
                        self.trigger_list.append(self.env.timeout(timeout_tsf - self.env.now , value=['timeout']))
            except Exception as e:
                self.sim_print(f"Unable to run timeout {e} {self.timers}")
                pass

            triggered_result = yield AnyOf(self.env, self.trigger_list)
            for result in triggered_result:
                if "timeout" == result.value[0]:
                    self.command_queue.put("timeout")
                for remote_mode, event in self.dict_model_vs_events.items():
                    if event == result:
                        self.dict_model_vs_events[remote_mode] = self.env.event()
                if self.user_trigger == result:
                    self.user_trigger = self.env.event()
            #get all commands from command queue
            while not self.command_queue.empty():
                command = self.command_queue.get()
                if command == "timeout":
                    if self.timeout != 0:
                        command_response = self.timeout_handler()
                    else:
                        temp_timeout_dict = copy.deepcopy(self.timers[self.env.now]) #Cannot delete dictionary items in runtime.
                        for timeout_handler, handler_args in temp_timeout_dict.items():
                            command_response =  getattr(self, timeout_handler)(handler_args) #Multiple responses cannot be handled.
                            self.deregister_timeout(timeout_handler)
                            #self.timers[self.env.now].pop(timeout_handler)
                        if(self.env.now in self.timers and len(self.timers[self.env.now]) == 0):
                            self.timers.pop(self.env.now)
                else:
                    #translate command string to command and args
                    command = command.split()
                    cmd = command[0]
                    cmd_args = command[1:]
                    


                    try:
                        if(cmd in self.dict_cmds_vs_py_handlers.keys()):
                            command_response = eval("self." + self.dict_cmds_vs_py_handlers[cmd])(cmd_args)                            
                        elif(cmd in self.dict_cmd_vs_c_handlers.keys()):
                            command_response = self.channel.sender(result)
                    except AssertionError as e:
                        print(f"Test case Failed {e}")
                        sys.exit(-1)
                    except Exception as e:
                        print(traceback.format_exc())
                        print(f"Command {cmd} not found in simulation or firmware handlers. Please check configuation file , {e}")
                        print("Available commands:")
                        print(json.dumps(self.dict_cmds_vs_py_handlers))
                        self.channel.sender("system end")
                        sys.exit(-1)

                for cmd in command_response.split("\n"):
                    if cmd != "" and cmd != "OK" and cmd != None:
                        self.process_invoke_command(cmd)

    def global_info_share(self, models_vs_objects_lut):
        self.dict_global_models_vs_object_lut = models_vs_objects_lut
        self.init_trigger_events(self.dict_global_models_vs_object_lut)
        self.process = self.env.process(self.model_process())
    
    def sim_print(self, *args ,**kwargs):
        print(f"{self.env.now} {self.model_name} ",end="")
        print(*args, **kwargs)
                
    def process_invoke_command(self, command):
        target_model_name = command.split()[0]
        #generate command string by remove target model name from command
        command_string = " ".join(command.split()[1:])
        #find target object of model from self.dict_global_class_vs_obj
        target_model = self.dict_global_models_vs_object_lut[target_model_name]
        #Add while string as single entry to command queue of target model
        target_model.command_queue.put(command_string)

        if target_model_name not in self.event_trigger_pending:
            self.event_trigger_pending.append(target_model_name)
        
    def trigger_target_models(self):
        for model_name in self.event_trigger_pending:
            self.dict_global_models_vs_object_lut[model_name].dict_model_vs_events[self.model_name].succeed("Event Triggered")
        self.event_trigger_pending = []

    def expect_handler(self, args):
        dict_name = args[4][:len(args[4])-1].split('.')
        dict_name_temp = 'self.'+dict_name[0]
        for i in range(1,len(dict_name)):
            dict_name_temp=dict_name_temp+'['+dict_name[i]+']'
        dict_name_eval = eval(dict_name_temp)

        expected_key = args[0][1:]
        expected_value = args[2]
        if expected_key not in dict_name_eval.keys():
            self.channel.sender("system end")
            assert expected_key in dict_name_eval.keys(), f"{self.env.now} Test case failed {expected_key} not present in {dict_name_eval}" 
            sys.exit(1) 
        elif dict_name_eval[expected_key] != expected_value:
            self.channel.sender("system end")
            assert dict_name_eval[expected_key] == expected_value, f"{self.env.now} Test case failed {expected_key} value {expected_value} is not matching with value in {dict_name_eval}"
            sys.exit(1)
        return "OK"
    
    def test_failed_handler(self, args):
        self.sim_print("Test-case failed, Exiting !!")
        self.sim_print(f"Failing test-case : {args}")
        # subprocess.call(["killall", "-I", "bt_lpw.elf"])
        self.channel.sender("system end")
        sys.exit(1)
    
    def do_not_expect_handler(self, args):
        dict_name = args[4][:len(args[4]) - 1].split('.')
        dict_name_temp = 'self.' + dict_name[0]
        for i in range(1, len(dict_name)):
            dict_name_temp = dict_name_temp + '[' + dict_name[i] + ']'
        dict_name_eval = eval(dict_name_temp)
        if args[0][1:] not in dict_name_eval.keys():
            return 'OK'
        elif args[0][1:] in dict_name_eval.keys() and dict_name_eval[args[0][1:]] != args[2]:
            return 'OK'
        self.test_failed_handler(args)
        return "OK"

    '''Function to generate a function for each command in self.dict_cmds_vs_py_handlers.
    Generated function takes a tcp channel, args and optional parameter time as argument and sends the command to the tcp channel.
    Sent data format is <time> <self.model_name> <command> <args>'''    
    def generate_send_command_function(self):
        for cmd in self.py_handlers.keys():
            def cmd_func(channel=None, args="", time=0, cmd=cmd):  # Add cmd=cmd to the parameters
                print(channel)
                if time == 0:
                    time = channel.tsf + 3
                send_data = f"{time} {self.model_name} {cmd} {args}"
                try:
                    channel.send(send_data)
                    receive_data = channel.receive()
                    receive_data = yaml.safe_load(receive_data)
                except Exception as e:
                    print(f"An error occurred: {e}")
                    sys.exit(-1)
                response = receive_data['data']
                channel.tsf = receive_data['tsf']
                return response
            setattr(self, cmd, cmd_func)