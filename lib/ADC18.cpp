#include "ADC18.h"

// ADC18::ADC18(PinName rdy, PinName rst, PinName chip_select, PinName int_pin, PinName mosi_pin, PinName miso_pin, PinName sck_pin)
ADC18::ADC18(PinName rdy, PinName chip_select, PinName int_pin, PinName mosi_pin, PinName miso_pin, PinName sck_pin)
    :
    spi_p(new SPI(mosi_pin, miso_pin, sck_pin)),
    _spi(*spi_p),
    // _adc18((adc18_t) {DigitalOut(rst), DigitalOut(chip_select), AnalogIn(rdy), DigitalIn(int_pin)})
    _adc18((adc18_t) {DigitalOut(chip_select), AnalogIn(rdy), DigitalIn(int_pin)})
    {
    // _adc18.rst = 1; // set reset high
    _adc18.chip_select = 1; // set chip select high
    _spi.format(8, 0);
    _spi.frequency(30000000);
    // adc18_set_conversion_mode(ADC18_CONV_MODE_CONTINUOUS);
}

ADC18::ADCData_6Channel ADC18::getADCData_6Channel() {
    ADC18::ADCData_6Channel adcData_6Channel;

    adcData_6Channel.ai1 = read_ADC_18(ADC18_CH_AI1_AI2_DIFFERENTIAL);
    adcData_6Channel.ai2 = read_ADC_18(ADC18_CH_AI3_AI4_DIFFERENTIAL);
    adcData_6Channel.ai3 = read_ADC_18(ADC18_CH_AI5_AI6_DIFFERENTIAL);
    adcData_6Channel.ai4 = read_ADC_18(ADC18_CH_AI7_AI8_DIFFERENTIAL);
    adcData_6Channel.ai5 = read_ADC_18(ADC18_CH_AI9_AI10_DIFFERENTIAL);
    adcData_6Channel.ai6 = read_ADC_18(ADC18_CH_AI11_AI12_DIFFERENTIAL);

    return adcData_6Channel;
}

ADC18::ADCData_6Channel ADC18::getADCData_6Channel_continuous() {
    ADC18::ADCData_6Channel adcData_6Channel;

    adcData_6Channel.ai1 = read_ADC_18_continuous(ADC18_CH_AI1_AI2_DIFFERENTIAL);
    adcData_6Channel.ai2 = read_ADC_18_continuous(ADC18_CH_AI3_AI4_DIFFERENTIAL);
    adcData_6Channel.ai3 = read_ADC_18_continuous(ADC18_CH_AI5_AI6_DIFFERENTIAL);
    adcData_6Channel.ai4 = read_ADC_18_continuous(ADC18_CH_AI7_AI8_DIFFERENTIAL);
    adcData_6Channel.ai5 = read_ADC_18_continuous(ADC18_CH_AI9_AI10_DIFFERENTIAL);
    adcData_6Channel.ai6 = read_ADC_18_continuous(ADC18_CH_AI11_AI12_DIFFERENTIAL);

    return adcData_6Channel;
}

float ADC18::read_ADC_18(uint8_t channel)
{
    if(ADC18_OK == adc18_write_register(&_adc18, ADC18_REG_GEN_CHNL_CTRL, ((uint32_t) channel << 8) | ((uint32_t) (ADC18_AIP_TEST_2MOHM_TO_AGND | ADC18_AIN_TEST_2MOHM_TO_AGND) << 16))); // set active channel
    {
        adc18_write_register(&_adc18, ADC18_REG_DCHNL_CMD, (uint32_t) (ADC18_START_CONVERSION | ADC18_DATA_RATE_11520_SPS) << 16); // start conversion

        while(_adc18.rdy.read() > 0.0); // wait until ready pin goes low

        adc18_write_register(&_adc18, ADC18_REG_DCHNL_CMD, (uint32_t) ADC18_STOP_CONVERSION << 16); // stop conversion

        float voltage;
        if(ADC18_OK == adc18_read_voltage(&_adc18, &voltage)) // read voltage
        {
            return voltage;
        }
    }
    return -1.0f;
}

float ADC18::read_ADC_18_continuous(uint8_t channel)
{
    if(ADC18_OK == adc18_write_register(&_adc18, ADC18_REG_GEN_CHNL_CTRL, ((uint32_t) channel << 8) | ((uint32_t) (ADC18_AIP_TEST_2MOHM_TO_AGND | ADC18_AIN_TEST_2MOHM_TO_AGND) << 16))); // set active channel
    {
        adc18_write_register(&_adc18, ADC18_REG_DCHNL_CMD, (uint32_t) (ADC18_START_CONVERSION | ADC18_DATA_RATE_23040_SPS) << 16); // start conversion

        // this function takes ~1000 us to execute

        int j = 0;
        float mean_voltage = 0.0;
        float start_time = 0;
        float total_time = 0;
        int samples_to_average = 10;
        for(int i = 0; i < samples_to_average - 1; i++)
        {
            if (j == 8) {
                start_time = us_ticker_read();
            }

            // ready pin takes ~100 us to go low but should only take ~8 us at the fastest data rate
            while(_adc18.rdy.read() > 0.0); // wait until ready pin goes low

            if (j == 8) {
                total_time = us_ticker_read() - start_time;
            }
            j += 1;

            float voltage;
            if(ADC18_OK == adc18_read_voltage(&_adc18, &voltage)) // read converted value from ADC18_REG_DCHNL_DATA
            {
                mean_voltage += voltage;
            }
        }

        adc18_write_register(&_adc18, ADC18_REG_DCHNL_CMD, (uint32_t) ADC18_STOP_CONVERSION << 16); // stop conversion

        float voltage;
        if(ADC18_OK == adc18_read_voltage(&_adc18, &voltage)) // read final converted value from ADC18_REG_DCHNL_DATA
        {
            mean_voltage += voltage;
        }

        return total_time;
        // return mean_voltage / samples_to_average;
    }
}

int ADC18::adc18_read_voltage(adc18_t *ctx, float *voltage)
{
    int32_t raw_adc;
    uint32_t reg_data;
    if (ADC18_OK == adc18_read_register(ctx, ADC18_REG_DCHNL_DATA, &reg_data))
    {
        raw_adc = ((int32_t) (reg_data << 8)) >> 8; // reg_data is 24 bit for voltage, pad first 8 bits of raw_adc with MSB of reg_data
        *voltage = (float)raw_adc / ADC18_DATA_RESOLUTION * ADC18_FULL_SCALE_VOLTAGE; // casting to float automatically implements two's complement
        return ADC18_OK;
    }
    return ADC18_ERROR;
}

void ADC18::adc18_reset_device()
{
//     _adc18.rst.write(0);
//     wait_us(100000);
//     _adc18.rst.write(1);
//     wait_us(1000000);
}

int ADC18::adc18_check_communication()
{
    // uint32_t prod_id;
    if (ADC18_OK == adc18_read_register(&_adc18, ADC18_REG_GEN_PROD, &_prod_id))
    {
        _prod_id = (_prod_id >> 16) & 0xFF;
        if(ADC18_PRODUCT_ID == _prod_id)
        {
            return ADC18_OK;
        }
    }
    return ADC18_ERROR;
}

int ADC18::adc18_set_conversion_mode(uint8_t mode)
{
    if (ADC18_OK == adc18_write_register(&_adc18, ADC18_REG_DCHNL_CTRL1, (uint32_t) mode << 16))
        {
            return ADC18_OK;
        }
    return ADC18_ERROR;
}

int ADC18::adc18_read_register(adc18_t *ctx, uint8_t reg, uint32_t *data_out)
{
    int error_flag = ADC18_OK;
    uint8_t data_buf[5] = {0};
    data_buf[0] = (reg << 1) | ADC18_SPI_READ_BIT;

    // this function takes ~14 us to execute

    ctx->chip_select.write(0); // set chip select low

    if(0 == _spi.write((const char *)&data_buf[0], 1, (char *)&data_buf[1], 4)) // write 1 byte and read 4 bytes
        {error_flag = ADC18_ERROR;} // no bytes written

    ctx->chip_select.write(1); // set chip select high

    // combine 32 bits of data
    *data_out = ((uint32_t)data_buf[1] << 32) | ((uint16_t)data_buf[2] << 16) | ((uint16_t)data_buf[3] << 8) | data_buf[4];
    return error_flag;
}

int ADC18::adc18_write_register(adc18_t *ctx, uint8_t reg, uint32_t data_in)
{
    int error_flag = ADC18_OK;
    uint8_t data_buf[5] = {0};
    data_buf[0] = (reg << 1) & (~ADC18_SPI_READ_BIT);
    data_buf[1] = (uint8_t) ((data_in >> 16) & 0xFF);
    data_buf[2] = (uint8_t) ((data_in >> 8) & 0xFF);
    data_buf[3] = (uint8_t) (data_in & 0xFF);

    ctx->chip_select.write(0); // set chip select low

    if(0 == _spi.write((const char *)data_buf, 4, nullptr, 0)) // write 4 bytes then discard read byte
        {error_flag = ADC18_ERROR;} // no bytes written

    ctx->chip_select.write(1); // set chip select high
    return error_flag;
}