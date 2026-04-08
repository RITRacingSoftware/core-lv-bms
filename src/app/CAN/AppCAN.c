#include <stdbool.h>

#include "can.h"
#include "AppCAN.h"

#include "config.h"

void CAN_error_handler() 
{

}

bool CAN_init()
{
    // if (!core_CAN_init(CAN_SEC, 10000000)) return false;
    
    // return true;
}

bool CAN_tx()
{
    // core_CAN_send_from_tx_queue_task(CAN_SEC);
    // return false;
}

void CAN_task_update()
{
    // requires DBC functions for packing
}
