// main.cpp
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include "PauseController.hpp"

using namespace std::chrono_literals;

std::atomic<bool> gRunning{true};
PauseController gPause;

void simulation_loop() {
    double t = 0.0;

    while (gRunning.load()) {
        gPause.wait_if_paused();  // blocks while paused

        // --- Your physics/render update here ---
        t += 0.016; // ~16 ms per frame
        // updatePlanets(0.016);

        static int frame = 0;
        if ((frame++ % 30) == 0) {
            std::cout << "[tick] t=" << t
                      << " paused=" << std::boolalpha
                      << gPause.is_paused() << "\n";
        }

        std::this_thread::sleep_for(16ms);
    }
}

void input_loop() {
    std::cout << "Press 'p' then Enter to pause/resume.\n";
    char c;
    while (gRunning.load() && (std::cin >> c)) {
        if (c == 'p' || c == 'P') {
            gPause.toggle();
            std::cout << (gPause.is_paused() ? "[paused]\n" : "[resumed]\n");
        }
        // no 'r' or 'q' handling anymore
    }
    // If stdin closes (e.g., IDE stops input), exit cleanly
    gPause.resume();           // ensure sim isn't stuck waiting
    gRunning.store(false);
}

int main() {
    std::thread sim(simulation_loop);
    std::thread ui(input_loop);

    ui.join();
    sim.join();
    std::cout << "Goodbye!\n";
    return 0;
}
