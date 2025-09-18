class baseclass:
    def __init__(self, parent_object, env):
        self.env = env
        self.parent_object = parent_object
        self.avg_current = 0
        self.total_current = 0
        self.total_time = 0
    
    def add_power_activity_handler(self, args) :
        avg_current = float(args[0])
        time = int(args[1])
        self.total_current += avg_current*time
        self.total_time += time
        self.avg_current = self.total_current / self.total_time
        print("contribution = ", 1.85*avg_current*time/35000)
        print(self.env.now, "Average current = ", self.avg_current, "Total Time = ", self.total_time, "Power = ", self.avg_current*1.85)
        return "OK"
