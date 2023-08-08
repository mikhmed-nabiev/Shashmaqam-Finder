//
// Created by Mikhmed Nabiev on 04.08.2023.
//
#ifndef SHASHMAQAMFINDER_MATH_SPECTOGRAM_HPP_
#define SHASHMAQAMFINDER_MATH_SPECTOGRAM_HPP_

#pragma once
#include "FastFourierTransform.hpp"
#include <vector>

using std::vector;

vector<double> ConvertTodB(const vector<complex>& amplitudes);

vector<vector<double>> GenerateSpectrogram(const vector<double>& samples,
                                           double stride_percent = 0.5,
                                           double window_ms = 20);

#endif //SHASHMAQAMFINDER_MATH_SPECTOGRAM_HPP_
