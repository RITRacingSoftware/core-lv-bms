#pragma once
#include <stdbool.h>

#define ADES_WRITEDEV(n) ((uint8_t)((n << 3) | 0b100))
#define ADES_READDEV(n) ((uint8_t)((n << 3) | 0b101))
#define ADES_READBLOCK(n) ((uint8_t)((n << 3) | 0b110))

/*** COMMANDS ***/
#define ADES_WRITEALL                   (0x02)
#define ADES_READALL                    (0x03)


/*** REGISTERS ***/
#define ADES_DEVCFG1                    (0x14)
#define ALIVECNT                        (0x01 << 9)

#define ADES_AUXGPIOCFG                 (0x16)
#define GPIOEN0                         (0x01 << 8)
#define GPIOEN1                         (0x01 << 9)
#define GPIOEN2                         (0x01 << 10)
#define GPIOEN3                         (0x01 << 11)
#define GPIOEN4                         (0x01 << 12)
#define GPIOEN5                         (0x01 << 13)

#define ADES_CELLREG(n)                 (0x47 + n)
#define ADES_AUXREG(n)                  (0x59 + n)

#define ADES_MEASUREEN1                 (0x64)
#define BLOCKEN                         (0x01 << 14)

#define ADES_MEASUREEN2                 (0x65)

#define ADES_SCANCTRL                   (0x66)
#define SCAN                            (0x01)
#define DATARDY                         (0x01 << 13)

bool ADES_init();
bool ADES_collect_all();
