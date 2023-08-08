#include "Constants.hpp"
#include "FastFourierTransform.hpp"
#include "Spectrogram.hpp"

using namespace constant;

template <class Field>
vector<typename FFTConverter<Field>::ObjectType> FFTConverter<Field>::Convert(
    const vector<double>& data) {
  size_t size = data.size();
  vector<complex> dft(data.begin(), data.end());
  if (!IsPowerOfTwo(size)) {
    dft.resize(GetNearestPowerOfTwo(size));
  }

  BitPermute(dft);

  for (size_t len = 2; len <= size; len <<= 1) {
    ObjectType primitive_root = field_.GetPrimitiveRoot(len);
    for (size_t i = 0; i < size; i += len) {
      ObjectType root(1);
      for (size_t j = 0; j < len / 2; ++j) {
        ObjectType left_part = dft[i + j];
        ObjectType right_part = dft[i + j + len / 2] * root;

        dft[i + j] = left_part + right_part;
        dft[i + j + len / 2] = left_part - right_part;
        root *= primitive_root;
      }
    }
  }

  return dft;
}

template <class IField>
vector<double> FFTConverter<IField>::Invert(const vector<ObjectType>& dft) {
  size_t size = dft.size();
  vector<ObjectType> dft_cpy(dft);
  if (!IsPowerOfTwo(size)) {
    throw std::runtime_error("not a power of two");
  }

  BitPermute(dft_cpy);

  for (size_t len = 2; len <= size; len <<= 1) {
    ObjectType primitive_root = field_.GetPrimitiveRoot((-1) * len);
    for (size_t i = 0; i < size; i += len) {
      ObjectType root(1);
      for (size_t j = 0; j < len / 2; ++j) {
        ObjectType left_part = dft_cpy[i + j];
        ObjectType right_part = dft_cpy[i + j + len / 2] * root;

        dft_cpy[i + j] = left_part + right_part;
        dft_cpy[i + j + len / 2] = left_part - right_part;
        root *= primitive_root;
      }
    }
  }

  vector<double> data(size);
  for (size_t i = 0; i < size; ++i) {
    data[i] = round((dft_cpy[i].real() / size));
  }

  return data;
}

template <class IField>
void FFTConverter<IField>::BitPermute(vector<ObjectType>& vec) {
  size_t size = vec.size();
  for (size_t j = 0, i = 1; i < size; ++i) {
    size_t bit = size >> 1;

    for (; (j & bit) != 0; bit >>= 1) {
      j ^= bit;
    }
    j ^= bit;

    if (i < j) {
      std::swap(vec[i], vec[j]);
    }
  }
}

template<class Field>
int64_t FFTConverter<Field>::GetNearestPowerOfTwo(int64_t n) {
  int64_t power = 1;
  while ((1 << power) < n) ++power;
  return (1 << power);
}

vector<double> ConvertTodB(const vector<complex>& amplitudes) {
  vector<double> magnitudes(amplitudes.size());
  for (size_t i = 0; i < amplitudes.size(); ++i) {
    auto magnitude = std::abs(amplitudes[i]);
    magnitudes[i] = 20 * std::log10(magnitude * magnitude  + kEps);
  }
  return magnitudes;
}

vector<vector<double>> GenerateSpectrogram(const vector<double>& samples,
                                           double stride_percent,
                                           double window_ms) {
  auto stride = static_cast<size_t>(0.001 * kSamplingRate * stride_percent * window_ms);
  auto window_size = static_cast<size_t>(0.001 * kSamplingRate * window_ms);

  size_t audio_size = samples.size();

  FFTConverter<ComplexField> converter;

  vector<vector<double>> spectrogram;

  for (size_t i = 0; i < audio_size; i += stride) {
    vector<double> slice(samples.begin() + i, samples.begin() + i + window_size);
    for (size_t j = 0; j < window_size; ++j) {
      slice[j] *= 0.5 * (1 - std::cos(2 * kPi * j / (window_size - 1)));
    }

    auto dft = converter.Convert(slice);

    spectrogram.emplace_back(ConvertTodB(dft));
  }

  return spectrogram;
}