import os
simpy_path = os.getenv("SIM_PATH")

import sys
sys.path.append(f"{simpy_path}/")
sys.path.append(f"{simpy_path}/../py_utils/")
from udp_framework.udp import udp

from datetime import datetime
from random import randint
from sim_models import sim_models
    
class llc_model(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.channel = udp('localhost', 5005, kwargs['env'])
        self.role = 0
        self.max_runtime = 0
        self.random_buffer_time = 0
        self.command_queue = []
        self.current_procedure = {"task": "None", "end_time": 0}

    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index

    # If there's no current procedure, it sets the current procedure to the new one and registers a timeout. 
    # If there's already a current procedure, it adds the new one to the command queue.
    def start_llc_procedure_handler(self, args):
        print(f"{self.env.now} Inside start_llc_procedure_handler in llc_model.py")
        self.end_tsf = int(args[0])
        role = int(args[1])
        random_buffer_time = int(args[3])
        end_time = random_buffer_time + int(args[2])
        self.scheduled_activities["end_tsf"] = str(self.end_tsf)
        if self.current_procedure["task"] == "None":
            print(f"{self.env.now} inside if condition of start_llc_procedure_handler in llc_model.py")
            self.current_procedure = {"task": role, "end_time": end_time}
            self.register_timeout(timeout=random_buffer_time+end_time, 
                                  handler="procedure_complete", args=None)
        else :
            print(f"{self.env.now} inside else condition of start_llc_procedure_handler in llc_model.py")
            self.command_queue.append({"task" : role, "end_time" : end_time})
        return "OK"

    # it removes the first element from the list. All the other elements shift down by one position, 
    # so the element that was second becomes the first
    # If there's no next procedure, it sets the current procedure to "None".
    def procedure_complete(self, args):
        print(f"{self.env.now} Inside procedure_complete in llc_model.py")
        stopped_procedure = self.current_procedure["task"]
        self.deregister_timeout("procedure_complete")
        # TODO: check end time is added with random buffer time
        if self.command_queue:
            print(f"{self.env.now} inside if condition of procedure_complete in llc_model.py")
            next_procedure = self.command_queue.pop(0)
            self.current_procedure = next_procedure
            self.register_timeout(timeout=next_procedure["end_time"], 
                                  handler="procedure_complete", args=None)
        else:
            print(f"{self.env.now} inside else condition of procedure_complete in llc_model.py")
            self.current_procedure = {"task": "None", "end_time": 0}            

        return self.procedure_stopped_handler(stopped_procedure)

    #it checks if the current procedure is the one to be stopped. 
    #If it is, it completes the procedure. If it's not, it removes the procedure from the command queue.
    def stop_llc_procedure_handler(self,args):
        print(f"{self.env.now} Inside stop_llc_procedure_handler in llc_model.py, args: {args}, role: {int(args[0])}")
        print(self.current_procedure["task"])
        if not self.command_queue:
            print("command_queue is empty")
            
        role = int(args[0])
        if self.current_procedure["task"] == role:
            print("iffffffffffffffffff")
            return self.procedure_complete(None)

        elif self.command_queue:
            print("eeeeeeeeeeeeeel")
            self.command_queue = [x for x in self.command_queue if x["task"] != role]
            return self.procedure_stopped_handler(role)
        
        
        return "OK"
    
    def procedure_stopped_handler(self, args):
        print(f"{self.env.now} Inside procedure_stopped_handler in llc_model.py")
        if(args is not None):
            self.role = args
            return f"btc_llc_interface_{args + 1} procedure_stopped {args}"