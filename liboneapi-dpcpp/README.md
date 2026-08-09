# liboneapi-dpcpp - Intel DPC++/C++ Compiler (icx/icpx) and headers

> **NOTE:**  
This package is not open source and does not contain any source code. Instead,
in order to "build" the exported target(s) it downloads (potentially large)
pre-built binaries provided by Intel for the target platform.
>
> CI for this package is disabled due to the above.  
Supported platforms/compilers are Windows/MSVC and Linux.

This is a `build2` package for the [Intel oneAPI DPC++/C++
Compiler](https://www.intel.com/content/www/us/en/developer/tools/oneapi/dpc-compiler.html).
It provides the `icx` and `icpx` compiler driver executables, along with the
headers they ship (`omp.h`, `mathimf.h`, and other host C/C++ headers under
`opt/compiler/include/`, plus the SYCL/CL/unified-runtime headers under
`include/`).


## Usage

To start using `liboneapi-dpcpp` in your project, add the following `depends`
value to your `manifest`, adjusting the version constraint as appropriate:

```
depends: liboneapi-dpcpp ^2026.0.0
```

Then import the headers for use with another compiler (e.g. to make `omp.h`
available to a system Clang or GCC build):

```
import libs = liboneapi-dpcpp%lib{dpcpp}
```

Or import the compiler as a build tool:

```
import! icx = liboneapi-dpcpp%exe{icx}
```


## Importable targets

This package provides the following importable targets:

```
lib{dpcpp}
exe{icx}
exe{icpx}
```

`lib{dpcpp}` is a binless, headers-only interface library: it exports
include paths only, it does not link against any shared or static library.


## Configuration variables

This package provides the following configuration variables:

```
[dir_path] config.liboneapi_dpcpp.cache ?= $out_root
```

The directory used to cache downloaded binary archives between builds. If
`config.liboneapi.cache` is set (a shared project-wide cache directory), it
takes precedence over the per-package default of `$out_root`.
