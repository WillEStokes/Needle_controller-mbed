# Needle_controller-mbed
Mbed K64F with Mikroe ADC 18 Click!

## Description

Read multi-channel differential data using an mbed K64F board with a Mikroe ADC 18 Click board. An ADC18 library file is included which is adapted from the official Mikroe source code. The pins used to construct an instance of the ADC18 class correspond to the pins for position 1 of the Mikroe Arduino Uno Click Shield (MIKROE-1581). With the exception of the ready pin which is ported to a digital pin for greater determinism during falling edge detection. A library file is also included to simultaneously read encoder data with QEI.

## Requirements

- mbed K64F board
- Mikroe Arduino Uno Click shield
- Mikroe ADC 18 Click board

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/WillEStokes/Needle_controller-mbed.git
    ```

3. Include the necessary libraries in your project.

## Usage

1. To initialise an instance of `NeedleController`:

To use this driver, you need to create an instance of the `NeedleController` class, passing the pin names for the red LED and status LED to the constructor.

    ```cpp
    NeedleController needleController(PinName redLED, PinName statusLED);
    ```

Then, you can use the methods provided by the `NeedleController` class to interact with the device.

    ```cpp
    needleController.run();
    ```

2. To initialise an instance of `ADC18`:

To use this driver, you need to create an instance of the `ADC18` class, passing the pin names for the ready signal, chip select, interrupt pin, MOSI (Master Out Slave In), MISO (Master In Slave Out), and SCK (Serial Clock) to the constructor.

    ```cpp
    ADC18 adc18(PinName rdy, PinName chip_select, PinName int_pin, PinName mosi_pin, PinName miso_pin, PinName sck_pin);
    ```

Then, you can use the methods provided by the `ADC18` class to interact with the ADC.

## Requirements

This driver requires the mbed.h library.

## API Reference

## NeedleController Class

This is a C++ driver for a needle insertion test system. As part of the system, this NeedleController device interfaces with an 18-bit Analog-to-Digital Converter (ADC) and a Quadrature Encoder Interface (QEI). The driver provides methods for getting system status, sensor data, starting and stopping data acquisition, and configuring the ADC.

### Class Definition

### The `NeedleController` class provides the following:

#### Enumerations

- `FID_LIST`: A list of accepted function IDs (FIDs) for various operations.
- `MSG_LIST`: A list of message errors.
- `BOARD_STATES`: A list of board states.

#### Methods

The class provides several methods for getting system status, system info, force-torque sensor data, encoder sensor data, all sensor data, starting and stopping data acquisition, resetting the ADC, checking the ADC, setting the ADC conversion mode, and setting the ADC data rate.

- `NeedleController(PinName redLED, PinName statusLED)`: Parameterised constructor. Initializes a new instance of the NeedleController class with specified LED pins.
- `void getStatus(const MessageHeader* data)`: Retrieves the status of the device.
- `void getSystemInfo(const MessageHeader* data)`: Retrieves the system information.
- `void getFTSensorData(const MessageHeader* data)`: Retrieves the Force-Torque sensor data.
- `void getEncoderSensorData(const MessageHeader* data)`: Retrieves the encoder sensor data.
- `void getAllSensorData(const MessageHeader* data)`: Retrieves all sensor data.
- `void getAllSensorDataMultiple(const Settings* data)`: Retrieves all sensor data multiple times based on the settings.
- `void startAcquisitionStream(const MessageHeader* data)`: Starts the acquisition stream.
- `void stopAcquisitionStream(const MessageHeader* data)`: Stops the acquisition stream.
- `void resetADC(const MessageHeader* data)`: Resets the ADC (Analog-to-Digital Converter).
- `void checkADC(const MessageHeader* data)`: Checks the status of the ADC.
- `void setADCConversionMode(const Settings* data)`: Sets the conversion mode of the ADC.
- `void setADCDataRate(const Settings* data)`: Sets the data rate of the ADC.
- `void streamData()`: Streams the data.
- `void clearAllData(AllData* allData)`: Clears all data.
- `void flipStatusLED()`: Toggles the status LED.
- `void setBoardState(int state)`: Sets the state of the board.
- `void initEthernet()`: Initializes the Ethernet connection.
- `void comReturn(const void* data, const int errorCode)`: Returns communication data with an error code.
- `const ComMessage* getComFromHeader(const MessageHeader* header)`: Retrieves the communication message from the header.

#### Function IDs

- `FID_GET_STATUS = 0`: Used to get the status of the device.
- `FID_GET_SYSTEM_INFO = 1`: Used to get the system information.
- `FID_GET_FT_SENSOR_DATA = 2`: Used to get the Force-Torque sensor data.
- `FID_GET_ENCODER_SENSOR_DATA = 3`: Used to get the encoder sensor data.
- `FID_GET_ALL_SENSOR_DATA = 4`: Used to get all sensor data.
- `FID_GET_ALL_SENSOR_DATA_MULTIPLE = 5`: Used to get all sensor data multiple times.
- `FID_START_ACQUISITION_STREAM = 6`: Used to start the acquisition stream.
- `FID_STOP_ACQUISITION_STREAM = 7`: Used to stop the acquisition stream.
- `FID_RESET_ADC = 8`: Used to reset the ADC (Analog-to-Digital Converter).
- `FID_CHECK_ADC = 9`: Used to check the status of the ADC.
- `FID_SET_ADC_CONVERSION_MODE = 10`: Used to set the conversion mode of the ADC.
- `FID_SET_ADC_DATA_RATE = 11`: Used to set the data rate of the ADC.

#### Requirements

This driver requires the mbed.h, EthernetInterface.h, ADC18.h, and QEI.h libraries.

## ADC18 Driver Class

This is a C++ driver for an 18-bit Analog-to-Digital Converter (ADC), specifically the ADC18 model. This driver allows you to interact with the ADC, providing methods for setting the conversion mode, setting the data rate, resetting the device, and reading the ADC data.

#### Features

- Set conversion mode
- Set data rate
- Reset device
- Read ADC data

#### Methods

The driver provides several methods for reading the ADC, checking the number of samples, reading the voltage, reading a register, and writing to a register.

- `ADC18::ADCData_6Channel getADCData_6Channel()`: Retrieves the 6-channel ADC data.
- `ADC18::ADCData_6Channel getADCData_6Channel_multiple(uint8_t samplesToAverage)`: Retrieves the 6-channel ADC data multiple times, averaging the specified number of samples.
- `int adc18_check_communication()`: Checks the communication with the ADC18 device.
- `int adc18_set_conversion_mode(uint8_t mode)`: Sets the conversion mode of the ADC18 device.
- `void adc18_set_data_rate(uint8_t rate)`: Sets the data rate of the ADC18 device.
- `void adc18_reset_device()`: Resets the ADC18 device.

## Credits

This project is maintained by Will Stokes.

## License

This driver is released under the MIT License.
