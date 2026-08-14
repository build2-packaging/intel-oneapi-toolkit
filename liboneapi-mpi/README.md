# liboneapi-mpi - Intel oneAPI MPI Library

> **NOTE:**
> This package is not open source and does not contain any source code. Instead,
> in order to "build" the exported target(s) it downloads (potentially large)
> pre-built binaries provided by Intel for the target platform.
>
> CI for this package is disabled due to the above.
> Supported platforms/compilers are Windows/MSVC and Linux.

This is a `build2` package for the [Intel oneAPI MPI Library](https://www.intel.com/content/www/us/en/developer/tools/oneapi/mpi-library.html),
a high-performance implementation of the Message Passing Interface (MPI) standard
built on the MPICH open-source implementation.


## Usage

To start using `liboneapi-mpi` in your project, add the following `depends`
value to your `manifest`, adjusting the version constraint as appropriate:

```
depends: liboneapi-mpi ^2026.0.0
```

Then import the library in your `buildfile`:

```
import libs = liboneapi-mpi%libs{mpi}
```

Or import the job launcher or diagnostic tool as a build tool:

```
import! mpiexec   = liboneapi-mpi%exe{mpiexec}
import! impi_info = liboneapi-mpi%exe{impi_info}
```


## Importable targets

This package provides the following importable targets:

```
libs{mpi}
libs{mpicxx}      (Linux only: C++ bindings)
libs{mpifort}     (Linux only: Fortran bindings)
exe{mpiexec}
exe{impi_info}
```


## Configuration variables

This package provides the following configuration variables:

```
[dir_path] config.liboneapi_mpi.cache ?= $out_root
```

The directory used to cache downloaded binary archives between builds. If
`config.liboneapi.cache` is set (a shared project-wide cache directory), it
takes precedence over the per-package default of `$out_root`.
