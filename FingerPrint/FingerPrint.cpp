#include "FingerPrint.hpp"

using namespace FingerPrintConstants;

bool PeakSortEn = true;

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
/*
 def peak_matrix(self, fraction=0.1, condition=2):
        '''
        Function that spots significant peaks in a given spectrogram.

        Parameters
        ----------
            fraction (float) : Fraction of spectrogram band to compute local comparisons, value between 0 and 1. By default `fraction=0.1`.
            condition (int)  : Axis in which we search the peaks. By default `condition=2`.
                axis=0: Time-based search (By rows).
                axis=0: Frequency-based search (By columns).
                axis=2: Time-Frequency-based search (Row+Columns).
        
        Returns
        ----------
            id_peaks (np.array): Array with the position (t,f) in which the peaks appear.
            peaks    (np.array): Array spectrogram-shaped just with the peaks in the same position.
        '''
        # Get Frenquency-Time-Spectrogram representations
        spectromap.get_spectrogram(self)
        
        # Time based
        if condition == 0:
            # Transpose and flat the spectrogram
            x = self.spectrogram.T
            x = x.flatten()
            # Find peaks according to the set length
            d = int(fraction*self.spectrogram.shape[condition])
            idx, _ = signal.find_peaks(x, distance=d)
            # Get matrix with the position of the peaks
            id_peaks = np.zeros(x.shape)
            id_peaks[idx] = True
            id_peaks = np.reshape(id_peaks, self.spectrogram.T.shape).T.astype('bool')

        # Frequency based
        elif condition == 1:
            x = self.spectrogram.flatten()
            # Find peaks according to the set length
            d = int(fraction*self.spectrogram.shape[condition])
            idx, _ = signal.find_peaks(x, distance=d)
            # Get matrix with the position of the peaks
            id_peaks = np.zeros(x.shape)
            id_peaks[idx] = True
            id_peaks = np.reshape(id_peaks, self.spectrogram.shape).astype('bool')

        # Time-Frequency based
        elif condition == 2:
            # Find peaks for both axis
            id_peaks0, _ = spectromap.peak_matrix(self, fraction, condition=0)
            id_peaks1, _ = spectromap.peak_matrix(self, fraction, condition=1)
            # Get just the points that are considered peaks in both axis
            id_peaks = (id_peaks0.astype('int32') + id_peaks1.astype('int32')) == 2
        
        # Return just the peaks of the spectrogram
        peaks = np.zeros(self.spectrogram.shape)
        peaks[id_peaks] = self.spectrogram[id_peaks]

        return id_peaks, peaks
*/

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

  for (int64_t y = 0; y < height - 1; y++) {
    for (int64_t x = 0; x < width; x++) {
      if (spectrogram_copy[y][x] == spectrogram[y][x]) {
        peaks.push_back(std::make_pair(x, y));
      }
    }
  }
  return peaks;
}

std::vector<std::pair<std::string, int>> GenerateFingerPrints(
    std::vector<std::vector<double>>& spectogram) {
  std::vector<std::pair<int, int>> peaks = GetPeaks(spectogram);
  std::vector<std::pair<std::string, int>> hashes = GenerateHashes(peaks);
  return hashes;
}