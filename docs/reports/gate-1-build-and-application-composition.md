# Gate 1 review report: Comet build and application composition

Status: Implementation ready; canonical toolchain and hardware evidence pending  
Plan version: `engine-v0-plan-1`  
Implementation revision: `0c6ebd3326b6b2ae988c8783ac21aab97954c21f`  
Pinned Doggo revision: `2fbe5d80bba4db39b101370e0675ddb093d3bfb6`

## Delivered

- Initialized the independent Comet repository and pinned Doggo at `lib/doggo` with its complete recursive dependency checkout.
- Added all six public CMake presets, private-header/source policy, formatting verification, host-tool separation, and Switch package integration through Doggo.
- Added the `comet::components`, `comet::gameplay`, and `comet::presentation` static target skeletons plus `comet-app`, `comet-editor`, and `comet-cooker` where permitted by the selected graph.
- Reused one component registration function in the runtime, editor, cooker, and tests.
- Added a minimal SDL/Engine Windows boot path and platform-neutral source for the libnx application path. Comet does not include or name backend APIs or the ECS implementation.
- Verified that the shipping graph omits the editor, cooker, and format/development targets.

## Local evidence

The fallback validation used CMake 3.31.6, Ninja 1.12.1, MSVC 19.44.35229, and clang-format 19.1.5 because the current shell does not expose the locked clang-cl or devkitPro toolchains.

| Check | Result |
| --- | --- |
| Shared preset parsing (`cmake --list-presets`) | Pass; all six names are available |
| Windows development fallback configure/build | Pass |
| `ctest --test-dir out/build/windows-msvc-validation --output-on-failure` | Pass, 3/3 |
| Development app `--smoke`, cooker, and editor | Pass, exit code 0 |
| Windows shipping fallback configure/build and app `--smoke` | Pass |
| Shipping target inspection for editor/cooker/development targets | Pass, zero matches |
| Private-header, naming, extension, no-EnTT, and formatting policies | Pass |
| Recursive Doggo/submodule revision inspection | Pass |
| `git diff --check` | Pass |

## Acceptance status

| Gate 1 criterion | Status | Evidence or remaining action |
| --- | --- | --- |
| Clean host configure/build/test | Partial | Development and shipping MSVC fallback graphs are green. Run `windows-clang-dev` in the configured CLion toolchain. |
| Clean Switch cross-build | Pending | Run the host `comet-cooker` target, set an absolute `COMET_HOST_TOOLS_DIR`, then configure/build `switch-dev` in the devkitPro environment. |
| Physical hardware boot/shutdown | Pending | Cold launch, orderly shutdown, and repeated relaunch require the target console. |
| No RHI-dependent game code | Pass | Comet links only Gate 1 Doggo targets and contains no graphics/backend API references. |

## Known risks

- Canonical clang-cl diagnostics may differ from the fallback compiler.
- The libnx entrypoint and NRO packaging path remain uncompiled until devkitPro is available.
- The repository does not yet declare a project-source license.

Gate 1 is not accepted yet. No Gate 2 render-fixture work is included.
