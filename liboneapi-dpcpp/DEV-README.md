# liboneapi-dpcpp

## Versions

  Platform  Component ID                            Version
  --------  ------------                            -------
  linux     `intel.oneapi.lin.dpcpp-cpp-common`     2026.0.0+947  (devel only)
  windows   `intel.oneapi.win.cpp-dpcpp-common`     2026.0.0+944  (devel only)

Content manifest basename:
  linux:   `intel.oneapi.lin.compiler.content`
  windows: `intel.oneapi.win.compiler.content`

There is no separate runtime component for this package; all files come from
the devel package. Note: `intel.oneapi.lin.dpcpp-cpp-common` itself depends on
a *further* separate component, `intel.oneapi.lin.dpcpp-cpp-common.runtime`
(SYCL execution runtime: `libsycl.so`, `libur_loader.so`). That component is
**not** packaged here (one package per one archive) - it's what
`liboneapi-openmp/tests/basics/buildfile` cites as the reason the
`openmp-omptarget` test is disabled, and packaging it is a natural follow-up.

## Tarball layout

Tarballs extract under `_installdir/compiler/<ver>/` which maps to
`dpcpp-devel/` after the version-segment strip in `redist.build`. The archive
is ~1.5GB unpacked (~180MB compressed) and is extracted in full - `icx`/`icpx`
have many implicit relative-path dependencies (own clang resource-dir under
`lib/clang/22/`, SYCL device offload objects under `lib/`) that are not worth
the risk of cherry-picking.

### Linux/Windows -> `extract{dpcpp-devel}`

  dpcpp-devel/bin/icx                        - C/DPC++ compiler driver
  dpcpp-devel/bin/icpx                       - C++/DPC++ compiler driver
  dpcpp-devel/bin/icpx.cfg                   - compiler config (not an exe target)
  dpcpp-devel/bin/dpcpp, dpcpp-cl, opencl-aot - SYCL-mode tools (not exposed as targets)
  dpcpp-devel/bin/compiler/clang*            - underlying LLVM/clang tools (not exposed)

  dpcpp-devel/opt/compiler/include/          - omp.h, mathimf.h, bfp754*.h, complex.h, sdlt/, etc.
  dpcpp-devel/include/                       - SYCL, CL, unified-runtime, xpti headers

  dpcpp-devel/lib/clang/22/                  - clang's own resource-dir (builtin headers, sanitizer runtime)
  dpcpp-devel/lib/libsycl-*.o, *.bc          - SYCL device offload objects

## build2 targets

  Target        Content
  ------        -------
  lib{dpcpp}    Binless, headers-only. Exports `-I opt/compiler/include -I include`
                via `cc.export.poptions`. No shared/static library backs it.

  exe{icx}      bin/icx
  exe{icpx}     bin/icpx
  file{icpx.cfg} bin/icpx.cfg
  file{clang++} bin/clang++ (recreated symlink, Linux-only; see Gotchas below)

The `.exe` extension is omitted from `package.json`. The buildfile assigns it
on Windows via the `exes` loop (`"$(p).exe"`).

Only `icx`/`icpx` are exposed as targets (not `dpcpp`/`dpcpp-cl`/`opencl-aot`
or the raw `clang*`/`llvm-*` tools under `bin/compiler/`), mirroring how
`liboneapi-ifort` exposes only `ifx`/`fpp`/`xfortcom`, not every tool in its
`bin/`.

## Gotchas found while verifying `bdep test`

- **`build/export.build` must redirect `exe{}` imports into `dpcpp-devel/bin/`.**
  `package.json`'s `exes` entries are path-qualified (e.g.
  `"icx": "dpcpp-devel/bin/icx"`), so the buildfile's own `exe{$p}` target is
  really named `dpcpp-devel/bin/icx`, not bare `icx`. A consumer doing
  `import! icx = liboneapi-dpcpp%exe{icx}` asks for a target literally named
  `icx` at the exported root - if `export.build` just does
  `export $out_root/dpcpp/$import.target` (the plain `bdep new` scaffold),
  that bare name doesn't match anything declared, and build2's ad-hoc
  `exe{~'/(.+)/'}` touch rule silently creates a *new*, empty, wrong-location
  target instead of erroring (`exec format error` when the test tries to run
  it). `liboneapi-ifort/build/export.build` already special-cases this with
  an `if ($target_type($import.target) == exe) ...`; this package copies
  that pattern.

- **`bin/clang++` does not exist after plain `bsdtar` extraction.** Intel's
  installer normally runs a post-install `linking_tool.sh` step (see the
  `customActions` in the component's own `manifest.json`) that, among other
  things, renames URL-encoded archive entries - the tar literally contains
  `bin/compiler/clang%2B%2B` - and creates the `bin/clang++` sibling symlink
  `icx`/`icpx` look for relative to themselves at runtime. Skipping that
  step (as plain `bsdtar` extraction does) makes `icpx` fail with
  `error #10408: The Intel LLVM Based compiler cannot be found`. The
  buildfile recreates just that one symlink
  (`ln -s compiler/clang dpcpp-devel/bin/clang++`), Linux-only - the Windows
  archive's layout has not been inspected, so nothing is guessed there.

- **`build/root.build` needs the `config.liboneapi_dpcpp.cache` declaration.**
  Every other package's `build/root.build` has this; the plain `bdep new`
  scaffold does not. Without it `redist.build`'s
  `cache = [dir_path] $(config.$(pkg).cache)` evaluates to `null` and fails
  with "invalid argument: null value ... while concatenating dir_path".

## Gotchas found while verifying `b dist` and `b install`

- **The Linux-only `clang++` symlink target must not be wrapped in `if`.**
  Wrapping the whole `$name-devel/bin/file{clang++}: extract{$name-devel}
  {{...}}` declaration in `if ($cc.target.class == 'linux')` (as originally
  written) produces a different build graph per platform - exactly what the
  recent `if`-block-elimination restructuring fixed elsewhere in this repo -
  and `b dist` flags it: "conditional dependency declaration may result in
  incomplete distribution". Fix: keep the declaration unconditional, gate
  the `exe{$p}` prerequisite edge with `include = (... && $cc.target.class
  == 'linux')`, and gate only the recipe body with a `%` + `if` conditional
  recipe (see `agent-skills-build2`'s
  `HOWTO/config-independent-build-graph.md`).

- **`lib{dpcpp}` needs an explicit `[rule_hint=cxx]`.** Unlike every sibling
  package's `libs{$n}`, which always has a real `liba{}`/`libs{}` member
  backing it (a real `.a`/`.so`/`.lib` extracted from the archive), this
  package's `lib{dpcpp}` is genuinely binless - its only prerequisites are
  the `import{$name-legals}`/`import{$name-includes}` dyndep *groups*, not
  direct `hxx{}`/`h{}` targets. Without a `rule_hint`, `b install` (which
  tries to build both the static and shared variants) fails ambiguously:
  `c::link_rule`/`cxx::link_rule` both decline to match (`no C, C, obj/lib
  prerequisite or hint`), eventually surfacing as a confusing `unable to
  execute : no such file or directory` while trying to install the legal
  files. This is the same requirement documented for a sourceless "metadata
  library" in `agent-skills-build2`'s `HOWTO/header-only-library.md`.
  `b update`/`bdep test` never hit this because they never need the
  for-install variant.

- **`config.install.sudo=` (explicit empty string) breaks the install
  command line**, producing the exact same `unable to execute : no such
  file or directory` as the `rule_hint` issue above, which is easy to
  conflate with it during diagnosis. Omit the variable entirely on a
  passwordless/scratch install rather than setting it to empty; this
  reproduces identically on every other package in this repo (confirmed
  against `liboneapi-ifort`), so it is not specific to this package.

Verified with the full `dist -> configure -> update -> test -> clean`
recipe, a real `b install` to a scratch root (headers land at a single
merged `include/` prefix as expected, confirmed via the generated
`libdpcpp*.pc` files' single `-I` flag), and an out-of-source `tests/`
build against that installed copy using
`config.import.liboneapi_dpcpp=<dev-build-out_root>` (the documented
workaround for importing a pre-built executable with no `--build2-metadata`
support - see `agent-skills-build2`'s `guides/packaging-guide-testing.md`
and `HOWTO/target-metadata.md`).

## Usage

Import the headers for use with another compiler (e.g. system Clang/GCC):

  import libs = liboneapi-dpcpp%lib{dpcpp}

Import the compiler(s) as build tools:

  import! icx  = liboneapi-dpcpp%exe{icx}
  import! icpx = liboneapi-dpcpp%exe{icpx}
