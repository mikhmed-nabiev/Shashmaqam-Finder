#include "AudioRecorder.hpp"

#include "../Math/Constants.hpp"
#include "AudioExceptions.cpp"

AudioRecorder::AudioRecorder() : stream_(nullptr), recording_(false) {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    std::string message = "PortAudio error: ";
    message += Pa_GetErrorText(err);
    throw PortAudioException(message);
  }
}

AudioRecorder::~AudioRecorder() {
  if (stream_) {
    Pa_CloseStream(stream_);
    stream_ = nullptr;
  }
  Pa_Terminate();
}

void AudioRecorder::StartRecording(int duration_in_seconds) {
  if (recording_) {
    std::string message = "Error: Recording is already in progress";
    throw PortAudioException(message);
  }

  recorded_data_.clear();

  PaError err;
  err = Pa_OpenDefaultStream(&stream_, constant::numInputChannels,
                             constant::numOutputChannels, paFloat32,
                             constant::kSamplingRate,
                             paFramesPerBufferUnspecified, AudioCallback, this);
  if (err != paNoError) {
    std::string message = "PortAudio error: ";
    message += Pa_GetErrorText(err);
    throw PortAudioException(message);
  }

  err = Pa_StartStream(stream_);
  if (err != paNoError) {
    std::string message = "PortAudio error: ";
    message += Pa_GetErrorText(err);
    throw PortAudioException(message);
  }
  recording_ = true;

  Pa_Sleep(duration_in_seconds * 1000);
  StopRecording();
}

void AudioRecorder::StopRecording() {
  if (!recording_) {
    std::string message = "Error: Recording is not in started";
    throw PortAudioException(message);
  }

  PaError err;
  err = Pa_StopStream(stream_);
  if (err != paNoError) {
    std::string message = "PortAudio error: ";
    message += Pa_GetErrorText(err);
    throw PortAudioException(message);
  }
  recording_ = false;
}

int AudioRecorder::AudioCallback(const void* input_buffer, void* output_buffer,
                                 unsigned long frames_per_buffer,
                                 const PaStreamCallbackTimeInfo* time_info,
                                 PaStreamCallbackFlags status_flags,
                                 void* user_data) {
  auto* recorder = static_cast<AudioRecorder*>(user_data);
  const auto* input = static_cast<const double*>(input_buffer);

  for (unsigned i = 0; i < frames_per_buffer; ++i) {
    recorder->recorded_data_.push_back(input[i]);
  }

  return paContinue;
}

void AudioRecorder::Clear() { recorded_data_.clear(); }

void AudioRecorder::SaveToWAV(const std::string& filename) {
  // WAV file header
  struct {
    char riff[4] = {'R', 'I', 'F', 'F'};
    uint32_t fileSize;
    char wave[4] = {'W', 'A', 'V', 'E'};
    char fmt[4] = {'f', 'm', 't', ' '};
    uint32_t fmtSize = 16;        // size of the fmt chunk
    uint16_t audioFormat = 1;     // PCM format
    uint16_t numChannels = 1;     // mono
    uint32_t sampleRate = 44100;  // adjust based on your recording settings
    uint32_t byteRate;            // calculated later
    uint16_t blockAlign;          // calculated later
    uint16_t bitsPerSample = 16;  // adjust based on your recording settings
    char data[4] = {'d', 'a', 't', 'a'};
    uint32_t dataSize;  // calculated later
  } wavHeader;

  wavHeader.byteRate = wavHeader.sampleRate * wavHeader.numChannels *
                       wavHeader.bitsPerSample / 8;
  wavHeader.blockAlign = wavHeader.numChannels * wavHeader.bitsPerSample / 8;
  wavHeader.dataSize = recorded_data_.size() * sizeof(double);

  // Calculate total file size
  wavHeader.fileSize = sizeof(wavHeader) - 8 + wavHeader.dataSize;

  // Write the header to the WAV file
  std::ofstream wavFile(filename.c_str(), std::ios::binary);
  if (!wavFile.is_open()) {
    return;
  }
  wavFile.write(reinterpret_cast<const char*>(&wavHeader), sizeof(wavHeader));

  // Write the recorded data to the WAV file
  wavFile.write(reinterpret_cast<const char*>(recorded_data_.data()),
                wavHeader.dataSize);
}