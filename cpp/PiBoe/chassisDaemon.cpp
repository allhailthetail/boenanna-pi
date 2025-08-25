#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <csignal>
#include <thread>
#include <chrono>
#include "chassis.h"   // BoeDriver, Speed, etc.

// ---------- small helpers ----------
static inline void trim(std::string& s) {
    while (!s.empty() && (s.back()=='\r' || std::isspace((unsigned char)s.back()))) s.pop_back();
    size_t i = 0; while (i < s.size() && std::isspace((unsigned char)s[i])) ++i;
    if (i) s.erase(0, i);
}
static inline std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

// ---------- graceful shutdown flag ----------
static volatile std::sig_atomic_t g_stop = 0;
static void handle_signal(int) { g_stop = 1; }

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>\n";
        return 1;
    }
    const std::string path = argv[1];

    // Install signal handlers (Ctrl-C / kill → clean exit)
    std::signal(SIGINT,  handle_signal);
    std::signal(SIGTERM, handle_signal);

    // Your chassis object lives for the whole program; destructor runs on clean exit.
    BoeDriver BOE{};

    while (!g_stop) {
        // --- read up to three lines ---
        std::string line1, line2, line3;

        if (std::ifstream in{path}) {
            std::getline(in, line1);
            std::getline(in, line2);
            std::getline(in, line3);
            // in closes automatically
        } else {
            // If file can't be opened, wait a bit and try again
            std::cerr << "chassisDaemon: can't open " << path << " for reading; retrying…\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        // If file had anything, clear it so each command is one-shot
        if (!line1.empty() || !line2.empty() || !line3.empty()) {
            if (!std::ofstream(path, std::ios::trunc)) {
                std::cerr << "chassisDaemon: failed to clear " << path << "\n";
            }
        }

        // Nothing to do? small nap and loop
        if (line1.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // --- normalize & parse ---
        trim(line1); trim(line2); trim(line3);
        line1 = lower(line1);
        line2 = lower(line2);

        double duration = 0.0;
        if (!line3.empty()) {
            try { duration = std::stod(line3); }
            catch (...) { std::cerr << "chassisDaemon: line3 not a number: '" << line3 << "'\n"; }
        }

        // If camera angle command:
        if (line1 == "camangle") {
            BOE.camAngle = std::stod(line3);
            BOE.setCamAngle();
            continue;
        }

        BOE.moveDuration = duration;   // seconds

        // Speed (optional; only for fwd/rev/yaw commands)
        if      (line2 == "low")    BOE.speed = Speed::Low;
        else if (line2 == "medium") BOE.speed = Speed::Medium;
        else if (line2 == "high")   BOE.speed = Speed::High;
        else if (!line2.empty())    std::cerr << "Warning: unknown speed '" << line2 << "'\n";

        // --- execute command ---
        if      (line1 == "forward") BOE.forward();
        else if (line1 == "reverse") BOE.reverse();
        else if (line1 == "yawcw")   BOE.yawCW();
        else if (line1 == "yawccw")  BOE.yawCCW();
        else {
            std::cerr << "chassisDaemon: unknown command '" << line1
                      << "' (expected forward/reverse/yawcw/yawccw)\n";
        }

        // brief pause to avoid busy-looping the filesystem
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Leaving main → BOE destructor runs here.
    std::cerr << "chassisDaemon: shutting down cleanly.\n";
    return 0;
}
