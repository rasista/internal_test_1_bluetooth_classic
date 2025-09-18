from random import randint
import re
class baseclass:
    def __init__(self, parent_object,env):
        self.env = env
        self.timeout = 0
        self.parent_object = parent_object
        self.scanner_state = "listen_off"
        self.pause_listen = 0
        self.pause_pending = 0
        self.ed_high_count = 0
        self.rx_ongoing_queue = dict()
        self.rx_done_queue = dict()
        self.tx_on_air_collison_count = dict()
        self.wifi_puncture_count = 0
        self.timeout_bkp = 0
        self.pending_adv = ""

    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index
        self.scan_window = int(dict_model_params['init_params']['scan_window'])
        self.scan_interval = int(dict_model_params['init_params']['scan_interval'])
        self.scan_variation = int(dict_model_params['init_params']['variance'])
        self.scan_synchronised = int(dict_model_params['init_params']['synchronised'])
        if self.scan_synchronised != 1:
            self.timeout = self.env.now + randint(0, self.scan_interval)
        else:
            self.timeout = self.env.now + randint(0, 500)

    def timeout_handler(self):
        if self.timeout_bkp:
            self.timeout = self.timeout_bkp
            self.timeout_bkp = 0
            return f"mesh_advertiser_{self.device_index} adv_req {self.pending_adv}\n"
        if self.scanner_state == "listen_off":
            self.start_scan()
        elif self.scanner_state in ["listen_on", "listen_paused" ]:
            return self.schedule_scan()
        return ""
    def start_scan(self):
        variation = randint(0, self.scan_variation)
        if self.pause_listen != 1:
            self.scanner_state = "listen_on"
        else: 
            self.scanner_state = "listen_paused"
        self.timeout = self.env.now + self.scan_window + variation
        self.print_current_state()
        return ""

    def schedule_scan(self):
        return_cmd = ""
        self.timeout = self.env.now + (self.scan_interval - self.scan_window)
        self.scanner_state = "listen_off"
        self.print_current_state()
        self.rx_ongoing_queue = dict()
        if self.pause_pending:
            self.pause_pending = 0
            return_cmd += f"mesh_advertiser_{self.device_index} listen_pause_req_ack {self.scanner_state}"
        return return_cmd

    def listen_pause_req_handler(self, args):
        self.pause_listen = 1
        if self.scanner_state == "listen_off":
            self.wifi_puncture_count +=1
            return f"mesh_advertiser_{self.device_index} listen_pause_req_ack {self.scanner_state}"
        elif bool(self.rx_ongoing_queue.keys()) == 0:
            if self.scanner_state == "listen_on":
                self.scanner_state = "listen_paused"
                self.print_current_state()
            return f"mesh_advertiser_{self.device_index} listen_pause_req_ack {self.scanner_state}"
        else:
            self.pause_pending = 1
        return ""

    def resume_listen_handler(self, args):
        self.pause_listen = 0
        if self.scanner_state == "listen_paused":
            self.scanner_state = "listen_on"
            self.print_current_state()
        return ""

    def print_current_state(self):
        #print(f"{self.env.now} mesh_scanner_{self.device_index} state {self.scanner_state}")
        return

    def tx_start_handler(self, args):
        tx_dev_id = args[0]
        adv_id = args[1]
        channel = int(args[2])
        if channel != ((self.device_index % 3) + 1):
            return ""
        self.ed_high_count += 1
        packet_id = f"{tx_dev_id}_{adv_id}"
        #print("packetID" , packet_id)
        if self.scanner_state == "listen_on":
            if self.ed_high_count == 1:
                self.rx_ongoing_queue[packet_id] = "clean"
                self.receiver_state("RX_START CLEAN")
            else:
                self.rx_ongoing_queue[packet_id] = "corrupted"
                #print("packetID-->" ,tx_dev_id)
                if(tx_dev_id not in self.tx_on_air_collison_count.keys()):
                    self.tx_on_air_collison_count[tx_dev_id] = 1
                else:
                    self.tx_on_air_collison_count[tx_dev_id] +=1
                self.receiver_state("RX_START CORRUPTED")
        return ""

    def tx_end_handler(self, args):
        return_cmd = ""
        tx_dev_id = args[0]
        adv_id = args[1]
        channel = int(args[2])
        if channel != ((self.device_index % 3) + 1):
            return ""
        self.ed_high_count -= 1
        #print(f"{self.env.now} scanner_{self.device_index} ed_high_count {self.ed_high_count}")
        packet_id = f"{tx_dev_id}_{adv_id}"
        if self.scanner_state == "listen_on":
            if packet_id in self.rx_ongoing_queue.keys():
                if self.ed_high_count != 0:
                    self.rx_ongoing_queue[packet_id] = "corrupted"
                    if(tx_dev_id not in self.tx_on_air_collison_count.keys()):
                        self.tx_on_air_collison_count[tx_dev_id] = 1
                    else:
                        self.tx_on_air_collison_count[tx_dev_id] +=1

                pkt_status = self.rx_ongoing_queue.pop(packet_id)
                if pkt_status == "clean":
                    self.receiver_state("RX_END CLEAN")
                    if adv_id not in self.rx_done_queue.keys():
                        self.rx_done_queue[adv_id] = self.env.now - int(adv_id)
                        #return_cmd = f"mesh_advertiser_{self.device_index} adv_req {adv_id}\n"
                        self.timeout_bkp = self.timeout
                        self.pending_adv = adv_id
                        self.timeout = self.env.now
                else:
                    self.receiver_state("RX_END CORRUPTED")
                if self.pause_pending:
                    self.pause_pending = 0
                    self.receiver_state("listen_paused post_rx")
                    self.scanner_state = "listen_paused"
                    self.print_current_state()
                    return_cmd += f"mesh_advertiser_{self.device_index} listen_pause_req_ack {self.scanner_state}"
        return return_cmd

    def receiver_state(self, state):
        #print(f"{self.env.now} mesh_scanner_{self.device_index} receiver_state {state}")
        return

        
        
        