#ifndef  SCHEDULER_LPW_H
#define  SCHEDULER_LPW_H

#define LPW_SCHED_ACTIVITY_SWITCH_DELAY_IN_US 3000    //TODO:update proper value, time from rail triggers to load next activity until RAIL  is configures
#define MAX_RADIO_USAGE_IN_US 6250                    //TODO:values would be differing based on packet types. Different values to be updated

#define APERIODIC_NOT_PUNCTURED 0
#define APERIODIC_TX_PUNCTURED 1
#define APERIODIC_RX_PUNCTURED 2

#define APERIODIC_TO_PERIODIC_THRESHOLD_US 3000

typedef struct aperiodic_puncture_restore_lut_s {
  void (*handler)(btc_dev_t *btc_dev_p);
} aperiodic_puncture_restore_lut_t;

void restore_aperiodic_role_punctured();
void debug_scheduled_tsf_expired_check(btc_trx_configs_t *btc_trx_configs);
void handle_overriding_role_abort_status(btc_trx_configs_t *btc_trx_configs);
void aperiodic_idle(btc_dev_t *btc_dev_p);
void aperiodic_restore_tx_abort(btc_dev_t *btc_dev_p);
void aperiodic_restore_rx_abort(btc_dev_t *btc_dev_p);

uint8_t check_aperiodic_role_overlapping_with_periodic_start(btc_dev_t *btc_dev_p, btc_trx_configs_t *btc_trx_configs);
btc_trx_configs_t* dequeue_periodic_role_from_periodic_queue();
void add_periodic_role_to_periodic_queue(btc_dev_t *btc_dev_p, btc_trx_configs_t *btc_trx_configs);
uint8_t get_periodic_role_start_tsf(btc_dev_t *btc_dev_p);
void  update_aperiodic_role(btc_dev_t *btc_dev_p, sm_t *state_machine);

void stop_aperiodic_role(btc_dev_t *btc_dev_p);
void stop_periodic_role(btc_dev_t *btc_dev_p, btc_trx_configs_t *btc_trx_configs);
void l2_sched_configure_next_activity_on_role_removal(btc_dev_t *btc_dev_p);

#endif
