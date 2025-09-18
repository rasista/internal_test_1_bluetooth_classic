import os
simpy_path = os.getenv("SIM_PATH")
 
import sys
sys.path.append(f"{simpy_path}/")
sys.path.append(f"{simpy_path}/../py_utils/")
from udp_framework.udp import udp
 
from datetime import datetime
from random import randint
from sim_models import sim_models
   
class rail(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.channel = udp('localhost', 5005,kwargs['env'])
        self.reason_of_timeout = 0
        self.max_delay = 0
        self.TX_start_tsf = 0
        self.TX_end_tsf = 0
        self.RX_start_tsf = 0
        self.RX_end_tsf = 0
        self.scheduled_activities = {}
       
    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index
   
    def schedule_rx_handler(self, args):

        self.RX_source_model = self.model_name
        self.RX_target_model = "rail_" + args[3]
        self.RX_start_tsf = int(args[1])
        self.RX_end_tsf = int(args[2])

        if "rx_start" in self.scheduled_activities or "tx_start" in self.scheduled_activities:
            try:
                del self.scheduled_activities["rx_start"]
                del self.scheduled_activities["rx_end"]
                del self.scheduled_activities["tx_start"]
                del self.scheduled_activities["tx_end"]
            except:
                pass
            self.deregister_timeout("tx_start_trigger_handler")
            self.deregister_timeout("rx_start_trigger_handler")
        self.scheduled_activities["rx_start"] = str(self.RX_start_tsf)
        self.scheduled_activities["rx_end"] = str(self.RX_end_tsf)
        self.register_timeout(timeout=self.RX_start_tsf - 1, handler="rx_start_trigger_handler", args=None)
        return "OK"
 
 
    def schedule_tx_handler(self, args):
        self.TX_source_model = self.model_name
        self.TX_target_model = "rail_" + args[3]
        self.TX_start_tsf = int(args[1])
        self.TX_end_tsf = int(args[2])
        if "rx_start" in self.scheduled_activities or "tx_start" in self.scheduled_activities:
            try:
                del self.scheduled_activities["tx_start"]
                del self.scheduled_activities["tx_end"]
                del self.scheduled_activities["rx_start"]
                del self.scheduled_activities["rx_end"]
               
            except:
                pass
            self.deregister_timeout("tx_start_trigger_handler")
            self.deregister_timeout("rx_start_trigger_handler")
        self.scheduled_activities["tx_start"] = str(self.TX_start_tsf)
        self.scheduled_activities["tx_end"] = str(self.TX_end_tsf)
       
        self.register_timeout(timeout=self.TX_start_tsf, handler="tx_start_trigger_handler", args=None)
        return "OK"
 
 
    def tx_start_trigger_handler(self, *args):
        return f"air_medium tx_start {self.TX_start_tsf} {self.TX_end_tsf} {self.model_name} {self.TX_target_model}"
     
 
    def rx_start_trigger_handler(self, *args):
        return f"air_medium rx_start {self.RX_start_tsf} {self.RX_end_tsf} {self.model_name} {self.RX_target_model}"
   
    def tx_done_handler(self, *args):
        if (self.env.now < self.TX_end_tsf):
            tx_done_resp = self.channel.sender(f"lpw tx_abort {self.device_index}")
        else:
            tx_done_resp = self.channel.sender(f"lpw tx_end {self.device_index}")
        return tx_done_resp
   
    def rx_done_handler(self, *args):
        if(self.env.now < self.RX_start_tsf):
            rx_done_resp = self.channel.sender(f"lpw rx_timeout {self.device_index}")
        else:
            rx_done_resp = self.channel.sender(f"lpw rx_done {self.device_index}")    
        return rx_done_resp
   
    def rx_timeout_handler(self, *args):
        if(self.env.now < self.RX_start_tsf):
            rx_timeout_resp = self.channel.sender(f"lpw rx_timeout {self.device_index}") ### FOR RX  ABORT
        else:
            rx_timeout_resp = self.channel.sender(f"lpw rx_timeout {self.device_index}")  
        return rx_timeout_resp
   
    def rx_abort_handler(self,args):
        return self.channel.sender(f"lpw rx_abort {self.device_index}")
   
    def tx_abort_handler(self,args):
        if (self.env.now < self.TX_end_tsf):
            return self.channel.sender(f"lpw tx_abort {self.device_index}")
   
    def set_var_handler(self,args):
        args_str = ' '.join(args[0:])
        return_str = self.channel.sender(f"lpw var_status {self.device_index} {args_str}")
        return return_str
       
    def rx_header_received_handler(self,args):
        return self.channel.sender(f"lpw rx_header_received {self.device_index}")
