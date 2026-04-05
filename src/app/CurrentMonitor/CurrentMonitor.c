#include <stdbool.h>
#include <stddef.h>

#include "ADC.h"
#include "timeout.h"
#include "config.h"

#include "CurrentMonitor.h"
#include "FaultManager.h"

float current;

static core_timeout_t overcurrent_timeout;
static core_timeout_t current_irr_timeout;

static void timeout_callback (core_timeout_t *timeout);

static void check_cell_current();

void CurrentMonitor_init()
{

    core_ADC_init(ADC5);
    core_ADC_setup_pin(CS_PORT, CS_PIN, 1);

    overcurrent_timeout.module = NULL;
    overcurrent_timeout.ref = FAULT_OVERCURRENT;
    overcurrent_timeout.timeout = OVERCURRENT_TIMEOUT_MS;
    overcurrent_timeout.callback = timeout_callback;
    overcurrent_timeout.single_shot = 0;
    core_timeout_insert(&overcurrent_timeout);

    current_irr_timeout.module = NULL;
    current_irr_timeout.ref = FAULT_CURRENT_IRR;
    current_irr_timeout.timeout = CURRENT_IRR_TIMEOUT_MS;
    current_irr_timeout.callback = timeout_callback;
    current_irr_timeout.single_shot = 0;
    core_timeout_insert(&current_irr_timeout);

}   

void CurrentMonitor_Task_Update()
{
    check_cell_current();
    // send current over CAN
}

void check_cell_current() 
{

    uint16_t raw_current;
    core_ADC_read_channel(CS_PORT, CS_PIN, &raw_current);
    current = ((3.3f * raw_current/4096.0f) - CS_OFFSET_VOLTAGE) * CS_GAIN;

    if ((current < OVERCURRENT_POSITIVE_I) || (current > OVERCURRENT_NEGATIVE_I)) core_timeout_reset(&overcurrent_timeout);

    if ((current) < CURRENT_IRR_I) core_timeout_reset(&current_irr_timeout);
}