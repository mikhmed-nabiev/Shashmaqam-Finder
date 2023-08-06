#include "Field.hpp"
#include "Constants.hpp"

complex ComplexField::GetPrimitiveRoot(int64_t power) {
  double angle = 2 * constant::kPi / static_cast<double>(power);
  return {std::cos(angle), std::sin(angle)};
}