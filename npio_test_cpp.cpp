#include <cassert>
#include "npio.h"

int main()
{
  npio::Array a("test1.npy");
  int64_t total = 0;
  int64_t* values = a.get<int64_t>();
  for (size_t i = 0; i < a.size(); ++i)
    total += values[i];
  assert(total == 4950);
  return 0;
}
