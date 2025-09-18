from udp import *
class baseclass:
    def __init__(self, parent_object, env):
        self.env = env
        self.parent_object = parent_object
        self.radio_state = "Unknown"
        self.channel = udp('localhost', 5005, env)
        self.timeout = 625 

    def timeout_handler(self):
        self.timeout = self.env.now + 625
        if self.radio_state != "Radio On":
            return "OK"
        return self.channel.sender("efr_scheduler pre_slot_isr")

    def radio_on_handler(self, args):
        self.radio_state = "Radio On"
        return "OK"

    def radio_off_handler(self, args):
        self.radio_state = "Radio Off"
        return "OK"
        