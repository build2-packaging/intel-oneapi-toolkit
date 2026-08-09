# intel-oneapi packaging notes

## Packages

  [liboneapi-compilers-common/DEV-README.md](liboneapi-compilers-common/DEV-README.md) - Compiler common runtime (libimf, libsvml, libintlc)
  [liboneapi-dpcpp/DEV-README.md](liboneapi-dpcpp/DEV-README.md)                     - DPC++/C++ compiler (icx/icpx), headers
  [liboneapi-mkl/DEV-README.md](liboneapi-mkl/DEV-README.md)                         - MKL targets, layout, platform notes
  [liboneapi-openmp/DEV-README.md](liboneapi-openmp/DEV-README.md)                   - OpenMP targets, layout, platform notes
  [liboneapi-tbb/DEV-README.md](liboneapi-tbb/DEV-README.md)                         - TBB targets, layout, platform notes

## Download mechanism

`redist.build` implements a three-step download per component:

  1. Fetch content manifest JSON from Intel CDN:
       {baseUrl}/{subdir}/{basename}{verSuffix}{version}.ext
     e.g. https://installer.repos.intel.com/oneapi/mkl/lin/
            intel.oneapi.lin.onemkl.content,v=2026.0.0+908.json

  2. Each package's `manifest.json` names its components explicitly via
     `runtime_id` (and `devel_id` where applicable). Parse the content
     manifest's `packages[]` array to find the entry whose ID ends with
     the given `runtime_id` or `devel_id`, then fetch that component's
     `manifest.json`.

  3. From the component `manifest.json`, locate `cupPayload.cup` and
     download it (cached as a `.tar` in `$out_root/`).

  4. Extract with `bsdtar`, stripping the path prefix up to and including
     the version segment:
       _installdir/<component>/<ver>/<rest>  ->  <extract-name>/<rest>

## Content manifest basenames

  Package             linux                                 windows
  -------             -----                                 -------
  dnnl (*)            intel.oneapi.lin.onednn.content       intel.oneapi.win.onednn.content
  mkl                 intel.oneapi.lin.onemkl.content       intel.oneapi.win.onemkl.content
  tbb                 intel.oneapi.lin.tbb_oneapi.content  intel.oneapi.win.tbb_oneapi.content
  openmp              intel.oneapi.lin.compiler.content    intel.oneapi.win.compiler.content
  compilers-common    intel.oneapi.lin.compiler.content     intel.oneapi.win.compiler.content
  dpcpp               intel.oneapi.lin.compiler.content     intel.oneapi.win.compiler.content

(*) oneDNN is open source (Apache-2.0). Upstream: https://github.com/uxlfoundation/oneDNN
    Not packaged here; consider building from source rather than wrapping Intel's binary.
    Single component (no devel/runtime split): intel.oneapi.win.dnnl / intel.oneapi.lin.dnnl.
    Note: the content manifest basename uses "onednn" while the component ID uses "dnnl".

## Cache directory

Downloaded component archives are placed in `$out_root` by default. They are
not registered as build2 targets, so `bdep clean` leaves them untouched. Set
one of the two `dir_path` variables below to redirect them to a different
location (for example, a directory shared across multiple build configurations):

  Variable                              Scope
  --------                              -----
  config.liboneapi.cache                global: applies to all packages when set
  config.liboneapi_<pkg>.cache          per-package: overrides the global value

Resolution order (evaluated in `root.build` for each package):

  1. `config.liboneapi_<pkg>.cache` if explicitly set, otherwise
  2. `config.liboneapi.cache` if explicitly set, otherwise
  3. `$out_root` (the package output directory)

The package name substitution follows build2 variable naming rules: hyphens
in the package name are replaced with underscores. For example,
`liboneapi-compilers-common` uses `config.liboneapi_compilers_common.cache`.

Set the global variable once to share a cache across all packages:

  bdep init -C @gcc cc config.cxx=g++ \
    "config.liboneapi.cache=/path/to/cache-dir"

Or override a single package:

  b "config.liboneapi_mkl.cache=/path/to/mkl-cache-dir"

## Updating to a new release

The only field that changes per release in each package's `manifest.json` is
`version` (e.g. `2026.0.0+908`). The `+build` suffix is not derivable from
the upstream version alone; the installer must be run once to fetch it.

### Step 1: obtain the offline installer

Download the offline installer from:
  https://www.intel.com/content/www/us/en/developer/tools/oneapi/oneapi-toolkit-download.html

Select: Intel oneAPI Toolkit - Linux or Windows - Offline Installer.

This produces intel-oneapi-toolkit-20YY.X.Y.Z.BUILD_offline.sh on Linux
and intel-oneapi-toolkit-20YY.X.Y.Z.BUILD_offline.exe on Windows.

### Step 2: list components

Run the appropriate command for the target platform. Both produce the same
tabular output: component ID, version+build string, installed flag, and name.

Linux - install.sh is a thin wrapper around the bootstrapper binary:

  Get all component IDs and version+build numbers (queries CDN, no install):
    ./install.sh --list-components

  --list-components downloads the package manager (~10 MB) but not the
  component payloads, and prints every component ID with its exact
  version+build string.

  Other useful bootstrapper flags:

    Download payloads without installing (requires EULA acceptance):
      ./install.sh -s --action downloadonly --eula accept \
        --components <id>:<id>:... \
        --download-dir <dir>

    --download-cache <dir>   where to store the package manager cache
    --proxy <url>            proxy support if needed

Windows - use the offline installer exe. The outer exe requires elevation.
Run from an administrator cmd.exe prompt:

    intel-oneapi-toolkit-YYYY.N.N.BBB_offline.exe --silent --a --list-components

  The output format is identical to the Linux bootstrapper output.

### Step 3: update manifest.json

Match the listed component IDs against the per-package DEV-READMEs to find
the relevant version+build strings. Update the `version` field in each
`manifest.json` accordingly.

  Package                  Windows component ID                        Linux component ID
  -------                  --------------------                        ------------------
  liboneapi-mkl            intel.oneapi.win.mkl.devel                  intel.oneapi.lin.mkl.devel
                           intel.oneapi.win.mkl.runtime                intel.oneapi.lin.mkl.runtime
  liboneapi-tbb            intel.oneapi.win.tbb.devel                  intel.oneapi.lin.tbb.devel
                           intel.oneapi.win.tbb.runtime                intel.oneapi.lin.tbb.runtime
  liboneapi-openmp         intel.oneapi.win.openmp                     intel.oneapi.lin.openmp
  liboneapi-compilers-     intel.oneapi.win.compilers-common           intel.oneapi.lin.compilers-common
    common                 intel.oneapi.win.compilers-common-runtime   intel.oneapi.lin.compilers-common.runtime
  liboneapi-dpcpp          intel.oneapi.win.cpp-dpcpp-common           intel.oneapi.lin.dpcpp-cpp-common

Note: `compilers-common` uses a hyphen before `runtime` on Windows
(`compilers-common-runtime`) but a dot on Linux (`compilers-common.runtime`).
This naming inconsistency is why all packages now use explicit `runtime_id`
and `devel_id` fields in `manifest.json` rather than relying on suffix
matching.

Also update the Versions table in each package's DEV-README.md to record the
verified version+build strings.

### Probing for libs, libi, and liba

After `bdep update` has extracted the component tarballs, run these commands
from the project root to discover what files belong under each package.json
key. Replace the example path with the appropriate package out_base.

Linux:

  Shared libraries (libs):

    `find ../intel-oneapi-toolkit-gcc/liboneapi-mkl/mkl -name "*.so*" ! -type l | sort`

  Static archives (liba):

    `find ../intel-oneapi-toolkit-gcc/liboneapi-mkl/mkl -name "*.a" | sort`

Windows (VS Developer Command Prompt):

  Shared libraries (libs - DLLs):

    `dir /s /b ..\intel-oneapi-toolkit-msvc\liboneapi-mkl\mkl\*.dll | sort`

  All .lib files (libi or liba, needs classification):

    `dir /s /b ..\intel-oneapi-toolkit-msvc\liboneapi-mkl\mkl\*.lib | sort`

  Classify each .lib (.obj members => liba, otherwise libi).
  Output sorted: liba first, then libi:

    `(for /r ..\intel-oneapi-toolkit-msvc\liboneapi-mkl\mkl %f in (*.lib) do @(dumpbin /ARCHIVEMEMBERS "%f" 2>nul | findstr /c:".obj" >nul && echo liba: %f || echo libi: %f)) | sort`

### Using upstream .pc files for verification

Intel ships pkg-config files for MKL, TBB, and OpenMP inside the devel
extract.  They are useful for cross-checking link lines and cross-package
`Requires` entries but are not a complete source of truth:

- **MKL**: one `.pc` per layered combo (interface x threading x static/dynamic).
  The `Libs:` line lists the exact `-l` flags in the correct order, including
  `-Wl,--no-as-needed` for the gomp variant.  The `Requires:` field covers the
  cross-package openmp and tbb dependencies.  Useful for verifying `loptions`
  and `depends` in `package.json`.
- **TBB / OpenMP**: minimal files (`-ltbb`, `-liomp5`).  Confirm the library
  name but add no inter-package dependency information.
- **ifort, compilers-common**: no `.pc` files ship.  All dependency information
  for those packages must be derived from `readelf` (see below).
- **Gaps in MKL coverage**: the GNU Fortran interface libs (`mkl_gf_lp64`,
  `mkl_gf_ilp64`), the BLACS/cluster targets, and the runtime vs. devel file
  split are not represented in any `.pc` file.

Location after `bdep update` (example for MKL):

  `find $(bdep config list | awk '/default/{print $2}')/liboneapi-mkl -name "*.pc"`

### Probing for inter-target DLL/SO dependencies

After extracting the component tarballs, run these commands from the project
root to find which shared libraries within the same package each library
directly imports. The result drives the `depends` entries in `package.json`.

Linux:

  For each non-symlink .so, print intra-package DT_NEEDED entries as
  `<lib> depends: <dep> ...`:

    `b=$(bdep config list | grep default | awk '{ print $2 }')/liboneapi-mkl; pkglibs=$(find $b -name "*.so*" ! -type l -exec basename {} \;); for f in $(find $b -name "*.so*" ! -type l); do deps=$(readelf -d "$f" 2>/dev/null | grep NEEDED | grep -oP '\[\K[^\]]+' | while read d; do echo "$pkglibs" | grep -qxF "$d" && printf '%s ' "$d"; done); [ -n "$deps" ] && echo "$(basename $f) depends: $deps"; done; echo done`

Windows (VS Developer Command Prompt):

  For each DLL, print one `<dll> depends: <dep>` line per intra-package import:

    `for /f "tokens=2" %c in ('bdep config list ^| findstr default') do @for /f %f in ('dir /s /b "%c\liboneapi-mkl\*.dll" 2^>nul') do @for /f %i in ('dumpbin /IMPORTS "%f" 2^>nul ^| findstr /ri "\.dll"') do @for /f %g in ('dir /s /b "%c\liboneapi-mkl\%i" 2^>nul') do @echo %~nxf depends: %~nxg`
