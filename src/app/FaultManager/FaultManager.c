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

void FaultManager_LSSM(uint8_t lssmByte) {}