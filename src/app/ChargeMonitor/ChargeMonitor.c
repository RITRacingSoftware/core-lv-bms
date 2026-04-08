#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"

#include "AppGPIO.h"
#include "ChargeMonitor.h"
#include "config.h"
#include "PackMonitor.h"
#include "FaultManager.h"

// revisit 

static ChargeState_e state;
static uint16_t bal_arr[NUM_CHIPS] = {0};

bool ChargeMonitor_init()
{
    core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, false);

    if (core_GPIO_digital_read(CHG_IN_PORT, CHG_IN_PIN)) {
        ChargeMonitor_set_state(ChargeState_CONNECTED_CHARGING);
        return true;
    }       
    ChargeMonitor_set_state(ChargeState_DISCONNECTED);
    return false;
}

bool ChargeMonitor_task_update()
{
    if ((state != ChargeState_DISCONNECTED) && (!core_GPIO_digital_read(CHG_IN_PORT, CHG_IN_PIN))) {
        FaultManager_set_err(ERR_CHARGING_DISCONNECTED);
    }

    core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, true); // CAN_send_chg_request(); in BMS

    switch(state)
    {
        case ChargeState_DISCONNECTED:
            break;

        case ChargeState_CONNECTED:
            break;

        case ChargeState_CONNECTED_CHARGING:
            if (max_cell_volt >= CELL_FULL_MAX_V) ChargeMonitor_set_state(ChargeState_CONNECTED_COMPLETE); // commented to delete in BMS code
            break;

        // case ChargeState_CONNECTED_BALANCING:
        //      balancing logic
        //      break;

        case ChargeState_CONNECTED_COMPLETE:
            break;

        case ChargeState_FAULTED:
            // send message over CAN
            break;
    }

    // send charge state over CAN

    return true;
}

void ChargeMonitor_set_state(ChargeState_e _state) 
{
    // if (state == ChargeState_CONNECTED_BALANCING) ADES_stop_balancing();

    state = _state;
    if (_state == ChargeState_FAULTED) {
        FaultManager_set_fault(FAULT_CHARGER);
        core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, false);
    }
    else if (_state == ChargeState_CONNECTED_BALANCING) {
        core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, false);
    }
    else if (_state == ChargeState_CONNECTED_CHARGING) {
        core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, true);
    }
    else if (_state == ChargeState_CONNECTED_COMPLETE) {
        core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, false);
    }
}

ChargeState_e ChargeMonitor_get_state()
{
    return state;
}
