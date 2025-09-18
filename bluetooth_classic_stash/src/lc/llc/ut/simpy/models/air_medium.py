import os
simpy_path = os.getenv("SIM_PATH")
 
import sys
sys.path.append(f"{simpy_path}/")
sys.path.append(f"{simpy_path}/../py_utils/")
from udp_framework.udp import udp
 
from sim_models import sim_models
 
class air_medium(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.channel = udp('localhost', 5005, kwargs['env'])
        self.rx_start_tsf = 0
        self.rx_end_tsf = 0
        self.rx_flag = 0
        self.source_model = ""
        self.target_model = ""
        self.register_rx_dict = {}
        self.register_tx_dict = {}
        self.tx_devices = {}  
        self.rx_devices = {}
       
    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index
        return
   
    def tx_start_handler(self, args):
        self.tx_start_tsf = int(args[0])
        self.tx_end_tsf = int(args[1])
        self.tx_source_model = args[2]
        self.tx_target_model = args[3]
        self.tx_source_model_obj = self.dict_global_models_vs_object_lut[self.tx_source_model]
        self.tx_target_model_obj = self.dict_global_models_vs_object_lut[self.tx_target_model]
 
        # Register TX device with start and end times
        self.tx_devices[self.tx_source_model_obj] = {"tx_start_tsf": self.tx_start_tsf, "tx_end_tsf": self.tx_end_tsf, "tx_target_model": self.tx_target_model, "tx_source_model": self.tx_source_model}
       
        # Check if RX is active
        if(self.rx_flag):
            for device in self.rx_devices.keys():
                if(device == self.tx_target_model_obj):
                    self.register_timeout(timeout = self.tx_end_tsf, handler = "tx_rx_end_handler", args=[self.tx_source_model, self.tx_target_model])
                    self.deregister_timeout("rx_timeout_handler")
 
                    # Remove TX and RX devices from their respective dictionaries
                    self.tx_devices.pop(self.tx_source_model_obj)
                    self.rx_devices.pop(self.tx_target_model_obj)
                    self.rx_flag = 0
                    break
        else:
            self.register_timeout(timeout = self.tx_end_tsf, handler = "tx_end_handler", args=[self.tx_source_model])
        return "OK"
   
    def rx_start_handler(self, args):
        self.rx_flag = 1
        self.rx_start_tsf = int(args[0])
        self.rx_end_tsf = int(args[1])
        self.rx_source_model = args[2]
        self.rx_target_model = args[3]
        self.rx_source_model_obj = self.dict_global_models_vs_object_lut[self.rx_source_model]
        self.rx_target_model_obj = self.dict_global_models_vs_object_lut[self.rx_target_model]
 
        # Register RX device with start and end times
        self.rx_devices[self.rx_source_model_obj] = {"rx_start_tsf": self.rx_start_tsf, "rx_end_tsf": self.rx_end_tsf, "rx_target_model": self.rx_target_model, "rx_source_model": self.rx_source_model}
       
        # Register timeout for RX end handler
        self.register_timeout(timeout = self.rx_end_tsf, handler = "rx_timeout_handler", args=[self.rx_source_model])
        return "OK"
   
    def tx_rx_end_handler(self, args):
        self.source_model = args[0]
        self.dest_model = args[1]
        self.source_model_obj = self.dict_global_models_vs_object_lut[self.source_model]
        self.dest_model_obj = self.dict_global_models_vs_object_lut[self.dest_model]
 
        # Deregister TX and RX timeout handlers
        self.deregister_timeout("tx_start_handler")
        self.deregister_timeout("tx_end_handler")
        self.deregister_timeout("rx_timeout_handler")

        return f"{self.source_model} tx_done {self.source_model_obj.TX_end_tsf} \n {self.dest_model} rx_done {self.source_model_obj.TX_end_tsf}"
   
    def tx_end_handler(self, args):
        self.source_model = args[0]
        self.source_model_obj = self.dict_global_models_vs_object_lut[self.source_model]

        self.deregister_timeout("tx_start_handler")
        self.deregister_timeout("tx_end_handler")
        self.deregister_timeout("rx_timeout_handler")
        return f"{self.source_model} tx_done {self.source_model_obj.TX_end_tsf}"
   
    def rx_timeout_handler(self, args):
        self.deregister_timeout("tx_start_handler")
        self.deregister_timeout("tx_end_handler")
        self.deregister_timeout("rx_timeout_handler")
        self.rx_flag = 0
        self.source_model = args[0]
        self.source_model_obj = self.dict_global_models_vs_object_lut[self.source_model]

        return f"{self.source_model} rx_timeout {self.source_model_obj.device_index}"
   
    def rx_end_handler(self, args):
        self.deregister_timeout("tx_start_handler")
        self.deregister_timeout("tx_end_handler")
        self.deregister_timeout("rx_timeout_handler")
        self.rx_flag = 0
        self.source_model = args[0]
        self.source_model_obj = self.dict_global_models_vs_object_lut[self.source_model]
        if (self.source_model == "rx_header_done"):
            self.deregister_timeout("rx_timeout_handler")
            self.register_timeout(timeout = self.rx_devices[self.rx_source_model_obj]["rx_end_tsf"], handler="rx_timeout_handler", args=[self.rx_source_model])
            return f"{self.source_model} rx_header_received {self.source_model_obj.device_index}"    
        return f"{self.source_model} rx_done {self.source_model_obj.device_index}"
   
    #This function is called due to external trigger
    def rx_header_done_handler(self,args):
        self.source_model = args[0]
        self.source_model_obj = self.dict_global_models_vs_object_lut[self.source_model]
        self.deregister_timeout("rx_timeout_handler")
        self.register_timeout(timeout = self.rx_devices[self.rx_source_model_obj]["rx_end_tsf"], handler="rx_timeout_handler", args=[self.rx_source_model])
        return f"{ self.source_model } rx_header_received {self.source_model_obj.device_index}"
