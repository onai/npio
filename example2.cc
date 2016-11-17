// Simple example to print the elements of an array in npy format.

#include <iostream>

#define NPIO_CXX_ENABLE_EXCEPTIONS
#include "npio.h"

int main()
{
  auto a = npio::Array("test2.npy");
  for (auto v: a.values<float>())
    std::cout << v << "\n";
}
