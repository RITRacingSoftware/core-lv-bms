#include "CurrentMonitor.h"

#include <stdbool.h>
#include <stdint.h>

#include "adc.h"
#include "can.h"
#include "filter.h"
#include "timeout.h"
#include "rtt.h"

#include "FaultManager.h"
#include "config.h"
#include "common_macros.h"
#include "AppCAN.h"
#include "AppGPIO.h"

core_filter_t filt;

static core_timeout_t overcurrent_timeout;
static core_timeout_t current_irr_timeout;

static void timeout_callback (core_timeout_t *timeout);

void CurrentMonitor_init()
{
    core_ADC_init(ADC2);
    core_ADC_setup_pin(CS_PORT, CS_PIN, 1);
    core_ADC_setup_pin(CS_REF_PORT, CS_REF_PIN, 1);

    core_filter_exp_lowpass_init(0.1f, &filt);

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

bool CurrentMonitor_task_update()
{
    uint16_t raw_cs = 1919;
    uint16_t raw_ref = 1919;
    core_ADC_read_channel(CS_PORT, CS_PIN, &raw_cs);
    core_ADC_read_channel(CS_REF_PORT, CS_PIN, &raw_ref);
    rprintf("Raw CS: %d, Raw ref: %d, Raw current: %d\n", raw_cs, raw_ref, (raw_cs - raw_ref));

    // float filt_v = (3.3f * raw_current/4096.0f);
    // float unfilt_v = SCALE(filt_v, 0.0f, 3.03f, 0.0f, 5.0f);
    // float current = (unfilt_v - CS_OFFSET_VOLTAGE) * CS_GAIN;

    // come back to
    
    // if ((current < OVERCURRENT_POS_I) ||(current > OVERCURRENT_NEG_I)) core_timeout_reset(&overcurrent_timeout);

    // if (current > CS_IRR_L) {
        // core_timeout_reset(&current_irr_timeout);
        // if (!CAN_pack_and_send_current(core_filter_update(current, &filt))) return false;
    // }
}

static void timeout_callback (core_timeout_t *timeout)
{
}
