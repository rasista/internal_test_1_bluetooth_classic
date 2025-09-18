import os
import json
simpy_path = os.getenv("SIMFRMWORK_PATH")
build_path = os.getenv("BUILD_PATH","build")


import sys
sys.path.append(f"{simpy_path}/")
sys.path.append(f"{simpy_path}/../py_utils/")
from udp_framework.udp import udp
sys.path.append(f"./")
from sim_models import sim_models
class btc_usched(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.env = kwargs["env"]
        self.timeout = 0
        self.channel = udp('localhost', 5005, kwargs["env"])
        
    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index = index

    def start_pi_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        json_string = json.dumps(command)
        json_string = json_string.replace(" ","")
        return self.channel.sender(f"usched start_pi {json_string}")
    def stop_pi_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        json_string = json.dumps(command)
        json_string = json_string.replace(" ","")
        return self.channel.sender(f"usched stop_pi {json_string}")      
    def start_pi_scan_handler(self,args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        json_string = json.dumps(command)
        json_string = json_string.replace(" ","")
        return self.channel.sender(f"usched start_pi_scan {json_string}")            
    def stop_pi_scan_handler(self,args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        json_string = json.dumps(command)
        json_string = json_string.replace(" ","")
        return self.channel.sender(f"usched stop_pi_scan {json_string}")       
    def start_acl_handler(self,args):
        print("starting from usched")
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        json_string = json.dumps(command)
        json_string = json_string.replace(" ","")
        return self.channel.sender(f"usched start_acl {json_string}")
    def stop_acl_handler(self,args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        json_string = json.dumps(command)
        json_string = json_string.replace(" ","")
        return self.channel.sender(f"usched stop_acl {json_string}")
    def usched_init_handler(self,args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        json_string = json.dumps(command)
        json_string = json_string.replace(" ","")
        return self.channel.sender(f"usched usched_init {json_string}")