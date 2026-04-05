#include "FaultManager.h"
#include <stdint.h>
#include "rtt.h"

static uint64_t faultList;
static uint64_t ignoreList;

void FaultManager_init() 
{
    faultList = 0;
    ignoreList = 0;
}

void FaultManager_set_fault(uint64_t faultCode)
{
    if (!(faultList & faultCode))           // fault code not already in fault list
    {
        faultList |= faultCode;             // add fault code to fault list
        if (!(faultList & ignoreList))      // if faults are not all in ignore list (?)
        {
            faultList |= FAULT_SHUTDOWN;    // set shut down true
            // send CAN message here
        }
    }
}

void FaultManager_set_err(uint64_t errorCode) 
{
        rprintf("Err: %d\n", errorCode);
}

bool FaultManager_read(uint64_t faultCode)
{
    return (faultList & faultCode);
}

void FaultManager_reset(uint16_t faultCode)
{
    faultList &= ~faultCode;
}

void FaultManager_LSSM(uint8_t lssmByte) {}