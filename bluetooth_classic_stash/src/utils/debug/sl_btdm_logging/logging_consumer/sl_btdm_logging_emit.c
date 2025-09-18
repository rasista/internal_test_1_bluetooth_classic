#include <stdio.h>
#include "sl_btdm_logging.h"
#ifndef LINUX
#include "SEGGER_RTT.h"
#endif
void sl_btdm_logging_emit_log_entry(uint8_t *log_entry, uint32_t log_entry_size)
{
#ifdef LINUX
    //! This function is a dummy
    printf("&^$\t");
    for (uint32_t i = 0; i < log_entry_size; i++)
    {
        printf("%02x\t", log_entry[i]);
    }
    printf("\n");
#else
// Write data to the RTT buffer
    unsigned bytesWritten = SEGGER_RTT_Write(5, log_entry, log_entry_size);
#endif
}