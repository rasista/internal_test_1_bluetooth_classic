import pytest
import os
import sys
import glob
import subprocess
from _pytest.terminal import TerminalReporter
from _pytest._code.code import ReprTraceback, ReprEntry, ReprFileLocation, Traceback
from _pytest.runner import TestReport
from pathlib import Path
from build_fw import start_btusched
error_dict = {}
stdout_file = open("pytest_stdout.log", "w")            #file pointer for stdout

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
    
# Initialize the testing session
@pytest.hookimpl
def pytest_sessionstart(session):
    global BUILD_PATH
    global SIM_PATH
    #print(os.environ)
    sys.path.append(SIM_PATH)
    sys.path.append(BUILD_PATH)
    #print(f"SIM_PATH : {SIM_PATH}")
    subprocess.run(["find", docker_base_path, "-name", "*.gcda", "-type", "f", "-delete"])
    subprocess.run(["find", docker_base_path, "-name", "*.gcno", "-type", "f", "-delete"])
    # subprocess.run(["make", "clean"], cwd=BUILD_PATH)
    # subprocess.run(["make", "sim_test=true"], cwd=BUILD_PATH)

# Capture stdout and print it 
@pytest.hookimpl
def pytest_report_teststatus(report, config):         
    global stdout_file
    if report.outcome != 'passed' and report.when == 'call':
        print(report.capstdout, file=stdout_file)
        pass
    if report.outcome == 'passed' and report.when == 'call':
        print(report.capstdout, file=stdout_file)
        error_dict[report.nodeid] = [report.outcome]
        pass
    return

# Store error string and run 
@pytest.hookimpl(tryfirst=True)                         
def pytest_runtest_makereport(item, call):
    global BUILD_PATH
    global error_dict, stdout_file
    if call.when == 'call':                             
        if call.excinfo:
            error_dict[item.name] = call.excinfo.value
    
    # Prints after test file ends
    elif call.when == 'teardown':                             
        if error_dict:
            print(error_dict)
        
        error_dict = {}

# Creating gcov after the test file ends
@pytest.fixture(scope="module", autouse=True)
def teardown():
    yield
    subprocess.run(["bash", f"{docker_base_path}/tools/run_gcov.sh"])

# # Starting the firmware before every testcase file
# @pytest.fixture(autouse=True)
# def run_before_tests():
#     # Code that will run before every test
#     start_btusched()

#     yield  # A test function will be run at this point

# Skips specified files so that test coverage is skipped for unnecessary files
def skip(path, pattern):
    return f"{pattern}" in str(path)
