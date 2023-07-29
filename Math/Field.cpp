#include "Field.h"

std::complex<double> ComplexField::GetPrimitiveRoot(int64_t power) {
  double angle = 2 * kPi / static_cast<double>(power);
  return {std::cos(angle), std::sin(angle)};
}