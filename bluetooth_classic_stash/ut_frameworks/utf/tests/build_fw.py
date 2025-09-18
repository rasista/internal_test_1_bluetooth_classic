import os
import subprocess
from subprocess import check_output
LOG_FILE_D1 = "FW_Logs_d1.txt"
LOG_FILE_D2 = "FW_Logs_d2.txt"

__all__ = ["start_btc_virtual_controller" , "reset_fwlogs_d1", "reset_fwlogs_d2" ,"get_uart_ports_a" , "get_uart_ports_b"]

docker_base_path = os.getenv("WORKSPACE_PATH", "")
if 'BUILD_PATH' not in os.environ:
    build_path = os.path.join(docker_base_path, "bin/")
    os.environ["BUILD_PATH"] = os.getenv("BUILD_PATH", build_path)
    BUILD_PATH = os.getenv("BUILD_PATH")
else:
    BUILD_PATH = os.getenv("BUILD_PATH")

# Clear content of log file
def reset_fwlogs_d1():
    with open(LOG_FILE_D1, "w"):
        pass

def reset_fwlogs_d2():
    with open(LOG_FILE_D2, "w"):
        pass

# Start the specified firmware
def start_btc_virtual_controller(ports , fw_name="btc_virtual_controller"):
    import logging
    logger = logging.getLogger()
    logger.info(f"ports -- > {ports}")
    pid = os.popen(f'pidof {fw_name}').read().rstrip().lstrip()
    logger.info(f"pid_a --> {pid}")
    # if pid:
    #     subprocess.Popen(["kill", "-9", str(pid)])              # Sends a "forceful termination" signal, -9 is SIGKILL (UNIX signal), it kills the process abruptly
    
    try:
        for port in ports:
            with open(LOG_FILE_D1, "a") as log_file:
                #runtime_arg = input("specify the port number in given folsmat 'dev/pts/port_nu'")
                runtime_arg = f"/dev/pts/{port}"
                logger.info(f"runtime arg -- > {runtime_arg}")
                logger.info(f"starting fw_name -- > {fw_name}")
                process = subprocess.Popen([f"{BUILD_PATH}/{fw_name}", runtime_arg],stdout=log_file, stderr=log_file)
                logger.info(f"process -- > {process}")
            #process = subprocess.Popen([f"{BUILD_PATH}/{fw_name} "],stdout=log_file, stderr=log_file)
    except Exception as e:
        logger.error(f"error: {e}")

def get_uart_ports_a():
    ports = os.listdir("/dev/pts")
    print(f"ports --> {ports[:2]}")
    return ports[:2] 

def get_uart_ports_b():
    ports = os.listdir("/dev/pts")
    print(f"ports --> {ports[2:4]}")
    return ports[2:4]

reset_fwlogs_d1()
reset_fwlogs_d2()




