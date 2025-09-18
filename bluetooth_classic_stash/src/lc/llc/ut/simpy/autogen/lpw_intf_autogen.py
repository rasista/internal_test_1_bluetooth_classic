import json
import copy 

class hss_cmd_payload_start_procedure_payload_init_btc_device_t():
    device_index = int(-1)
    bd_address = list(range(6, -1))
    sync_word = list(range(8, -1))
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_init_btc_device {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_init_btc_device\":{"
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s" % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_init_btc_device(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_page_t():
    device_index = int(-1)
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_page {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_page\":{"
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_page(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_page_scan_t():
    device_index = int(-1)
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_page_scan {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_page_scan\":{"
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_page_scan(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_inquiry_t():
    device_index = int(-1)
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_inquiry {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_inquiry\":{"
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_inquiry(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_inquiry_scan_t():
    device_index = int(-1)
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_inquiry_scan {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_inquiry_scan\":{"
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_inquiry_scan(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_acl_central_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    bd_address = list(range(6, -1))
    sync_word = list(range(8, -1))
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_acl_central {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_acl_central\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_acl_central(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_acl_peripheral_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    bd_address = list(range(6, -1))
    sync_word = list(range(8, -1))
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_acl_peripheral {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_acl_peripheral\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_acl_peripheral(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_esco_central_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    bd_address = list(range(6, -1))
    sync_word = list(range(8, -1))
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_esco_central {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_esco_central\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_esco_central(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_esco_peripheral_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    bd_address = list(range(6, -1))
    sync_word = list(range(8, -1))
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_esco_peripheral {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_esco_peripheral\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_esco_peripheral(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_roleswitch_central_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    bd_address = list(range(6, -1))
    sync_word = list(range(8, -1))
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_roleswitch_central {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_roleswitch_central\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_roleswitch_central(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_roleswitch_peripheral_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    bd_address = list(range(6, -1))
    sync_word = list(range(8, -1))
    priority = int(-1)
    step_size = int(-1)
    end_tsf = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_roleswitch_peripheral {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_roleswitch_peripheral\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"priority\":%s," % self.priority
        if(len(string)):
            json_string += string
        string = ''
        string = "\"step_size\":%s," % self.step_size
        if(len(string)):
            json_string += string
        string = ''
        string = "\"end_tsf\":%s" % self.end_tsf
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_roleswitch_peripheral(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.priority = int(json_cmd_string[key]["priority"])
                self.step_size = int(json_cmd_string[key]["step_size"])
                self.end_tsf = int(json_cmd_string[key]["end_tsf"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_test_mode_t():
    device_index = int(-1)
    bd_address = list(range(3, -1))
    sync_word = list(range(8, -1))
    br_edr_mode = int(-1)
    channel_index = int(-1)
    link_type = int(-1)
    inter_packet_gap = int(-1)
    scrambler_seed = int(-1)
    pkt_type = int(-1)
    pkt_len = int(-1)
    payload_type = int(-1)
    tx_power = int(-1)
    hopping_type = int(-1)
    loopback_mode = int(-1)
    transmit_mode = int(-1)
    encryption_mode = int(-1)
    no_of_pkts = int(-1)
    rx_window = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload_test_mode {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload_test_mode\":{"
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"br_edr_mode\":%s," % self.br_edr_mode
        if(len(string)):
            json_string += string
        string = ''
        string = "\"channel_index\":%s," % self.channel_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"link_type\":%s," % self.link_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"inter_packet_gap\":%s," % self.inter_packet_gap
        if(len(string)):
            json_string += string
        string = ''
        string = "\"scrambler_seed\":%s," % self.scrambler_seed
        if(len(string)):
            json_string += string
        string = ''
        string = "\"pkt_type\":%s," % self.pkt_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"pkt_len\":%s," % self.pkt_len
        if(len(string)):
            json_string += string
        string = ''
        string = "\"payload_type\":%s," % self.payload_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"tx_power\":%s," % self.tx_power
        if(len(string)):
            json_string += string
        string = ''
        string = "\"hopping_type\":%s," % self.hopping_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"loopback_mode\":%s," % self.loopback_mode
        if(len(string)):
            json_string += string
        string = ''
        string = "\"transmit_mode\":%s," % self.transmit_mode
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encryption_mode\":%s," % self.encryption_mode
        if(len(string)):
            json_string += string
        string = ''
        string = "\"no_of_pkts\":%s," % self.no_of_pkts
        if(len(string)):
            json_string += string
        string = ''
        string = "\"rx_window\":%s" % self.rx_window
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload_test_mode(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.br_edr_mode = int(json_cmd_string[key]["br_edr_mode"])
                self.channel_index = int(json_cmd_string[key]["channel_index"])
                self.link_type = int(json_cmd_string[key]["link_type"])
                self.inter_packet_gap = int(json_cmd_string[key]["inter_packet_gap"])
                self.scrambler_seed = int(json_cmd_string[key]["scrambler_seed"])
                self.pkt_type = int(json_cmd_string[key]["pkt_type"])
                self.pkt_len = int(json_cmd_string[key]["pkt_len"])
                self.payload_type = int(json_cmd_string[key]["payload_type"])
                self.tx_power = int(json_cmd_string[key]["tx_power"])
                self.hopping_type = int(json_cmd_string[key]["hopping_type"])
                self.loopback_mode = int(json_cmd_string[key]["loopback_mode"])
                self.transmit_mode = int(json_cmd_string[key]["transmit_mode"])
                self.encryption_mode = int(json_cmd_string[key]["encryption_mode"])
                self.no_of_pkts = int(json_cmd_string[key]["no_of_pkts"])
                self.rx_window = int(json_cmd_string[key]["rx_window"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_page_t():
    device_index = int(-1)
    clk_e_offset = int(-1)
    hci_trigger = int(-1)
    tx_pwr_index = int(-1)
    remote_scan_rep_mode = int(-1)
    bd_address = list(range(6, -1))
    sync_word = list(range(8, -1))
    lt_addr = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_page {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_page\":{"
        string = "\"clk_e_offset\":%s," % self.clk_e_offset
        if(len(string)):
            json_string += string
        string = ''
        string = "\"hci_trigger\":%s," % self.hci_trigger
        if(len(string)):
            json_string += string
        string = ''
        string = "\"tx_pwr_index\":%s," % self.tx_pwr_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"remote_scan_rep_mode\":%s," % self.remote_scan_rep_mode
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"lt_addr\":%s" % self.lt_addr
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_page(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.clk_e_offset = int(json_cmd_string[key]["clk_e_offset"])
                self.hci_trigger = int(json_cmd_string[key]["hci_trigger"])
                self.tx_pwr_index = int(json_cmd_string[key]["tx_pwr_index"])
                self.remote_scan_rep_mode = int(json_cmd_string[key]["remote_scan_rep_mode"])
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.lt_addr = int(json_cmd_string[key]["lt_addr"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_page_scan_t():
    device_index = int(-1)
    window = int(-1)
    interval = int(-1)
    curr_scan_type = int(-1)
    tx_pwr_index = int(-1)
    bd_address = list(range(6, -1))
    sync_word = list(range(8, -1))
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_page_scan {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_page_scan\":{"
        string = "\"window\":%s," % self.window
        if(len(string)):
            json_string += string
        string = ''
        string = "\"interval\":%s," % self.interval
        if(len(string)):
            json_string += string
        string = ''
        string = "\"curr_scan_type\":%s," % self.curr_scan_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"tx_pwr_index\":%s," % self.tx_pwr_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bd_address\":%s," % self.bd_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s" % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_page_scan(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.window = int(json_cmd_string[key]["window"])
                self.interval = int(json_cmd_string[key]["interval"])
                self.curr_scan_type = int(json_cmd_string[key]["curr_scan_type"])
                self.tx_pwr_index = int(json_cmd_string[key]["tx_pwr_index"])
                self.bd_address = copy.deepcopy(json_cmd_string[key]["bd_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_inquiry_t():
    device_index = int(-1)
    lap_address = list(range(3, -1))
    sync_word = list(range(8, -1))
    hci_trigger = int(-1)
    length = int(-1)
    EIR_value = int(-1)
    tx_pwr_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_inquiry {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_inquiry\":{"
        string = "\"lap_address\":%s," % self.lap_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"hci_trigger\":%s," % self.hci_trigger
        if(len(string)):
            json_string += string
        string = ''
        string = "\"length\":%s," % self.length
        if(len(string)):
            json_string += string
        string = ''
        string = "\"EIR_value\":%s," % self.EIR_value
        if(len(string)):
            json_string += string
        string = ''
        string = "\"tx_pwr_index\":%s" % self.tx_pwr_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_inquiry(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.lap_address = copy.deepcopy(json_cmd_string[key]["lap_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.hci_trigger = int(json_cmd_string[key]["hci_trigger"])
                self.length = int(json_cmd_string[key]["length"])
                self.EIR_value = int(json_cmd_string[key]["EIR_value"])
                self.tx_pwr_index = int(json_cmd_string[key]["tx_pwr_index"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_E0_encryption_t():
    device_index = int(-1)
    masterClock = int(-1)
    encKey0 = int(-1)
    encKey1 = int(-1)
    encKey2 = int(-1)
    encKey3 = int(-1)
    bdAddr0 = int(-1)
    bdAddr1 = int(-1)
    keyLength = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_E0_encryption {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_E0_encryption\":{"
        string = "\"masterClock\":%s," % self.masterClock
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encKey0\":%s," % self.encKey0
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encKey1\":%s," % self.encKey1
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encKey2\":%s," % self.encKey2
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encKey3\":%s," % self.encKey3
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bdAddr0\":%s," % self.bdAddr0
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bdAddr1\":%s," % self.bdAddr1
        if(len(string)):
            json_string += string
        string = ''
        string = "\"keyLength\":%s" % self.keyLength
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_E0_encryption(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.masterClock = int(json_cmd_string[key]["masterClock"])
                self.encKey0 = int(json_cmd_string[key]["encKey0"])
                self.encKey1 = int(json_cmd_string[key]["encKey1"])
                self.encKey2 = int(json_cmd_string[key]["encKey2"])
                self.encKey3 = int(json_cmd_string[key]["encKey3"])
                self.bdAddr0 = int(json_cmd_string[key]["bdAddr0"])
                self.bdAddr1 = int(json_cmd_string[key]["bdAddr1"])
                self.keyLength = int(json_cmd_string[key]["keyLength"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_AES_encryption_t():
    device_index = int(-1)
    aesKeyBytes_0_3 = int(-1)
    aesKeyBytes_4_7 = int(-1)
    aesKeyBytes_8_11 = int(-1)
    aesKeyBytes_12_15 = int(-1)
    aesIv1 = int(-1)
    aesIv2 = int(-1)
    dayCountAndDir = int(-1)
    zeroLenAclW = int(-1)
    aesPldCntr1 = int(-1)
    aesPldCntr2 = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_AES_encryption {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_AES_encryption\":{"
        string = "\"aesKeyBytes_0_3\":%s," % self.aesKeyBytes_0_3
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesKeyBytes_4_7\":%s," % self.aesKeyBytes_4_7
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesKeyBytes_8_11\":%s," % self.aesKeyBytes_8_11
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesKeyBytes_12_15\":%s," % self.aesKeyBytes_12_15
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesIv1\":%s," % self.aesIv1
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesIv2\":%s," % self.aesIv2
        if(len(string)):
            json_string += string
        string = ''
        string = "\"dayCountAndDir\":%s," % self.dayCountAndDir
        if(len(string)):
            json_string += string
        string = ''
        string = "\"zeroLenAclW\":%s," % self.zeroLenAclW
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesPldCntr1\":%s," % self.aesPldCntr1
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesPldCntr2\":%s" % self.aesPldCntr2
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_AES_encryption(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.aesKeyBytes_0_3 = int(json_cmd_string[key]["aesKeyBytes_0_3"])
                self.aesKeyBytes_4_7 = int(json_cmd_string[key]["aesKeyBytes_4_7"])
                self.aesKeyBytes_8_11 = int(json_cmd_string[key]["aesKeyBytes_8_11"])
                self.aesKeyBytes_12_15 = int(json_cmd_string[key]["aesKeyBytes_12_15"])
                self.aesIv1 = int(json_cmd_string[key]["aesIv1"])
                self.aesIv2 = int(json_cmd_string[key]["aesIv2"])
                self.dayCountAndDir = int(json_cmd_string[key]["dayCountAndDir"])
                self.zeroLenAclW = int(json_cmd_string[key]["zeroLenAclW"])
                self.aesPldCntr1 = int(json_cmd_string[key]["aesPldCntr1"])
                self.aesPldCntr2 = int(json_cmd_string[key]["aesPldCntr2"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_encryption_parameters_aes_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    encryption_mode = int(-1)
    aesKeyBytes_0_3 = int(-1)
    aesKeyBytes_4_7 = int(-1)
    aesKeyBytes_8_11 = int(-1)
    aesKeyBytes_12_15 = int(-1)
    aesIv1 = int(-1)
    aesIv2 = int(-1)
    dayCountAndDir = int(-1)
    zeroLenAclW = int(-1)
    aesPldCntr1 = int(-1)
    aesPldCntr2 = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_encryption_parameters_aes {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_encryption_parameters_aes\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encryption_mode\":%s," % self.encryption_mode
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesKeyBytes_0_3\":%s," % self.aesKeyBytes_0_3
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesKeyBytes_4_7\":%s," % self.aesKeyBytes_4_7
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesKeyBytes_8_11\":%s," % self.aesKeyBytes_8_11
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesKeyBytes_12_15\":%s," % self.aesKeyBytes_12_15
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesIv1\":%s," % self.aesIv1
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesIv2\":%s," % self.aesIv2
        if(len(string)):
            json_string += string
        string = ''
        string = "\"dayCountAndDir\":%s," % self.dayCountAndDir
        if(len(string)):
            json_string += string
        string = ''
        string = "\"zeroLenAclW\":%s," % self.zeroLenAclW
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesPldCntr1\":%s," % self.aesPldCntr1
        if(len(string)):
            json_string += string
        string = ''
        string = "\"aesPldCntr2\":%s" % self.aesPldCntr2
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_encryption_parameters_aes(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.encryption_mode = int(json_cmd_string[key]["encryption_mode"])
                self.aesKeyBytes_0_3 = int(json_cmd_string[key]["aesKeyBytes_0_3"])
                self.aesKeyBytes_4_7 = int(json_cmd_string[key]["aesKeyBytes_4_7"])
                self.aesKeyBytes_8_11 = int(json_cmd_string[key]["aesKeyBytes_8_11"])
                self.aesKeyBytes_12_15 = int(json_cmd_string[key]["aesKeyBytes_12_15"])
                self.aesIv1 = int(json_cmd_string[key]["aesIv1"])
                self.aesIv2 = int(json_cmd_string[key]["aesIv2"])
                self.dayCountAndDir = int(json_cmd_string[key]["dayCountAndDir"])
                self.zeroLenAclW = int(json_cmd_string[key]["zeroLenAclW"])
                self.aesPldCntr1 = int(json_cmd_string[key]["aesPldCntr1"])
                self.aesPldCntr2 = int(json_cmd_string[key]["aesPldCntr2"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_encryption_parameters_e0_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    encryption_mode = int(-1)
    masterClock = int(-1)
    encKey0 = int(-1)
    encKey1 = int(-1)
    encKey2 = int(-1)
    encKey3 = int(-1)
    bdAddr0 = int(-1)
    bdAddr1 = int(-1)
    keyLength = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_encryption_parameters_e0 {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_encryption_parameters_e0\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encryption_mode\":%s," % self.encryption_mode
        if(len(string)):
            json_string += string
        string = ''
        string = "\"masterClock\":%s," % self.masterClock
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encKey0\":%s," % self.encKey0
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encKey1\":%s," % self.encKey1
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encKey2\":%s," % self.encKey2
        if(len(string)):
            json_string += string
        string = ''
        string = "\"encKey3\":%s," % self.encKey3
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bdAddr0\":%s," % self.bdAddr0
        if(len(string)):
            json_string += string
        string = ''
        string = "\"bdAddr1\":%s," % self.bdAddr1
        if(len(string)):
            json_string += string
        string = ''
        string = "\"keyLength\":%s" % self.keyLength
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_encryption_parameters_e0(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.encryption_mode = int(json_cmd_string[key]["encryption_mode"])
                self.masterClock = int(json_cmd_string[key]["masterClock"])
                self.encKey0 = int(json_cmd_string[key]["encKey0"])
                self.encKey1 = int(json_cmd_string[key]["encKey1"])
                self.encKey2 = int(json_cmd_string[key]["encKey2"])
                self.encKey3 = int(json_cmd_string[key]["encKey3"])
                self.bdAddr0 = int(json_cmd_string[key]["bdAddr0"])
                self.bdAddr1 = int(json_cmd_string[key]["bdAddr1"])
                self.keyLength = int(json_cmd_string[key]["keyLength"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_inquiry_scan_t():
    device_index = int(-1)
    window = int(-1)
    interval = int(-1)
    eir_data_length = int(-1)
    eir_data = list(range(260, -1))
    EIR_value = int(-1)
    lap_address = list(range(3, -1))
    sync_word = list(range(8, -1))
    curr_scan_type = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_inquiry_scan {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_inquiry_scan\":{"
        string = "\"window\":%s," % self.window
        if(len(string)):
            json_string += string
        string = ''
        string = "\"interval\":%s," % self.interval
        if(len(string)):
            json_string += string
        string = ''
        string = "\"eir_data_length\":%s," % self.eir_data_length
        if(len(string)):
            json_string += string
        string = ''
        string = "\"eir_data\":%s," % self.eir_data
        if(len(string)):
            json_string += string
        string = ''
        string = "\"EIR_value\":%s," % self.EIR_value
        if(len(string)):
            json_string += string
        string = ''
        string = "\"lap_address\":%s," % self.lap_address
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sync_word\":%s," % self.sync_word
        if(len(string)):
            json_string += string
        string = ''
        string = "\"curr_scan_type\":%s" % self.curr_scan_type
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_inquiry_scan(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.window = int(json_cmd_string[key]["window"])
                self.interval = int(json_cmd_string[key]["interval"])
                self.eir_data_length = int(json_cmd_string[key]["eir_data_length"])
                self.eir_data = copy.deepcopy(json_cmd_string[key]["eir_data"])
                self.EIR_value = int(json_cmd_string[key]["EIR_value"])
                self.lap_address = copy.deepcopy(json_cmd_string[key]["lap_address"])
                self.sync_word = copy.deepcopy(json_cmd_string[key]["sync_word"])
                self.curr_scan_type = int(json_cmd_string[key]["curr_scan_type"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_afh_parameters_acl_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    no_of_valid_afh_channels = int(-1)
    piconet_type = int(-1)
    afh_new_channel_map = list(range(10, -1))
    channel_map_in_use = list(range(10, -1))
    afh_new_channel_map_instant = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_afh_parameters_acl {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_afh_parameters_acl\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"no_of_valid_afh_channels\":%s," % self.no_of_valid_afh_channels
        if(len(string)):
            json_string += string
        string = ''
        string = "\"piconet_type\":%s," % self.piconet_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"afh_new_channel_map\":%s," % self.afh_new_channel_map
        if(len(string)):
            json_string += string
        string = ''
        string = "\"channel_map_in_use\":%s," % self.channel_map_in_use
        if(len(string)):
            json_string += string
        string = ''
        string = "\"afh_new_channel_map_instant\":%s" % self.afh_new_channel_map_instant
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_afh_parameters_acl(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.no_of_valid_afh_channels = int(json_cmd_string[key]["no_of_valid_afh_channels"])
                self.piconet_type = int(json_cmd_string[key]["piconet_type"])
                self.afh_new_channel_map = copy.deepcopy(json_cmd_string[key]["afh_new_channel_map"])
                self.channel_map_in_use = copy.deepcopy(json_cmd_string[key]["channel_map_in_use"])
                self.afh_new_channel_map_instant = int(json_cmd_string[key]["afh_new_channel_map_instant"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_sniff_parameters_acl_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    sniff_instant = int(-1)
    sniff_interval = int(-1)
    sniff_attempt = int(-1)
    sniff_timeout = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_sniff_parameters_acl {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_sniff_parameters_acl\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sniff_instant\":%s," % self.sniff_instant
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sniff_interval\":%s," % self.sniff_interval
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sniff_attempt\":%s," % self.sniff_attempt
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sniff_timeout\":%s" % self.sniff_timeout
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_sniff_parameters_acl(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.sniff_instant = int(json_cmd_string[key]["sniff_instant"])
                self.sniff_interval = int(json_cmd_string[key]["sniff_interval"])
                self.sniff_attempt = int(json_cmd_string[key]["sniff_attempt"])
                self.sniff_timeout = int(json_cmd_string[key]["sniff_timeout"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_device_parameters_acl_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    peer_role = int(-1)
    tx_pwr_index = int(-1)
    flow = int(-1)
    br_edr_mode = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_device_parameters_acl {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_device_parameters_acl\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"peer_role\":%s," % self.peer_role
        if(len(string)):
            json_string += string
        string = ''
        string = "\"tx_pwr_index\":%s," % self.tx_pwr_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"flow\":%s," % self.flow
        if(len(string)):
            json_string += string
        string = ''
        string = "\"br_edr_mode\":%s" % self.br_edr_mode
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_device_parameters_acl(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.peer_role = int(json_cmd_string[key]["peer_role"])
                self.tx_pwr_index = int(json_cmd_string[key]["tx_pwr_index"])
                self.flow = int(json_cmd_string[key]["flow"])
                self.br_edr_mode = int(json_cmd_string[key]["br_edr_mode"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_test_mode_params_t():
    device_index = int(-1)
    test_scenario = int(-1)
    hopping_mode = int(-1)
    tx_freq = int(-1)
    rx_freq = int(-1)
    pwr_cntrl_mode = int(-1)
    poll_period = int(-1)
    pkt_type = int(-1)
    link_type = int(-1)
    test_lt_type = int(-1)
    test_started = int(-1)
    test_mode_type = int(-1)
    sco_pkt_type = int(-1)
    len_of_seq = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_test_mode_params {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_test_mode_params\":{"
        string = "\"test_scenario\":%s," % self.test_scenario
        if(len(string)):
            json_string += string
        string = ''
        string = "\"hopping_mode\":%s," % self.hopping_mode
        if(len(string)):
            json_string += string
        string = ''
        string = "\"tx_freq\":%s," % self.tx_freq
        if(len(string)):
            json_string += string
        string = ''
        string = "\"rx_freq\":%s," % self.rx_freq
        if(len(string)):
            json_string += string
        string = ''
        string = "\"pwr_cntrl_mode\":%s," % self.pwr_cntrl_mode
        if(len(string)):
            json_string += string
        string = ''
        string = "\"poll_period\":%s," % self.poll_period
        if(len(string)):
            json_string += string
        string = ''
        string = "\"pkt_type\":%s," % self.pkt_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"link_type\":%s," % self.link_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"test_lt_type\":%s," % self.test_lt_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"test_started\":%s," % self.test_started
        if(len(string)):
            json_string += string
        string = ''
        string = "\"test_mode_type\":%s," % self.test_mode_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"sco_pkt_type\":%s," % self.sco_pkt_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"len_of_seq\":%s" % self.len_of_seq
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_test_mode_params(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.test_scenario = int(json_cmd_string[key]["test_scenario"])
                self.hopping_mode = int(json_cmd_string[key]["hopping_mode"])
                self.tx_freq = int(json_cmd_string[key]["tx_freq"])
                self.rx_freq = int(json_cmd_string[key]["rx_freq"])
                self.pwr_cntrl_mode = int(json_cmd_string[key]["pwr_cntrl_mode"])
                self.poll_period = int(json_cmd_string[key]["poll_period"])
                self.pkt_type = int(json_cmd_string[key]["pkt_type"])
                self.link_type = int(json_cmd_string[key]["link_type"])
                self.test_lt_type = int(json_cmd_string[key]["test_lt_type"])
                self.test_started = int(json_cmd_string[key]["test_started"])
                self.test_mode_type = int(json_cmd_string[key]["test_mode_type"])
                self.sco_pkt_type = int(json_cmd_string[key]["sco_pkt_type"])
                self.len_of_seq = int(json_cmd_string[key]["len_of_seq"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_device_parameters_esco_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    peer_role = int(-1)
    lt_addr = int(-1)
    flow = int(-1)
    t_esco = int(-1)
    d_esco = int(-1)
    w_esco = int(-1)
    is_esco = int(-1)
    is_edr = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_device_parameters_esco {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_device_parameters_esco\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"peer_role\":%s," % self.peer_role
        if(len(string)):
            json_string += string
        string = ''
        string = "\"lt_addr\":%s," % self.lt_addr
        if(len(string)):
            json_string += string
        string = ''
        string = "\"flow\":%s," % self.flow
        if(len(string)):
            json_string += string
        string = ''
        string = "\"t_esco\":%s," % self.t_esco
        if(len(string)):
            json_string += string
        string = ''
        string = "\"d_esco\":%s," % self.d_esco
        if(len(string)):
            json_string += string
        string = ''
        string = "\"w_esco\":%s," % self.w_esco
        if(len(string)):
            json_string += string
        string = ''
        string = "\"is_esco\":%s," % self.is_esco
        if(len(string)):
            json_string += string
        string = ''
        string = "\"is_edr\":%s" % self.is_edr
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_device_parameters_esco(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.peer_role = int(json_cmd_string[key]["peer_role"])
                self.lt_addr = int(json_cmd_string[key]["lt_addr"])
                self.flow = int(json_cmd_string[key]["flow"])
                self.t_esco = int(json_cmd_string[key]["t_esco"])
                self.d_esco = int(json_cmd_string[key]["d_esco"])
                self.w_esco = int(json_cmd_string[key]["w_esco"])
                self.is_esco = int(json_cmd_string[key]["is_esco"])
                self.is_edr = int(json_cmd_string[key]["is_edr"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    peer_role = int(-1)
    lt_addr = int(-1)
    tx_pwr_index = int(-1)
    flow = int(-1)
    clk_e_offset = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"peer_role\":%s," % self.peer_role
        if(len(string)):
            json_string += string
        string = ''
        string = "\"lt_addr\":%s," % self.lt_addr
        if(len(string)):
            json_string += string
        string = ''
        string = "\"tx_pwr_index\":%s," % self.tx_pwr_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"flow\":%s," % self.flow
        if(len(string)):
            json_string += string
        string = ''
        string = "\"clk_e_offset\":%s" % self.clk_e_offset
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.peer_role = int(json_cmd_string[key]["peer_role"])
                self.lt_addr = int(json_cmd_string[key]["lt_addr"])
                self.tx_pwr_index = int(json_cmd_string[key]["tx_pwr_index"])
                self.flow = int(json_cmd_string[key]["flow"])
                self.clk_e_offset = int(json_cmd_string[key]["clk_e_offset"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_roleswitch_parameters_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    new_lt_addr = int(-1)
    roleswitch_slot_offset = int(-1)
    roleswitch_instant = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_roleswitch_parameters {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_roleswitch_parameters\":{"
        string = "\"connection_device_index\":%s," % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        string = "\"new_lt_addr\":%s," % self.new_lt_addr
        if(len(string)):
            json_string += string
        string = ''
        string = "\"roleswitch_slot_offset\":%s," % self.roleswitch_slot_offset
        if(len(string)):
            json_string += string
        string = ''
        string = "\"roleswitch_instant\":%s" % self.roleswitch_instant
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_roleswitch_parameters(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
                self.new_lt_addr = int(json_cmd_string[key]["new_lt_addr"])
                self.roleswitch_slot_offset = int(json_cmd_string[key]["roleswitch_slot_offset"])
                self.roleswitch_instant = int(json_cmd_string[key]["roleswitch_instant"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel_t():
    device_index = int(-1)
    channel = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel\":{"
        string = "\"channel\":%s" % self.channel
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.channel = int(json_cmd_string[key]["channel"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_page_t():
    device_index = int(-1)
    current_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_page {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_page\":{"
        string = "\"current_device_index\":%s" % self.current_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_page(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.current_device_index = int(json_cmd_string[key]["current_device_index"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_inquiry_t():
    device_index = int(-1)
    current_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_inquiry {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_inquiry\":{"
        string = "\"current_device_index\":%s" % self.current_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_inquiry(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.current_device_index = int(json_cmd_string[key]["current_device_index"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_page_scan_t():
    device_index = int(-1)
    current_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_page_scan {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_page_scan\":{"
        string = "\"current_device_index\":%s" % self.current_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_page_scan(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.current_device_index = int(json_cmd_string[key]["current_device_index"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_inquiry_scan_t():
    device_index = int(-1)
    current_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_inquiry_scan {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_inquiry_scan\":{"
        string = "\"current_device_index\":%s" % self.current_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_inquiry_scan(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.current_device_index = int(json_cmd_string[key]["current_device_index"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_acl_central_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_acl_central {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_acl_central\":{"
        string = "\"connection_device_index\":%s" % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_acl_central(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_esco_central_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_esco_central {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_esco_central\":{"
        string = "\"connection_device_index\":%s" % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_esco_central(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_roleswitch_central_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_roleswitch_central {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_roleswitch_central\":{"
        string = "\"connection_device_index\":%s" % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_roleswitch_central(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_acl_peripheral_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_acl_peripheral {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_acl_peripheral\":{"
        string = "\"connection_device_index\":%s" % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_acl_peripheral(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_esco_peripheral_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_esco_peripheral {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_esco_peripheral\":{"
        string = "\"connection_device_index\":%s" % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_esco_peripheral(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral_t():
    device_index = int(-1)
    connection_device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral\":{"
        string = "\"connection_device_index\":%s" % self.connection_device_index
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_device_index = int(json_cmd_string[key]["connection_device_index"])
        return self.jasonize()
class hss_cmd_payload_start_procedure_payload_t():
    device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure_payload {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure_payload\":{"
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure_payload(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
        return self.jasonize()
class hss_cmd_payload_start_procedure_t():
    device_index = int(-1)
    id = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_start_procedure {self.device_index}  "
        json_string = "{\"hss_cmd_payload_start_procedure\":{"
        string = "\"id\":%s," % self.id
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_start_procedure(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.id = int(json_cmd_string[key]["id"])
                print("no data ")
        return self.jasonize()
class hss_cmd_payload_get_procedure_payload_t():
    device_index = int(-1)
    id = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_get_procedure_payload {self.device_index}  "
        json_string = "{\"hss_cmd_payload_get_procedure_payload\":{"
        string = "\"id\":%s" % self.id
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_get_procedure_payload(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.id = int(json_cmd_string[key]["id"])
        return self.jasonize()
class hss_cmd_payload_set_procedure_payload_t():
    device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure_payload {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure_payload\":{"
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure_payload(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
        return self.jasonize()
class hss_cmd_payload_set_procedure_t():
    device_index = int(-1)
    id = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_set_procedure {self.device_index}  "
        json_string = "{\"hss_cmd_payload_set_procedure\":{"
        string = "\"id\":%s," % self.id
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_set_procedure(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.id = int(json_cmd_string[key]["id"])
                print("no data ")
        return self.jasonize()
class hss_cmd_payload_stop_procedure_payload_t():
    device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure_payload {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure_payload\":{"
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure_payload(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
        return self.jasonize()
class hss_cmd_payload_stop_procedure_t():
    device_index = int(-1)
    id = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_stop_procedure {self.device_index}  "
        json_string = "{\"hss_cmd_payload_stop_procedure\":{"
        string = "\"id\":%s," % self.id
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_stop_procedure(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.id = int(json_cmd_string[key]["id"])
                print("no data ")
        return self.jasonize()
class hss_cmd_payload_get_procedure_t():
    device_index = int(-1)
    id = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload_get_procedure {self.device_index}  "
        json_string = "{\"hss_cmd_payload_get_procedure\":{"
        string = "\"id\":%s," % self.id
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload_get_procedure(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.id = int(json_cmd_string[key]["id"])
                print("no data ")
        return self.jasonize()
class hss_cmd_payload_t():
    device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd_payload {self.device_index}  "
        json_string = "{\"hss_cmd_payload\":{"
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd_payload(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                print("no data ")
                print("no data ")
                print("no data ")
                print("no data ")
        return self.jasonize()
class hss_cmd_t():
    device_index = int(-1)
    cmd_type = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_cmd {self.device_index}  "
        json_string = "{\"hss_cmd\":{"
        string = "\"cmd_type\":%s," % self.cmd_type
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_cmd(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.cmd_type = int(json_cmd_string[key]["cmd_type"])
                print("no data ")
        return self.jasonize()
class hss_event_payload_page_complete_t():
    device_index = int(-1)
    connection_info_idx = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_event_payload_page_complete {self.device_index}  "
        json_string = "{\"hss_event_payload_page_complete\":{"
        string = "\"connection_info_idx\":%s" % self.connection_info_idx
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_event_payload_page_complete(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_info_idx = int(json_cmd_string[key]["connection_info_idx"])
        return self.jasonize()
class hss_event_payload_page_scan_complete_t():
    device_index = int(-1)
    connection_info_idx = int(-1)
    remote_bd_address = list(range(6, -1))
    
    def jasonize(self):
        base_cmd = f"lpw hss_event_payload_page_scan_complete {self.device_index}  "
        json_string = "{\"hss_event_payload_page_scan_complete\":{"
        string = "\"connection_info_idx\":%s," % self.connection_info_idx
        if(len(string)):
            json_string += string
        string = ''
        string = "\"remote_bd_address\":%s" % self.remote_bd_address
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_event_payload_page_scan_complete(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.connection_info_idx = int(json_cmd_string[key]["connection_info_idx"])
                self.remote_bd_address = copy.deepcopy(json_cmd_string[key]["remote_bd_address"])
        return self.jasonize()
class hss_event_payload_btc_slot_offset_t():
    device_index = int(-1)
    roleswitch_slot_offset = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_event_payload_btc_slot_offset {self.device_index}  "
        json_string = "{\"hss_event_payload_btc_slot_offset\":{"
        string = "\"roleswitch_slot_offset\":%s" % self.roleswitch_slot_offset
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_event_payload_btc_slot_offset(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.roleswitch_slot_offset = int(json_cmd_string[key]["roleswitch_slot_offset"])
        return self.jasonize()
class hss_event_payload_t():
    device_index = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_event_payload {self.device_index}  "
        json_string = "{\"hss_event_payload\":{"
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_event_payload(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                print("no data ")
                print("no data ")
                print("no data ")
        return self.jasonize()
class hss_event_t():
    device_index = int(-1)
    event_type = int(-1)
    event_status = int(-1)
    
    def jasonize(self):
        base_cmd = f"lpw hss_event {self.device_index}  "
        json_string = "{\"hss_event\":{"
        string = "\"event_type\":%s," % self.event_type
        if(len(string)):
            json_string += string
        string = ''
        string = "\"event_status\":%s," % self.event_status
        if(len(string)):
            json_string += string
        string = ''
        json_string += "}}"
        json_string = json_string.replace(" ","")
        return base_cmd + json_string

    def prep_payload_hss_event(self, args):
        json_cmd_string = json.loads(args)
        key_list = list(json_cmd_string.keys())
        for key in key_list:
            if key == "device_index":
                self.device_index = int(json_cmd_string["device_index"])
            else:
                self.event_type = int(json_cmd_string[key]["event_type"])
                self.event_status = int(json_cmd_string[key]["event_status"])
                print("no data ")
        return self.jasonize()

if __name__ == "__main__":
    init_btc_device = hss_cmd_payload_start_procedure_payload_init_btc_device_t()
    init_btc_device.bd_address = [11, 22, 33, 44, 55, 66]
    print(init_btc_device.jasonize())

    start_page = hss_cmd_payload_start_procedure_payload_page_t()
    start_page.priority = 1
    start_page.step_size = 2
    start_page.end_tsf = 123456
    print(start_page.jasonize())