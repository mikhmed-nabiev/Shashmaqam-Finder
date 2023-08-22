//
// Created by Akhmedov R on 07.08.2023.
//
#ifndef AUDIO_RECORDER_HPP
#define AUDIO_RECORDER_HPP

#include <portaudio.h>
#include <vector>

class AudioRecorder{
 public:
  AudioRecorder();
  ~AudioRecorder();

  void startRecording(int durationInSeconds);
  void stopRecording();

  bool isRecording() const;

  std::vector<double>& getRecordedData();

 private:
  static int audioCallback(const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo *timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData);

  PaStream* stream_;
  bool recording_;
  std::vector<double> recordedData_;
};

#endif // AUDIO_RECORDER_HPP