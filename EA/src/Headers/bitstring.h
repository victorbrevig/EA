#ifndef BITSTRING_H
#define BITSTRING_H
#include <vector>
#include "threeSATinstance.h"

class Bitstring
{
  std::vector<bool> content;
  std::vector<uint32_t> indexes; //Helper for random ordering of mutations
  mutable uint32_t cachedFitness;
  mutable bool cachedFitnessIsValid;
public:

  Bitstring();
  Bitstring(uint32_t size);
  Bitstring(uint32_t size, bool defaultValue);
  uint32_t NumberOfBits() const;
  void Setbit(uint32_t index, bool bit);
  void Flip(uint32_t index);
  void MutationLSFI(const ThreeSATInstance& threeSATInstance);
  void Mutation(double mutationRate);
  uint32_t GetFitness(const ThreeSATInstance& threeSATInstance) const;
  static uint32_t HammingDistance(const Bitstring& a, const Bitstring& b);
  static Bitstring GPX(const Bitstring& a, const Bitstring& b, const ThreeSATInstance& threeSATInstance);
  static Bitstring TwoPointCrossover(const Bitstring& a, const Bitstring& b);
};
#endif