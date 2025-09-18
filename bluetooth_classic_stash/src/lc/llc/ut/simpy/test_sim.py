import sys
import os
import subprocess
import pytest
import glob
from pathlib import Path
from build_fw import start_btlpw
from sim import *
import pdb

SIM_PATH = os.getenv("SIM_PATH")
BUILD_PATH = os.getenv("BUILD_PATH")

sys.path.append(SIM_PATH)
sys.path.append(BUILD_PATH)
yaml_file = f"testcase_configs/s3_btc_dev.yaml"

def test_page():
  testcase_file = f"test_cases/page.txt"
  sim_time = 200000000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)

def test_page_scan():
  testcase_file = f"test_cases/page_scan.txt"
  sim_time = 400000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)

def test_inquiry():
  testcase_file = f"test_cases/inquiry.txt"
  sim_time = 7000000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
    
def test_inquiry_scan():
  testcase_file = f"test_cases/inquiry_scan.txt"
  sim_time = 3000000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)

def test_esco_central():
  testcase_file = f"test_cases/esco_central.txt"
  sim_time = 50000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)

def test_esco_peripheral():
  testcase_file = f"test_cases/esco_peripheral.txt"
  sim_time = 50000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)

# def test_roleswitch_central():
#   testcase_file = f"test_cases/roleswitch_central.txt"
#   sim_time = 60000
#   output_file = None
#   start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
# def test_roleswitch_central_failure_stage1():
#   testcase_file = f"test_cases/roleswitch_central_failure_stage1.txt"
#   sim_time = 60000
#   output_file = None
#   start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
# def test_roleswitch_central_failure_stage2():
#   testcase_file = f"test_cases/roleswitch_central_failure_stage2.txt"
#   sim_time = 60000
#   output_file = None
#   start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
# def test_roleswitch_peripheral():
#   testcase_file = f"test_cases/roleswitch_peripheral.txt"
#   sim_time = 60000
#   output_file = None
#   start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
# def test_roleswitch_peripheral_failure_stage1():
#   testcase_file = f"test_cases/roleswitch_peripheral_failure_stage1.txt"
#   sim_time = 60000
#   output_file = None
#   start_simulation(yaml_file, testcase_file, sim_time, output_file)

# def test_roleswitch_peripheral_failure_stage2():
#   testcase_file = f"test_cases/roleswitch_peripheral_failure_stage2.txt"
#   sim_time = 60000
#   output_file = None
#   start_simulation(yaml_file, testcase_file, sim_time, output_file)

def test_afh():
  testcase_file = f"test_cases/afh_instant_after_tx.txt"
  sim_time = 20000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
    
def test_sniff_central():
  testcase_file = f"test_cases/sniff_central.txt"
  sim_time = 50000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)

def test_sniff_peripheral():
  testcase_file = f"test_cases/sniff_peripheral.txt"
  sim_time = 50000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)

def test_page_scan_updated():
  testcase_file = f"test_cases/page_scan_with_2_fhs_rx.txt"
  sim_time = 200000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)


def test_acl_central():
  testcase_file = f"test_cases/acl_central.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_acl_peripheral():
  testcase_file = f"test_cases/acl_peripheral.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_scheduler_tx_puncture():
  testcase_file = f"test_cases/scheduler_aperiodic_periodic_tx_puncture.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_scheduler_periodic_aperiodic():
  testcase_file = f"test_cases/scheduler_periodic_aperiodic.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
   
def test_scheduler_rx_puncture():
  testcase_file = f"test_cases/scheduler_aperiodic_periodic_rx_puncture.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_scheduler_puncture_tx_middle_slot():
  testcase_file = f"test_cases/scheduler_aperiodic_puncture_immediate_tx.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_scheduler_puncture_rx_middle_slot():
  testcase_file = f"test_cases/scheduler_aperiodic_puncture_immediate_rx.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_scheduler_stop_aperiodic():
  testcase_file = f"test_cases/scheduler_stop_aperiodic.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_scheduler_stop_periodic_role():
  testcase_file = f"test_cases/scheduler_stop_periodic.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_scheduler_stop_periodic_remove_from_queue():
  testcase_file = f"test_cases/scheduler_periodic_remove_role_queue.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_scheduler_periodic_together():
  testcase_file = f"test_cases/scheduler_periodic_together.txt"
  sim_time = 60000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_scheduler_periodics_case():
  testcase_file = f"test_cases/scheduler_periodics_roles.txt"
  sim_time = 600000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_crc_hec_status_check_acl_central():
  testcase_file = f"test_cases/crc_hec_status_acl_central.txt"
  sim_time = 30000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_crc_hec_status_check_acl_peripheral():
  testcase_file = f"test_cases/crc_hec_status_acl_peripheral.txt"
  sim_time = 30000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_crc_hec_status_check_esco_central():
  testcase_file = f"test_cases/crc_hec_status_fail_esco_central.txt"
  sim_time = 30000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
def test_crc_hec_status_check_esco_peripheral():
  testcase_file = f"test_cases/crc_hec_status_fail_esco_peripheral.txt"
  sim_time = 30000
  output_file = None
  start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
# def test_crc_hec_status_check_rs_central():
#   testcase_file = f"test_cases/crc_hec_status_rs_central.txt"
#   sim_time = 30000
#   output_file = None
#   start_simulation(yaml_file, testcase_file, sim_time, output_file)
  
# def test_crc_hec_status_check_rs_peripheral():
#   testcase_file = f"test_cases/crc_hec_status_rs_peripheral.txt"
#   sim_time = 40000
#   output_file = None
#   start_simulation(yaml_file, testcase_file, sim_time, output_file)
