// Compiled directly with icpx (via testscript, not build2's own cxx rule)
// to verify the packaged compiler resolves its own bundled omp.h with no
// extra include paths - the gap that motivated this package.
#include <omp.h>

int main ()
{
#pragma omp parallel
  {
  }
  return 0;
}
