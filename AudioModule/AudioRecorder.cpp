//
// Created by Akhmedov R on 07.08.2023.
//
#include "AudioRecorder.hpp"
#include "AudioExceptions.cpp"
#include "AudioConsts.hpp"

AudioRecorder::AudioRecorder() : stream_(nullptr), recording_(false){
	PaError err = Pa_Initialize();
	if (err != paNoError) {
		std::string message = "PortAudio error: ";
		message += Pa_GetErrorText(err);
		throw PortAudioException(message);
	}
}

AudioRecorder::~AudioRecorder(){
	if (stream_) {
		Pa_CloseStream(stream_);
		stream_ = nullptr;
	}
	Pa_Terminate();
}


void AudioRecorder::startRecording(int durationInSeconds) {
	if (recording_) {
		std::string message = "Error: Recording is already in progress";
		throw PortAudioException(message);
	}

	recordedData_.clear();
  
	PaError err;
	err = Pa_OpenDefaultStream( &stream_,
                              constant::numInputChannels,
                              constant::numOutputChannels,
                              paFloat32,
                              constant::kSamplingRate,
                              paFramesPerBufferUnspecified,
                              audioCallback,
                              this);
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

	Pa_Sleep(durationInSeconds * 1000);
	stopRecording();
}

void AudioRecorder::stopRecording() {
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

bool AudioRecorder::isRecording() const {
  return recording_;
}

std::vector<double>& AudioRecorder::getRecordedData() {
  return recordedData_;
}

int AudioRecorder::audioCallback(const void *inputBuffer, void *outputBuffer,
                           		   unsigned long framesPerBuffer,
                           		   const PaStreamCallbackTimeInfo *timeInfo,
                           		   PaStreamCallbackFlags statusFlags,
                           		   void *userData) {

    auto* recorder = static_cast<AudioRecorder*>(userData);
	const auto* input = static_cast<const double*>(inputBuffer);

	for (unsigned i = 0; i < framesPerBuffer; ++i) {
		recorder->recordedData_.push_back(input[i]);
	}

	return paContinue;
}
