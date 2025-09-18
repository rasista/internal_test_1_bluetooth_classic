from sim_models import sim_models
import subprocess
import os
import sys
simulation_env_path = os.getenv("SIM_PATH", ".")
firmware_path = os.getenv("BUILD_PATH")
sys.path.append(f"{firmware_path}/models/")
sys.path.append(f"{simulation_env_path}/models")
sys.path.append(f"{simulation_env_path}/")
sys.path.append(f"{simulation_env_path}/../py_utils")
from udp_framework.udp import udp


class default_model(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.env = kwargs['env']
        self.cmds = kwargs['cmds']
        if self.env:
            self.channel = udp('localhost', 5005, kwargs['env'])
            self.env.process(self.simulation_parent_process(self.env))
        
    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index

    def py_cmd_handler(self, *args):
        return args[0]
        pass

    def fw_cmd_handler(self, *args):
        cmd = " ".join(args[0])
        return self.channel.sender(cmd)
    
    '''
    #invoke <timestamp/timedelta> <command> <args>
    timestamp  - time could be an absolute value
    timedelta - delta time with respect to previous timestamp 
        if previous timestamp is x(absolute value) and we specify timedelta to be +2 then 
        the command will be invoked at x+2
    '''

    def simulation_parent_process(self, env):
        timeout_val = 0
        for cmd in self.cmds:
            if(cmd[0].startswith("+")):
                timeout_val += eval(cmd[0])
            else:
                timeout_val = int(cmd[0])
            yield self.env.timeout(timeout_val - env.now)
            try:
                target_model = self.dict_global_models_vs_object_lut[cmd[1]]
                #convert rest of the list to string
                cmd_string = " ".join(cmd[2])
                target_model.command_queue.put(cmd_string)
                target_model.user_trigger.succeed("Event Triggered")
            except Exception as e:
                print(f"ERROR : Simulation Ended abruptly as model {cmd[1]} is not imported into framework, check configuration file or test case file")
                print(f"ERROR : Models currently imported into simulation : ")
                models = [model for model, value in self.dict_global_models_vs_object_lut.items()]
                print(f"ERROR : {models}")
                self.channel.sender('end')
                sys.exit(-1)
                