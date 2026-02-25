#include "FaultManager.h"
#include <stdint.h>

static uint64_t faultList = 0;
static uint64_t ignoreList = 0;

void FaultManager_init() 
{
    faultList = 0;
}

void FaultManager_set(uint64_t faultCode)
{
    if (!(faultList & faultCode)) // fault code not already in fault list
    {
        if (!(faultCode & ignoreList))
        {
            faultList |= FAULT_PDU_SHUTDOWN; // Taken from BMS FaultManager. Does LVBMS also shut down the PDU? 
            // function to set fault here
            faultList |= faultCode;
            // add fault vector to CAN message (params: bus, fault vector frame ID, bits?, faultList)
        }
        else 
        {
            faultList |= faultCode;
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