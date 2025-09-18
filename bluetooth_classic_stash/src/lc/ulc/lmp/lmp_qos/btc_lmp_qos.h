#ifndef _BTC_LMP_QOS_H
#define _BTC_LMP_QOS_H

#define IS_GUARANTEED_SERVICE(dev_mgmt_info) ((dev_mgmt_info)->service_type == GUARANTEED)
#define MIN_POLL_INTERVAL    6      // Minimum poll interval (6 slots)

#define IS_POLL_INTERVAL_VALID(interval)  ((interval) >= MIN_POLL_INTERVAL)

void lmp_qos_setup(ulc_t *ulc, uint8_t peer_id);
uint8_t process_lmp_quality_of_service_req(ulc_t *ulc, uint8_t peer_id, uint16_t requested_poll_interval, uint8_t requested_nbc);

#endif
