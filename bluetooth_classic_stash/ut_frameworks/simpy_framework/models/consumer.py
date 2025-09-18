from sim_models import sim_models
class producer(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def producer_hi_handler(self, args):
        self.sim_print(f"Hi from producer {args[0]}")
        return f"producer consumer_hello {args[0]}"