#ifndef STAT_H
#define STAT_H

typedef struct {
  uint16_t schedule_id_tx;  // Incremented in rail_schedule_id_tx
  uint16_t schedule_id_rx;  // Incremented in rail_schedule_id_rx
  uint16_t schedule_tx;     // Incremented in rail_schedule_tx
  uint16_t schedule_rx;     // Incremented in rail_schedule_rx

  // TX related stats
  uint16_t tx_sent;                 // Incremented in BTC_RAIL_SEQ_txCb
  uint16_t tx_aborted;              // Incremented in BTC_RAIL_SEQ_txCb

  // RX related stats
  uint16_t rx_timeout;                  // Incremented in BTC_RAIL_SEQ_rxSearchTimeoutCb
  uint16_t rx_packet_done;              // Incremented in BTC_RAIL_SEQ_rxCb
  uint16_t rx_packet_received;          // Incremented in BTC_RAIL_SEQ_rxCb
  uint16_t rx_packet_aborted;           // Incremented in BTC_RAIL_SEQ_rxCb
  uint16_t id_pkt_rx;                  // Incremented in BTC_RAIL_SEQ_rxSearchTimeoutCb
  uint16_t hec_error;                   // Incremented in BTC_RAIL_SEQ_rxSearchTimeoutCb
} stats_t;

#endif
