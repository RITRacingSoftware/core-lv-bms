#include <stdbool.h>
#include <stddef.h>

#include "ADC.h"
#include "timeout.h"
#include "config.h"

#include "CurrentMonitor.h"
#include "FaultManager.h"

// store currents in array?

float cell_curr;

static core_timeout_t overcurrent_timeout;
static void timeout_callback (core_timeout_t *timeout);

static void check_cell_current();

void CurrentMonitor_init()
{
    overcurrent_timeout.module = NULL;
    overcurrent_timeout.ref = FAULT_OVERCURRENT;
    overcurrent_timeout.timeout = OVERCURRENT_TIMEOUT_MS;
    overcurrent_timeout.callback = timeout_callback;
    overcurrent_timeout.single_shot = 0;
    core_timeout_insert(&overcurrent_timeout);

}

void CurrentMonitor_Task_Update()
{
    check_cell_current();
}

void check_cell_current() 
{
    bool irrational = false;

    float raw_current;
    core_ADC_read_channel(CS_PORT, CS_PIN, &raw_current);
    current = ((3.3f * raw_current/4096.0f) - CS_OFFSET_VOLTAGE) * CS_GAIN;

    if ((current < OVERCURRENT_POSITIVE) || (current > OVERCURRENT_NEGATIVE)) core_timeout_reset(&overcurrent_timeout);
}

// current limiting?