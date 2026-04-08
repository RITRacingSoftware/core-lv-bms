#include <stdint.h>
#include "FaultManager.h"
#include "AppGPIO.h"
#include "rtt.h"
#include "config.h"
#include "ChargeMonitor.h"

static uint64_t faultList = 0;
static uint64_t ignoreList = 0;
static uint64_t errorCode = 0;

void FaultManager_set_fault(uint64_t faultCode) 
{
    ChargeMonitor_set_state(ChargeState_FAULTED);
    if (!(faultList & faultCode)) {
        faultList |= faultCode;
        if (!(faultList & ignoreList)) {
            faultList |= FAULT_SHUTDOWN; // correct fault?
            GPIO_set_shutdown_pin(false);
            // CAN_send_fault_list(faultList);
        }
    }
}

void FaultManager_set_err(uint64_t faultCode) 
{
    rprintf("Err: %d\n", faultCode);
    errorCode = faultCode;
    // Add message with error code to CAN tx queue
}

void FaultManager_task_update() {
    // Send fault list over CAN
    // Add message with error code to CAN tx queue
}


void FaultManager_LSSM(uint8_t lssmByte) {}
