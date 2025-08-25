#include "chassis.h"
#include <thread>
#include <chrono>

int main() {

    // Initialize the Chassis Object:
    BoeDriver BOE {};

    BOE.moveDuration = 3;

    BOE.foreward();
    BOE.reverse();
    BOE.yawCW();
    BOE.yawCCW();

    BOE.camAngle = 180;
    BOE.setCamAngle();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    BOE.camAngle = 0;
    BOE.setCamAngle();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    BOE.camAngle = 90;
    BOE.setCamAngle();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}
