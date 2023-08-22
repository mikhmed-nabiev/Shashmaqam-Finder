#include "AudioRecorder.hpp"
#include "Math/Constants.hpp"
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
}

int AudioRecorder::AudioCallback(const void *input_buffer, void *output_buffer,
                           unsigned long frames_per_buffer,
                           const PaStreamCallbackTimeInfo *time_info,
                           PaStreamCallbackFlags status_flags, void *user_data) {

  auto *recorder = static_cast<AudioRecorder *>(user_data);
  const auto *input = static_cast<const double *>(input_buffer);

  for (unsigned i = 0; i < frames_per_buffer; ++i) {
    recorder->recorded_data_.push_back(input[i]);
  }

  return paContinue;
}
