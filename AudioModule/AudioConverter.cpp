#include "AudioConverter.hpp"
#include "AudioExceptions.cpp"
#include <iostream>

AudioConverter::AudioConverter(const std::string &fileName, int bytesPerSample)
    : fileName_(fileName), bytesPerSample_(bytesPerSample) {
  std::ifstream audioFile(fileName_, std::ios::binary);
  if (!audioFile) {
    std::string message = "Failed to open the WAV file.";
    throw AudioConverterException(message);
  }

  ReadWavHeader(audioFile);
  ReadAudioData(audioFile);

  audioFile.close();
}

void AudioConverter::ReadWavHeader(std::ifstream &audioFile) {
  char chunkID[4];
  uint32_t chunkSize;
  char format[4];

  audioFile.read(chunkID, 4);
  if (std::string(chunkID, 4) != "RIFF") {
    std::string message = "Invalid WAV file: Missing 'RIFF' chunk.";
    throw AudioConverterException(message);
  }

  audioFile.read(reinterpret_cast<char *>(&chunkSize), 4);

  audioFile.read(format, 4);
  if (std::string(format, 4) != "WAVE") {
    std::string message = "Invalid WAV file: Missing 'WAVE' format.";
    throw AudioConverterException(message);
  }

  char subchunk1ID[4];
  uint32_t subchunk1Size;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;

  audioFile.read(subchunk1ID, 4);
  audioFile.read(reinterpret_cast<char *>(&subchunk1Size), 4);
  audioFile.read(reinterpret_cast<char *>(&audioFormat), 2);
  audioFile.read(reinterpret_cast<char *>(&numChannels), 2);
  audioFile.read(reinterpret_cast<char *>(&sampleRate), 4);
  audioFile.read(reinterpret_cast<char *>(&byteRate), 4);
  audioFile.read(reinterpret_cast<char *>(&blockAlign), 2);
  audioFile.read(reinterpret_cast<char *>(&bitsPerSample), 2);

  // Validate audio format (assuming PCM for simplicity)
  if (audioFormat != 1) {
    std::string message = "Unsupported audio format.";
    throw AudioConverterException(message);
  }

  // Seek to the start of the audio data
  audioFile.seekg(4 + 4 + subchunk1Size, std::ios::beg);
}

void AudioConverter::ReadAudioData(std::ifstream &file) {
  constexpr int16_t MAX_INT_16 = 32767;
  constexpr int32_t MAX_INT_32 = 2147483647;

  while (!file.eof()) {
    int32_t sample;
    file.read(reinterpret_cast<char *>(&sample), bytesPerSample_);

    if (!file.eof()) {
      double value;

      if (bytesPerSample_ == 2) {
        value = static_cast<double>(sample) / MAX_INT_16;
      } else if (bytesPerSample_ == 4) {
        value = static_cast<double>(sample) / MAX_INT_32;
      }

      audioData_.push_back(value);
    }
  }
}
