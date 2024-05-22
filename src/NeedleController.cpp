#include "../debug.h"
#include "NeedleController.h"
#include <string.h>
#include <math.h>

/*! Initialise list of responding functions */
const NeedleController::ComMessage NeedleController::comMessages[] = {
    {FID_GET_STATUS, (NeedleController::messageHandlerFunc)&NeedleController::getStatus},
    {FID_GET_SYSTEM_INFO, (NeedleController::messageHandlerFunc)&NeedleController::getSystemInfo},
    {FID_GET_FT_SENSOR_DATA, (NeedleController::messageHandlerFunc)&NeedleController::getFTSensorData},
    {FID_GET_ENCODER_SENSOR_DATA, (NeedleController::messageHandlerFunc)&NeedleController::getEncoderSensorData},
    {FID_GET_ALL_SENSOR_DATA, (NeedleController::messageHandlerFunc)&NeedleController::getAllSensorData},
    {FID_GET_ALL_SENSOR_DATA_MULTIPLE, (NeedleController::messageHandlerFunc)&NeedleController::getAllSensorDataMultiple},
    {FID_START_ACQUISITION_STREAM, (NeedleController::messageHandlerFunc)&NeedleController::startAcquisitionStream},
    {FID_STOP_ACQUISITION_STREAM, (NeedleController::messageHandlerFunc)&NeedleController::stopAcquisitionStream},
    {FID_RESET_ADC, (NeedleController::messageHandlerFunc)&NeedleController::resetADC},
    {FID_CHECK_ADC, (NeedleController::messageHandlerFunc)&NeedleController::checkADC},
    {FID_SET_ADC_CONVERSION_MODE, (NeedleController::messageHandlerFunc)&NeedleController::setADCConversionMode},
    {FID_SET_ADC_DATA_RATE, (NeedleController::messageHandlerFunc)&NeedleController::setADCDataRate},
};

BufferedSerial NeedleController::pc(USBTX, USBRX, 9600);

/*! Parameterised constructor */
NeedleController::NeedleController(
        PinName redLED,
        PinName statusLED )
        :
        _redLED(redLED),
        _statusLED(statusLED),
        _adc18_FT(
                // PTB2,   // rdy
                D7,     // rdy
                D10,    // chip_select
                D2,     // int_pin
                D11,    // mosi
                D12,    // miso
                D13 ),  // sck
        _qei_x(D0, D1, NC, QEI::X4_ENCODING),
        _qei_y(D3, D5, NC, QEI::X4_ENCODING),
        _qei_z(D9, D6, NC, QEI::X4_ENCODING),
        _fidCount(sizeof (comMessages) / sizeof (ComMessage)), // constant
        _msgHeaderLength(sizeof (MessageHeader)) {
    // turn LED ON by default
    _redLED = 0;
    _statusLED = 1;
    _thread.start(callback(&_queue, &EventQueue::dispatch_forever));
}

/*! Get status */
void NeedleController::getStatus(const MessageHeader* data) { 
    static SystemStatus status; // static is needed to avoid memory allocation every time the function is called
    
    status.header.packetLength = sizeof(SystemStatus);
    status.header.fid = FID_GET_STATUS;
    
    status.boardState = _boardState;
    
    _socket->send((char*) &status, sizeof(SystemStatus));
}

/*! Get system info */
void NeedleController::getSystemInfo(const MessageHeader* data) { 
    static SystemInfo systemInfo;
    
    systemInfo.header.packetLength = sizeof(SystemInfo);
    systemInfo.header.fid = FID_GET_SYSTEM_INFO;
    
    strcpy(systemInfo.fwVersion, FW_VERSION);
    strcpy(systemInfo.boardId, ID);
    strcpy(systemInfo.ipAddr, _ipAddr.get_ip_address());
    strcpy(systemInfo.macAddr, _eth.get_mac_address());
    
    _socket->send((char*) &systemInfo, sizeof(SystemInfo));
}

void NeedleController::getFTSensorData(const MessageHeader* data) { 
    static FTDataMessage ftDataMessage;
    ftDataMessage.header.packetLength = sizeof(ftDataMessage);
    ftDataMessage.header.fid = FID_GET_FT_SENSOR_DATA;
    ftDataMessage.adcData_6Channel = _adc18_FT.getADCData_6Channel();
    
    _socket->send((char*) &ftDataMessage, sizeof(FTDataMessage));
}

void NeedleController::getEncoderSensorData(const MessageHeader* data) { 
    static EncoderDataMessage encoderDataMessage;
    encoderDataMessage.header.packetLength = sizeof(encoderDataMessage);
    encoderDataMessage.header.fid = FID_GET_ENCODER_SENSOR_DATA;
    
    encoderDataMessage.encoder_data.xPos = static_cast<float>(_qei_x.getPulses()) * QEI_SCALE_FACTOR;
    encoderDataMessage.encoder_data.yPos = static_cast<float>(_qei_y.getPulses()) * QEI_SCALE_FACTOR;
    encoderDataMessage.encoder_data.zPos = static_cast<float>(_qei_z.getPulses()) * QEI_SCALE_FACTOR;
    
    _socket->send((char*) &encoderDataMessage, sizeof(EncoderDataMessage));
}

void NeedleController::getAllSensorData(const MessageHeader* data) { 
    static AllDataMessage allDataMessage;
    allDataMessage.header.packetLength = sizeof(allDataMessage);
    allDataMessage.header.fid = FID_GET_ALL_SENSOR_DATA;
    clearAllData(&allDataMessage.allData);
    _startTime = us_ticker_read();

    allDataMessage.allData.adcData_6Channel = _adc18_FT.getADCData_6Channel();

    allDataMessage.allData.time = us_ticker_read() - _startTime;
    D(printf("Time: %d\n", allDataMessage.allData.time));

    allDataMessage.allData.encoder_data.xPos = static_cast<float>(_qei_x.getPulses()) * QEI_SCALE_FACTOR;
    allDataMessage.allData.encoder_data.yPos = static_cast<float>(_qei_y.getPulses()) * QEI_SCALE_FACTOR;
    allDataMessage.allData.encoder_data.zPos = static_cast<float>(_qei_z.getPulses()) * QEI_SCALE_FACTOR;
    
    _socket->send((char*) &allDataMessage, sizeof(AllDataMessage));
}

void NeedleController::getAllSensorDataMultiple(const Settings* data) { 
    static AllDataMessage allDataMessage;
    allDataMessage.header.packetLength = sizeof(allDataMessage);
    allDataMessage.header.fid = FID_GET_ALL_SENSOR_DATA;
    clearAllData(&allDataMessage.allData);
    _startTime = us_ticker_read();

    allDataMessage.allData.adcData_6Channel = _adc18_FT.getADCData_6Channel_multiple(data->value);

    allDataMessage.allData.time = us_ticker_read() - _startTime;
    D(printf("Time: %d\n", allDataMessage.allData.time));

    allDataMessage.allData.encoder_data.xPos = static_cast<float>(_qei_x.getPulses()) * QEI_SCALE_FACTOR;
    allDataMessage.allData.encoder_data.yPos = static_cast<float>(_qei_y.getPulses()) * QEI_SCALE_FACTOR;
    allDataMessage.allData.encoder_data.zPos = static_cast<float>(_qei_z.getPulses()) * QEI_SCALE_FACTOR;
    
    _socket->send((char*) &allDataMessage, sizeof(AllDataMessage));
}

void NeedleController::streamData() {
    _queueID = _queue.call_in(10ms, callback(this, &NeedleController::streamData));
    static AllData allData;
    clearAllData(&allData);

    allData.time = us_ticker_read() - _startTime;
    allData.encoder_data.xPos = static_cast<float>(_qei_x.getPulses()) * QEI_SCALE_FACTOR;
    allData.encoder_data.yPos = static_cast<float>(_qei_y.getPulses()) * QEI_SCALE_FACTOR;
    allData.encoder_data.zPos = static_cast<float>(_qei_z.getPulses()) * QEI_SCALE_FACTOR;

    uint8_t samplesToAverage = 5;
    for (int i = 0; i < samplesToAverage; ++i) {
        // _adcData_6Channel = _adc18_FT.getADCData_6Channel();
        _adcData_6Channel = _adc18_FT.getADCData_6Channel_multiple(3);

        allData.adcData_6Channel.ai1 += _adcData_6Channel.ai1;
        allData.adcData_6Channel.ai2 += _adcData_6Channel.ai2;
        allData.adcData_6Channel.ai3 += _adcData_6Channel.ai3;
        allData.adcData_6Channel.ai4 += _adcData_6Channel.ai4;
        allData.adcData_6Channel.ai5 += _adcData_6Channel.ai5;
        allData.adcData_6Channel.ai6 += _adcData_6Channel.ai6;
    }

    allData.adcData_6Channel.ai1 = allData.adcData_6Channel.ai1 / samplesToAverage;
    allData.adcData_6Channel.ai2 = allData.adcData_6Channel.ai2 / samplesToAverage;
    allData.adcData_6Channel.ai3 = allData.adcData_6Channel.ai3 / samplesToAverage;
    allData.adcData_6Channel.ai4 = allData.adcData_6Channel.ai4 / samplesToAverage;
    allData.adcData_6Channel.ai5 = allData.adcData_6Channel.ai5 / samplesToAverage;
    allData.adcData_6Channel.ai6 = allData.adcData_6Channel.ai6 / samplesToAverage;

    _socket->send((char*) &allData, sizeof(AllData));
}

void NeedleController::startAcquisitionStream(const MessageHeader* data) {
    _startTime = us_ticker_read();
    _qei_x.reset();
    _qei_y.reset();
    _qei_z.reset();
    streamData();
}

void NeedleController::stopAcquisitionStream(const MessageHeader* data) {
    _queue.cancel(_queueID);
}

void NeedleController::clearAllData(AllData* allData) {
    allData->time = 0;

    allData->adcData_6Channel.ai1 = 0;
    allData->adcData_6Channel.ai2 = 0;
    allData->adcData_6Channel.ai3 = 0;
    allData->adcData_6Channel.ai4 = 0;
    allData->adcData_6Channel.ai5 = 0;
    allData->adcData_6Channel.ai6 = 0;

    allData->encoder_data.xPos = 0;
    allData->encoder_data.yPos = 0;
    allData->encoder_data.zPos = 0;
}

void NeedleController::resetADC(const MessageHeader* data) {
    _adc18_FT.adc18_reset_device();
}

void NeedleController::checkADC(const MessageHeader* data) {
    _adc18_FT.adc18_check_communication();
}

void NeedleController::setADCConversionMode(const Settings* data) {
    _adc18_FT.adc18_set_conversion_mode(data->value);
}

void NeedleController::setADCDataRate(const Settings* data) {
    _adc18_FT.adc18_set_data_rate(data->value);
}

/*! LED Functions */
void NeedleController::flipStatusLED() {
    _redLED = !_redLED;
    _statusLED = !_statusLED;
}

/*! Initialising Ethernet */
void NeedleController::initEthernet() {
    // Set static IP
    _eth.set_network(IP_ADDRESS, NETWORK_MASK, GATEWAY);

    // Bring up the ethernet interface
    _eth.connect();

    // Show the network address
    _eth.get_ip_address(&_ipAddr);

    // Open a socket on the network interface, and create a TCP connection
    _server.open(&_eth);
    _server.bind(7851);
    _server.listen(1);
    _server.set_blocking(true);
    _server.set_timeout(-1);
}

/*! Getting a function pointer based on the FID */
const NeedleController::ComMessage* NeedleController::getComFromHeader(const MessageHeader* header) {

    if (header->fid >= _fidCount) { // Prevent getting out of an array
        return NULL;
    }

    return &comMessages[header->fid];
}

void NeedleController::comReturn(const void* data, const int errorCode) {
    MessageHeader *message = (MessageHeader*) data;
    message->packetLength = _msgHeaderLength;
    message->error = errorCode;
    _socket->send((char*) message, _msgHeaderLength);
}

/*! Setting the pump state */
void NeedleController::setBoardState(int state) {
    
    _boardState = state;
    
    switch(_boardState) {
        case WAIT_FOR_CONNECTION:
            // Blink status LED
            _tickerNeedleController.attach(callback(this, &NeedleController::flipStatusLED), 500ms);
            break;
        case CONNECTED:
            // Solid status LED
            _tickerNeedleController.detach();
            _redLED = 0;
            _statusLED = 1;
            break;
        default:
            _tickerNeedleController.detach();
            _redLED = 0;
            _statusLED = 1;
            break;
    }
}

/*! Main function */
void NeedleController::run() {
    // Indicate initialising state of a system
    setBoardState(WAIT_FOR_CONNECTION);
    D(printf("Initialising ethernet...\n"));
    
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
        D(printf("Connected...\n"));
        
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
                // D(printf("FID to call: %d\n", comMessage->fid));
                // Fact: comMessage->fid is equivalent to (*comMessage).fid
                (this->*comMessage->replyFunc)((void*)data);
            } else {
                comReturn(data, MSG_ERROR_NOT_SUPPORTED);
            }
        }
        
        // Client disconnected
        D(printf("Client disconnected...\n"));
        _socket->close();
        
        // Indicate disconnected state
        setBoardState(WAIT_FOR_CONNECTION);
    }
}