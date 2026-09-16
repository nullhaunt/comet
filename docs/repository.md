# Repository and checkout policy

Status: Accepted for Gate 1  
Plan version: `engine-v0-plan-1`

Comet is an independent `main`-branch repository and pins Doggo at `lib/doggo`. Initialize the complete checkout before configuring:

```powershell
git submodule update --init --recursive lib/doggo
```

Generated builds, packages, cooked assets, IDE-local settings, and user presets stay untracked. Shared CMake preset names are checked in. CLion-specific executable paths and environment variables belong in the configured CLion toolchain or the ignored `CMakeUserPresets.json`.

The repository does not yet declare a project-source license. That remains a distribution blocker until the owner chooses one; Doggo's third-party notices remain authoritative for its transitive source dependencies.
