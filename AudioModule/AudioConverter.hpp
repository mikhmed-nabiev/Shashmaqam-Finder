#ifndef SHASHMAQAMFINDER_AUDIOMODULE_AUDIOCONVERTER_HPP_
#define SHASHMAQAMFINDER_AUDIOMODULE_AUDIOCONVERTER_HPP_

#include <cstdint>
#include <fstream>
#include <vector>

class AudioConverter {
public:
  AudioConverter(const std::string &fileName, int bytesPerSample = 2);

  inline std::vector<double> GetData() const { return audioData_; }

private:
  void ReadWavHeader(std::ifstream &file);
  void ReadAudioData(std::ifstream &file);

  std::string fileName_;
  int bytesPerSample_;
  std::vector<double> audioData_;
};

#endif // SHASHMAQAMFINDER_AUDIOMODULE_AUDIOCONVERTER_HPP_