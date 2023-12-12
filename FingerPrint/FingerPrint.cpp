#include "FingerPrint.hpp"

bool PeakSortEn = true;

constexpr int MIN_HASH_TIME_DELTA = 0;
constexpr int MAX_HASH_TIME_DELTA = 200;
constexpr int FINGERPRINT_REDUCTION = 20;

std::string CalculateSHA1(const std::string& input) {
  unsigned char hash[FINGERPRINT_REDUCTION];
  SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(),
       hash);

  // Convert hash to hexadecimal string
  std::ostringstream hash_stream;
  hash_stream << std::hex << std::setw(2) << std::setfill('0');
  for (unsigned char byte : hash) {
    hash_stream << static_cast<int>(byte);
  }
  return hash_stream.str();
}

std::vector<std::pair<std::string, int>> GenerateHashes(
    std::vector<std::pair<int, int>>& peaks, const int fan_value = 5) {
  // Sort peaks by time if PeakSort is enabled (enabling may improve performance
  // but accuracy will be damaged)
  if (PeakSortEn) {
    std::sort(peaks.begin(), peaks.end(), [](const auto& lhs, const auto& rhs) {
      return lhs.second < rhs.second;
    });
  }

  std::vector<std::pair<std::string, int>> hashes;

  for (size_t i = 0; i < peaks.size(); i++) {
    for (size_t j = i + 1; j < i + fan_value && j < peaks.size(); j++) {
      int freq1 = peaks[i].first;
      int freq2 = peaks[j].first;
      int time1 = peaks[i].second;
      int time2 = peaks[j].second;
      int time_delta = time2 - time1;
      if (time_delta > MIN_HASH_TIME_DELTA &&
          time_delta < MAX_HASH_TIME_DELTA) {
        std::string input = std::to_string(freq1) + "|" +
                            std::to_string(freq2) + "|" +
                            std::to_string(time_delta);
        hashes.push_back({CalculateSHA1(input), time1});
      }
    }
  }
  return hashes;
}

// TODO: Optimization
std::vector<std::pair<int, int>> GetPeaks(
    const std::vector<std::vector<double>>& spectrogram, int radius = 10) {
  std::vector<std::pair<int, int>> peaks;
  std::vector<std::vector<double>> spectrogram_copy(
      spectrogram.size(), std::vector<double>(spectrogram[0].size()));
  int width = spectrogram[0].size();
  int height = spectrogram.size();
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double value = spectrogram[y][x];
      for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
          if (i == 0 && j == 0) {
            continue;
          }
          if (x + i < 0 || x + i >= width || y + j < 0 || y + j >= height) {
            continue;
          }
          if (spectrogram[y + j][x + i] > value) {
            value = spectrogram[y + j][x + i];
          }
        }
      }

      spectrogram_copy[y][x] = value;
    }
  }

  for (int y = 0; y < height - 1; y++) {
    for (int x = 0; x < width; x++) {
      if (spectrogram_copy[y][x] == spectrogram[y][x]) {
        peaks.push_back(std::make_pair(x, y));
      }
    }
  }
  return peaks;
}

std::vector<std::pair<std::string, int>> GetFingerPrints(
    std::vector<std::vector<double>>& spectogram) {
  std::vector<std::pair<int, int>> peaks = GetPeaks(spectogram);
  std::vector<std::pair<std::string, int>> hashes = GenerateHashes(peaks);
  return hashes;
}