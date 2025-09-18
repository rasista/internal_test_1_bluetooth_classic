from sim_models import sim_models
import json

class beacon_transmit(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.tbtt = 100000
        self.duration = 350

    def configure_beacon_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        self.tbtt = int(command["tbtt"])
        self.duration = command["duration"]
        self.next_tbtt = ((self.env.now // self.tbtt)*self.tbtt) + self.tbtt
        self.deregister_timeout("beacon_tx_handler")
        self.register_timeout(timeout = self.next_tbtt,  handler = "beacon_tx_handler")
        return ""

    def beacon_tx_handler(self, args):
        xmit_req = dict()
        xmit_req["receiver"] = "broadcast"
        xmit_req["packet"] = "beacon"
        xmit_req["duration"] = self.duration
        xmit_req["callback_model"] = self.model_name
        return f"wifi_xmit_ap xmit_req {json.dumps(xmit_req)}"

    def tx_end_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        if command["packet"] == "beacon":
            self.next_tbtt = self.next_tbtt + self.tbtt
            self.deregister_timeout("beacon_tx_handler")
            self.register_timeout(timeout = self.next_tbtt,  handler = "beacon_tx_handler")
        return ""
