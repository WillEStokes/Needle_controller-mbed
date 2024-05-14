# Needle_controller-mbed

## Description

This project provides code to read multi-channel differential data using an mbed K64F board with a Mirkoe ADC 18 Click board. Additionally, it includes code and a library to read encoder data from 3 QEIs. The ADC 18 Click commands are also contained in their own library.

## Requirements

- mbed K64F board
- Mirkoe ADC 18 Click board
- 3 QEIs (Quadrature Encoder Interfaces)

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/your/repo.git
    ```

2. Include the necessary libraries in your project.

## Usage

1. Include the required headers:

    ```cpp
    #include "../debug.h"
    #include "NeedleController.h"
    #include <string.h>
    #include <math.h>
    ```

2. Initialise the NeedleController:

    ```cpp
    NeedleController needleController(PinName redLED, PinName statusLED);
    ```

## API Reference

### NeedleController Class

#### Methods

- `NeedleController(PinName redLED, PinName statusLED)`: Parameterised constructor.
- `void getStatus(const MessageHeader* data)`: Get status.
- `void getSystemInfo(const MessageHeader* data)`: Get system info.
- `void getFTSensorData(const MessageHeader* data)`: Get force/torque sensor data.
- `void getEncoderSensorData(const MessageHeader* data)`: Get encoder sensor data.
- `void getAllSensorData(const MessageHeader* data)`: Get all sensor data.
- `void startAcquisitionStream(const MessageHeader* data)`: Start acquisition stream.
- `void stopAcquisitionStream(const MessageHeader* data)`: Stop acquisition stream.
- `void resetADC(const MessageHeader* data)`: Reset ADC.
- `void checkADC(const MessageHeader* data)`: Check ADC communication.
- `void setADCConversionMode(const MessageHeader* data)`: Set ADC conversion mode.
- `void flipStatusLED()`: Flip status LED.
- `void initEthernet()`: Initialise Ethernet.
- `const ComMessage* getComFromHeader(const MessageHeader* header)`: Get function pointer based on the FID.
- `void comReturn(const void* data, const int errorCode)`: Communication return.
- `void setBoardState(int state)`: Set board state.
- `void run()`: Main function.

#### Constants

- `FID_GET_STATUS`
- `FID_GET_SYSTEM_INFO`
- `FID_GET_FT_SENSOR_DATA`
- `FID_GET_ENCODER_SENSOR_DATA`
- `FID_GET_ALL_SENSOR_DATA`
- `FID_START_ACQUISITION_STREAM`
- `FID_STOP_ACQUISITION_STREAM`
- `FID_RESET_ADC`
- `FID_CHECK_ADC`
- `FID_SET_ADC_CONVERSION_MODE`

## Credits

This project is maintained by Will Stokes.
