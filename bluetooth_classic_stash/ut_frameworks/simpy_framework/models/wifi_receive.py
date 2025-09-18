from sim_models import sim_models
import json
import random

MARKOV_STATE_GOOD = 1
MARKOV_STATE_BAD = 0

class wifi_receive(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.receive_on = 0
        self.listen_on = 0
        self.listen_window = 0
        self.receive_tsf = 0
        self.callback_model = None
        self.p_good_to_bad = 0
        self.p_bad_to_good = 0
        self.per_in_good = 0
        self.per_in_bad = 0
        self.current_markov_state = MARKOV_STATE_GOOD
        self.listen_start_tsf = 0
        self.listen_slice = dict()
        self.total_listen_time = 0
    def receive_req_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        for key in command.keys():
            setattr(self, key, command[key])
        self.register_timeout(timeout = self.env.now + self.listen_window, handler = "listen_end_handler", args = self.callback_model)
        self.listen_on = 1
        self.listen_start_tsf = self.env.now
        self.sim_print("listen_start")
        return ""
    
    def listen_end_handler(self, args):
        self.listen_on = 0
        self.receive_on = 0
        self.sim_print("listen_end")
        listen_duration = self.env.now - self.listen_start_tsf
        listen_slice_index = listen_duration//10000
        if listen_slice_index in self.listen_slice.keys():
            self.listen_slice[listen_slice_index] += 1
        else:
            self.listen_slice[listen_slice_index] = 1
        self.deregister_timeout("listen_end_handler")
        self.total_listen_time += listen_duration
        self.sim_print(f"listen_slices: {json.dumps(self.listen_slice)}")
        return f"{self.callback_model} listen_end"

    def tx_start_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        if self.listen_on:
            self.sim_print(f"tx_start {json.dumps(command)}")
            self.receive_on = 1
            self.receive_tsf = self.env.now
        return ""
    
    def markov_parameters_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        self.p_good_to_bad = float(command["p_good_to_bad"])
        self.p_bad_to_good = float(command["p_bad_to_good"])
        self.per_in_good = float(command["per_in_good"])
        self.per_in_bad = float(command["per_in_bad"])
        return ""

    def markov_receive_model(self):
        per = 0
        if self.current_markov_state == MARKOV_STATE_GOOD:
            per = self.per_in_good
            self.current_markov_state = random.choices([MARKOV_STATE_GOOD, MARKOV_STATE_BAD], [1 - self.p_good_to_bad, self.p_good_to_bad])[0]
        else:
            per = self.per_in_bad
            self.current_markov_state = random.choices([MARKOV_STATE_GOOD, MARKOV_STATE_BAD], [self.p_bad_to_good, 1 - self.p_bad_to_good])[0]
        self.sim_print(f"markov_state {self.current_markov_state}")
        packet_received = random.choices([1, 0], [(1- per), per])[0]
        return packet_received

    def tx_end_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        command["receive_tsf"] = self.receive_tsf
        return_msg = ""
        if self.receive_on:
            self.sim_print(f"tx_end {json.dumps(command)}")
            if self.markov_receive_model() == 1:
                self.sim_print("packet_received")
                command["packet_status"] = "received"
                return_msg = f"{self.callback_model} rx_done {json.dumps(command)}"
            else :
                self.sim_print("packet_dropped")
                command["packet_status"] = "dropped"
                return_msg = f"{self.callback_model} rx_done {json.dumps(command)}"
            self.receive_on = 1
        return return_msg