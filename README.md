# Intel® oneAPI Toolkit Development Environment

> **NOTE:**  
These packages are not open source and do not contain any source code. Instead,
in order to "build" the exported target(s) they download (potentially large)
pre-built binaries provided by Intel for the target platform.
>
> CI for these packages are disabled due to the above.  
Supported platforms/compilers are Windows/MSVC & Linux. MacOS is (officially)
not supported.

This is a `build2` package repository for [Intel® oneAPI Toolkit](https://www.intel.com/content/www/us/en/developer/tools/oneapi/oneapi-toolkit.html).
Note that it currently does not contain all of the packages.

Currently packaged:
- oneMKL (Math Kernel Library)
- oneTBB (Threading Building Blocks)
- OpenMP runtime (libiomp5)
- Fortran compiler runtime (ifort/ifx)
- Compiler common runtime (libimf, libsvml, libintlc, xpti, qkmalloc)
- MPI Library
- DPC++/C++ Compiler (icx/icpx, host compilation)

Not yet packaged:
- DPC++/SYCL offload runtime (libsycl, libur_loader)
- IPP (Integrated Performance Primitives)
- VTune Profiler

This file contains setup instructions and other details that are more
appropriate for package development rather than consumption. If you want to use
`intel-oneapi-toolkit` in your `build2`-based project, then instead see the accompanying
package(s) corresponding [`README.md`](liboneapi-mkl/README.md) file.

The development setup for `intel-oneapi-toolkit` uses the standard `bdep`-based workflow.
For example:

```
git clone https://github.com/build2-packaging/intel-oneapi-toolkit.git
cd intel-oneapi-toolkit

bdep init -C @gcc cc config.cxx=g++
bdep update
bdep test
```

See [DEV-README.md](./DEV-README.md) for the download mechanism, per-package
layout, and instructions for updating to a new upstream release.
