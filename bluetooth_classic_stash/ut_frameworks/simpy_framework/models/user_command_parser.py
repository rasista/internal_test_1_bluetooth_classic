class baseclass:
    def __init__(self, parent_object):
        self.timeout = 1
        self.parent_object = parent_object

    def timeout_handler(self):
        print("timeout_handled")
        