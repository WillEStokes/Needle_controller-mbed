#include "FT.h"

FT::FT( 
        PinName xForce,
        PinName yForce,
        PinName zForce,
        PinName xTorque,
        PinName yTorque,
        PinName zTorque
                        )
        :
        _xForce(xForce),
        _yForce(yForce),
        _zForce(zForce),
        _xTorque(xTorque),
        _yTorque(yTorque),
        _zTorque(zTorque),
        _debugPin(D8)
        {
            _startIndex = 0;
            _i = 0;
            _debugPin = 1;
            clearBuffer();
            _thread.start(callback(&_queue, &EventQueue::dispatch_forever));
        }

FT::FTElement FT::getFTElement() {
    static FT::FTElement ftElement;

    ftElement.xForce = _xForce.read();
    ftElement.yForce = _yForce.read();
    ftElement.zForce = _zForce.read();
    ftElement.xTorque = _xTorque.read();
    ftElement.yTorque = _yTorque.read();
    ftElement.zTorque = _zTorque.read();

    return ftElement;
}

FT::FTBufferPacket FT::getFTBuffer() {
    _i = 0;
    _startIndex = _i;
    return _ftBufferPacket;
}

// Just write new data into the circular buffer
void FT::readData() {
    // _queueID = _queue.call_in(1ms, callback(this, &FT::readData));
    _tickerFT.attach_us(_queue.event(this, &FT::readData), 500);
    
    _ftBufferPacket.values.element[_i].xForce = _xForce.read();
    _ftBufferPacket.values.element[_i].yForce = _yForce.read();
    _ftBufferPacket.values.element[_i].zForce = _zForce.read();
    _ftBufferPacket.values.element[_i].xTorque = _xTorque.read();
    _ftBufferPacket.values.element[_i].yTorque = _yTorque.read();
    _ftBufferPacket.values.element[_i].zTorque = _zTorque.read();

    _ftBufferPacket.startIndex = _startIndex;
    _ftBufferPacket.endIndex = _i;
    _i = (_i + 1) % 100;
    _debugPin = !_debugPin;
}

// void FT::startAcquisition() {
//     _queueID = _queue.call_in(100ms, callback(this, &FT::readData));
// }

void FT::stopAcquisition() {
    // _queue.cancel(_queueID);
    _tickerFT.detach();
    _startIndex = 0;
    _i = 0;
    clearBuffer();
}

void FT::clearBuffer() {
    memset(_ftBufferPacket.values.element, 0, sizeof(_ftBufferPacket.values.element));

    _ftBufferPacket.startIndex = 0;
    _ftBufferPacket.endIndex = 0;
}

FT::FTBufferPacket FT::initBuffer() {
    FTBufferPacket ftBufferPacket;
    memset(_ftBufferPacket.values.element, 0, sizeof(_ftBufferPacket.values.element));

    ftBufferPacket.startIndex = 0;
    ftBufferPacket.endIndex = 0;

    return ftBufferPacket;
}