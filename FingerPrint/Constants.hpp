//
// Created by Mikhmed Nabiev on 06.08.2023.
//

#ifndef SHASHMAQAMFINDER_MATH_UTILS_HPP_
#define SHASHMAQAMFINDER_MATH_UTILS_HPP_

#pragma once
#include <cstdint>
#include <numbers>

namespace FingerPrintConstants {
  constexpr int64_t MIN_HASH_TIME_DELTA = 0;
  constexpr int64_t MAX_HASH_TIME_DELTA = 200;
  constexpr int64_t FINGERPRINT_REDUCTION = 20;
  constexpr int64_t FAN_VALUE = 5;
  constexpr int64_t RADIUS = 10;
}

#endif //SHASHMAQAMFINDER_MATH_UTILS_HPP_
