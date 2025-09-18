from sim_models import sim_models
import json
class wifi_xmit(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.receivers = []
        self.callback_model = None

    def xmit_req_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        self.callback_model = command["callback_model"]
        
        self.send_to = []
        if command["receiver"] in self.receivers:
            self.send_to.append(self.receivers)
        elif command["receiver"] == "broadcast":
            self.send_to = self.receivers
        
        return_message = ""
        self.tx_command = dict()
        self.tx_command["packet"] = command["packet"]
        
        for receiver in self.send_to:
            return_message += f"{receiver} tx_start {json.dumps(self.tx_command)}\n"
        self.sim_print(return_message)
        self.register_timeout(timeout = (self.env.now+int(command["duration"])), handler = "tx_complete_handler")
        return return_message

    def tx_complete_handler(self, args):
        return_message = ""
        for receiver in self.send_to:
            message = f"{receiver} tx_end {json.dumps(self.tx_command)}\n"
            return_message += message
            self.sim_print(message)
        message = f"{self.callback_model} tx_end {json.dumps(self.tx_command)}"
        return_message += message
        self.sim_print(message)
        return return_message

    def register_listener_handler(self, args):
        command_args = ""
        for arg in args:
            command_args += arg
        command = eval(command_args)
        if command["receiver"] not in self.receivers:
            self.receivers.append(command["receiver"])
        return ""
    
