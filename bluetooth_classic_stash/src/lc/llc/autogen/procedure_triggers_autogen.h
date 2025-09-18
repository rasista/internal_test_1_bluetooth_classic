#ifndef __PROCEDURE_TRIGGERS_AUTOGEN_H__
#define __PROCEDURE_TRIGGERS_AUTOGEN_H__

enum procedure_triggers_e {
    START_PROCEDURE,
    TX_DONE,
    TX_ABORT,
    RX_DONE,
    RX_TIMEOUT,
    RX_ENDSYNC,
    RX_HEADER_RECEIVED,
    RX_ABORT,
    procedures_max_num_triggers
};
#endif