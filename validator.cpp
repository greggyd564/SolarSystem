#include <iostream>
#include "validator.hpp"

int main() {
    using namespace ssim;

    Limits L;
    L.r_min = 0.0;
    L.r_max = 100.0;
    L.v_max = 0.3;   // your hard velocity cap
    L.mass_max_rel = 0.5; // your mass limit

    const bool autoClamp = true; // change to false if you want rejection instead

    std::cout << "Enter: mass_rel (m/M*), px, py, vx, vy\n> ";
    double mass_rel, px, py, vx, vy;
    if (!(std::cin >> mass_rel >> px >> py >> vx >> vy)) {
        std::cerr << "❌ Invalid input. Enter five numbers separated by spaces.\n";
        return 1;
    }

    // Validate mass
    if (auto c = validateMass(mass_rel, L); !c.ok) {
        std::cerr << "❌ " << c.msg << "\n";
        return 1;
    }

    // Validate position
    if (auto c = validatePosition(px, py, L); !c.ok) {
        std::cerr << "❌ " << c.msg << "\n";
        return 1;
    }

    // Validate velocity
    if (auto c = validateVelocity(vx, vy, L); !c.ok) {
        if (autoClamp) {
            auto v = clampVelocity(vx, vy, L.v_max);
            vx = v[0]; vy = v[1];
            std::cout << "⚠️ Velocity exceeded 0.3; it was clamped.\n";
        } else {
            std::cerr << "❌ " << c.msg << "\n";
            return 1;
        }
    }

    std::cout << "✅ Validated inputs:\n"
              << "  Mass (m/M*): " << mass_rel << "\n"
              << "  Position:    (" << px << ", " << py << ")\n"
              << "  Velocity:    (" << vx << ", " << vy << ")\n";

    return 0;
}
