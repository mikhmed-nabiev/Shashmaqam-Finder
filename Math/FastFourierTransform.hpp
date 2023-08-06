#ifndef SHASHMAQAM_FINDER_Math_FASTFOURIERTRANSFORM_H_
#define SHASHMAQAM_FINDER_Math_FASTFOURIERTRANSFORM_H_

#pragma once
#include "Field.hpp"
#include <vector>

using std::vector;

template <class Field>
class FFTConverter {
 public:
  using ObjectType = typename Field::ObjectType;

  vector<ObjectType> Convert(const vector<double>& poly);

  vector<double> Invert(const vector<ObjectType>& dft);

 private:
  int64_t GetNearestPowerOfTwo(int64_t n);

  bool IsPowerOfTwo(int64_t num) { return 0 == (num & (num - 1)); }

  void BitPermute(vector<ObjectType>& vec);

  Field field_;
  vector<Field> roots_;
};

#endif //SHASHMAQAM_FINDER_Math_FASTFOURIERTRANSFORM_H_
