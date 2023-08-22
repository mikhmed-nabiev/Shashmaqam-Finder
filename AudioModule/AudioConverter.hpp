//
// Created by Akhmedov R on 07.08.2023.
//
#ifndef AUDIO_RECORDER_HPP
#define AUDIO_RECORDER_HPP

#include <vector>
#include <fstream>
#include <cstdint>

class AudioConverter{
 public:
  AudioConverter(const std::string& fileName, int bytesPerSample = 2);

  std::vector<double> get_data() const;

 private:
  void readWavHeader(std::ifstream& file);
  void readAudioData(std::ifstream& file);

  std::string fileName_;
  int bytesPerSample_;
  std::vector<double> audioData_;
};

#endif