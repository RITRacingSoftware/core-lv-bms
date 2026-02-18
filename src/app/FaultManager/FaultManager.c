#include "FaultManager.h"
#include <stdint.h>

#define IGNORE_LIST () // ignore list for this?

static uint64_t faultList;

void FaultManager_init() 
{
    faultList = 0;
}

void FaultManager_set(uint64_t faultCode)
{
    if (!(faultList & faultCode)) // fault code not already in fault list
    {
        if (!(faultCode & IGNORE_LIST))
        {
            faultList |= // which fault?
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

bool FaultManager_read(uint64_t faultCode)
{
    return (faultList & faultCode);
}

void FaultManager_reset(uint16_t faultCode)
{
    faultList &= ~faultCode;
}