#include "../debug.h"
#include "FTSensor.h"
#include <string.h>
#include <math.h>

/*! Initialise list of responding functions */
const FTSensor::ComMessage FTSensor::comMessages[] = {
    {FID_GET_STATUS, (FTSensor::messageHandlerFunc)&FTSensor::getStatus},
    {FID_GET_SYSTEM_INFO, (FTSensor::messageHandlerFunc)&FTSensor::getSystemInfo},
    {FID_GET_SENSOR_ELEMENT, (FTSensor::messageHandlerFunc)&FTSensor::getSensorData},
    {FID_GET_SENSOR_BUFFER, (FTSensor::messageHandlerFunc)&FTSensor::getSensorBuffer},
    {FID_START_ACQUISITION, (FTSensor::messageHandlerFunc)&FTSensor::startBufferedAcquisition},
    {FID_STOP_ACQUISITION, (FTSensor::messageHandlerFunc)&FTSensor::stopBufferedAcquisition},
    {FID_START_ACQUISITION_STREAM, (FTSensor::messageHandlerFunc)&FTSensor::startAcquisitionStream},
    {FID_STOP_ACQUISITION_STREAM, (FTSensor::messageHandlerFunc)&FTSensor::stopAcquisitionStream},
};

/*! Parameterised constructor */
FTSensor::FTSensor(
        PinName redLED,
        PinName statusLED )
        :
        _redLED(redLED),
        _statusLED(statusLED),
        _ft(D10, D13, D12, D11, PTB3, PTB10, PTC11, PTC10),
        // _ft(),
        _fidCount(sizeof (comMessages) / sizeof (ComMessage)), // constant
        _msgHeaderLength(sizeof (MessageHeader)) { //
    
    // Turn LED ON by default
    _redLED = 0;
    _statusLED = 1;
    _calibratedValue = 0.0;
    _startIndex = 0;
    _endIndex = 0;
    _dataElements = 0;
    _thread.start(callback(&_queue, &EventQueue::dispatch_forever));
    // _sensorBufferPacket = _ft.initBuffer();
}

/*! Get status */
void FTSensor::getStatus(const MessageHeader* data) { 
    static SystemStatus status; // static is needed to avoid memory allocation every time the function is called
    
    status.header.packetLength = sizeof(SystemStatus);
    status.header.fid = FID_GET_STATUS;
    
    status.boardState = _boardState;
    
    _socket->send((char*) &status, sizeof(SystemStatus));
}

/*! Get system info */
void FTSensor::getSystemInfo(const MessageHeader* data) {
    static SystemInfo systemInfo;
    
    systemInfo.header.packetLength = sizeof(SystemInfo);
    systemInfo.header.fid = FID_GET_SYSTEM_INFO;
    
    strcpy(systemInfo.fwVersion, FW_VERSION);
    strcpy(systemInfo.boardId, PUMP_ID);
    strcpy(systemInfo.ipAddr, _ipAddr.get_ip_address());
    strcpy(systemInfo.macAddr, _eth.get_mac_address());
    
    _socket->send((char*) &systemInfo, sizeof(SystemInfo));
}

void FTSensor::getSensorData(const MessageHeader* data) { 
    static SensorElement sensorElement; // static is needed to avoid memory allocation every time the function is called
    sensorElement.header.packetLength = sizeof(sensorElement);
    sensorElement.header.fid = FID_GET_SENSOR_ELEMENT;
    sensorElement.ftElement = _ft.getFTElement();

    // Thread* threadFT = new Thread(osPriorityNormal, 2048);
    // threadFT->start(callback(this, &FTSensor::getFTDataThread));

    // threadFT->join();

    // delete threadFT;

    // FT::FTElement ftElement = _ft.getFTElement();
    // float _analogValues[6] = {ftElement.xForce, ftElement.yForce, ftElement.zForce, 
    //                           ftElement.xTorque, ftElement.yTorque, ftElement.zTorque};

    // // Apply calibration matrix
    // for (int i = 0; i < 6; i++) {
    //     _calibratedValue = 0.0f;
    //     for (int j = 0; j < 6; j++) {
    //         _calibratedValue += _analogValues[j] * _calibrationMatrix[i][j];
    //     }
    //     switch (i) {
    //         case 0:
    //             sensorElement.ftElement.xForce = _calibratedValue;
    //             break;
    //         case 1:
    //             sensorElement.ftElement.yForce = _calibratedValue;
    //             break;
    //         case 2:
    //             sensorElement.ftElement.zForce = _calibratedValue;
    //             break;
    //         case 3:
    //             sensorElement.ftElement.xTorque = _calibratedValue;
    //             break;
    //         case 4:
    //             sensorElement.ftElement.yTorque = _calibratedValue;
    //             break;
    //         case 5:
    //             sensorElement.ftElement.zTorque = _calibratedValue;
    //             break;
    //     }
    // }
    
    _socket->send((char*) &sensorElement, sizeof(SensorElement));
}

// void FTSensor::getSensorBuffer(const MessageHeader* data) { 
//     static SensorBuffer sensorBuffer; // static is needed to avoid memory allocation every time the function is called
    
//     sensorBuffer.header.packetLength = sizeof(SensorBuffer);
//     sensorBuffer.header.fid = FID_GET_SENSOR_BUFFER;
//     sensorBuffer.values = _ft.getFTBuffer().values;
    
//     _socket->send((char*) &sensorBuffer, sizeof(SensorBuffer));
// }

void FTSensor::getSensorBuffer(const MessageHeader* data) { 
    static SensorBuffer sensorBuffer; // static is needed to avoid memory allocation every time the function is called
    sensorBuffer.header.fid = FID_GET_SENSOR_BUFFER;
    
    _sensorBufferPacket = _ft.getFTBuffer();
    _startIndex = _sensorBufferPacket.startIndex;
    _endIndex = _sensorBufferPacket.endIndex;
    _dataElements = _endIndex - _startIndex + 1;

    sensorBuffer.header.packetLength = sizeof(sensorBuffer.header) + _dataElements * sizeof(FT::FTElement);

    memcpy(&sensorBuffer.values, &_sensorBufferPacket.values.element[_startIndex], _dataElements * sizeof(FT::FTElement));

    _socket->send((char*) &sensorBuffer, sensorBuffer.header.packetLength);
}

void FTSensor::startBufferedAcquisition(const MessageHeader* data) {
    _ft.readData();
    // _ft.startAcquisition();
}

void FTSensor::stopBufferedAcquisition(const MessageHeader* data) {
    _ft.stopAcquisition();
}

void FTSensor::startAcquisitionStream(const MessageHeader* data) {
    streamData();
}

void FTSensor::streamData() {
    _queueID = _queue.call_in(5ms, callback(this, &FTSensor::streamData));
    static FT::FTElement ftElement; // static is needed to avoid memory allocation every time the function is called
    // ftElement = _ft.getFTElement();

    for (int i = 1; i < 25; ++i) {
        _data = _ft.getFTElement();
        ftElement.xForce += _data.xForce;
        ftElement.yForce += _data.yForce;
        ftElement.zForce += _data.zForce;
        ftElement.xTorque += _data.xTorque;
        ftElement.yTorque += _data.yTorque;
        ftElement.zTorque += _data.zTorque;
    }

    ftElement.xForce = ftElement.xForce / 25.0f;
    ftElement.yForce = ftElement.yForce / 25.0f;
    ftElement.zForce = ftElement.zForce / 25.0f;
    ftElement.xTorque = ftElement.xTorque / 25.0f;
    ftElement.yTorque = ftElement.yTorque / 25.0f;
    ftElement.zTorque = ftElement.zTorque / 25.0f;

    _socket->send((char*) &ftElement, sizeof(FT::FTElement));
}

void FTSensor::stopAcquisitionStream(const MessageHeader* data) {
    _queue.cancel(_queueID);
}

/*! LED Functions */
void FTSensor::flipStatusLED() {
    _redLED = !_redLED;
    _statusLED = !_statusLED;
}

/*! Initialising Ethernet */
void FTSensor::initEthernet() {
    // Set static IP
    _eth.set_network(IP_ADDRESS, NETWORK_MASK, GATEWAY);

    // Bring up the ethernet interface
    _eth.connect();

    // Show the network address
    _eth.get_ip_address(&_ipAddr);

    // Open a socket on the network interface, and create a TCP connection to mbed.org
    _server.open(&_eth);
    _server.bind(7851);
    _server.listen(1);
    _server.set_blocking(true);
    _server.set_timeout(-1);
}

/*! Getting a function pointer based on the FID */
const FTSensor::ComMessage* FTSensor::getComFromHeader(const MessageHeader* header) {

    if (header->fid >= _fidCount) { //Prevent getting out of an array
        return NULL;
    }

    return &comMessages[header->fid];
}

void FTSensor::comReturn(const void* data, const int errorCode) {
    MessageHeader *message = (MessageHeader*) data;
    message->packetLength = _msgHeaderLength;
    message->error = errorCode;
    _socket->send((char*) message, _msgHeaderLength);
}

/*! Setting the pump state */
void FTSensor::setBoardState(int state) {
    
    _boardState = state;
    
    switch(_boardState) {
        case WAIT_FOR_CONNECTION:
            // Blink status LED
            _tickerFTSensor.attach(callback(this, &FTSensor::flipStatusLED), 500ms);
            break;
        case CONNECTED:
            // Solid status LED
            _tickerFTSensor.detach();
            _redLED = 0;
            _statusLED = 1;
            break;
        default:
            _tickerFTSensor.detach();
            _redLED = 0;
            _statusLED = 1;
            break;
    }
}

/*! Main function */
void FTSensor::run() {
    // Indicate initialising state of a system
    setBoardState(WAIT_FOR_CONNECTION);
    
    initEthernet();

    // Initialise data buffer (receive)
    char data[256];
    // Create pointer to header data
    MessageHeader* header;

    while (true) {
        // Indicate state of a system
        setBoardState(WAIT_FOR_CONNECTION);
        
        _socket = _server.accept();
        _socket->getpeername(&_clientAddr);
        // _socket->set_timeout(1000);
        
        // Indicate state of a system
        setBoardState(CONNECTED);
        
        while(true) {
            // Wait for a header
            _socketBytes = _socket->recv(data, _msgHeaderLength);
            if (_socketBytes <= 0) break;
            
            // if (_socketBytes > 0) {
            header = (MessageHeader*)data;
            
            if (header->packetLength != _msgHeaderLength) {
                if (_socket->recv(data + _msgHeaderLength, header->packetLength - _msgHeaderLength) <= 0) break;
            }
            
            const ComMessage* comMessage = getComFromHeader(header);
            
            if(comMessage != NULL && comMessage->replyFunc != NULL) {
                // Fact: comMessage->fid is equivalent to (*comMessage).fid
                (this->*comMessage->replyFunc)((void*)data);
            } else {
                comReturn(data, MSG_ERROR_NOT_SUPPORTED);
            }
        }
        
        // Client disconnected
        _socket->close();
        
        // Indicate disconnected state
        setBoardState(WAIT_FOR_CONNECTION);
    }
}