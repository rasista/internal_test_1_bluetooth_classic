#ifndef SL_HOST_TO_LPW_INTF_H_
#define SL_HOST_TO_LPW_INTF_H_

#include "sl_btc_seq_interface.h"
#include "stdint.h"
#include "stddef.h"

#define PKB_DATA_OFFSET sizeof(pkb_t)

enum hss_cmd_start_procedure_payload_e {
  hss_cmd_init_btc_device,
  hss_cmd_start_page,
  hss_cmd_start_page_scan,
  hss_cmd_start_inquiry,
  hss_cmd_start_inquiry_scan,
  hss_cmd_start_acl_central,
  hss_cmd_start_acl_peripheral,
  hss_cmd_start_esco_central,
  hss_cmd_start_esco_peripheral,
  hss_cmd_start_roleswitch_central,
  hss_cmd_start_roleswitch_peripheral,
  hss_cmd_start_test_mode,
  max_hss_cmd_lpw_procedures
};
enum hss_cmd_set_configs_e {
  hss_cmd_set_page,
  hss_cmd_set_page_scan,
  hss_cmd_set_inquiry,
  hss_cmd_set_inquiry_scan,
  hss_cmd_set_afh_parameters_acl,
  hss_cmd_set_sniff_parameters_acl,
  hss_cmd_set_device_parameters_acl,
  hss_cmd_set_device_parameters_esco,
  hss_cmd_set_device_parameters_roleswitch,
  hss_cmd_set_roleswitch_parameters,
  hss_cmd_set_encryption,
  hss_cmd_set_fixed_btc_channel,
  hss_cmd_set_test_mode_params,
  max_hss_cmd_set_procedures
};
enum hss_cmd_stop_procedure_payload_e {
  hss_cmd_stop_page,
  hss_cmd_stop_inquiry,
  hss_cmd_stop_page_scan,
  hss_cmd_stop_inquiry_scan,
  hss_cmd_stop_acl_central,
  hss_cmd_stop_roleswitch_central,
  hss_cmd_stop_acl_peripheral,
  hss_cmd_stop_roleswitch_peripheral,
  max_hss_cmd_stop_procedures_aperiodic,
  hss_cmd_stop_esco_central,
  hss_cmd_stop_esco_peripheral,
  max_hss_cmd_stop_procedures_periodic
};
enum hss_cmd_get_configs_e { 
  hss_cmd_get_roleswitch_parameter_slot_offset,
  max_hss_cmd_get_procedures
 };
enum hss_cmd_payload_e {
  hss_cmd_start_procedure,
  hss_cmd_stop_procedure,
  hss_cmd_set_procedure,
  hss_cmd_get_procedure,
  max_hss_cmd_types
};

#endif
