#include "FingerPrint.hpp"

using namespace FingerPrintConstants;

bool PeakSortEn = true;

bool compare(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) {
  int lhs_size = lhs.first * lhs.first + lhs.second * lhs.second;
  int rhs_size = rhs.first * rhs.first + rhs.second * rhs.second;
  return lhs_size <= rhs_size;
}

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
    std::vector<std::pair<int, int>>& peaks) {
  // Sort peaks by time if PeakSort is enabled (enabling may improve performance
  // but accuracy will be damaged)
  if (PeakSortEn) {
    // std::sort(peaks.begin(), peaks.end(), compare);
    std::sort(peaks.begin(), peaks.end(), [](const auto& lhs, const auto& rhs) {
      return lhs.second < rhs.second;
    });
  }

  std::vector<std::pair<std::string, int>> hashes;

  for (size_t i = 0; i < peaks.size(); i++) {
    for (size_t j = i + 1; j < i + FAN_VALUE && j < peaks.size(); j++) {
      int64_t freq1 = peaks[i].first;
      int64_t freq2 = peaks[j].first;
      int64_t time1 = peaks[i].second;
      int64_t time2 = peaks[j].second;
      int64_t time_delta = time2 - time1;
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
    const std::vector<std::vector<double>>& spectrogram) {
  std::vector<std::pair<int, int>> peaks;
  std::vector<std::vector<double>> spectrogram_copy(
      spectrogram.size(), std::vector<double>(spectrogram[0].size()));
  int64_t width = spectrogram[0].size();
  int64_t height = spectrogram.size();
  for (int64_t y = 0; y < height; y++) {
    for (int64_t x = 0; x < width; x++) {
      double value = spectrogram[y][x];
      for (int64_t i = -RADIUS; i <= RADIUS; i++) {
        for (int64_t j = -RADIUS; j <= RADIUS; j++) {
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

  for (int64_t y = 0; y < height; y++) {
    for (int64_t x = 0; x < width; x++) {
      if (spectrogram_copy[y][x] == spectrogram[y][x]) {
        peaks.push_back(std::make_pair(x, y));
      }
    }
  }
  return peaks;
}

std::vector<double> Flatten(const std::vector<std::vector<double>>& matrix,
                            bool transpose = false) {
  size_t n = matrix.size(),
         m = matrix[0].size();  // TODO: UB - be careful for mateix[0]
  std::vector<double> flatted(n * m);
  size_t idx = 0;
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      if (transpose) {
        flatted[idx++] = matrix[j][i];
      } else {
        flatted[idx++] = matrix[i][j];
      }
    }
  }
  return flatted;
}

// TODO: Optimize
double LocalMaximum(const std::vector<double>& arr, const size_t idx) {
  double maximum = arr[idx];
  for (int i = -RADIUS; i <= RADIUS; i++) {
    if ((i + idx) >= 0 && (i + idx) < arr.size()) {
      maximum = std::max(maximum, arr[i]);
    }
  }
  return maximum;
}

std::vector<std::vector<bool>> Get1DPeaks(const std::vector<double>& arr,
                                          const size_t n, const size_t m,
                                          bool transpose = false) {
  std::vector<std::vector<bool>> mask(n, std::vector<bool>(m, 0));
  std::vector<double> filtred_arr(arr.size());
  for (size_t i = 0; i < arr.size(); i++) {
    filtred_arr[i] = LocalMaximum(arr, i);
  }
  for (size_t k = 0; k < arr.size(); k++) {
    if (filtred_arr[k] == arr[k]) {
      size_t i = k / n;
      size_t j = k % m;
      if (transpose) {
        std::swap(i, j);
      }
      mask[i][j] = true;
    }
  }
  return mask;
}

std::vector<std::pair<int, int>> GetFLattedPeaks(
    const std::vector<std::vector<double>>& spectrogram) {
  size_t n = spectrogram.size(), m = spectrogram[0].size();
  auto flatten_peaks = Get1DPeaks(Flatten(spectrogram), n, m);
  auto flatten_peaks_tranposed =
      Get1DPeaks(Flatten(spectrogram, true), n, m, true);

  std::vector<std::pair<int, int>> peaks = {{0, 0}};
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      if (flatten_peaks[i][j] && flatten_peaks_tranposed[i][j]) {
        peaks.emplace_back(i, j);
      }
    }
  }
  return peaks;
}

std::vector<std::pair<std::string, int>> GenerateFingerPrints(
    std::vector<std::vector<double>>& spectogram) {
  std::vector<std::pair<int, int>> peaks = GetPeaks(spectogram);
  // std::vector<std::pair<int, int>> peaks = GetFLattedPeaks(spectogram);
  std::vector<std::pair<std::string, int>> hashes = GenerateHashes(peaks);
  return hashes;
}