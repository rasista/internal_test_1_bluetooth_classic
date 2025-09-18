import os
import logging

simpy_path = os.getenv("SIM_PATH")

import sys
sys.path.append(f"{simpy_path}/")
sys.path.append(f"{simpy_path}/../py_utils/")
from udp_framework.udp import udp

from datetime import datetime
from random import randint
from sim_models import sim_models
from llc_model import *
    
class btc_llc_interface(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.channel = udp('localhost', 5005, kwargs['env'])
        self.end_tsf = 0
        self.start_role = -1
        self.stop_role = -1
        self.max_runtime = 0
        self.start_tsf = 0
        self.previous_delay_start = 2
        self.previous_delay_stop = 2

    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index

    def start_procedure_handler(self, args):
        print(f"{self.env.now} Inside start_procedure_handler in btc_llc_interface.py {args}")
        self.end_tsf = int(args[0])
        curr_time = self.env.now
        self.start_role = int(args[1])
        if self.start_role == 0 or self.start_role == 1:
            self.max_runtime = curr_time + int(args[2])
        elif self.start_role == 2 or self.start_role == 3:
            self.max_runtime = int(args[2])
        self.scheduled_activities["end_tsf"] = str(self.end_tsf)
        
        # TODO: See if random time works, in random time if host sends stop before start + delay, then stop will be executed first (add delay for start in stop too?????)
        # TODO: If random is required, figure out how to make sure that stop is not executed before start
        self.previous_delay_start = 3 - self.previous_delay_start
        delay = self.previous_delay_start

        self.delayed_message = f"llc_model start_llc_procedure {self.end_tsf} {self.start_role} {self.max_runtime} {delay}"
        self.register_timeout(timeout = delay + curr_time, handler="send_delayed_message_handler", args=None)

        return "OK"

    def send_delayed_message_handler(self, args):
        # print(f"{self.delayed_message} at {self.env.now}")
        return self.delayed_message

    def stop_procedure_handler(self,args):
        print(f"{self.env.now} Inside stop_procedure_handler in btc_llc_interface.py {args}")
        curr_time = self.env.now
        self.stop_role = int(args[1])
        if(self.stop_role != self.start_role):
            print("Invalid role")
            return
        self.previous_delay_stop = 3 - self.previous_delay_stop
        delay = self.previous_delay_stop

        self.delayed_message = f"llc_model stop_llc_procedure {self.stop_role} {delay}"
        self.deregister_timeout("send_delayed_message_handler")
        self.register_timeout(timeout = delay + curr_time, handler="send_delayed_message_handler", args=None)
        return "OK"
    
    def procedure_stopped_handler(self, args):
        print(f"{self.env.now} Inside procedure_stopped_handler in btc_llc_interface.py")
        self.role = int(args[0])
        # return_v = self.channel.sender(f"usched lpw_stop_callback {self.role}")
        return_v = self.channel.sender(f"usched lpw_stop_acl_callback {self.role}")
        self.deregister_timeout("send_delayed_message_handler")
        return return_v