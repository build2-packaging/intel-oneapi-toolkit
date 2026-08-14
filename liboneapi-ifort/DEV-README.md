# liboneapi-ifort

## Versions

  Platform  Component ID                           Version
  --------  ------------                           -------
  linux     `intel.oneapi.lin.ifort-compiler`      2026.0.0+947  (devel only)
  windows   `intel.oneapi.win.ifort-compiler`      2026.0.0+944  (devel only)

Content manifest basename:
  linux:   `intel.oneapi.lin.compiler.content`
  windows: `intel.oneapi.win.compiler.content`

There is no separate runtime component; all files come from the devel package.

## Tarball layout

Tarballs extract under `_installdir/compiler/<ver>/` which maps to
`ifort-devel/` after the version-segment strip in `redist.build`.

### Linux  ->  `extract{ifort-devel}`

  ifort-devel/bin/fpp               - Fortran preprocessor
  ifort-devel/bin/ifx               - Intel Fortran Compiler driver
  ifort-devel/bin/ifx.cfg           - compiler config (not an exe target)
  ifort-devel/bin/compiler/xfortcom - backend compiler (subdirectory)

  ifort-devel/lib/libifcore.so.5    - Fortran core runtime (ELF, SONAME libifcore.so.5)
  ifort-devel/lib/libifcoremt.so.5  - thread-safe Fortran core runtime
  ifort-devel/lib/libifport.so.5    - Fortran portability library
  ifort-devel/lib/libicaf.so        - Coarray Fortran runtime (not in ifort-rt, see below)

  ifort-devel/lib/libifcore.so      - GNU linker script: INPUT(libifcore.so.5)
  ifort-devel/lib/libifcoremt.so    - GNU linker script: INPUT(libifcoremt.so.5)
  ifort-devel/lib/libifport.so      - GNU linker script: INPUT(libifport.so.5)

### Windows  ->  `extract{ifort-devel}`

  ifort-devel/bin/libifcoremd.dll   - Fortran core runtime (DLL CRT variant)
  ifort-devel/bin/libifportmd.dll   - Fortran portability library (DLL CRT variant)

  ifort-devel/lib/libifcoremd.lib   - liba (static archive)
  ifort-devel/lib/libifportmd.lib   - libi

  Note: there is no separate thread-safe DLL on Windows; libifcoremd.dll covers
  both roles.

## build2 targets

  Target      Linux libs                    Windows libs / libi / liba
  ------      ----------                    --------------------------
  ifort       libifcore.so.5                libifcoremd.dll
              libifcoremt.so.5              libifportmd.dll
              libifport.so.5                libi: libifportmd.lib
                                            liba: libifcoremd.lib

  exe{ifx}      bin/ifx                     bin/ifx
  exe{fpp}      bin/fpp                     bin/fpp
  exe{xfortcom} bin/compiler/xfortcom       bin/compiler/xfortcom
  file{ifx.cfg} bin/ifx.cfg                bin/ifx.cfg

The `.exe` extension is included explicitly in `package.json` for Windows paths
(e.g. `ifx.exe`, `fpp.exe`, `compiler/xfortcom.exe`). The buildfile uses
`exe{$(p)...}` (not `exe{$p}`): the trailing `...` in a build2 name pattern
prevents the last dot from being used as the extension separator, so the full
path (including `.exe`) becomes the target stem rather than a hard-coded
extension. Without it, the Windows-platform exe targets declared with
`include = false` on Linux would carry `.exe` as a target-level extension,
causing the test runner to attempt executing non-existent `.exe` files on Linux
and fail with exec-format errors.

`libicaf.so` (Coarray Fortran runtime) is not included in `ifort` because it
has an undeclared dependency on MPI (`libmpi.so.12`). It can be added once MPI
is packaged as a separate build2 package and declared as a dependency here.

## Usage

Link Fortran-compiled code against `libs{ifort}`.

Import the compiler via:

  import! ifx = liboneapi-ifort%exe{ifx}
