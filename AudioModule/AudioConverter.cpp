#include "AudioConverter.hpp"

#include <iostream>

#include "AudioExceptions.cpp"

AudioConverter::AudioConverter(const std::string& file_name,
                               int bytes_per_sample)
    : file_name_(file_name), bytes_per_sample_(bytes_per_sample) {
  std::ifstream audio_file(file_name_, std::ios::binary);
  if (!audio_file) {
    std::string message = "Failed to open the WAV file.";
    throw AudioConverterException(message);
  }

  ReadWavHeader(audio_file);
  ReadAudioData(audio_file);

  audio_file.close();
}

void AudioConverter::ReadWavHeader(std::ifstream& audio_file) {
  char chunk_id[4];
  uint32_t chunk_size;
  char format[4];

  audio_file.read(chunk_id, 4);
  if (std::string(chunk_id, 4) != "RIFF") {
    std::string message = "Invalid WAV file: Missing 'RIFF' chunk.";
    throw AudioConverterException(message);
  }

  audio_file.read(reinterpret_cast<char*>(&chunk_size), 4);

  audio_file.read(format, 4);
  if (std::string(format, 4) != "WAVE") {
    std::string message = "Invalid WAV file: Missing 'WAVE' format.";
    throw AudioConverterException(message);
  }

  char subchunk1ID[4];
  uint32_t subchunk1_size;
  uint16_t audio_format;
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;

  audio_file.read(subchunk1ID, 4);
  audio_file.read(reinterpret_cast<char*>(&subchunk1_size), 4);
  audio_file.read(reinterpret_cast<char*>(&audio_format), 2);
  audio_file.read(reinterpret_cast<char*>(&num_channels), 2);
  audio_file.read(reinterpret_cast<char*>(&sample_rate), 4);
  audio_file.read(reinterpret_cast<char*>(&byte_rate), 4);
  audio_file.read(reinterpret_cast<char*>(&block_align), 2);
  audio_file.read(reinterpret_cast<char*>(&bits_per_sample), 2);

  // Validate audio format (assuming PCM for simplicity)
  if (audio_format != 1) {
    std::string message = "Unsupported audio format.";
    throw AudioConverterException(message);
  }

  // Seek to the start of the audio data
  audio_file.seekg(4 + 4 + subchunk1_size, std::ios::beg);
}

void AudioConverter::ReadAudioData(std::ifstream& file) {
  constexpr int16_t MAX_INT_16 = 32767;
  constexpr int32_t MAX_INT_32 = 2147483647;

  while (!file.eof()) {
    int32_t sample;
    file.read(reinterpret_cast<char*>(&sample), bytes_per_sample_);

    if (!file.eof()) {
      double value;

      if (bytes_per_sample_ == 2) {
        value = static_cast<double>(sample) / MAX_INT_16;
      } else if (bytes_per_sample_ == 4) {
        value = static_cast<double>(sample) / MAX_INT_32;
      }

      audio_data_.push_back(value);
    }
  }
}
