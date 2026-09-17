# Comet build contract

Status: Accepted for Gate 0; presets become executable in Gate 1.

## Checkout

Comet is an independent repository. Doggo must be initialized at its pinned revision:

```powershell
git submodule update --init --recursive lib/doggo
```

Normal configure is offline. It does not fetch Doggo or third-party source.

## Presets and commands

Comet exposes the same public preset names and toolchain baselines as Doggo:

- `windows-clang-dev`
- `windows-clang-profile`
- `windows-clang-shipping`
- `switch-dev`
- `switch-profile`
- `switch-shipping`

CLion should open the checked-in `CMakePresets.json`. Configure its toolchain so `clang-cl`, Ninja, and the Windows SDK
environment are available; keep machine-specific paths in the CLion toolchain or ignored
`../../doggo/CMakeUserPresets.json`.

Canonical host commands:

```powershell
cmake --preset windows-clang-dev
cmake --build --preset windows-clang-dev
ctest --preset windows-clang-dev
```

Canonical Switch sequence:

```powershell
cmake --preset windows-clang-dev
cmake --build --preset windows-clang-dev --target comet-cooker

cmake --preset switch-dev
cmake --build --preset switch-dev
```

Host and Switch build trees are separate beneath `out/build/<preset>`. Cooked assets are written beneath
`out/assets/<target-profile>` and packaging consumes only the matching profile.

Gate 2 shader manifests are compiled as part of the Comet application graph. Windows writes SPIR-V beneath
`out/build/<preset>/generated/shaders/<shader-name>/`. Switch writes DKSH to the equivalent generated shader
directory, stages the compiled blobs and manifest beneath `generated/romfs/comet-app/`, and packages that ROMFS
tree into `comet-app.nro`. Neither runtime consumes source GLSL.

## Integration requirements

- Add Doggo through `add_subdirectory(lib/doggo)` without altering Doggo source or cache defaults globally.
- Link named `doggo::<module>` targets; do not include Doggo source directories manually.
- Comet's top-level configuration explicitly chooses the platform and enabled development facilities.
- Runtime, editor, and cooker use the same component-registration target.
- C++ files use only `.cpp` and `.hpp`; every Comet header is project-private beneath `src/`, follows the naming rules
  in `docs/architecture.md`, and is formatted with the repository `.clang-format`.
- The Switch build imports the already built Windows cooker by absolute path; it never executes a Switch target during
  its build.
- Shipping presets exclude editor panels, development UI, discovery, remote commands, and source-format importers from
  the graph.

## Content builds

A content cook is deterministic for the tuple of source bytes, import settings, Comet revision, Doggo revision, schema
versions, tool revision, and target profile. Missing or stale host tools are configure/build errors rather than reasons
to fall back to runtime source loading.
