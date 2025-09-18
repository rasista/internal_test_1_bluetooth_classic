import sys
import os
import subprocess
import pytest
import glob
from pathlib import Path
from build_fw import start_btusched
from sim import *

SIM_PATH = os.getenv("SIM_PATH")
BUILD_PATH = os.getenv("BUILD_PATH")

sys.path.append(SIM_PATH)
sys.path.append(BUILD_PATH)

# def test_non_connected():
#     testcase_file = f"test_cases/non_connected.txt"
#     sim_time = 2500000
#     output_file = None
#     start_simulation(None, testcase_file, sim_time, output_file)

# def test_non_connected():
#     testcase_file = f"test_cases/non_connected_scans.txt"
#     sim_time = 2500000
#     output_file = None
#     start_simulation(None, testcase_file, sim_time, output_file)

def test_connected():
    testcase_file = f"test_cases/connected.txt"
    sim_time = 2500000
    output_file = None
    start_simulation(None, testcase_file, sim_time, output_file)

# def test_inquiry_scan():
#     testcase_file = f"test_cases/inquiry_scan.txt"
#     sim_time = 1000000
#     output_file = None
#     start_simulation(None, testcase_file, sim_time, output_file)

# def test_page():
#     testcase_file = f"test_cases/page.txt"
#     sim_time = 1000000
#     output_file = None
#     start_simulation(None, testcase_file, sim_time, output_file)

# def test_page_scan():
#     testcase_file = f"test_cases/page_scan.txt"
#     sim_time = 1000000
#     output_file = None
#     start_simulation(None, testcase_file, sim_time, output_file)s