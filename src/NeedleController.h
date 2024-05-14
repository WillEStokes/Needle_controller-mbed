#ifndef NEEDLECONTROLLER_H
#define NEEDLECONTROLLER_H

#include "mbed.h"
#include "EthernetInterface.h"
#include "ADC18.h"
#include "QEI.h"

#define ID "NeedleController01"
#define FW_VERSION "1.0"
#define IP_ADDRESS "192.168.5.101"
#define GATEWAY "192.168.5.1"
#define NETWORK_MASK "255.255.255.0"
#define TCP_PORT 7851
#define QEI_SCALE_FACTOR 2.9E-5

class NeedleController {

    /*! List of accepted function IDs (FIDs) */
    enum FID_LIST {
        FID_GET_STATUS = 0,
        FID_GET_SYSTEM_INFO = 1,
        FID_GET_FT_SENSOR_DATA = 2,
        FID_GET_ENCODER_SENSOR_DATA = 3,
        FID_GET_ALL_SENSOR_DATA = 4,
        FID_START_ACQUISITION_STREAM = 5,
        FID_STOP_ACQUISITION_STREAM = 6,
        FID_RESET_ADC = 7,
        FID_CHECK_ADC = 8,
        FID_SET_ADC_CONVERSION_MODE = 9,
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
    NeedleController(
            PinName redLED,
            PinName statusLED );

    void run();

private:
    /*! Type Definitions of the Class */
    typedef void (NeedleController::*messageHandlerFunc)(const void*);
    
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
        char boardId[19];
        char ipAddr[14];
        char macAddr[20];
    } __attribute__((__packed__)) SystemInfo;

    typedef struct {
        MessageHeader header;
        ADC18::ADCData_6Channel adcData_6Channel;
    } __attribute__((__packed__)) FTDataMessage;

    typedef struct {
        float xPos;
        float yPos;
        float zPos;
    } __attribute__((__packed__)) EncoderData;

    typedef struct {
        MessageHeader header;
        EncoderData encoder_data;
    } __attribute__((__packed__)) EncoderDataMessage;

    typedef struct {
        uint32_t time;
        ADC18::ADCData_6Channel adcData_6Channel;
        EncoderData encoder_data;
    } __attribute__((__packed__)) AllData;

    typedef struct {
        MessageHeader header;
        AllData allData;
    } __attribute__((__packed__)) AllDataMessage;
    
    /*! Array of FIDs and corresponding pointers to functions */
    static const ComMessage comMessages[];
    
    /*! Functions */
    void getStatus(const MessageHeader* data);
    void getSystemInfo(const MessageHeader* data);
    void getFTSensorData(const MessageHeader* data);
    void getEncoderSensorData(const MessageHeader* data);
    void getAllSensorData(const MessageHeader* data);
    void startAcquisitionStream(const MessageHeader* data);
    void stopAcquisitionStream(const MessageHeader* data);
    void resetADC(const MessageHeader* data);
    void checkADC(const MessageHeader* data);
    void setADCConversionMode(const MessageHeader* data);
    void getFTDataThread();
    void getEncoderDataThread();
    void streamData();
    void clearAllData(AllData* allData);
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

    /*! Class instances */
    ADC18 _adc18_FT;
    QEI _qei_x;
    QEI _qei_y;
    QEI _qei_z;

    /*! Pins used */
    DigitalOut _redLED;
    DigitalOut _statusLED;

    /*! Constants */
    const int _fidCount;
    const int _msgHeaderLength;

    /*! System info variables */
    SocketAddress _ipAddr;
    
    /*! Variables */
    uint8_t _boardState;
    uint32_t _startTime;
    int _socketBytes;
    int _queueID;
    ADC18::ADCData_6Channel _adcData_6Channel;

    /*! Threads & Timing */
    Thread _thread;
    EventQueue _queue;
    
    /*! Tickers */
    Ticker _tickerNeedleController;
};

#endif