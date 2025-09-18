class baseclass:
    def __init__(self, parent_object, env):
        self.parent_object = parent_object
        self.env = env
        self.timeout = 0
        self.add_task_handlers = dict()
        self.remove_task_handlers = dict()
        self.start_task_handlers = dict()
        self.stop_task_handlers = dict()
        self.current_active_task = "idle"

    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index
        self.add_task_handlers = dict_model_params["init_params"]["add_task_handlers"]
        self.remove_task_handlers = dict_model_params["init_params"]["remove_task_handlers"]
        self.start_task_handlers = dict_model_params["init_params"]["start_task_handlers"]
        self.stop_task_handlers = dict_model_params["init_params"]["stop_task_handlers"]
        self.task_priorities = dict_model_params["init_params"]["task_priorities"] 

    def task_handler(self, args, task_dict):
        task = args[0]
        if task in task_dict.keys():
            return task_dict[task]
        return ""

    def add_task_handler(self, args):
        return getattr(self, self.task_handler(args, self.add_task_handlers))(args)

    def add_esco_task_handler(self, args):
        
        priority = int(self.task_priorities["esco"]["start_priority"])
        return (f"model_coex_arbitrator add_activity esco { priority } { self.env.now + 50 } { self.env.now + 50 + 2500 }")

    def remove_task_handler(self, args):
        return getattr(self, self.task_handler(args, self.remove_task_handlers))(args)

    def start_task_handler(self, args):
        return getattr(self, self.task_handler(args, self.start_task_handlers))(args)
    
    def start_esco_task_handler(self, args):
        print(f"{ self.env.now } esco started")
        return ""

    def stop_task_handler(self, args):
        return getattr(self, self.task_handler(args, self.stop_task_handlers))(args)
    
    def stop_esco_task_handler(self, args):
        print(f"{ self.env.now } esco stopped")
        priority = int(self.task_priorities["esco"]["start_priority"])
        return f"model_coex_arbitrator add_activity esco { priority } { self.env.now + 5000} { self.env.now + 5000 + 2500 }"

    def next_task_handler(self, args):
        next_task = args[0]
        return_cmd = ""
        if self.current_active_task != "idle":
            return_cmd = return_cmd + self.stop_task_handler([self.current_active_task])
        if next_task != "idle":
            return_cmd = return_cmd + self.start_task_handler(args)
        self.current_active_task = next_task
        return return_cmd
