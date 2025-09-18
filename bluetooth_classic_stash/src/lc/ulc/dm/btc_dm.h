#ifndef BTC_DM_H
#define BTC_DM_H

void add_pkt_to_acld_q(ulc_t *ulc, pkb_t *lmp_tx_pkt, uint8_t peer_id);
void btc_deint(ulc_t *ulc);
void flush_queue(queue_t *q);
#endif // BTC_DM_H

