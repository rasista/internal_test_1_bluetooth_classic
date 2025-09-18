from sim_models import sim_models
import subprocess
import os
simulation_env_path = os.getenv("SIMFRMWORK_PATH")
firmware_path = os.getenv("FW_PATH")
import sys
sys.path.append(f"{firmware_path}/models/")
sys.path.append(f"{simulation_env_path}/models")
sys.path.append(f"{simulation_env_path}/")
sys.path.append(f"{simulation_env_path}/../py_utils")
from udp_framework.udp import udp

class gdb_console(sim_models):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.channel = udp('localhost', 5005, kwargs['env'])

    
    def secondary_init_function(self, index, dict_model_params):
        self.device_index = index

    def get_fw_var_details(self, gdb_variable):
        os.environ["GDB_READ_VARIABLE"] = gdb_variable
        pid = os.popen('pidof bt_lpw.elf').read().rstrip().lstrip()
        subprocess.run(['gdb','-q','-x','py-gdb-testing/pygdb.py','--pid',f'{pid}'],
                       capture_output=True, text=True)
        with open(f"{simulation_path}/gdbOut.txt") as gdb_rd:
            env_result = eval(gdb_rd.read())
        return env_result

    def expect_fw_var_handler(self, args):
        gdb_variable = args[0][1:]
        gdb_var_value = int(args[2].replace('"', ''))
        args = "".join(gdb_variable)
        cmd_result = self.get_fw_var_details(gdb_variable)
        if int(cmd_result[gdb_variable]) != gdb_var_value:
            print(f"env_result[gdb_variable] is {cmd_result[gdb_variable]} but gdb_var_value is {gdb_var_value}")
            print("Phattt gaya ")
            self.channel.sender("system end")
            sys.exit(-1)
        return "OK"