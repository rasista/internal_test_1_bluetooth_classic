#include "generic_command_handling.h"




void restart_cmd_handler(int argc, char *argv[]);
void update_sys_tick_handler(int argc, char *argv[]);
void end_cmd_handler(int argc, char *argv[]);
command_lut_entry_t system_commands_lut[] =
{
	{.command_name = "restart", .handler = restart_cmd_handler},
	{.command_name = "update_sys_tick", .handler = update_sys_tick_handler},
	{.command_name = "end", .handler = end_cmd_handler},
	{.command_name = NULL, .handler = NULL}
};


module_lut_entry_t system_module_lut[] =
{
	{.module_name = NULL, .sub_modules = NULL, .commands = NULL, .custom_handler = NULL }
};
void tx_end_handler(int argc, char *argv[]);
void rx_done_handler(int argc, char *argv[]);
void rx_timeout_handler(int argc, char *argv[]);
void rx_abort_handler(int argc, char *argv[]);
void backoff_done_handler(int argc, char *argv[]);
void tx_abort_handler(int argc, char *argv[]);
void var_status_handler(int argc, char *argv[]);
void rx_header_received_handler(int argc, char *argv[]);
void set_payload_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_init_btc_device_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_page_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_page_scan_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_inquiry_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_inquiry_scan_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_esco_central_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_esco_peripheral_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_roleswitch_central_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_roleswitch_peripheral_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_acl_central_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_acl_peripheral_handler(int argc, char *argv[]);
void hss_cmd_payload_start_procedure_payload_test_mode_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_page_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_inquiry_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_page_scan_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_inquiry_scan_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_afh_parameters_acl_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_sniff_parameters_acl_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_device_parameters_acl_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_device_parameters_esco_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch_handler(int argc, char *argv[]);
void hss_cmd_payload_set_procedure_payload_roleswitch_parameters_handler(int argc, char *argv[]);
void hss_cmd_payload_stop_procedure_payload_acl_central_handler(int argc, char *argv[]);
void hss_cmd_payload_stop_procedure_payload_esco_central_handler(int argc, char *argv[]);
void hss_cmd_payload_stop_procedure_payload_roleswitch_central_handler(int argc, char *argv[]);
void hss_cmd_payload_stop_procedure_payload_acl_peripheral_handler(int argc, char *argv[]);
void hss_cmd_payload_stop_procedure_payload_esco_peripheral_handler(int argc, char *argv[]);
void hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral_handler(int argc, char *argv[]);
command_lut_entry_t lpw_commands_lut[] =
{
	{.command_name = "tx_end", .handler = tx_end_handler},
	{.command_name = "rx_done", .handler = rx_done_handler},
	{.command_name = "rx_timeout", .handler = rx_timeout_handler},
	{.command_name = "rx_abort", .handler = rx_abort_handler},
	{.command_name = "backoff", .handler = backoff_done_handler},
	{.command_name = "tx_abort", .handler = tx_abort_handler},
	{.command_name = "var_status", .handler = var_status_handler},
	{.command_name = "rx_header_received", .handler = rx_header_received_handler},
	{.command_name = "set_payload", .handler = set_payload_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_init_btc_device", .handler = hss_cmd_payload_start_procedure_payload_init_btc_device_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_page", .handler = hss_cmd_payload_start_procedure_payload_page_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_page_scan", .handler = hss_cmd_payload_start_procedure_payload_page_scan_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_inquiry", .handler = hss_cmd_payload_start_procedure_payload_inquiry_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_inquiry_scan", .handler = hss_cmd_payload_start_procedure_payload_inquiry_scan_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_esco_central", .handler = hss_cmd_payload_start_procedure_payload_esco_central_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_esco_peripheral", .handler = hss_cmd_payload_start_procedure_payload_esco_peripheral_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_roleswitch_central", .handler = hss_cmd_payload_start_procedure_payload_roleswitch_central_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_roleswitch_peripheral", .handler = hss_cmd_payload_start_procedure_payload_roleswitch_peripheral_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_acl_central", .handler = hss_cmd_payload_start_procedure_payload_acl_central_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_acl_peripheral", .handler = hss_cmd_payload_start_procedure_payload_acl_peripheral_handler},
	{.command_name = "hss_cmd_payload_start_procedure_payload_test_mode", .handler = hss_cmd_payload_start_procedure_payload_test_mode_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_page", .handler = hss_cmd_payload_set_procedure_payload_page_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_inquiry", .handler = hss_cmd_payload_set_procedure_payload_inquiry_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_page_scan", .handler = hss_cmd_payload_set_procedure_payload_page_scan_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_inquiry_scan", .handler = hss_cmd_payload_set_procedure_payload_inquiry_scan_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_afh_parameters_acl", .handler = hss_cmd_payload_set_procedure_payload_afh_parameters_acl_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_sniff_parameters_acl", .handler = hss_cmd_payload_set_procedure_payload_sniff_parameters_acl_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_device_parameters_acl", .handler = hss_cmd_payload_set_procedure_payload_device_parameters_acl_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_device_parameters_esco", .handler = hss_cmd_payload_set_procedure_payload_device_parameters_esco_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch", .handler = hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch_handler},
	{.command_name = "hss_cmd_payload_set_procedure_payload_roleswitch_parameters", .handler = hss_cmd_payload_set_procedure_payload_roleswitch_parameters_handler},
	{.command_name = "hss_cmd_payload_stop_procedure_payload_acl_central", .handler = hss_cmd_payload_stop_procedure_payload_acl_central_handler},
	{.command_name = "hss_cmd_payload_stop_procedure_payload_esco_central", .handler = hss_cmd_payload_stop_procedure_payload_esco_central_handler},
	{.command_name = "hss_cmd_payload_stop_procedure_payload_roleswitch_central", .handler = hss_cmd_payload_stop_procedure_payload_roleswitch_central_handler},
	{.command_name = "hss_cmd_payload_stop_procedure_payload_acl_peripheral", .handler = hss_cmd_payload_stop_procedure_payload_acl_peripheral_handler},
	{.command_name = "hss_cmd_payload_stop_procedure_payload_esco_peripheral", .handler = hss_cmd_payload_stop_procedure_payload_esco_peripheral_handler},
	{.command_name = "hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral", .handler = hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral_handler},
	{.command_name = NULL, .handler = NULL}
};


module_lut_entry_t lpw_module_lut[] =
{
	{.module_name = NULL, .sub_modules = NULL, .commands = NULL, .custom_handler = NULL }
};
command_lut_entry_t root_commands_lut[] =
{
	{.command_name = NULL, .handler = NULL}
};

extern module_lut_entry_t system_module_lut[];
extern command_lut_entry_t system_commands_lut[];
extern module_lut_entry_t lpw_module_lut[];
extern command_lut_entry_t lpw_commands_lut[];

module_lut_entry_t root_module_lut[] =
{
	{.module_name = "system", .sub_modules = system_module_lut, .commands = system_commands_lut, .custom_handler = NULL },
	{.module_name = "lpw", .sub_modules = lpw_module_lut, .commands = lpw_commands_lut, .custom_handler = NULL },
	{.module_name = NULL, .sub_modules = NULL, .commands = NULL, .custom_handler = NULL }
};
