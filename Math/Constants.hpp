//
// Created by Mikhmed Nabiev on 06.08.2023.
//

#ifndef SHASHMAQAMFINDER_MATH_UTILS_HPP_
#define SHASHMAQAMFINDER_MATH_UTILS_HPP_

#pragma once
#include <cstdint>
#include <numbers>

namespace constant {
  const double kPi = std::numbers::pi;
  const double kEps = 1e-6;
  const uint64_t kSamplingRate = 44100;
  const int numInputChannels = 1;
  const int numOutputChannels = 0;
}

#endif //SHASHMAQAMFINDER_MATH_UTILS_HPP_
