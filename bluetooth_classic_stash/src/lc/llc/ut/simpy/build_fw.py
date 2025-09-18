import os
import subprocess
from subprocess import check_output
import logging
LOG_FILE = "FW_Logs.txt"

docker_base_path = os.getenv("WORKSPACE_PATH", "")
if 'BUILD_PATH' not in os.environ:
    build_path = os.path.join(docker_base_path, "bin/")
    os.environ["BUILD_PATH"] = os.getenv("BUILD_PATH", build_path)
    BUILD_PATH = os.getenv("BUILD_PATH")
else:
    BUILD_PATH = os.getenv("BUILD_PATH")

# Clear content of log file
def reset_fwlogs():
    with open(LOG_FILE, "w"):
        pass

# Start the specified firmware
def start_btlpw(fw_name="btc_llc_ut_simpy"):
    #checking for pid of the fw process
    pid = os.popen(f'pidof {fw_name}').read().rstrip().lstrip()
    logger = logging.getLogger()
    
    #if process present kill all the instances
    if pid:
        try:
            for i in pid.split():
                subprocess.Popen(["kill", "-2", str(i)])              # Sends a "forceful termination" signal, -2 is SIGINT (UNIX signal), it is equivalent to pressing Ctrl+C
        except:
            logger.info(f"Couldn't Kill {pid}")
    
    try:
        with open(LOG_FILE, "a") as log_file:
            process = subprocess.Popen([f"{BUILD_PATH}/{fw_name}"],stdout=log_file, stderr=log_file)
    except Exception as e:
        logger.info("error: {e}")

reset_fwlogs()