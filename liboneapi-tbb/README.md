# liboneapi-tbb - Intel oneAPI Threading Building Blocks

> **NOTE:**  
This package is not open source and does not contain any source code. Instead,
in order to "build" the exported target(s) it downloads (potentially large)
pre-built binaries provided by Intel for the target platform.
>
> CI for this package is disabled due to the above.  
Supported platforms/compilers are Windows/MSVC and Linux.

This is a `build2` package for the [Intel oneAPI Threading Building Blocks
(oneTBB)](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onetbb.html) C++ library. It provides
scalable parallel algorithms, a work-stealing task scheduler, concurrent
containers, and a thread-safe scalable memory allocator that adapts
automatically to the available hardware parallelism.


## Usage

To start using `liboneapi-tbb` in your project, add the following `depends`
value to your `manifest`, adjusting the version constraint as appropriate:

```
depends: liboneapi-tbb ^2026.0.0
```

Then import the library in your `buildfile`:

```
import libs = liboneapi-tbb%libs{tbb}
```


## Importable targets

This package provides the following importable targets:

```
libs{tbb}
libs{tbb-malloc}
libs{tbb-malloc-proxy}
libs{tbb-bind}
```


## Configuration variables

This package provides the following configuration variables:

```
[dir_path] config.liboneapi_tbb.cache ?= $out_root
```

The directory used to cache downloaded binary archives between builds. If
`config.liboneapi.cache` is set (a shared project-wide cache directory), it
takes precedence over the per-package default of `$out_root`.
