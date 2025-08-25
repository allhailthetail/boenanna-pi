#include "chassis.h"
#include <thread>
#include <chrono>

int main() {

    // Initialize the Chassis Object:
    BoeDriver BOE {};

    // Set time to drive in seconds
    BOE.moveDuration = 3;

    // Go forward:
    BOE.forward();

    // Reverse
    BOE.reverse();

    // Go right/CW
    BOE.yawCW();

    // go left/CCW
    BOE.yawCCW();

    // camera full-left
    BOE.camAngle = 180;
    BOE.setCamAngle();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // camera full-right
    BOE.camAngle = 0;
    BOE.setCamAngle();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // camera should return to straight-ahead...

    return 0;
}
