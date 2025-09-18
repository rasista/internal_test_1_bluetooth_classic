#ifndef USCHED_ROLE_H
#define USCHED_ROLE_H

#include <stdint.h>

/****
    structure contains pi start parameters.
    end_tsf and curr_priority are placeholders, will be updated inside role.c
    clock_offset is rfu for inquiry
****/

typedef struct usched_pi_params_s {
    uint32_t start_priority;
    uint32_t priority_step_size;
    uint32_t end_priority;
    uint32_t role;
    uint32_t min_runtime;
    uint32_t max_runtime;
    uint32_t pi_total_duration;
    uint32_t curr_priority;
    uint32_t end_tsf;
    uint32_t delay;
    uint8_t address[6];
    uint8_t mode;
    uint16_t clock_offset;
    uint8_t current_device_index;
} usched_pi_params_t;

/****
    structure contains pi scan start parameters
    end_tsf and curr_priority are placeholders, will be updated inside role.c
****/

typedef struct usched_pi_scan_params_s {
    uint32_t start_priority;
    uint32_t priority_step_size;
    uint32_t end_priority;
    uint32_t pi_scan_window;
    uint32_t role;
    uint32_t pi_scan_interval;
    uint32_t curr_priority;
    uint32_t end_tsf;
    uint8_t address[3];
    uint8_t mode;
    uint8_t current_device_index;
} usched_pi_scan_params_t;

/****
    structure contains acl start parameters
    role = central (0) or peripheral (1)
    end_tsf and conn_info_index are placeholders, will be updated inside role.c
    acl_priority - LOW_PRIORITY or HIGH_PRIORITY( eg - A2DP)
****/

typedef struct usched_acl_params_s {
    uint32_t start_priority;
    uint32_t priority_step_size;
    uint32_t end_priority;
    uint32_t role;
    uint32_t min_runtime;
    uint32_t max_runtime;
    uint32_t duration;
    uint32_t acl_priority;
    uint32_t conn_info_index;
    uint32_t poll_interval;
    uint32_t is_tx_pending;
    uint32_t is_rx_pending;
} usched_acl_params_t;

#endif /* USCHED_ROLE_H */