#ifndef FT_H
#define FT_H

#include "mbed.h"
// #include "AnalogRead.h"

class FT
{
    public:
        typedef struct {
            float xForce;
            float yForce;
            float zForce;
            float xTorque;
            float yTorque;
            float zTorque;
        } __attribute__((__packed__)) FTElement;

        typedef struct {
            FTElement element[100];
        } __attribute__((__packed__)) FTBufferValues;

        typedef struct {
            FTBufferValues values;
            uint8_t startIndex;
            uint8_t endIndex;
        } __attribute__((__packed__)) FTBufferPacket;
        
        FT( PinName xForce,
            PinName yForce,
            PinName zForce,
            PinName xTorque,
            PinName yTorque,
            PinName zTorque);

        FT::FTElement getFTElement();
        FTBufferPacket getFTBuffer();
        FTBufferPacket initBuffer();
        void readData();
        // void startAcquisition();
        void stopAcquisition();
        void clearBuffer();

    private:

    /*! Pins */
    AnalogIn _xForce;
    AnalogIn _yForce;
    AnalogIn _zForce;
    AnalogIn _xTorque;
    AnalogIn _yTorque;
    AnalogIn _zTorque;
    DigitalOut _debugPin;

    /*! Variables */
    FTBufferPacket _ftBufferPacket;
    uint8_t _i;
    uint8_t _startIndex;
    int _queueID;

    /*! Threads & Timing */
    Thread _thread;
    EventQueue _queue;
    Ticker _tickerFT;
};

#endif