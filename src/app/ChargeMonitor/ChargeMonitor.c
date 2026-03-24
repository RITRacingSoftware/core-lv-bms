#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"

#include "ChargeMonitor.h"
#include "config.h"
#include "PackMonitor.h"

float max_cell_volt_balance = 0;
int max_cell_index = 0;

static ChargingState_e state;

// needs getter for state? VC state machine has one

void ChargeMonitor_init()
{
    core_GPIO_init(CHG_IN_PORT, CHG_IN_PIN, GPIO_MODE_INPUT, GPIO_PULLDOWN);
    core_GPIO_init(CHG_ENA_PORT, CHG_ENA_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN); // should be pulldown?
    state = ChargingState_DISCONNECTED;
}

void ChargeMonitor_Task_Update()
{

    switch(state)
    {
        case ChargingState_DISCONNECTED:
        // Detect if charger gets connected
        if (core_GPIO_digital_read(CHG_IN_PORT, CHG_IN_PIN)) state = ChargingState_CHARGING;


        case ChargingState_CHARGING:
        // Enable charging (pull pin high)
        core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, true); // need to check whether charging already?



        /*  Pause charging
            Check if highest cell at CELL_FULL_MAX_V, balance, state = COMPLETE
            Else, state = CHARGING
        */
        case ChargingState_BALANCING:
        if (core_GPIO_digital_read(CHG_ENA_PORT, CHG_ENA_PIN)) core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, false);
        core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, false);

        for (int cell = 0; cell < NUM_CELLS; cell++) {
            if (cell_volt_arr[cell] > max_cell_volt_balance)
            {
                max_cell_volt_balance = cell_volt_arr[cell];
                max_cell_index++;
            }
        }

        if (cell_volt_arr[max_cell_index] >= CELL_FULL_MAX_V) {
            // balancing function for cells other than max full one
        }  
        state = ChargingState_CHARGING;

        /* Disable charging (pull pin low)
        */
        case ChargingState_COMPLETE:
        core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, false);



        /* Disable charging (pull pin low)
        */
        case ChargingState_FAULT:
        core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, false);

    }
}

void ChargeMonitor_set_fault()
{
    state = ChargingState_FAULT;
}

ChargingState_e getState() 
{
    return state;
}