from sim_models import sim_models
class test1(sim_models):
    def __init__(self, *args):
        super().__init__(*args)
        self.register_timeout(100, "timeout_handler")

    def timeout_handler(self):
        print(f"{self.env.now}")
        self.register_timeout(self.env.now + 100, "timeout_handler")
        return ""