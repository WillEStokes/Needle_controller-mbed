#ifndef FTSENSOR_H
#define FTSENSOR_H

#include "mbed.h"
#include "EthernetInterface.h"
#include "FT.h"

#define PUMP_ID "FTSensor01"
#define FW_VERSION "1.0"
#define IP_ADDRESS "192.168.5.101"
#define GATEWAY "192.168.5.1"
#define NETWORK_MASK "255.255.255.0"
#define TCP_PORT 7851

class FTSensor {

    /*! List of accepted function IDs (FIDs) */
    enum FID_LIST {
        FID_GET_STATUS = 0,
        FID_GET_SYSTEM_INFO = 1,
        FID_GET_SENSOR_ELEMENT = 2,
        FID_GET_SENSOR_BUFFER = 3,
        FID_START_ACQUISITION = 4,
        FID_STOP_ACQUISITION = 5,
        FID_START_ACQUISITION_STREAM = 6,
        FID_STOP_ACQUISITION_STREAM = 7,
    };
    
    /*! List of message errors */
    enum MSG_LIST {
        MSG_OK = 0,
        MSG_ERROR_NOT_SUPPORTED = 1,
    };
    
    enum BOARD_STATES {
        WAIT_FOR_CONNECTION, // Blinking red LED
        CONNECTED, // Solid red LED
    };

public:
    // Constructor
    FTSensor(
            PinName redLED,
            PinName statusLED );

    void run();

private:
    /*! Type Definitions of the Class */
    typedef void (FTSensor::*messageHandlerFunc)(const void*);
    
    typedef struct {
        uint16_t packetLength;
        uint8_t fid;
        uint8_t error;
    } __attribute__((__packed__)) MessageHeader;

    typedef struct {
        uint8_t fid;
        messageHandlerFunc replyFunc;
    } __attribute__((__packed__)) ComMessage;

    typedef struct {
        MessageHeader header;
        uint8_t boardState;
    } __attribute__((__packed__)) SystemStatus;
        
    typedef struct {
        MessageHeader header;
        char fwVersion[5];
        char boardId[13];
        char ipAddr[14];
        char macAddr[20];
    } __attribute__((__packed__)) SystemInfo;

    typedef struct {
        MessageHeader header;
        FT::FTElement ftElement;
    } __attribute__((__packed__)) SensorElement;

    typedef struct {
        MessageHeader header;
        FT::FTElement values[100];
    } __attribute__((__packed__)) SensorBuffer;
    
    /*! Array of FIDs and corresponding pointers to functions */
    static const ComMessage comMessages[];
    
    /*! Functions */
    void getStatus(const MessageHeader* data);
    void getSystemInfo(const MessageHeader* data);
    void getSensorData(const MessageHeader* data);
    void getSensorBuffer(const MessageHeader* data);
    void startBufferedAcquisition(const MessageHeader* data);
    void stopBufferedAcquisition(const MessageHeader* data);
    void startAcquisitionStream(const MessageHeader* data);
    void stopAcquisitionStream(const MessageHeader* data);
    void streamData();
    void flipStatusLED();
    void setBoardState(int state);
    void initEthernet();
    void comReturn(const void* data, const int errorCode);
    const ComMessage* getComFromHeader(const MessageHeader* header);

    /*! Comms */
    EthernetInterface _eth;
    TCPSocket* _socket;
    TCPSocket _server;
    SocketAddress _clientAddr;

    /*! Instance of ft */
    FT _ft;

    /*! Pins used */
    DigitalOut _redLED;
    DigitalOut _statusLED;

    /*! Constants */
    const int _fidCount;
    const int _msgHeaderLength;
    const float _calibrationMatrix[6][6] = {
        {0.00135, -0.04604, 0.02344, -3.22662, -0.00322, 3.17008},
        {-0.00694, 4.16569, 0.02971, -1.87643, 0.02738, -1.79608},
        {3.69182, 0.02713, 3.83425, -0.03329, 3.65801, -0.067},
        {0.15408, 25.37956, 21.34115, -11.71178, -20.50426, -10.51265},
        {-23.9405, 0.07684, 12.2178, 19.58172, 12.22442, -19.60731},
        {0.09832, 17.49271, -0.32435, 15.76607, -0.09154, 15.24495}
    };

    /*! System info variables */
    SocketAddress _ipAddr;
    
    /*! Variables */
    uint8_t _boardState;
    uint8_t _startIndex;
    uint8_t _endIndex;
    uint8_t _dataElements;
    int _socketBytes;
    float _calibratedValue;
    float _analogValues[6];
    FT::FTBufferPacket _sensorBufferPacket;
    FT::FTElement _data;
    int _queueID;

    /*! Threads & Timing */
    Thread _thread;
    EventQueue _queue;
    
    /*! Tickers */
    Ticker _tickerFTSensor;
};

#endif