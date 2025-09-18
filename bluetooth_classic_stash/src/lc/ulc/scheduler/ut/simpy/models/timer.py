import os
simpy_path = os.getenv("SIM_PATH")

import sys
sys.path.append(f"{simpy_path}/")
sys.path.append(f"{simpy_path}/../py_utils/")
from udp_framework.udp import udp

from datetime import datetime
from random import randint
from sim_models import sim_models
    

class timer(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.channel = udp('localhost', 5005, kwargs['env'])
        self.start_tsf = 0
        self.role = 0

    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index
    
    # define a start timer handler that takes max_runtime and curr_time and uses the both to calculate the time till the timer expires
    def start_timer_handler(self, args):
        self.start_tsf = int(args[0])
        self.role = int(args[1])
        self.register_timeout(timeout=self.start_tsf, handler="stop_timer_handler", args=None)
        return "OK"
    
    # define a timer complete handler that sends a message to the usched
    def stop_timer_handler(self, args):
        self.deregister_timeout("stop_timer_handler")
        return_v = self.channel.sender(f"usched timer_stop_callback {self.role}")
        return return_v
        # return "OK"

    def timer_expired_handler(self, args):
        self.deregister_timeout("stop_timer_handler")
        return_v = self.channel.sender(f"usched timer_expire_callback {self.role}")

        return return_v