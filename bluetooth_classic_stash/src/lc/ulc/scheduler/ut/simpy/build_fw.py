import os
import subprocess
from subprocess import check_output
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
def start_btusched(fw_name="btc_usched_ut_simpy"):
    pid = os.popen(f'pidof {fw_name}').read().rstrip().lstrip()
    if pid:
        subprocess.Popen(["kill", "-9", str(pid)])              # Sends a "forceful termination" signal, -9 is SIGKILL (UNIX signal), it kills the process abruptly
    
    try:
        with open(LOG_FILE, "a") as log_file:
            process = subprocess.Popen([f"{BUILD_PATH}/{fw_name}"],stdout=log_file, stderr=log_file)
    except Exception as e:
        print("error: {e}")

reset_fwlogs()

# start_btusched()