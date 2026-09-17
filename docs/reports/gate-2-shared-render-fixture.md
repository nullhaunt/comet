# Gate 2 review report: Comet shared render fixture

Status: Accepted  
Plan version: `engine-v0-plan-1`  
Implementation base revision: `cd350fb44569ee8cb5089072022fb7baaf0cc654`  
Pinned Doggo revision: `c17eb5dd1f20a04d63705201f208f572c375a8d2`

## Delivered

- Added one Comet-owned indexed triangle, 2 x 2 RGBA texture, shader manifest, and shared-subset GLSL pair.
- Added a Comet presentation fixture that creates and renders those inputs exclusively through Doggo's public RHI.
- Added Comet-side SPIR-V and DKSH builds from the same manifest and GLSL sources.
- Added Switch ROMFS staging for the DKSH pair and manifest, then attached that ROMFS tree to `comet-app.nro`.
- Added a focused composition test for fixture dimensions, index validity, and texture size.
- Extended the source-policy check to reject direct platform or graphics-backend APIs in Comet source.
- Preserved the Gate 1 `--smoke` path and added `--render-smoke` for a three-frame Windows render check.
- Added no Gate 3 asset database, cooker, ECS, reflection, scene, zone, or streaming work.

## Verification environment

The Windows development build used CMake 4.3.1, Ninja 1.13.2, clang-cl 19.1.5, and Vulkan SDK 1.4.357.0.
The Switch development cross-build ran under WSL with CMake 3.28.3, Ninja 1.13.2, and devkitA64 GCC 16.1.0.

The captured devkitPro package manifest was:

```text
deko3d 0.5.0-1
devkitA64 r30-1
devkitA64-gdb 14.1-1
libnx 4.12.0-1
switch-tools 1.13.1-1
uam 1.1.0-1
```

| Preset or artifact | Command or inspection | Result |
| --- | --- | --- |
| `windows-clang-dev` | `cmake --preset windows-clang-dev` | Pass |
| `windows-clang-dev` | `cmake --build --preset windows-clang-dev` | Pass; emitted vertex and fragment SPIR-V plus the copied manifest |
| Host tests | `ctest --preset windows-clang-dev` | Pass, 4/4 |
| Headless application | `comet-app.exe --smoke` | Pass, exit code 0 |
| Windows render fixture | `comet-app.exe --render-smoke` | Pass; three frames rendered, GPU completion was observed, validation reported no errors, and shutdown was clean |
| `switch-wsl-dev` | `cmake --preset switch-wsl-dev` under WSL with `DEVKITPRO=/opt/devkitpro` | Pass |
| Switch application package | `cmake --build out/build/switch-wsl-dev --target comet-app-nro` | Pass; emitted the ELF, NACP, and 444840-byte NRO |
| Switch shaders | Inspect generated shader files | Pass; vertex and fragment DKSH outputs are each 512 bytes |
| Switch ROMFS | Inspect the staged tree and NRO build edge | Pass; the manifest and both DKSH files are staged under `shaders/comet.fixture.textured/` and supplied through `--romfsdir` |
| Switch debug backend | Inspect the ELF link edge | Pass; the Debug application links `deko3dd` |
| Portable boundary | `comet-source-policy` and direct token scan | Pass; Comet source names no Vulkan, deko3d, SDL3, libnx, or `switch.h` API |
| Repository state | `git diff --check` | Pass before this report-only change |

## Hardware evidence

The project owner ran `comet-app.nro` on physical Switch hardware and confirmed that the Comet-owned textured triangle
rendered successfully and the application exited cleanly. No hardware defect was reported.

This was a functional Gate 2 verification. Switch mode, clocks, and performance measurements were not captured and are
not claimed by this report.

## Acceptance status

| Gate 2 criterion | Status | Evidence or remaining action |
| --- | --- | --- |
| The identical Comet-authored geometry appears through both backends | Pass | The Windows Vulkan fixture rendered three frames, and the project owner confirmed the matching Comet-owned fixture on physical Switch hardware. Both paths use the same vertices, indices, texture bytes, manifest, GLSL sources, and portable Doggo RHI calls. |
| Target-specific shader and package outputs are produced through Comet | Pass | The Comet graph generated SPIR-V on Windows, DKSH on Switch, staged the Switch artifacts into ROMFS, and generated `comet-app.nro`. |
| Comet does not use backend APIs | Pass | Comet calls only Doggo public Core, Platform, Math, and RHI APIs; the source-policy test rejects direct backend/platform API tokens. |
| No Gate 3 work is included | Pass | The change contains only the render fixture, shader/build integration, focused tests, and gate documentation. |

## Known risks

- Windows used clang-cl 19.1.5 rather than the locked 23.1.1 reference compiler.
- The hardware result is functional evidence only; it does not include mode, clock, or performance measurements.

Comet Gate 2 is accepted. No Gate 3 work is included in this change.
