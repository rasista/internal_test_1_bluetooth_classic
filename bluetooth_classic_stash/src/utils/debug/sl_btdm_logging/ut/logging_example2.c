#include "stdint.h"
#include <stdlib.h>  // For srand() and rand()
#include <unistd.h>  // For sleep()
#include "sl_btdm_logging.h"
#include "sl_btdm_log_autogen.h"
#include "time.h"

void sl_btdm_fast_logging_backend();

void *fast_log_producer1(void * ctx)
{
    int random_number = 0;
    //1 create seed for random number generator
    srand(time(NULL));
    //! Produce fast_log messages
    for (int i = 0; i < 100; i++)
    {
        //! generate a random number between 0 and 3
        random_number = i % 4;
        if (random_number == 0)
        {
            sl_btdm_fast_log(example_no_arg_2, COMPONENT_SEQ_BTC, TRACE, "fast_log message with no arguments");
        }
        else if (random_number == 1)
        {
            sl_btdm_fast_log(example_one_arg_2, COMPONENT_SEQ_BTC, DEBUG, "fast_log message with one argument %d", i);
        }
        else if (random_number == 2)
        {
            sl_btdm_fast_log(example_two_args_2, COMPONENT_SEQ_BTC, WARN, "fast_log message with two arguments %d %c", 
            i, 0x41);//!sdfkbf
        } 
        else
        {
            sl_btdm_fast_log(example_three_args_2, COMPONENT_SEQ_BTC, ERROR, "fast_log message with three arguments %d %w %b", i + 1, i + 2, i + 3);
        }
        //! sleep for 500 ms
        usleep(20000);
    }
    sleep(5);
    return NULL;
}

void *fast_log_consumer1(void * ctx)
{
    //! Consume fast_log messages
    while (1)
    {
        sl_btdm_fast_logging_backend();
    }
    return NULL;
}
