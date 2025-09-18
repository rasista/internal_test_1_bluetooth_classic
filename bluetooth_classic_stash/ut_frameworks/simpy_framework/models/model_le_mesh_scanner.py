from random import randint
import re
class baseclass:
    def __init__(self, parent_object,env):
        self.env = env
        self.timeout = 0
        self.parent_object = parent_object

    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index
        self.scan_window = int(dict_model_params['init_params']['scan_window'])
        self.scan_interval = int(dict_model_params['init_params']['scan_interval'])
        self.scan_variation = int(dict_model_params['init_params']['variance'])
        self.dev_range = int(dict_model_params['init_params']['listen_range'])
        self.scan_synchronised = int(dict_model_params['init_params']['synchronised'])
        if self.scan_synchronised != 1:
            self.timeout = self.env.now + randint(0, self.scan_interval)
        else:
            self.timeout = self.env.now + randint(0, 500)

    def timeout_handler(self):
        return self.schedule_scan()
    
    def rx_done_handler(self, args):
        transmit_device_id = re.split("model_le_mesh_advertiser_", args[0])
        transmit_device_id = int(transmit_device_id[1])
        if (transmit_device_id < self.device_index) and (transmit_device_id >= self.device_index - self.dev_range):
            return f"model_le_mesh_advertiser_{self.device_index} adv_req {args[1]}"
        return ""

    def schedule_scan(self):
        variation = randint(0,self.scan_variation)
        self.timeout = self.env.now + self.scan_interval + variation
        return_cmd = f"model_medium schedule_listen {self.parent_object.model_name} {self.env.now} {self.env.now + 1 + self.scan_window - variation} {self.dev_range}\n"
        if self.scan_synchronised == 1:
            return_cmd = return_cmd + f"model_le_mesh_advertiser_{self.device_index} listen_window {self.env.now} {self.env.now + 1 + self.scan_window - variation} {self.timeout}"
        return return_cmd

