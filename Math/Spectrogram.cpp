//
// Created by Mikhmed Nabiev on 06.08.2023.
//
#include "FastFourierTransform.hpp"
#include "Spectrogram.hpp"
#include "Constants.hpp"

using namespace constant;

// vector<double> ConvertTodB(const vector<complex>& amplitudes) {
//   vector<double> magnitudes(amplitudes.size());
//   for (size_t i = 0; i < amplitudes.size(); ++i) {
//     auto magnitude = std::abs(amplitudes[i]);
//     magnitudes[i] = 20 * std::log10(magnitude * magnitude  + kEps);
//   }
//   return magnitudes;
// }

// vector<vector<double>> GenerateSpectrogram(const vector<double>& samples,
//                                            double stride_percent,
//                                            double window_ms) {
//   auto stride = static_cast<size_t>(0.001 * kSamplingRate * stride_percent * window_ms);
//   auto window_size = static_cast<size_t>(0.001 * kSamplingRate * window_ms);

//   size_t audio_size = samples.size();

//   FFTConverter<ComplexField> converter;

//   vector<vector<double>> spectrogram;

//   for (size_t i = 0; i < audio_size; i += stride) {
//     vector<double> slice(samples.begin() + i, samples.begin() + i + window_size);
//     for (size_t j = 0; j < window_size; ++j) {
//       slice[j] *= 0.5 * (1 - std::cos(2 * kPi * j / (window_size - 1)));
//     }

//     auto dft = converter.Convert(slice);

//     spectrogram.emplace_back(ConvertTodB(dft));
//   }

//   return spectrogram;
// }
