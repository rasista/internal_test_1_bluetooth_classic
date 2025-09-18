class baseclass:
    def __init__(self, parent_object, env):
        self.parent_object = parent_object
        self.env = env
        self.timeout = 0

    def timeout_handler(self) :
        self.timeout = 0
        return "model_ull resume_protocol"

    def radio_request_handler(self, args):
        return "model_radio resume_radio "+str(args)

    def radio_sleep_handler(self, args):
        self.timeout = self.env.now + int(args[0])
        return "OK"


'''
    pause_request_coexmanager
    Coex-manager pause request......coex-manager requests protocol to release radio 

'''
