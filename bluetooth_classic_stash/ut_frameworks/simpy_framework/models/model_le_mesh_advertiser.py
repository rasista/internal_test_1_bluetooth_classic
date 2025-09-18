from random import randint
class baseclass:
    def __init__(self, parent_object,env):
        self.env = env
        self.timeout = 0
        self.parent_object = parent_object
        self.adv_req_queue = dict()
        self.adv_done_queue = list()

    def secondary_init_function(self, index, dict_model_params):
        self.adv_relay_repeat_count = int(dict_model_params['init_params']['adv_relay_repeat_count'])
        self.adv_relay_step_interval = int(dict_model_params['init_params']['adv_relay_step_interval'])
        self.adv_relay_min_random_delay = int(dict_model_params['init_params']['adv_relay_min_random_delay'])
        self.adv_relay_max_random_delay = int(dict_model_params['init_params']['adv_relay_max_random_delay'])
        self.adv_packet_duration = int(dict_model_params['init_params']['adv_packet_duration'])
        self.device_index = index
    def timeout_handler(self):
        target_key = min(self.adv_req_queue.keys())
        target_value = self.adv_req_queue.pop(target_key)
        if bool(self.adv_req_queue):
            self.timeout = min(self.adv_req_queue.keys())
        else:
            self.timeout = 0
        return f"model_medium schedule_tx {self.parent_object.model_name} {self.env.now} {self.adv_packet_duration} {target_value}"

    def adv_request_immediate_handler(self, args):
        pkt_id = self.env.now + 100000
        for i in range(1, self.adv_relay_repeat_count):
            target_tsf = pkt_id + i*self.adv_relay_step_interval*10000
            self.adv_req_queue[target_tsf] = pkt_id
        self.adv_req_queue[pkt_id] = pkt_id
        self.timeout = min(self.adv_req_queue.keys())
        if bool(self.adv_req_queue):
            self.timeout = min(self.adv_req_queue.keys())
        else:
            self.timeout = 0
        return ""
    
    def adv_request_handler(self, args):
        if args[0] not in self.adv_done_queue:
            self.adv_done_queue.append(args[0])
            for i in range(0, self.adv_relay_repeat_count):
                self.adv_req_queue[self.env.now + randint(self.adv_relay_min_random_delay,self.adv_relay_max_random_delay) + i*self.adv_relay_step_interval*10000] = args[0]
        if bool(self.adv_req_queue):
            self.timeout = min(self.adv_req_queue.keys())
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
                #print(f"{self.env.now} {self.device_index} AdjustTX {listen_start} {listen_end} {original_tsf} {tsf}")
                self.adv_req_queue[tsf_new] = id
                for tsf_temp, id_temp in self.adv_req_queue.items():
                    if id_temp == tsf:
                        self.adv_req_queue[tsf_temp] = tsf_new
        
        if bool(self.adv_req_queue):
            self.timeout = min(self.adv_req_queue.keys())
        return ""