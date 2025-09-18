import os
simulation_env_path = os.getenv("SIM_PATH", "/home/src/ut_frameworks/simpy_framework")
build_path = os.getenv("BUILD_PATH","build")

import json
import sys
sys.path.append(f"{simulation_env_path}/")
sys.path.append(f"{build_path}/")
sys.path.append(f"{build_path}/build/")
sys.path.append(f"./")
sys.path.append(f"{simulation_env_path}/../../src/lc/llc/ut/simpy/autogen/")

simpy_path = os.getenv("SIM_PATH")
sys.path.append(f"{simpy_path}/../py_utils/")
from udp_framework.udp import udp
from lpw_intf_autogen import *
from sim_models import sim_models

class btc_dev(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.channel = udp('localhost', 5005,kwargs['env'])
        self.env = kwargs['env']
        self.timeout = 0
        self.channel = udp('localhost', 5005, self.env)


    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index

    def init_device_handler(self, args):
        init_btc_device = hss_cmd_payload_start_procedure_payload_init_btc_device_t()
        json_string = init_btc_device.prep_payload_hss_cmd_payload_start_procedure_payload_init_btc_device(args[0])
        return self.channel.sender(f"{json_string}")   
    
       
    def init_page_handler(self, args):    
        bd_address = args[2].split(":")
        remote_scan_rep_mode = args[3]
        clk_e_offset = args[1]
        for i in range(0, 6):
            bd_address[i] = int(bd_address[i], 16)
        return self.channel.sender(f"lpw init_page {args[0]} {clk_e_offset} {bd_address[0]} {bd_address[1]} {bd_address[2]} {bd_address[3]} {bd_address[4]} {bd_address[5]} {remote_scan_rep_mode}")

    def start_page_handler(self, args):
        start_page = hss_cmd_payload_start_procedure_payload_page_t()
        json_string = start_page.prep_payload_hss_cmd_payload_start_procedure_payload_page(args[0])
        return self.channel.sender(f"{json_string}")   
    
    def start_esco_central_handler(self,args):
        start_esco_central = hss_cmd_payload_start_procedure_payload_esco_central_t()
        json_string = start_esco_central.prep_payload_hss_cmd_payload_start_procedure_payload_esco_central(args[0])
        return self.channel.sender(f"{json_string}")
    
    def start_esco_peripheral_handler(self,args):
        start_esco_peripheral = hss_cmd_payload_start_procedure_payload_esco_peripheral_t()
        json_string = start_esco_peripheral.prep_payload_hss_cmd_payload_start_procedure_payload_esco_peripheral(args[0])
        return self.channel.sender(f"{json_string}")
    
    def start_acl_central_handler(self,args):
        start_acl_central = hss_cmd_payload_start_procedure_payload_acl_central_t()
        json_string = start_acl_central.prep_payload_hss_cmd_payload_start_procedure_payload_acl_central(args[0])
        print(json_string)
        return self.channel.sender(f"{json_string}")
    
    def  start_acl_peripheral_handler(self,args):
        start_acl_peripheral = hss_cmd_payload_start_procedure_payload_acl_peripheral_t()
        json_string = start_acl_peripheral.prep_payload_hss_cmd_payload_start_procedure_payload_acl_peripheral(args[0])
        return self.channel.sender(f"{json_string}")

    
    def init_page_scan_handler(self, args):
        
        return self.channel.sender(f"lpw init_page_scan {args[0]} {args[1]} {args[2]} {args[3]}")
    
    def start_page_scan_handler(self, args):
        start_page_scan = hss_cmd_payload_start_procedure_payload_page_scan_t()
        json_string = start_page_scan.prep_payload_hss_cmd_payload_start_procedure_payload_page_scan(args[0])
        return self.channel.sender(f"{json_string}")  
  

    
    def init_inquiry_handler(self, args):
        current_device_index = args[0]
        LAP_address = args[1].split(":")
        for i in range(0, 3):
            LAP_address[i] = int(LAP_address[i], 16)
        length  = int(args[2])
        num_response    = int(args[3])
        clk_e_offset    = int(args[4])
        EIR_value       = int(args[5])
        return self.channel.sender(f"lpw init_inquiry {int(args[0])} {LAP_address[0]} {LAP_address[1]} {LAP_address[2]} {length} {num_response} {clk_e_offset} {EIR_value}")
    
    def start_inquiry_handler(self, args):
        start_inquiry = hss_cmd_payload_start_procedure_payload_inquiry_t()
        json_string = start_inquiry.prep_payload_hss_cmd_payload_start_procedure_payload_inquiry(args[0])
        return self.channel.sender(f"{json_string}")
    
    def init_inquiry_scan_handler(self,args):
        return self.channel.sender(f"lpw init_inquiry_scan {args[0]} {args[1]} {args[2]} {args[3]}")
        
    def start_inquiry_scan_handler(self,args):
        start_inquiry_scan = hss_cmd_payload_start_procedure_payload_inquiry_scan_t()
        json_string = start_inquiry_scan.prep_payload_hss_cmd_payload_start_procedure_payload_inquiry_scan(args[0])
        return self.channel.sender(f"{json_string}")    

    def start_acl_central_handler(self,args):
        start_acl_central = hss_cmd_payload_start_procedure_payload_acl_central_t()
        json_string = start_acl_central.prep_payload_hss_cmd_payload_start_procedure_payload_acl_central(args[0])
        return self.channel.sender(f"{json_string}")    
   
    def start_acl_peripheral_handler(self,args):
        start_acl_peripheral = hss_cmd_payload_start_procedure_payload_acl_peripheral_t()
        json_string = start_acl_peripheral.prep_payload_hss_cmd_payload_start_procedure_payload_acl_peripheral(args[0])
        return self.channel.sender(f"{json_string}")    

    
    def complete_event_handler(self, *args):
        status = [format(int(key, 16), 'x') for key in args[0][0:4]]
        status = int(''.join(status))
        conn_handle = [format(int(key, 16), 'x') for key in list(reversed(args[0][4:6]))]
        conn_handle = int(''.join(conn_handle))
        bd_addr = [format(int(key, 10), 'x') for key in args[0][6:12]]
        bd_addr = ':'.join(bd_addr)
        link_type = int(args[0][12], 16)
        encryption_enabled = int(args[0][13], 16)
        self.sim_print(f"Paging process is completed status:{status} bd_addr:{bd_addr} conn_handle:{conn_handle} link_type:{link_type} encryption_enabled:{encryption_enabled}")
        return 'OK'
    
    def roleswitch_complete_handler(self, *args):
        status = [format(int(key, 16), 'x') for key in args[0][0:4]]
        status = int(''.join(status))
        self.sim_print(f"roleswitch process is completed status:{status}")
        return 'OK'
        
    def payload_buffer_handler(self,args):
        return self.channel.sender(f"lpw set_payload {args[0]} {args[1]}")
    
    def start_roleswitch_central_handler(self,args):
        start_roleswitch_central = hss_cmd_payload_start_procedure_payload_roleswitch_central_t()
        json_string = start_roleswitch_central.prep_payload_hss_cmd_payload_start_procedure_payload_roleswitch_central(args[0])
        return self.channel.sender(f"{json_string}")
    
    def start_roleswitch_peripheral_handler(self,args):
        start_roleswitch_peripheral = hss_cmd_payload_start_procedure_payload_roleswitch_peripheral_t()
        json_string = start_roleswitch_peripheral.prep_payload_hss_cmd_payload_start_procedure_payload_roleswitch_peripheral(args[0])
        return self.channel.sender(f"{json_string}")
    
    def start_test_mode_handler(self,args):
        start_test_mode = hss_cmd_payload_start_procedure_payload_test_mode_t()
        json_string = start_test_mode.prep_payload_hss_cmd_payload_start_procedure_payload_test_mode(args[0])
        return self.channel.sender(f"{json_string}")
    
    def set_page_handler(self,args):
        set_page = hss_cmd_payload_set_procedure_payload_page_t()
        json_string = set_page.prep_payload_hss_cmd_payload_set_procedure_payload_page(args[0])
        return self.channel.sender(f"{json_string}")
    
    def set_inquiry_handler(self,args):
        set_inquiry = hss_cmd_payload_set_procedure_payload_inquiry_t()
        json_string = set_inquiry.prep_payload_hss_cmd_payload_set_procedure_payload_inquiry(args[0])
        return self.channel.sender(f"{json_string}")
    
    def set_inquiry_scan_handler(self,args):
        set_inquiry_scan = hss_cmd_payload_set_procedure_payload_inquiry_scan_t()
        json_string = set_inquiry_scan.prep_payload_hss_cmd_payload_set_procedure_payload_inquiry_scan(args[0])
        return self.channel.sender(f"{json_string}")
    
    def set_page_scan_handler(self,args):
        set_page_scan = hss_cmd_payload_set_procedure_payload_page_scan_t()
        json_string = set_page_scan.prep_payload_hss_cmd_payload_set_procedure_payload_page_scan(args[0])
        return self.channel.sender(f"{json_string}")
    
    def set_afh_parameters_acl_handler(self,args):
        set_afh_parameters_acl = hss_cmd_payload_set_procedure_payload_afh_parameters_acl_t()
        json_string = set_afh_parameters_acl.prep_payload_hss_cmd_payload_set_procedure_payload_afh_parameters_acl(args[0])
        return self.channel.sender(f"{json_string}")
    
    def set_sniff_parameters_acl_handler(self,args):
        set_sniff_parameters_acl = hss_cmd_payload_set_procedure_payload_sniff_parameters_acl_t()
        json_string = set_sniff_parameters_acl.prep_payload_hss_cmd_payload_set_procedure_payload_sniff_parameters_acl(args[0])
        return self.channel.sender(f"{json_string}")
    
    def set_device_parameters_acl_handler(self,args):
        set_device_parameters_acl = hss_cmd_payload_set_procedure_payload_device_parameters_acl_t()
        json_string = set_device_parameters_acl.prep_payload_hss_cmd_payload_set_procedure_payload_device_parameters_acl(args[0])
        return self.channel.sender(f"{json_string}")   
    
    def set_device_parameters_esco_handler(self,args):
        set_device_parameters_esco = hss_cmd_payload_set_procedure_payload_device_parameters_esco_t()
        json_string = set_device_parameters_esco.prep_payload_hss_cmd_payload_set_procedure_payload_device_parameters_esco(args[0])
        return self.channel.sender(f"{json_string}")     
    
    def set_device_parameters_roleswitch_handler(self,args):
        set_device_parameters_roleswitch = hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch_t()
        json_string = set_device_parameters_roleswitch.prep_payload_hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch(args[0])
        return self.channel.sender(f"{json_string}")   
    
    def set_roleswitch_parameters_handler(self,args):
        set_roleswitch_parameters = hss_cmd_payload_set_procedure_payload_roleswitch_parameters_t()
        json_string = set_roleswitch_parameters.prep_payload_hss_cmd_payload_set_procedure_payload_roleswitch_parameters(args[0])
        return self.channel.sender(f"{json_string}")    
    
    def stop_acl_handler(self,args):
        stop_acl = hss_cmd_payload_stop_procedure_payload_acl_central_t()
        json_string = stop_acl.prep_payload_hss_cmd_payload_stop_procedure_payload_acl_central(args[0])
        return self.channel.sender(f"{json_string}")
    
    def stop_esco_handler(self,args):
        stop_esco = hss_cmd_payload_stop_procedure_payload_esco_central_t()
        json_string = stop_esco.prep_payload_hss_cmd_payload_stop_procedure_payload_esco_central(args[0])
        return self.channel.sender(f"{json_string}")     
    
    def stop_roleswitch_handler(self,args):
        stop_roleswitch = hss_cmd_payload_stop_procedure_payload_roleswitch_central_t()
        json_string = stop_roleswitch.prep_payload_hss_cmd_payload_stop_procedure_payload_roleswitch_central(args[0])
        return self.channel.sender(f"{json_string}")  
    
    def stop_acl_peripheral_handler(self,args):
        stop_acl_peripheral = hss_cmd_payload_stop_procedure_payload_acl_peripheral_t()
        json_string = stop_acl_peripheral.prep_payload_hss_cmd_payload_stop_procedure_payload_acl_peripheral(args[0])
        return self.channel.sender(f"{json_string}")
    
    def stop_esco_peripheral_handler(self,args):
        stop_esco_peripheral = hss_cmd_payload_stop_procedure_payload_esco_peripheral_t()
        json_string = stop_esco_peripheral.prep_payload_hss_cmd_payload_stop_procedure_payload_esco_peripheral(args[0])
        return self.channel.sender(f"{json_string}")     
    
    def stop_roleswitch_peripheral_handler(self,args):
        stop_roleswitch_peripheral = hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral_t()
        json_string = stop_roleswitch_peripheral.prep_payload_hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral(args[0])
        return self.channel.sender(f"{json_string}") 
    
    def stop_inquiry_handler(self,args):
        stop_inquiry = hss_cmd_payload_stop_procedure_payload_inquiry_t()
        json_string = stop_inquiry.prep_payload_hss_cmd_payload_stop_procedure_payload_inquiry(args[0])
        return self.channel.sender(f"{json_string}")  
   
    def stop_inquiry_scan_handler(self,args):
        stop_inquiry_scan = hss_cmd_payload_stop_procedure_payload_inquiry_scan_t()
        json_string = stop_inquiry_scan.prep_payload_hss_cmd_payload_stop_procedure_payload_inquiry_scan(args[0])
        return self.channel.sender(f"{json_string}")  
   
    def stop_page_handler(self,args):
        stop_page = hss_cmd_payload_stop_procedure_payload_page_t()
        json_string = stop_page.prep_payload_hss_cmd_payload_stop_procedure_payload_page(args[0])
        return self.channel.sender(f"{json_string}")
   
    def stop_page_scan_handler(self,args):
        stop_page_scan = hss_cmd_payload_stop_procedure_payload_page_scan_t()
        json_string = stop_page_scan.prep_payload_hss_cmd_payload_stop_procedure_payload_page_scan(args[0])
        return self.channel.sender(f"{json_string}")       
    
    def procedure_complete_handler(self, *args):
        status = [format(int(key, 16), 'x') for key in args[0][0:4]]
        status = int(''.join(status))
        self.sim_print(f"procedure is complete, process is completed status:{status}")
        return 'OK'