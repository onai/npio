// Simple example to save a random array to npy format

#include <random>
#include <vector>
#include "npio.h"

int main()
{
  std::vector<float> v(1000);
  std::mt19937 rng;
  std::normal_distribution<float> d(0.0, 1.0);
  for (auto &x : v)
    x = d(rng);
  return npio::save("example4-out.npy", {10, 100}, &v[0]);
}
