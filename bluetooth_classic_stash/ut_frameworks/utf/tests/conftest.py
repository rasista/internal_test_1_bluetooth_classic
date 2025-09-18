import pytest
import os
import subprocess
import logging
import random
import time
logger = logging.getLogger(__name__)

# Global variables
debug = False
test_report = False

# Function to export environment variables

def exportEnvVars():
    global docker_base_path, SIM_PATH, BUILD_PATH
    
    docker_base_path = os.getenv("WORKSPACE_PATH", "")
    if "SIM_PATH" not in os.environ:
        sim_path = os.path.join(docker_base_path, "ut_frameworks/simpy_framework/")
        os.environ["SIM_PATH"] = os.getenv("SIM_PATH", sim_path)
        SIM_PATH = os.getenv("SIM_PATH")
    else:
        SIM_PATH = os.getenv("SIM_PATH")

    if 'BUILD_PATH' not in os.environ:
        build_path = os.path.join(docker_base_path, "bin/")
        os.environ["BUILD_PATH"] = os.getenv("BUILD_PATH", build_path)
        BUILD_PATH = os.getenv("BUILD_PATH")
    else:
        BUILD_PATH = os.getenv("BUILD_PATH") 
    
# Function to kill the firmware
def killFw(fw_name="btc_virtual_controller"):
    global debug
    if not debug:
        pid_fw = os.popen(f'pidof {fw_name}').read().rstrip().lstrip()
        if pid_fw:
            for i in pid_fw.split():
                subprocess.Popen(["kill", "-2", str(i)])              # Sends a "forceful termination" signal, -9 is SIGKILL (UNIX signal), it kills the process abruptly

# Function to kill Socat Ports
def killSocats():
    global debug
    if not debug:
        pid_socats = os.popen(f'pidof socat').read().rstrip().lstrip()
        if pid_socats:
            for i in pid_socats.split():
                subprocess.Popen(["kill", "-2", str(i)])              # Sends a "forceful termination" signal, -9 is SIGKILL (UNIX signal), it kills the process abruptly
        subprocess.run(["rm", "-rf", f"{BUILD_PATH}/D*"])

# Function to generate Socat Ports
def generateSocats():
    global debug
    if not os.path.isdir(f"{BUILD_PATH}"):
        subprocess.run(["sh","configure.sh"])
    if not debug:
        subprocess.Popen(["socat", "-d", "-d", f"pty,b115200,raw,echo=0,link={BUILD_PATH}/D1_UARTpy", f"pty,b115200,raw,echo=0,link={BUILD_PATH}/D1_UARTfw"])
        subprocess.Popen(["socat", "-d", "-d", f"pty,b115200,raw,echo=0,link={BUILD_PATH}/D2_UARTpy", f"pty,b115200,raw,echo=0,link={BUILD_PATH}/D2_UARTfw"])

# Function to kill the setup
def killSetup():
    killFw()
    killSocats()
    
def pytest_addoption(parser):
    parser.addoption('--debugs', action='store_true', default=False, help='Debugging')
    parser.addoption('--test_report', action='store_true', default=False, help='Test Report')

@pytest.fixture
def debugs(request):    
    return request.config.getoption('debugs')
    
# Initialize the testing session
@pytest.hookimpl
def pytest_sessionstart(session):
    global debug
    exportEnvVars()
    generateSocats()
    if not test_report:
        subprocess.run(["find", docker_base_path, "-name", "*.gcda", "-type", "f", "-delete"])
        subprocess.run(["find", docker_base_path, "-name", "*.gcno", "-type", "f", "-delete"])
        if not debug:
            subprocess.run(["make", "clean"], cwd=BUILD_PATH)
            try:
                result = subprocess.run(["make", "-j"], cwd=BUILD_PATH)
                logger.info(f"Make command completed with exit code: {result.returncode}")
                if (result.returncode != 0):
                    logger.error(f"Make command failed with exit code: {result.returncode}")
                    raise Exception("Make command failed")
            except subprocess.CalledProcessError as e:
                logger.error(f"Make command failed with exit code: {e.returncode}")
                # Handle the error as needed, for example, by raising an exception
                exit(0)
    
# Called after command line options have been parsed and all plugins and initial configuration are in place
def pytest_configure(config):
    global test_report
    global debug
    debug = config.getoption('debugs')
    test_report = config.getoption('test_report')
       
# Fixture that runs before each test function
@pytest.fixture(autouse=True)
def run_before_tests():
    killSetup()
    generateSocats()
    global BUILD_PATH
    if test_report:
        subprocess.run(["find", docker_base_path, "-name", "*.gcda", "-type", "f", "-delete"])
        subprocess.run(["find", docker_base_path, "-name", "*.gcno", "-type", "f", "-delete"])
        subprocess.run(["make", "clean"], cwd=BUILD_PATH)
        try:
            result = subprocess.run(["make", "-j"], cwd=BUILD_PATH)
            logger.info(f"Make command completed with exit code: {result.returncode}")
            if (result.returncode != 0):
                logger.error(f"Make command failed with exit code: {result.returncode}")
                raise Exception("Make command failed")
        except subprocess.CalledProcessError as e:
            logger.error(f"Make command failed with exit code: {e.returncode}")
            # Handle the error as needed, for example, by raising an exception
            exit(0)
    time.sleep(random.randint(1, 3))
    yield  # A test function will be run at this point

'''
    Fixtures to run after tests
'''
@pytest.hookimpl
def pytest_runtest_makereport(item, call):
    if call.when == 'call':
        if call.excinfo:
            logger.error(f"Error in test: {item.name}")
            logger.error(call.excinfo.value)
    elif call.when == 'teardown':
        if test_report:
            logger.info(f"Teardown after test: {item.name} - {call.when} - {call.excinfo} - Running Gcov")
            killSetup()
            subprocess.run(["bash", f"{docker_base_path}/tools/run_gcov.sh", f"{item.name}"])

'''
    Fixtures to run after all tests
'''
@pytest.hookimpl
def pytest_sessionfinish(session, exitstatus):
    logger.info("Test session finished")
    killSetup()
    if not test_report:
        subprocess.run(["bash",f"{docker_base_path}/tools/run_gcov.sh","allTests"])
