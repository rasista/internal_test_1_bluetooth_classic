from sim_models import sim_models

class producer(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.hi_count = 0

    def send_producer_hi_handler(self, args):
        cmd = ""
        cmd += f"consumer producer_hi {self.hi_count}\n"
        self.hi_count += 1
        cmd += f"consumer producer_hi {self.hi_count}\n"
        self.hi_count += 1
        cmd += f"consumer producer_hi {self.hi_count}\n"
        self.hi_count += 1
        return cmd
    
    def consumer_hello_handler(self, args):
        self.sim_print(f"Hello from consumer {args[0]}")
        self.register_timeout(timeout = self.env.now + 5, handler = "send_producer_hi_handler")
        return ""
