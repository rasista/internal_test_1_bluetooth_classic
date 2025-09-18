from sim_models import sim_models
import json

class beacon_receive(sim_models):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.next_beacon = 0
        self.per = 0
        self.tbtt = 0
        self.skip_count = 0
        self.min_listen_window = 0
        self.max_listen_window = 0
        self.listen_window_step = 0
        self.pre_wakeup = 0
        self.listen_window = 0
        self.total_beacons = 0
        self.received_beacons = 0
        self.dropped_beacons = 0

    def configure_listen_window_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        for key in command.keys():
            setattr(self, key, command[key])
        self.listen_window = self.min_listen_window
        return ""

    def sync_beacon_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        for key in command.keys():
            setattr(self, key, command[key])
        self.register_timeout(timeout = (self.next_beacon - self.pre_wakeup), handler = "receive_window_start_handler")
        return ""
    
    def receive_window_start_handler(self, args):
        listen_req = dict()
        listen_req["listen_window"] = self.listen_window
        listen_req["callback_model"] = self.model_name
        return f"wifi_receive_sta receive_req {json.dumps(listen_req)}"

    def listen_end_handler(self, args):
        self.next_beacon = self.next_beacon + (self.tbtt*(self.skip_count + 1))
        self.register_timeout(timeout = (self.next_beacon - self.pre_wakeup), handler = "receive_window_start_handler")
        return ""

    def rx_done_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        return_cmd = ""
        if command["packet"] == "beacon": 
            self.next_beacon = int(command["receive_tsf"])
            if command["packet_status"] == "received":
                self.sim_print(f"rx_done {json.dumps(command)}")
                self.listen_window = self.min_listen_window
                self.received_beacons += 1
                self.total_beacons += 1
                return_cmd = "wifi_receive_sta listen_end"
            else :
                self.listen_window += self.listen_window_step
                if self.listen_window > self.max_listen_window:
                    self.listen_window = 4000000
                self.dropped_beacons += 1
                self.total_beacons += 1
        self.sim_print(f"Total Beacons: {self.total_beacons}, Received Beacons: {self.received_beacons}, Pass Probability: {self.received_beacons/self.total_beacons}")
        return return_cmd