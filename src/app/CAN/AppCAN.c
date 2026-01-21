#include "AppCAN.h"
#include "config.h"

#include <stdbool.h>

// Core includes
#include "can.h"

void CAN_error_handler() {}

bool CAN_tx()
{
    core_CAN_send_from_tx_queue_task(CAN_PRI);
}

bool CAN_rx_main() 
{

}