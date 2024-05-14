#ifndef ADC18_H
#define ADC18_H

#include "mbed.h"

#define ADC18_REG_GEN_PROD                  0x00
#define ADC18_REG_GEN_REV                   0x01
#define ADC18_REG_GEN_CNFG                  0x02
#define ADC18_REG_GEN_CHNL_CTRL             0x03
#define ADC18_REG_GEN_GPIO_CTRL             0x04
#define ADC18_REG_GEN_GPI_INT               0x05
#define ADC18_REG_GEN_GPI_DATA              0x06
#define ADC18_REG_GEN_INT                   0x07
#define ADC18_REG_GEN_INTEN                 0x08
#define ADC18_REG_GEN_PWR_CTRL              0x09
#define ADC18_REG_DCHNL_CMD                 0x20
#define ADC18_REG_DCHNL_STA                 0x21
#define ADC18_REG_DCHNL_CTRL1               0x22
#define ADC18_REG_DCHNL_CTRL2               0x23
#define ADC18_REG_DCHNL_DATA                0x24
#define ADC18_REG_DCHNL_N_SEL               0x25
#define ADC18_REG_DCHNL_N_SOC               0x26
#define ADC18_REG_DCHNL_N_SGC               0x27

#define ADC18_PRODUCT_ID                    0x18
#define ADC18_DATA_RESOLUTION               0x7FFFFFul
#define ADC18_FULL_SCALE_VOLTAGE            25

#define ADC18_START_CONVERSION              0x30
#define ADC18_STOP_CONVERSION               0x10
#define ADC18_DATA_RATE_1_SPS               0x00
#define ADC18_DATA_RATE_3_SPS               0x01
#define ADC18_DATA_RATE_5_SPS               0x02
#define ADC18_DATA_RATE_10_SPS              0x03
#define ADC18_DATA_RATE_13_SPS              0x04
#define ADC18_DATA_RATE_15_SPS              0x05
#define ADC18_DATA_RATE_50_SPS              0x06
#define ADC18_DATA_RATE_60_SPS              0x07
#define ADC18_DATA_RATE_150_SPS             0x08
#define ADC18_DATA_RATE_300_SPS             0x09
#define ADC18_DATA_RATE_900_SPS             0x0A
#define ADC18_DATA_RATE_1800_SPS            0x0B
#define ADC18_DATA_RATE_2880_SPS            0x0C
#define ADC18_DATA_RATE_5760_SPS            0x0D
#define ADC18_DATA_RATE_11520_SPS           0x0E
#define ADC18_DATA_RATE_23040_SPS           0x0F

#define ADC18_CONV_MODE_CONTINUOUS          0x00
#define ADC18_CONV_MODE_SINGLE_CYCLE        0x02
#define ADC18_CONV_MODE_CONT_SINGLE_CYCLE   0x03

#define ADC18_CH_AI1_SINGLE_ENDED           0x00
#define ADC18_CH_AI2_SINGLE_ENDED           0x01
#define ADC18_CH_AI3_SINGLE_ENDED           0x02
#define ADC18_CH_AI4_SINGLE_ENDED           0x03
#define ADC18_CH_AI5_SINGLE_ENDED           0x04
#define ADC18_CH_AI6_SINGLE_ENDED           0x05
#define ADC18_CH_AI7_SINGLE_ENDED           0x06
#define ADC18_CH_AI8_SINGLE_ENDED           0x07
#define ADC18_CH_AI9_SINGLE_ENDED           0x08
#define ADC18_CH_AI10_SINGLE_ENDED          0x09
#define ADC18_CH_AI11_SINGLE_ENDED          0x0A
#define ADC18_CH_AI12_SINGLE_ENDED          0x0B
#define ADC18_CH_AI1_AI2_DIFFERENTIAL       0x0C
#define ADC18_CH_AI3_AI4_DIFFERENTIAL       0x0D
#define ADC18_CH_AI5_AI6_DIFFERENTIAL       0x0E
#define ADC18_CH_AI7_AI8_DIFFERENTIAL       0x0F
#define ADC18_CH_AI9_AI10_DIFFERENTIAL      0x10
#define ADC18_CH_AI11_AI12_DIFFERENTIAL     0x11

#define ADC18_AIP_TEST_2MOHM_TO_AGND        0x40
#define ADC18_AIP_TEST_2MOHM_TO_HVDD        0x80
#define ADC18_AIP_TEST_2MOHM_TO_HVDD_AGND   0xC0
#define ADC18_AIN_TEST_DISABLED             0x00
#define ADC18_AIN_TEST_2MOHM_TO_AGND        0x10
#define ADC18_AIN_TEST_2MOHM_TO_HVDD        0x20
#define ADC18_AIN_TEST_2MOHM_TO_HVDD_AGND   0x30

#define ADC18_SPI_READ_BIT                  0x01

class ADC18
{
    public:
        typedef struct
        {
            // Output pins
            // DigitalOut rst; /**< Active-Low reset pin. */
            DigitalOut chip_select; /**< Chip select pin descriptor (used for SPI driver). */

            // Input pins
            AnalogIn rdy; /**< Active-Low data ready pin. */
            DigitalIn int_pin; /**< Active-Low interrupt pin. */
        } adc18_t;

        // Constructor
        // ADC18(PinName rdy, PinName rst, PinName chip_select, PinName int_pin, PinName mosi_pin, PinName miso_pin, PinName sck_pin);
        ADC18(PinName rdy, PinName chip_select, PinName int_pin, PinName mosi_pin, PinName miso_pin, PinName sck_pin);

        typedef enum
        {
            ADC18_OK = 0,
            ADC18_ERROR = -1
        } error;
        
        typedef struct {
            float ai1;
            float ai2;
            float ai3;
            float ai4;
            float ai5;
            float ai6;
        } __attribute__((__packed__)) ADCData_6Channel;

        ADC18::ADCData_6Channel getADCData_6Channel();
        ADC18::ADCData_6Channel getADCData_6Channel_continuous();
        float read_ADC_18(uint8_t channel);
        float read_ADC_18_continuous(uint8_t channel);
        int adc18_read_voltage(adc18_t *ctx, float *voltage);
        void adc18_reset_device();
        int adc18_check_communication();
        int adc18_set_conversion_mode(uint8_t mode);
        int adc18_read_register(adc18_t *ctx, uint8_t reg, uint32_t *data_out);
        int adc18_write_register(adc18_t *ctx, uint8_t reg, uint32_t data_in);

    private:

    /*! Variables */
    SPI *spi_p;
    SPI &_spi;
    int _error_flag;
    uint32_t _prod_id;
    adc18_t _adc18;
};

#endif