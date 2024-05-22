# Needle_controller-mbed

Mbed FRDM K64F with Mikroe ADC 18 Click!

Read multi-channel differential data using an mbed FRDM K64F board with a Mikroe ADC 18 Click board.

This is a C++ driver for an mbed FRDM K64F device which handles all the sensor measurements in a bespoke laboratory needle-insertion test system. However it can be adapted for other applications requiring reading multiple analogue input channels with wide voltage-range (+/- 10 V) at fast data rates, and/or reading multiple encoder values.

The driver is designed to be slave to a host device which sends and receives commands via Ethernet TCP/IP. See `Needle_controller_tester.py` in the project root for an example of how to establish a connection and parse commands.

## Description

A parent `NeedleController` class interfaces with a 24-bit Analog-to-Digital Converter (ADC) via the `ADC18` class and a Quadrature Encoder Interface (QEI) via the `QEI` class.

In the parent `NeedleController` class, the pins used to construct an instance of the `ADC18` class correspond to the pins for position 1 of the Mikroe Arduino Uno Click Shield. With the exception of the ready pin `rdy` which is ported to a digital pin by a simple external circuit, for greater determinism during falling edge detection.

The `ADC18` class file is adapted from the [official Mikroe source code](https://libstock.mikroe.com/projects/view/4951/adc-18-click).

A `QEI` class file is also included to simultaneously read encoder data with QEI [documented here](https://os.mbed.com/users/aberk/code/QEI/).

## Hardware Requirements

- mbed FRDM K64F board
- Mikroe Arduino Uno Click Shield (MIKROE-1581)
- Mikroe ADC 18 Click board (MIKROE-5132)

## Installation

Clone the repository:

```bash
git clone https://github.com/WillEStokes/Needle_controller-mbed.git
```

## Compilation

The project uses an offline version of the mbed library which is configured for the FRDM K64F target using the instruction in the [mbed-cmake](https://github.com/USCRPL/mbed-cmake) repo.

To compile the project:

1. Get CMake by downloading the installer from [here](https://cmake.org/download/)
2. Get the latest release of Ninja by downloading the exe from [here](https://github.com/ninja-build/ninja/releases)
3. If necessary, update PATH variables so the CMake and ninja are visible on your system
4. From VS Code, cd into the build directory from the project root and run 'ninja' in the command console

## API Reference

### 1. NeedleController Class

The device flashed with this driver interfaces with a 24-bit Analog-to-Digital Converter (ADC) and a Quadrature Encoder Interface (QEI). The driver provides methods for getting system status, sensor data, starting and stopping data acquisition, and configuring the ADC.

#### Usage

To use this driver, you need to create an instance of the `NeedleController` class, passing the pin names for the red LED and status LED to the constructor.

```cpp
NeedleController needleController(PinName redLED, PinName statusLED);
```

Then, you can use the methods provided by the `NeedleController` class to interact with the device.

```cpp
needleController.run();
```

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

### 2. ADC18 Driver Class

This is a C++ driver for a 24-bit Analog-to-Digital Converter (ADC), specifically the ADC18 model. This driver allows you to interact with the ADC, providing methods for setting the conversion mode, setting the data rate, resetting the device, and reading the ADC data.

#### Features

- Set conversion mode
- Set data rate
- Reset device
- Read ADC data

#### Usage

To use this driver, you need to create an instance of the `ADC18` class, passing the pin names for the ready signal, chip select, interrupt pin, MOSI (Master Out Slave In), MISO (Master In Slave Out), and SCK (Serial Clock) to the constructor.

```cpp
ADC18 adc18(PinName rdy, PinName chip_select, PinName int_pin, PinName mosi_pin, PinName miso_pin, PinName sck_pin);
```

Then, you can use the methods provided by the `ADC18` class to interact with the ADC.

#### Methods

The driver provides several methods for reading the ADC, checking the number of samples, reading the voltage, reading a register, and writing to a register.

- `ADC18::ADCData_6Channel getADCData_6Channel()`: Retrieves the 6-channel ADC data.
- `ADC18::ADCData_6Channel getADCData_6Channel_multiple(uint8_t samplesToAverage)`: Retrieves the 6-channel ADC data multiple times, averaging the specified number of samples.
- `int adc18_check_communication()`: Checks the communication with the ADC18 device.
- `int adc18_set_conversion_mode(uint8_t mode)`: Sets the conversion mode of the ADC18 device.
- `void adc18_set_data_rate(uint8_t rate)`: Sets the data rate of the ADC18 device.
- `void adc18_reset_device()`: Resets the ADC18 device.

#### Requirements

This driver requires the mbed.h library.

## Credits

This project is maintained by Will Stokes.

## License

This driver is released under the [MIT License](https://opensource.org/licenses/MIT).
