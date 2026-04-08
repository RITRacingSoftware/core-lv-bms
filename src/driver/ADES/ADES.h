#pragma once

#include <stdint.h>
#include <stdbool.h>

#define POOP_LOOP_DELAY_MS 10

#define ADES_WRITEDEV(n) ((n << 3) | 0b100)
#define ADES_READDEV(n) ((n << 3) | 0b101)
#define ADES_READBLOCK(n) ((n << 3) | 0b110)

/*** COMMANDS ***/
#define ADES_WRITEALL                   (0x02)
#define ADES_READALL                    (0x03)


/*** REGISTERS ***/
#define ADES_DEVCFG1                    (0x14)
#define UARTCFG_EXT                     (0x00 << 14)
#define UARTCFG_INT                     (0x01 << 14)
#define UARTCFG_DIFF                    (0x02 << 14)
#define UARTCFG_DUAL                    (0x03 << 14)
#define ALIVECNT                        (0x01 << 9)
#define ALERTEN                         (0x01 << 1)

#define ACQCFG                          (0x62)
#define ADCCALEN                        (0x01 << 14)

#define ADES_AUXGPIOCFG                 (0x16)
#define GPIOEN0                         (0x01 << 8)
#define GPIOEN1                         (0x01 << 9)
#define GPIOEN2                         (0x01 << 10)
#define GPIOEN3                         (0x01 << 11)
#define GPIOEN4                         (0x01 << 12)
#define GPIOEN5                         (0x01 << 13)

#define ADES_CELLREG(n)                 (0x47 + n)

#define ADES_BLOCKREG                   (0x55)

#define ADES_AUXREG(n)                  (0x59 + n)

#define ADES_AUXTIMEREG                 (0x61)

#define ADES_MEASUREEN1                 (0x64)
#define BLOCKEN                         (0x01 << 14)

#define ADES_MEASUREEN2                 (0x65)

#define ADES_SCANCTRL                   (0x66)
#define SCAN                            (0x01)
#define SCANMODE                        (0x01 << 1)     // for ramp scan
#define DATARDY                         (0x01 << 13)

#define ADES_BALSWCTRL                  (0x6F)

#define ADES_BALEXP(n)                  (0x70 + n)

#define ADES_BALAUTOUVTHR               (0x7E)

#define ADES_BALDLYCTRL                 (0x7F)

#define ADES_BALCTRL                    (0x80)

#define ADES_BALUVSTAT                  (0x82)


bool ADES_init();
bool ADES_collect_all(uint16_t *raw_cell_voltages, uint16_t *raw_chip_voltages, uint16_t *raw_temps);
bool ADES_force_balance(uint8_t chip, uint8_t cell, bool reset);
bool ADES_init_balancing();
