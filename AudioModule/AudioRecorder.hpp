#ifndef SHASHMAQAMFINDER_AUDIOMODULE_AUDIORECORDER_HPP_
#define SHASHMAQAMFINDER_AUDIOMODULE_AUDIORECORDER_HPP_

#include <portaudio.h>
#include <vector>

class AudioRecorder {
public:
  AudioRecorder();
  ~AudioRecorder();

  void StartRecording(int duration_in_seconds);
  void StopRecording();

  inline bool IsRecording() const { return recording_; }

  inline std::vector<double> &GetRecordedData() { return recorded_data_; }

private:
  static int AudioCallback(const void* input_buffer, void* output_buffer,
                           unsigned long frames_per_buffer,
                           const PaStreamCallbackTimeInfo* time_info,
                           PaStreamCallbackFlags status_flags, void* user_data);

  PaStream* stream_;
  bool recording_;
  std::vector<double> recorded_data_;
};

#endif // SHASHMAQAMFINDER_AUDIOMODULE_AUDIORECORDER_HPP_