// Compiled natively by build2 with whatever compiler this test config uses
// (e.g. system Clang or GCC), linked against lib{dpcpp} for its exported
// include paths only. Verifies liboneapi-dpcpp's cc.export.poptions makes
// omp.h discoverable to an independent compiler - the gap that motivated
// this package.
#include <omp.h>

int main () { return 0; }
