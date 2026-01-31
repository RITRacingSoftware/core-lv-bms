#pragma once
#include <stdbool.h>
#include <stdint.h>


#define CRC_POLY                        (0xB2)
#define M17_HELLO_ALL                   (0x57)
#define M17_NORMAL_LSSM                 (0x84)

/*** COMMANDS ***/
#define M17_CLR_TXBUF                   (0x40)
#define M17_CLR_RXBUF                   (0x42)
#define M17_SWPOR                       (0x4A)

/*** REGISTERS ***/
#define M17_STATUS_RX                   (0x01)
#define RX_ERR                          (0x01 << 7)
#define RX_BUSY                         (0x01 << 5)
#define RX_STOP                         (0x01 << 1)
#define RX_EMPTY                        (0x01)

#define M17_STATUS_TX                   (0x03)
#define TX_FULL                         (0x01 << 2)

#define M17_STATUS_LSSM_BYTE            (0x05)
#define COMM_ERR                        (0x01 << 5)

#define M17_ALERT_RX                    (0x11)
#define RX_ERR_ALRT                     (0x01 << 7)
#define RX_OVRFLW_ERR_ALRT              (0x01 << 3)

#define M17_CONFIG_GEN0                 (0x60)

#define M17_CONFIG_GEN1                 (0x62)
#define BAUD_RATE_500K                  (0x1 << 4)
#define BAUD_RATE_1M                    (0x2 << 4)
#define BAUD_RATE_2M                    (0x3 << 4)
#define BAUD_RATE_4M                    (0x4 << 4)

#define M17_CONFIG_GEN2                 (0x64)
#define TX_PREAMBLES                    (0x01 << 5)
#define TX_QUEUE                        (0x01 << 4)

#define M17_CONFIG_GEN3                 (0x66)
#define ALRTPCKT_TIMING_1280_US         (0x08) // Set keep-alive to 1.28mS

#define M17_CONFIG_GEN4                 (0x68)
#define MS_EN_MASTER_SINGLE_UART        (0x02 << 4)
#define DC_EN_DATA_RX                   (0x02 << 2)
#define DC_EN_DATA_NO_RX                (0x03 << 2)
#define ALIVECOUNT_EN                   (0x02)

#define M17_CONFIG_GEN5                 (0x6A)
#define ALRTPCKT_DBNC_16                (0x08)

#define M17_CONFIG_COMM                 (0x76)
#define COMM_TO_DLY_DISABLE             (0x04)

#define M17_ALRTPCKTBUF_RD_PTR          (0x8C)

#define M17_RX_RD_NXT_MSG               (0x93)

#define M17_RX_WR_PTR                   (0x99)

#define M17_NXT_LDQ                     (0xB0)

#define M17_LDQ                         (0xC0)

#define M17_LDQ_PTR                     (0xC2)

bool M17_init();
bool M17_write_ADES_reg(uint8_t dest, uint8_t reg_addr, uint16_t msg);
bool M17_read_ADES_reg(uint8_t dest, uint8_t reg_addr, uint16_t *rxBuf, uint8_t rxLen);
uint8_t M17_num_active_chips();
