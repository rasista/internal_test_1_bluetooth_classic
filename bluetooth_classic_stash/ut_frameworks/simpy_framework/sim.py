import simpy
import os
import sys
import inspect
import argparse
import yaml
import glob
import time
import json
# Get the directory containing the current file
current_dir = os.path.dirname(os.path.abspath(__file__))

# Add the py_utils directory to the Python path
sys.path.append(os.path.join(current_dir, '../py_utils'))

from sockets.tcp import tcp_server

simulation_env_path = os.getenv("SIM_PATH", ".")
build_path = os.getenv("BUILD_PATH")
try:
    fw_path = ".."
except:
    pass
sys.path.append(f"models/")
sys.path.append(f"{fw_path}/models")
sys.path.append(f"{build_path}/models")
sys.path.append(f"{simulation_env_path}/")
from udp_framework.udp import *



errors = []
udp_config = {'ip': '', 'port': 0}          # Global dictionary

'''
    Generation of model details(i.e., Commands and respective python handlers, Number of Instances) from the list of models specified
    in the test case files.
    syntax to include models from test case file is :
    #import {Models:{"<model_name>":<instances>}}
A
    e.g: #import {'Models':{'rail':3, 'air_medium':1,'btc_dev':3, 'gdb_console':1}}

    Key 'rail' specifies the name of the model/python file
    Value 3 specifies number of instances of each model.

    Note: Either of command line argument for test case config file or 
          above mentioned method should used to include models into the 
          simulation framework but not both.
'''

def generate_model_details(env, model_name, model_detail):
    model_dict = {}
    if bool(isinstance(model_detail, dict)) and "aliases" in model_detail.keys():
        model_dict['aliases'] = model_detail["aliases"]
    else:
        model_dict['instances'] = model_detail
    model_dict['secondary_init_function'] = 'secondary_init_function'
    if model_name:
        module = __import__(model_name)
        class_module = [(name, class_obj) for name, class_obj in inspect.getmembers(module, inspect.isclass) if class_obj.__module__ == module.__name__]
        model_obj = class_module[0][1](env=None , model_name=None , model_params_dict=None, complete_name=None , cmds=None, instance_index=None)
        methods = inspect.getmembers(model_obj, predicate=inspect.ismethod)
        py_handlers = {}
        for each_method, valuue in methods:
            if each_method.split("_")[-1] == 'handler':
                command = "_".join(each_method.split("_")[0:len(each_method.split("_"))-1])
                py_handlers[command] = each_method
        #py_handlers_dict["python_handlers"] = py_handlers
        model_dict['python_handlers'] = py_handlers
        del model_obj
        return model_dict

class MultiplePrint:
    def __init__(self, *files):
        self.files = files

    def write(self, text):
        for f in self.files:
            f.write(text)

    def flush(self):
        for f in self.files:
            f.flush()

# Loads the yaml file and command line argument
def load_config(config_file, cmd_file, env):
    configs = {}
    if config_file is not None:
        try:
            with open(config_file) as yaml_file:
                configs = yaml.safe_load(yaml_file)
                configs['Models']['default_model'] = generate_model_details(env, "default_model", 1)
        except Exception as e:
            print(f"Unable to open {config_file} : {e}")
            exit()
    
    else:
        configs_final_dict = {}
        config_models_dict = dict()
        lines = load_commands(cmd_file)
        for line in lines:
            if line.startswith('#import'):
                del config_models_dict
                config_models_dict = {}
                config_models = "".join(line.split()[1:]).rstrip().lstrip()
                config_models_dict = eval(config_models)
                for models, instances in config_models_dict['Models'].items():
                    configs_final_dict[models] = generate_model_details(env, models, instances)
        
        configs_final_dict['default_model'] = generate_model_details(env, "default_model", 1)
        configs['Models'] = configs_final_dict
    return configs
    
# Loads the command line argument
def load_commands(cmd_file):
    try:
        with open(cmd_file) as file:
            return file.readlines()
    except:
        print("Unable to Open", cmd_file)
        assert False, f"Unable to Open, {cmd_file}"

# Creates output file based on the given file name
def create_output_file(output_file):
    if output_file:
        output_file = os.path.join(os.getcwd(), output_file)
        txtFile = open(output_file, 'w+')

        tee_print = MultiplePrint(sys.stdout, txtFile)
        sys.stdout = tee_print
    return output_file

# Sets up and runs the simulation environment based on configuration and command files
def start_simulation(config_file, cmd_file, sim_time, output_file):
    udp_ip = "localhost"
    udp_port = 5005
    env = simpy.Environment()
    output_file = create_output_file(output_file)           # Output file for simulation results

    cmd_lines = load_commands(cmd_file)                     # User commands from specified file
    if not cmd_lines:
        return 

    configs = load_config(config_file, cmd_file, env) 
    if len(configs["Models"]) == 1:
        print("WARNING : No models have been imported, Simulation is proceeding with the default model")
    
    if "udp_config" in configs.keys():
        udp_config.update(configs.get('udp_config', {}))        # Update UDP configuration based on simulation configuration
        udp_ip = udp_config['ip']
        udp_port = udp_config['port']
    init = udp(udp_ip, udp_port, env)
    
    cmds = [(line.split()[1], line.split()[2], line.split()[3:]) for line in cmd_lines if line.startswith('#invoke')]
    dict_models_vs_objects = {}
    for models in configs['Models']:
        aliases = ['$']        
        if "instances" in configs['Models'][models].keys():
            instances = int(configs['Models'][models]["instances"])
            if instances != 1:
                aliases = [i for i in range(1, instances+1)]
        if "aliases" in configs['Models'][models].keys():
            aliases = configs['Models'][models]["aliases"]
        index = 0
        for alias in aliases:
            index += 1
            if alias == '$':
                model_name = models
            else :
                model_name = f"{models}_{alias}"
            module = __import__(models, fromlist=[models])
            class_module = [(name, class_obj) for name, class_obj in inspect.getmembers(module, inspect.isclass) if class_obj.__module__ == module.__name__]
            dict_models_vs_objects[model_name] = class_module[0][1](env=env, model_name=models,
                                                                    model_params_dict=configs['Models'],
                                                                    complete_name=model_name,
                                                                    cmds=cmds,
                                                                    instance_index=index)
            
    for objects in dict_models_vs_objects.values():
        objects.global_info_share(dict_models_vs_objects)
    print("\n\n\n######################Test Summary#####################")
    for model, model_obj in dict_models_vs_objects.items():
        print(f"{model}: {model_obj}:{json.dumps(model_obj.dict_cmds_vs_py_handlers)}")
    print("######################Test Starts######################\n\n\n")
    env.run(until = sim_time)
    init.sender("system end")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = "Runs simulation environment")
    parser.add_argument('-cfg', type=str, help='Test Case Configuration File')
    parser.add_argument('-cmds', type=str, help='User Commands')
    parser.add_argument('-sim',required=True, type = int, help='Simulation time')
    parser.add_argument('-out',type=str, help='Output file name')
    args = parser.parse_args()

    start_simulation(args.cfg, args.cmds, args.sim, args.out)
    
def listen_for_command_channel(**kwargs):
    if "port" in kwargs:
        command_channel = tcp_server('localhost', kwargs.port)
    else:
        command_channel = tcp_server('localhost', 12345)
    command_channel.listen()
    data = command_channel.receive()
    data = yaml.safe_load(data)
    print(f"Received data: {data}")
    command_channel.tsf = data["tsf"]
    return command_channel