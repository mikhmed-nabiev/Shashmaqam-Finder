#ifndef SHASHMAQAM_FINDER_Math_FASTFOURIERTRANSFORM_H_
#define SHASHMAQAM_FINDER_Math_FASTFOURIERTRANSFORM_H_

#pragma once
#include "Field.h"
#include <vector>

template <class Field>
class FFTConverter {
 public:
  using ObjectType = typename Field::ObjectType;

  std::vector<ObjectType> Convert(const std::vector<int64_t>& poly);

  std::vector<int64_t> Invert(const std::vector<ObjectType>& dft);

 private:
  bool IsPowerOfTwo(int64_t num) { return 0 == (num & (num - 1)); }

  void BitPermute(std::vector<ObjectType>& vec);

  Field field_;
  std::vector<Field> roots_;
};

#endif //SHASHMAQAM_FINDER_Math_FASTFOURIERTRANSFORM_H_
