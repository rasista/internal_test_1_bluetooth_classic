from udp import *
class baseclass:
    def __init__(self, parent_object, env):
        self.env = env
        self.parent_object = parent_object
        self.radio_state = "Unknown"
        self.channel = udp('localhost', 5005, env)
        self.timeout = 0

    def timeout_handler(self):
        self.timeout = 0
        #print("-=-=-=-=--=-=-=-=--=-=-=-=--=-=-=-=-TimeoutHandler-=-=-=-=--=-=-=-=--=-=-=-=--=-=-=-=--=-=-=-=-",__name__)
        c_response = self.channel.sender("efr_scheduler post_slot_isr")
        return c_response


    def schedule_isr_handler(self, args):
        #print("-=-=-=-=--=-=-=-=--=-=-=-=--=-=-=-=--=-=-=-=-schedule_isr_handler-=-=-=-=--=-=-=-=--=-=-=-=--=-=-=-=-",__name__, args[0])
        self.timeout = self.env.now + int(args[0])
        return "OK"
