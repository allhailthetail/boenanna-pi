/*
 chassis.h

 NOTE: This header implementation requires the PiPCA9685 library to be installed in order to compile and run
 Please refer to:
 https://github.com/barulicm/PiPCA9685/tree/main

 Then, simply include chassis.h in your program...
*/

#include <unistd.h>
#include <iostream>
#include <cmath>
#include <PiPCA9685/PCA9685.h>

namespace {
    uint16_t us_to_ticks(double us, double freq_hz) {
        double t = us * 4096.0 * freq_hz / 1'000'000.0;
        if (t < 0) t = 0;
        if (t > 4095) t = 4095;
        return static_cast<uint16_t>(std::lround(t));
    }

    void set_pulse_us(PiPCA9685::PCA9685& pca,
                      int ch,
                      double us,
                      const float freq) {
        pca.set_pwm(ch, 0, us_to_ticks(us, freq));
    }

    /*
    NOTE: min_us and max_us may need adjusting depending on the specific make of servo...
    These values determine how far the servo will swing each way and is
    apparently unique to the brand of servo you have.
    */
    void set_angle(PiPCA9685::PCA9685& pca,
                   int ch,
                   double deg,
                   double freq=50.0,
                   double min_us=500,
                   double max_us=2500) {
        if (deg < 0) deg = 0; if (deg > 180) deg = 180;
        double us = min_us + (max_us - min_us) * (deg / 180.0);
        set_pulse_us(pca, ch, us, freq);
    }

    enum class Speed {
        Low,
        Medium,
        High
    };

    class BoeDriver {

        // Set up a pulse control session:
        PiPCA9685::PCA9685 pca{};
        double freq {};

        int LEFT  {}; // Servo pins for bot (can be modified on object init):
        int RIGHT {};
        int CAM   {};

    public:
        float moveDuration {};             // default move duration (sec)
        Speed speed {};                         // default low speed
        int camAngle {};                    // default look straight ahead

        // Chassis Full Stop
        void stop() {
            set_pulse_us(pca, LEFT, 1500, freq);  //Left stop
            set_pulse_us(pca, RIGHT, 1500, freq); // Right Stop
        }

        void forward() {
            switch(speed) {
            case Speed::Low: {
                set_pulse_us(pca, LEFT,  1575, freq);   // left wheel forward
                set_pulse_us(pca, RIGHT, 1452, freq);   // right wheel forward
            } break;
            case Speed::Medium: {
                set_pulse_us(pca, LEFT,  1650, freq);   // left wheel forward
                set_pulse_us(pca, RIGHT, 1350, freq);   // right wheel forward
            } break;
            case Speed::High: {
                set_pulse_us(pca, LEFT,  1700, freq);   // left wheel forward
                set_pulse_us(pca, RIGHT, 1300, freq);   // right wheel forward
            } break;
            default: {
                std::cerr << "Unrecognized command" << '\n';
            }
            }

            // sleep for specified duration and call stop()
            usleep(moveDuration * 1'000'000);
            stop();
        }

        void reverse() {
            switch(speed) {
            case Speed::Low: {
                set_pulse_us(pca, LEFT,  1452, freq);  // left wheel backward
                set_pulse_us(pca, RIGHT, 1575, freq);  // right wheel backward
            } break;
            case Speed::Medium: {
                set_pulse_us(pca, LEFT,  1350, freq);   // left wheel backward
                set_pulse_us(pca, RIGHT, 1650, freq);   // right wheel backward
            } break;
            case Speed::High: {
                set_pulse_us(pca, LEFT,  1300, freq);   // left wheel backward
                set_pulse_us(pca, RIGHT, 1700, freq);   // right wheel backward
            } break;
            default: {
                std::cerr << "Unrecognized command" << '\n';
            }
            }

            // sleep for specified duration and call stop()
            usleep(moveDuration * 1'000'000);
            stop();
        }

        void yawCW () {
            switch(speed) {
            case Speed::Low: {
                set_pulse_us(pca, LEFT,  1575, freq);   // left wheel forward
                set_pulse_us(pca, RIGHT, 1575, freq);   // right wheel backward
            } break;
            case Speed::Medium: {
                set_pulse_us(pca, LEFT,  1650, freq);   // left wheel forward
                set_pulse_us(pca, RIGHT, 1650, freq);   // right wheel backward
            } break;
            case Speed::High: {
                set_pulse_us(pca, LEFT,  1700, freq);   // left wheel forward
                set_pulse_us(pca, RIGHT, 1700, freq);   // right wheel reverse
            } break;
            default: {
                std::cerr << "Unrecognized command" << '\n';
            }
            }

            // sleep for specified duration and call stop()
            usleep(moveDuration * 1'000'000);
            stop();
        }

        void yawCCW () {
            switch(speed) {
            case Speed::Low: {
                set_pulse_us(pca, LEFT,  1452, freq);   // left wheel backward
                set_pulse_us(pca, RIGHT, 1452, freq);   // right wheel forward
            } break;
            case Speed::Medium: {
                set_pulse_us(pca, LEFT,  1350, freq);   // left wheel backward
                set_pulse_us(pca, RIGHT, 1350, freq);   // right wheel forward
            } break;
            case Speed::High: {
                set_pulse_us(pca, LEFT,  1300, freq);   // left wheel backward
                set_pulse_us(pca, RIGHT, 1300, freq);   // right wheel forward
            } break;
            default: {
                std::cerr << "Unrecognized command" << '\n';
            }
            }

            // sleep for specified duration and call stop()
            usleep(moveDuration * 1'000'000);
            stop();
        }

        // Moves camera to specified angle (0-180deg)
        void setCamAngle() {
            set_angle(pca, CAM, camAngle, freq);
        }

        /*
        on init, bring chassis to a stop and point
        camera straight ahead.
        */
        BoeDriver(int lpin=0,
                  int rpin=1,
                  int campin=2,
                  double custfreq=50.0) {

            // Set default values:
            freq = custfreq; // Unknown if this depends on model of servo...
            LEFT = lpin;     // Significant - these are set on object creation and are static from here on...
            RIGHT = rpin;
            CAM = campin;

            // These constantly change...
            moveDuration = 1.0;
            speed = Speed::Low;
            camAngle = 90;

            // Setup instance of pca for control:
            pca.set_pwm_freq(freq);

            stop();
            set_angle(pca, CAM, camAngle, freq);  // Cam Look Ahead (90deg)
        }

        /*
        On destruction, bring chassis to a stop and point
        camera straight ahead.
        */
        ~BoeDriver() {
            camAngle = 90;
            set_pulse_us(pca, LEFT, 1500, freq);  //Left stop
            set_pulse_us(pca, RIGHT, 1500, freq); // Right Stop
            set_angle(pca, CAM, camAngle, freq);  // Cam Look Ahead (90deg)
        }
    };
}
