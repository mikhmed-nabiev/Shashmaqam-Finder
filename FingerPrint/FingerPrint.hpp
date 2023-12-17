#ifndef SHASHMAQAMFINDER_FINGEEPRINT_HPP_
#define SHASHMAQAMFINDER_FINGEEPRINT_HPP_

#include <openssl/sha.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "Constants.hpp"

std::vector<std::pair<std::string, int>> GenerateFingerPrints(
    std::vector<std::vector<double>> &spectogram);

#endif  // SHASHMAQAMFINDER_FINGEEPRINT_HPP_