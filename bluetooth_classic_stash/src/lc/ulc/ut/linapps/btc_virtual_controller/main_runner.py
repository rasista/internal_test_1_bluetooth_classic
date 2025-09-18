import pytest

# List your test files here (relative or absolute paths)
test_files = [
    "test_inquiry.py",
    "test_periodic_inquiry_case.py",
    "test_bt_connection_cancel_procedure.py",
    "test_read_clk_offset_procedure.py",
    "test_read_remote_extended_features_procedure.py",
    "test_read_remote_version_information_procedure.py",
    "test_name_req_procedure.py",
    "test_suported_features_procedure.py",
    "test_power_control_procedure_legacy.py",
    "test_power_control_procedure_enhanced.py",
    "test_legacy_authentication_procedure.py",
    "test_qos_lmp_procedure.py",
    "test_sniff_and_sniff_subrating_procedure.py",
    "test_ssp_pass_key_entry_authentication_procedure.py",
    "test_ssp_numaric_comparision_authentication_procedure.py",
    "test_afh_commands.py"
]

if __name__ == "__main__":
    # Run only the selected test files, Stop after first failure
    pytest.main(test_files + ["--log-cli-level=DEBUG", "--maxfail=1"])