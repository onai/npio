// Simple example to load an array, modify it and save it.

#define NPIO_CXX_ENABLE_EXCEPTIONS
#include "npio.h"

int main()
{
  auto a = npio::Array("test2.npy");
  for (auto &v : a.values<float>())
    v += 10.0f;
  a.save("example3-out.npy");
}
