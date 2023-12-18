#ifndef SHASHMAQAMFINDER_AUDIOMODULE_AUDIOCONVERTER_HPP_
#define SHASHMAQAMFINDER_AUDIOMODULE_AUDIOCONVERTER_HPP_

#include <cstdint>
#include <fstream>
#include <vector>

class AudioConverter {
 public:
  AudioConverter(const std::string& file_name, int bytes_per_sample = 2);

  inline std::vector<double> GetData() const { return audio_data_; }

 private:
  void ReadWavHeader(std::ifstream& file);
  void ReadAudioData(std::ifstream& file);

  std::string file_name_;
  int bytes_per_sample_;
  std::vector<double> audio_data_;
};

#endif  // SHASHMAQAMFINDER_AUDIOMODULE_AUDIOCONVERTER_HPP_