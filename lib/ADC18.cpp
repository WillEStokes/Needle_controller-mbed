#include "../debug.h"
#include "ADC18.h"

ADC18::ADC18(PinName rdy, PinName chip_select, PinName int_pin, PinName mosi_pin, PinName miso_pin, PinName sck_pin)
    :
    spi_p(new SPI(mosi_pin, miso_pin, sck_pin)),
    _spi(*spi_p),
    _chip_select(chip_select),
    _rdy(rdy),
    _int_pin(int_pin),
    _samples_to_average(10),
    _num_falling_edges(0),
    _acquire(false),
    _data_rate(ADC18_DATA_RATE_23040_SPS)
    {
    _chip_select = 1; // set chip select high
    _spi.format(8, 0);
    _spi.frequency(30000000);
    adc18_set_conversion_mode(ADC18_CONV_MODE_SINGLE_CYCLE);
}

ADC18::ADCData_6Channel ADC18::getADCData_6Channel()
{
    ADC18::ADCData_6Channel adcData_6Channel;
    _samples_to_average = 1;

    adcData_6Channel.ai1 = read_ADC_18(ADC18_CH_AI1_AI2_DIFFERENTIAL);
    adcData_6Channel.ai2 = read_ADC_18(ADC18_CH_AI3_AI4_DIFFERENTIAL);
    adcData_6Channel.ai3 = read_ADC_18(ADC18_CH_AI5_AI6_DIFFERENTIAL);
    adcData_6Channel.ai4 = read_ADC_18(ADC18_CH_AI7_AI8_DIFFERENTIAL);
    adcData_6Channel.ai5 = read_ADC_18(ADC18_CH_AI9_AI10_DIFFERENTIAL);
    adcData_6Channel.ai6 = read_ADC_18(ADC18_CH_AI11_AI12_DIFFERENTIAL);

    return adcData_6Channel;
}

float ADC18::read_ADC_18(uint8_t channel)
{
    if(ADC18_OK == adc18_write_register(ADC18_REG_GEN_CHNL_CTRL, ((uint32_t) channel << 8) | ((uint32_t) (ADC18_AIP_TEST_2MOHM_TO_AGND | ADC18_AIN_TEST_2MOHM_TO_AGND) << 16))); // set active channel
    {
        adc18_write_register(ADC18_REG_DCHNL_CMD, (uint32_t) (ADC18_START_CONVERSION | _data_rate) << 16); // start conversion

        while(_rdy.read() > 0.0); // wait until ready pin goes low

        adc18_write_register(ADC18_REG_DCHNL_CMD, (uint32_t) ADC18_STOP_CONVERSION << 16); // stop conversion

        float voltage;
        if(ADC18_OK == adc18_read_voltage(&voltage)) // read voltage
        {
            return voltage;
        }
    }
    return -1.0f;
}

void ADC18::check_num_samples()
{
    if (_num_falling_edges >= _samples_to_average - 1) {
        _num_falling_edges = 0;
        _acquire = false;
        return;
    }
    _num_falling_edges++;
}

ADC18::ADCData_6Channel ADC18::getADCData_6Channel_multiple(uint8_t samplesToAverage)
{
    _samples_to_average = samplesToAverage;
    float voltage;
    ADC18::ADCData_6Channel adcData_6Channel;
    ADC18_Channel channel = static_cast<ADC18_Channel>(ADC18_CH_AI1_AI2_DIFFERENTIAL);

    // iterate over each channel
    for (int i = 0; i < 6; i++) {
        
        _mean_voltage = 0.0;

        // set active channel
        if (ADC18_OK == adc18_write_register(ADC18_REG_GEN_CHNL_CTRL, ((uint32_t)channel << 8) | ((uint32_t)(ADC18_AIP_TEST_2MOHM_TO_AGND | ADC18_AIN_TEST_2MOHM_TO_AGND) << 16))) {

            // adc18_write_register(ADC18_REG_DCHNL_CMD, (uint32_t)(ADC18_START_CONVERSION | _data_rate) << 16); // start conversion
            
            _acquire = true;
            while (_acquire) {
                adc18_write_register(ADC18_REG_DCHNL_CMD, (uint32_t)(ADC18_START_CONVERSION | _data_rate) << 16); // start conversion

                while(_rdy.read() > 0.0);

                adc18_write_register(ADC18_REG_DCHNL_CMD, (uint32_t)ADC18_STOP_CONVERSION << 16); // stop conversion

                check_num_samples(); // samples reached?

                if (ADC18_OK == adc18_read_voltage(&voltage)) {
                    _mean_voltage += voltage; // accumulate mean voltage
                }
            }

            // adc18_write_register(ADC18_REG_DCHNL_CMD, (uint32_t)ADC18_STOP_CONVERSION << 16); // stop conversion

            // // read final converted value from DCHNL_DATA
            // if (ADC18_OK == adc18_read_voltage(&voltage)) {
            //     _mean_voltage += voltage; // accumulate mean voltage
            // }

            switch (i) {
                case 0:
                    adcData_6Channel.ai1 = _mean_voltage / _samples_to_average;
                    break;
                case 1:
                    adcData_6Channel.ai2 = _mean_voltage / _samples_to_average;
                    break;
                case 2:
                    adcData_6Channel.ai3 = _mean_voltage / _samples_to_average;
                    break;
                case 3:
                    adcData_6Channel.ai4 = _mean_voltage / _samples_to_average;
                    break;
                case 4:
                    adcData_6Channel.ai5 = _mean_voltage / _samples_to_average;
                    break;
                case 5:
                    adcData_6Channel.ai6 = _mean_voltage / _samples_to_average;
                    break;
                default:
                    break;
            }
        }

        if (i == 5) {
            break;
        }

        channel = static_cast<ADC18_Channel>(channel + 1);
    }

    return adcData_6Channel;
}

int ADC18::adc18_read_voltage(float *voltage)
{
    int32_t raw_adc;
    uint32_t reg_data;
    if (ADC18_OK == adc18_read_register(ADC18_REG_DCHNL_DATA, &reg_data))
    {
        raw_adc = ((int32_t) (reg_data << 8)) >> 8; // reg_data is 24 bit for voltage, pad first 8 bits of raw_adc with MSB of reg_data
        *voltage = (float)raw_adc / ADC18_DATA_RESOLUTION * ADC18_FULL_SCALE_VOLTAGE; // casting to float automatically implements two's complement
        return ADC18_OK;
    }
    return ADC18_ERROR;
}

void ADC18::adc18_reset_device()
{
// analogue input pin on Mikroe Uno shield corresponding to reset cannot be written

//     _adc18.rst.write(0);
//     wait_us(100000);
//     _adc18.rst.write(1);
//     wait_us(1000000);
}

int ADC18::adc18_check_communication()
{
    if (ADC18_OK == adc18_read_register(ADC18_REG_GEN_PROD, &_prod_id))
    {
        _prod_id = (_prod_id >> 16) & 0xFF;
        if(ADC18_PRODUCT_ID == _prod_id)
        {
            D(printf("ADC18: Communication successful\n"));
            return ADC18_OK;
        }
    }
    return ADC18_ERROR;
}

int ADC18::adc18_set_conversion_mode(uint8_t mode)
{
    if (ADC18_OK == adc18_write_register(ADC18_REG_DCHNL_CTRL1, (uint32_t) mode << 16))
        {
            D(printf("ADC18: Conversion mode set to %d\n", mode));
            return ADC18_OK;
        }
    return ADC18_ERROR;
}

void ADC18::adc18_set_data_rate(uint8_t rate)
{
    _data_rate = rate;
    D(printf("ADC18: Data rate set to %d\n", rate));
}

int ADC18::adc18_read_register(uint8_t reg, uint32_t *data_out)
{
    int error_flag = ADC18_OK;
    uint8_t data_buf[5] = {0};
    data_buf[0] = (reg << 1) | ADC18_SPI_READ_BIT;

    // this function takes ~14 us to execute

    _chip_select.write(0); // set chip select low

    if(0 == _spi.write((const char *)&data_buf[0], 1, (char *)&data_buf[1], 4)) // write 1 byte and read 4 bytes
        {error_flag = ADC18_ERROR;} // no bytes written

    _chip_select.write(1); // set chip select high

    // combine 32 bits of data
    *data_out = ((uint32_t)data_buf[1] << 24) | ((uint16_t)data_buf[2] << 16) | ((uint16_t)data_buf[3] << 8) | data_buf[4];
    return error_flag;
}

int ADC18::adc18_write_register(uint8_t reg, uint32_t data_in)
{
    int error_flag = ADC18_OK;
    uint8_t data_buf[5] = {0};
    data_buf[0] = (reg << 1) & (~ADC18_SPI_READ_BIT);
    data_buf[1] = (uint8_t) ((data_in >> 16) & 0xFF);
    data_buf[2] = (uint8_t) ((data_in >> 8) & 0xFF);
    data_buf[3] = (uint8_t) (data_in & 0xFF);

    _chip_select.write(0); // set chip select low

    if(0 == _spi.write((const char *)data_buf, 4, nullptr, 0)) // write 4 bytes then discard read byte
        {error_flag = ADC18_ERROR;} // no bytes written

    _chip_select.write(1); // set chip select high
    return error_flag;
}