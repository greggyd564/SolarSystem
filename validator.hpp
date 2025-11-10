#pragma once
#include <array>
#include <cmath>
#include <string>
#include <sstream>

namespace ssim {

struct Limits {
  double mass_min_rel = 0.0;
  double mass_max_rel = 0.5;   // mass ≤ 0.5 × M*
  double r_min = 0.0;          // min radius allowed
  double r_max = 100.0;        // max scene radius
  double v_max = 0.3;          // |v| ≤ 0.3
};

struct Check {
  bool ok;
  std::string msg;
};

inline bool finite(double x) { return std::isfinite(x); }
inline double mag2(double x, double y) { return std::sqrt(x*x + y*y); }

inline Check validateMass(double mass_rel, const Limits& L) {
  if (!finite(mass_rel) || mass_rel < 0.0)
    return {false, "Mass must be a non-negative, finite value (relative to M*)."};
  if (mass_rel < L.mass_min_rel || mass_rel > L.mass_max_rel) {
    std::ostringstream os;
    os << "Mass out of range. Allowed: [" << L.mass_min_rel
       << ", " << L.mass_max_rel << "] × M*.";
    return {false, os.str()};
  }
  return {true, ""};
}

inline Check validatePosition(double px, double py, const Limits& L) {
  if (!finite(px) || !finite(py))
    return {false, "Position must be finite numbers."};
  const double r = mag2(px, py);
  if (r < L.r_min) {
    std::ostringstream os; os << "Position too close. Min r = " << L.r_min << ".";
    return {false, os.str()};
  }
  if (r > L.r_max) {
    std::ostringstream os; os << "Position too far. Max r = " << L.r_max << ".";
    return {false, os.str()};
  }
  return {true, ""};
}

inline Check validateVelocity(double vx, double vy, const Limits& L) {
  if (!finite(vx) || !finite(vy))
    return {false, "Velocity must be finite numbers."};
  const double v = mag2(vx, vy);
  if (v > L.v_max) {
    std::ostringstream os; os << "Velocity magnitude " << v
       << " exceeds maximum " << L.v_max << ".";
    return {false, os.str()};
  }
  return {true, ""};
}

inline std::array<double,2> clampVelocity(double vx, double vy, double v_max) {
  const double v = mag2(vx, vy);
  if (v == 0.0 || v <= v_max) return {vx, vy};
  const double s = v_max / v;
  return {vx * s, vy * s};
}

} // namespace ssim
