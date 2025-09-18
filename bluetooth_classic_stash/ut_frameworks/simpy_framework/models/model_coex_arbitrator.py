class baseclass:
    def __init__(self, parent_object, env):
        self.parent_object = parent_object
        self.env = env
        self.timeout = 0
        self.arbitration_list = dict()
        self.current_activity = "idle"
        self.arbitration_list["idle"] = {"priority" : 0, "start" : 0, "end" : 0}
    
    def add_activity_handler(self, args):
        activity = args[0]
        priority = args[1]
        start = args[2]
        end = args[3]

        self.arbitration_list[activity] = dict()
        self.arbitration_list[activity]["priority"] = int(priority)
        self.arbitration_list[activity]["start"] = int(start)
        self.arbitration_list[activity]["end"] = int(end)
        return self.timeout_handler()
    
    def remove_activity_handler(self, args):
        activity = args[0]
        if activity in self.arbitration_list.keys():
            self.arbitration_list.pop(activity)
        if self.current_activity == activity:
            self.current_activity = "idle"
        return self.timeout_handler()

    def timeout_handler(self):
        if len(self.arbitration_list.keys()) == 0:
            self.current_activity = "idle"
            self.timeout = 0
            return ""
        self.timeout = self.env.now + 1
        next_activity = self.current_activity
        for activity, activity_details in self.arbitration_list.items():
            if (activity_details["start"] <= self.env.now):
                if ((activity_details["end"] >= self.env.now) | (activity_details["end"] == 0)):
                    if ((self.arbitration_list[next_activity]["end"] <= self.env.now)):
                        if (self.arbitration_list[next_activity]["end"] != 0):
                            next_activity = activity
                    if (self.arbitration_list[next_activity]["priority"] < activity_details["priority"]):
                        next_activity = activity
        if ((self.arbitration_list[next_activity]["end"] <= self.env.now)):
            if (self.arbitration_list[next_activity]["end"] != 0):
                next_activity = "idle"
        if next_activity != self.current_activity:
            self.current_activity = next_activity
            return f"model_soft_dmp next_task { self.current_activity }"
        return ""