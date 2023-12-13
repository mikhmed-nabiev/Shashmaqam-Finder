#ifndef SHASHMAQAMFINDER_FINGEEPRINT_HPP_
#define SHASHMAQAMFINDER_FINGEEPRINT_HPP_

#include <openssl/sha.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace constant {
  constexpr int64_t MIN_HASH_TIME_DELTA = 0;
  constexpr int64_t MAX_HASH_TIME_DELTA = 150;
  constexpr int64_t FINGERPRINT_REDUCTION = 20;
}

std::vector<std::pair<std::string, int>> GetFingerPrints(
    std::vector<std::vector<double>> &spectogram);

#endif  // SHASHMAQAMFINDER_FINGEEPRINT_HPP_