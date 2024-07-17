//
// Created by danan on 2024-07-17.
//

#ifndef AD7175_CONSTANTS_H
#define AD7175_CONSTANTS_H

//CRC constants
#define AD7175_CRC_POLYNOMIAL 0x07 // x^8 + x^2 + x +1 (MSB first)
#define AD7175_CRC_CHECK_CODE 0xF3

// Communications register. Write to me first before reading/writing from the register
#define AD7175_COMMS 0x00

// Status register.
#define AD7175_STATUS 0x00

// Set conversion mode (single or continuous), standby/power down, calibration, clock source selection and internal reference
#define AD7175_ADCMODE 0x01

#define AD7175_IFMODE 0x02
#define AD7175_REGCHECK 0x03
#define AD7175_DATA 0x04
#define AD7175_GPIOCON 0x06
#define AD7175_ID 0x07

// Channel Registers
#define AD7175_CH0 0x10
#define AD7175_CH1 0x11
#define AD7175_CH2 0x12
#define AD7175_CH3 0x13

// Selects Output Coding and Reference Voltage
#define AD7175_SETUPCON0 0x20
#define AD7175_SETUPCON1 0x21
#define AD7175_SETUPCON2 0x22
#define AD7175_SETUPCON3 0x23

// Selects the Digital Filter
#define AD7175_FILTCON0 0x28
#define AD7175_FILTCON1 0x29
#define AD7175_FILTCON2 0x2A
#define AD7175_FILTCON3 0x2B

// Offset Calibration Coefficients
#define AD7175_OFFSET0 0x30
#define AD7175_OFFSET1 0x30
#define AD7175_OFFSET2 0x30
#define AD7175_OFFSET3 0x30

// Gain Calibration Coefficients
#define AD7175_GAIN0 0x38
#define AD7175_GAIN1 0x39
#define AD7175_GAIN2 0x3A
#define AD7175_GAIN3 0x3B

/* Communication Register bits */
#define COMM_REG_WEN    (0 << 7)
#define COMM_REG_WR     (0 << 6)
#define COMM_REG_RD     (1 << 6)

/* Status Register bits */
#define STATUS_REG_RDY      (1 << 7)
#define STATUS_REG_ADC_ERR  (1 << 6)
#define STATUS_REG_CRC_ERR  (1 << 5)
#define STATUS_REG_REG_ERR  (1 << 4)
#define STATUS_REG_CH(x)    ((x) & 0x03)

/* ADC Mode Register */
#define ADC_MODE_REG_REF_EN         (1 << 15)
#define ADC_MODE_REG_SINGSYNC_EN         (1 << 13)
#define ADC_MODE_REG_DELAY(x)       (((x) & 0x7) << 8)
#define ADC_MODE_REG_DELAY_1ms      ADC_MODE_REG_DELAY(7)
#define ADC_MODE_REG_MODE(x)        (((x) & 0x7) << 4)
#define ADC_MODE_REG_MODE_SINGLE       ADC_MODE_REG_MODE(1)
#define ADC_MODE_REG_CLKSEL(x))     (((x) & 0x3) << 2)

/* Interface Mode Register bits */
#define INTF_MODE_REG_DOUT_RESET    (1 << 8)
#define INTF_MODE_REG_CONT_READ     (1 << 7)
#define INTF_MODE_REG_DATA_STAT     (1 << 6)
#define INTF_MODE_REG_CRC_EN        (0x02 << 2)
#define INTF_MODE_REG_CRC_STAT(x)   (((x) & INTF_MODE_REG_CRC_EN) == INTF_MODE_REG_CRC_EN)

/* GPIO Configuration Register */
#define GPIO_CONF_REG_MUX_IO        (1 << 12)
#define GPIO_CONF_REG_SYNC_EN       (1 << 11)
#define GPIO_CONF_REG_ERR_EN(x)     (((x) & 0x3) << 9)
#define GPIO_CONF_REG_ERR_DAT       (1 << 8)
#define GPIO_CONF_REG_IP_EN1        (1 << 5)
#define GPIO_CONF_REG_IP_EN0        (1 << 4)
#define GPIO_CONF_REG_OP_EN1        (1 << 3)
#define GPIO_CONF_REG_OP_EN0        (1 << 2)
#define GPIO_CONF_REG_DATA1         (1 << 1)
#define GPIO_CONF_REG_DATA0         (1 << 0)

/* ID Register */
#define ID_REG_PRODUCT_ID(x)        (((x) & 0xFF) << 8)

/* Channel Map Register 1-4 */
#define CH_MAP_REG_CHEN         (1 << 15)
#define CH_MAP_REG_SETUP(x)     (((x) & 0x7) << 12)
#define CH_MAP_REG_AINPOS(x)    (((x) & 0x1F) << 5)
#define CH_MAP_REG_AINNEG(x)    (((x) & 0x1F) << 0)

/* Setup Configuration Register 1-4 */
#define SETUP_CONF_REG_CHOP_MD(x)       (((x) & 0x3) << 14)
#define SETUP_CONF_REG_BI_UNIPOLAR      (1 << 12)
#define SETUP_CONF_REG_REF_BUF_P        (1 << 11)
#define SETUP_CONF_REG_REF_BUF_N        (1 << 10)
#define SETUP_CONF_REG_AIN_BUF_P        (1 << 9)
#define SETUP_CONF_REG_AIN_BUF_N        (1 << 8)
#define SETUP_CONF_REG_BRNOUT_EN        (1 << 7)
#define SETUP_CONF_REG_REF_SEL(x)       (((x) & 0x3) << 4)

/* Filter Configuration Register 1-4 */
#define FILT_CONF_REG_EXTCLKFREQ(x)     (((x) & 0x3) << 13)
#define FILT_CONF_REG_ENHFILTEN         (1 << 11)
#define FILT_CONF_REG_ENHFILTSEL(x)     (((x) & 0x7) << 8)
#define FILT_CONF_REG_ORDER(x)          (((x) & 0x7) << 5)
#define FILT_CONF_REG_ODR(x)            (((x) & 0x1F) << 0)

#endif //AD7175_CONSTANTS_H
