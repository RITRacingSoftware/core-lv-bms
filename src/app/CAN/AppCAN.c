#include "AppCAN.h"
#include "config.h"

#include <stdbool.h>


#include "can.h"

static void send_CAN_errors(); // why static?
static bool CAN_add_filters();

int id_arr[NUM_IDS] = {
    // how many/which IDs used?
    CAN_ERRORS_FRAME_ID //
};

void CAN_error_handler() 
{

}

bool CAN_init()
{
    if (!core_CAN_init(CAN_BUS, 10000000)) return false;
    if (!CAN_add_filters()) return false;
    
    return true;
}

bool CAN_tx()
{
    core_CAN_send_from_tx_queue_task(CAN_BUS);
    return false;
}

void CAN_Task_Update()
{
    // requires DBC functions for packing
}

static void send_CAN_errors() 
{
    uint16_t msg = 0;

    ((uint16_t *)&msg)[0] = core_CAN_errors.arbitration_error;
    ((uint16_t *)&msg)[1] = core_CAN_errors.data_error;

    core_CAN_add_message_to_tx_queue(CAN_BUS, CAN_ERRORS_FRAME_ID, 8, msg);
    // logic from VC code but can't find MAIN_DBC_VC_CAN_ERRORS_FRAME_ID (CAN_ERRORS_FRAME_ID) defined anywhere
    // put in ID array at top of file for now
}

static bool CAN_add_filters()
{
    int min;
    int max;
    bool status = true;

    min = id_arr[0]; // the VC CAN code assigns these by traversing through the array and finding the largest and smallest. Why? Are they not ordered?
    max = id_arr[NUM_IDS - 1];

    status = (status && core_CAN_add_filter(CAN_BUS, false, min, max));

    return status;


}