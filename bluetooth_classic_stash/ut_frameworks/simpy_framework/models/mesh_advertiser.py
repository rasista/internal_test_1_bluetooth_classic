from random import randint
class baseclass:
    def __init__(self, parent_object,env):
        self.env = env
        self.timeout = 0
        self.parent_object = parent_object
        self.adv_req_queue = dict()
        self.adv_ongoing_queue = dict()
        self.adv_ongoing_queue_count = 0
        self.adv_done_queue = list()

    def secondary_init_function(self, index, dict_model_params):
        self.adv_relay_repeat_count = int(dict_model_params['init_params']['adv_relay_repeat_count'])
        self.adv_relay_step_interval = int(dict_model_params['init_params']['adv_relay_step_interval'])
        self.adv_relay_min_random_delay = int(dict_model_params['init_params']['adv_relay_min_random_delay'])
        self.adv_relay_max_random_delay = int(dict_model_params['init_params']['adv_relay_max_random_delay'])
        self.adv_packet_duration = int(dict_model_params['init_params']['adv_packet_duration'])
        self.listen_range = int(dict_model_params['init_params']['listen_range'])
        self.instances = int(dict_model_params['init_params']['instances'])
        self.device_index = index

    def generate_tx_status_cmd(self, tx_status, adv_id, channel):
        return_cmd = ""
        for i in range(1, self.instances + 1):
            if (i != self.device_index) & (i >= self.device_index - self.listen_range) & (i <= self.device_index + self.listen_range):
                return_cmd += f"mesh_scanner_{i} {tx_status} {self.device_index} { adv_id } {channel}\n"

        return return_cmd
    def handle_tx_train(self):
        return_cmd = ""
        adv_state = self.adv_ongoing_queue[self.adv_ongoing_queue_count]["state"]
        adv_id = self.adv_ongoing_queue[self.adv_ongoing_queue_count]["adv_id"]
        if adv_state == "FIRST_TX_SCHEDULED":
            return_cmd = self.generate_tx_status_cmd("tx_start", adv_id, 1)
            self.adv_ongoing_queue[self.adv_ongoing_queue_count]["state"] = "FIRST_TX_START"
            self.timeout = self.env.now + self.adv_packet_duration
        elif adv_state == "FIRST_TX_START":
            return_cmd = self.generate_tx_status_cmd("tx_end", adv_id, 1)
            self.adv_ongoing_queue[self.adv_ongoing_queue_count]["state"] = "SECOND_TX_SCHEDULED"
            self.timeout = self.env.now + 150
        elif adv_state == "SECOND_TX_SCHEDULED":
            return_cmd = self.generate_tx_status_cmd("tx_start", adv_id, 2)
            self.adv_ongoing_queue[self.adv_ongoing_queue_count]["state"] = "SECOND_TX_START"
            self.timeout = self.env.now + self.adv_packet_duration
        elif adv_state == "SECOND_TX_START":
            return_cmd = self.generate_tx_status_cmd("tx_end", adv_id, 2)
            self.adv_ongoing_queue[self.adv_ongoing_queue_count]["state"] = "THIRD_TX_SCHEDULED"
            self.timeout = self.env.now + 150
        elif adv_state == "THIRD_TX_SCHEDULED":
            return_cmd = self.generate_tx_status_cmd("tx_start", adv_id, 3)
            self.adv_ongoing_queue[self.adv_ongoing_queue_count]["state"] = "THIRD_TX_START"
            self.timeout = self.env.now + self.adv_packet_duration
        elif adv_state == "THIRD_TX_START":
            return_cmd = self.generate_tx_status_cmd("tx_end", adv_id, 3)
            return_cmd += f"mesh_scanner_{self.device_index} resume_listen\n"
            if bool(self.adv_req_queue):
                self.timeout = min(self.adv_req_queue.keys())
                if self.timeout <= self.env.now:
                    self.timeout = self.env.now + 1
            else:
                self.timeout = 0
            self.adv_ongoing_queue.popitem()
            if bool(self.adv_ongoing_queue.keys()):
                assert()
            self.adv_ongoing_queue_count = 0
        else:
            assert()
        return return_cmd

    def timeout_handler(self):
        self.timeout = 0
        return_cmd = ""
        if bool(self.adv_ongoing_queue.keys()):
            return_cmd = self.handle_tx_train()
        else:
            if bool(self.adv_req_queue):
                adv_tx_tsf = min(self.adv_req_queue.keys())
                adv_id = self.adv_req_queue.pop(adv_tx_tsf)
                self.adv_ongoing_queue_count += 1
                self.adv_ongoing_queue[self.adv_ongoing_queue_count] = {"adv_id" : adv_id, "state" : "FIRST_SCHEDULE_PENDING" , "orig_schedule" : adv_tx_tsf}
                return_cmd = f"mesh_scanner_{self.device_index} listen_pause_req"
        return return_cmd

    def adv_request_handler(self, args):
        id = args[0]
        if id == "generate_id":
            pkt_id = self.env.now
        else:
            pkt_id = int(id)
        if pkt_id in self.adv_done_queue:
            return ""
        for i in range(0, self.adv_relay_repeat_count):
            target_tsf = self.env.now + i*self.adv_relay_step_interval*10000 + randint(self.adv_relay_min_random_delay,self.adv_relay_max_random_delay)
            self.adv_req_queue[target_tsf] = pkt_id

        if self.adv_ongoing_queue_count == 0:
            if bool(self.adv_req_queue):
                self.timeout = min(self.adv_req_queue.keys())
                if self.timeout <= self.env.now:
                    assert()
            else:
                self.timeout = 0
        return ""
    

    def reschedule_tx_handler(self, args):
        self.adv_req_queue[int(args[0])] = args[2]
        self.timeout = min(self.adv_req_queue.keys())
        return ""
        

    def listen_window_handler(self, args):
        listen_start = int(args[0])
        listen_end = int(args[1])
        period_end = int(args[2])
        need_id_update = 0
        original_tsf = dict()
        period = period_end - listen_start
        duty = listen_end - listen_start
        duty_cycle = (duty / period)
        adv_req_queue_copy = self.adv_req_queue.copy()
        for tsf in adv_req_queue_copy.keys():
            if tsf in range(listen_start, period_end):
                id = self.adv_req_queue.pop(tsf)
                tsf_new = listen_start + ((tsf - listen_start) * duty_cycle)
                tsf_new = int(tsf_new)
                self.adv_req_queue[tsf_new] = id
                for tsf_temp, id_temp in self.adv_req_queue.items():
                    if id_temp == tsf:
                        self.adv_req_queue[tsf_temp] = tsf_new
        
        if bool(self.adv_req_queue):
            self.timeout = min(self.adv_req_queue.keys())
        return ""

    def listen_pause_req_ack_handler(self, args):
        if args[0] == "listen_paused":
            self.timeout = self.env.now + 300
        else:
            self.timeout = self.env.now + 150
        self.adv_ongoing_queue[self.adv_ongoing_queue_count]["state"] = "FIRST_TX_SCHEDULED"
        self.adv_done_queue.append(self.adv_ongoing_queue[self.adv_ongoing_queue_count]["adv_id"])
        return ""