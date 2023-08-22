#ifndef SHASHMAQAMFINDER_AUDIOMODULE_AUDIORECORDER_HPP_
#define SHASHMAQAMFINDER_AUDIOMODULE_AUDIORECORDER_HPP_

#include <portaudio.h>
#include <vector>

class AudioRecorder {
public:
  AudioRecorder();
  ~AudioRecorder();

  void startRecording(int durationInSeconds);
  void stopRecording();

  inline bool isRecording() const { return recording_; }

  inline std::vector<double> &getRecordedData() { return recordedData_; }

private:
  static int audioCallback(const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo *timeInfo,
                           PaStreamCallbackFlags statusFlags, void *userData);

  PaStream *stream_;
  bool recording_;
  std::vector<double> recordedData_;
};

#endif // SHASHMAQAMFINDER_AUDIOMODULE_AUDIORECORDER_HPP_