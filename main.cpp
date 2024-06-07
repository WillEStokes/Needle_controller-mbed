#include "debug.h"
#include "NeedleController.h"
#include "mbed.h"
#include "EthernetInterface.h"

NeedleController needleController(
    LED1,
    D8 );

int main(int, char**) {
    needleController.run();
}