# Comet architecture

Status: Accepted  
Plan version: `engine-v0-plan-1`  
Accepted: 2026-09-16

Comet is the first game built on Doggo and the proving workload for the v0 playable sandbox. This document defines game-side ownership and integration. Doggo's engine architecture at `lib/doggo/docs/architecture.md` remains authoritative for engine behavior after the submodule is initialized in Gate 1.

## Repository boundary

Doggo is pinned at `lib/doggo` as a Git submodule and included through `add_subdirectory`. Comet includes Doggo's API headers but never reaches into Doggo source directories or backend implementations. A mechanism needed by multiple games belongs in Doggo; a rule unique to this game's content or behavior remains in Comet.

`external/` is reserved for a game-specific third-party dependency approved through an ADR. It is not a second copy of a Doggo dependency.

## Planned targets

| Target | Kind | Responsibility |
| --- | --- | --- |
| `comet::components` | Static library | Game component declarations and their explicit registrations |
| `comet::gameplay` | Static library | Fixed-tick systems, interactions, and game rules |
| `comet::presentation` | Static library | Game-specific cameras, animation policy, render features, and UI data |
| `comet-app` | Executable | Platform-neutral application composition plus selected platform entrypoint |
| `comet-editor` | Windows executable | Doggo editor shell plus Comet panels and registrations |
| `comet-cooker` | Host executable | Doggo cooker core plus Comet component/import registrations |

`comet::components` is the shared schema seam. Runtime, editor, and cooker link the same registration code so type names, field IDs, defaults, and versions cannot drift. It contains data and registration, not gameplay loops or editor widgets.

The application explicitly creates Doggo services and Comet systems. It does not use a global engine singleton or backend-specific entrypoints in gameplay code.

## Files, headers, and names

Comet is an application, not an API, so it has project-private headers exclusively:

```text
src/Main.cpp
src/components/components_Player.hpp
src/components/components_Player.cpp
```

- Every Comet header lives beneath `src/`; Comet has no `include/` tree and installs or exports no headers.
- Files within a module use `<module>_<Name>.hpp` and `<module>_<Name>.cpp`.
- Files without a parent module use PascalCase names such as `Main.cpp` and `Main.hpp`.
- `.cpp` and `.hpp` are the only C++ source/header extensions.
- The checked-in `.clang-format` is authoritative.

Comet uses only Doggo's world/component API. It never includes EnTT or encodes EnTT identities in data. That separation preserves the possibility of a future custom ECS without putting replacement work on the current roadmap.

## Game world

Comet follows the engine's zone/subarea model:

- Major authored areas are zones and may use explicit presentation transitions.
- Rooms, nearby exterior cells, props, and dependent assets may stream as subareas.
- Cross-zone references are soft UUID references and tolerate unloaded targets.
- Persistent game state is separate from transient ECS identity.
- Simulation runs at fixed 60 Hz; presentation targets stable 30 Hz.

The first sandbox contains only enough game behavior to validate movement, camera control, collision, animation, loading boundaries, and development workflows. It is not the start of production quest/combat architecture.

## Content ownership

Authoritative source content lives beneath `assets/source/`. Scene and project metadata are JSON. Blender exports glTF 2.0 for 3D interchange. Cooked content is target-specific build output and is not committed.

Every source asset has a stable UUID independent of its path. Moves do not change identity. Import settings are explicit inputs to the cook key. Comet registrations extend Doggo's generic cooker without changing source content in place.

## Editor integration

`comet-editor` statically links Comet's component schema and optional game-specific panels into Doggo's editor shell. The editor owns durable scene data. A connected Windows or Switch runtime is a mirror; runtime values return to source only through an explicit capture operation.

No gameplay-code hot reload is planned. C++ behavior changes require rebuild and relaunch. Cooked assets, shaders, and scene deltas may reload through Doggo's live link.

## Performance posture

Content is authored for the budgets and measurement process in Doggo's testing contract. Comet supplies the versioned representative zone, traversal/input recording, and stress variants used for qualification. It does not compensate for budget failures with hidden platform-specific gameplay behavior.

## Non-goals for v0

Quest architecture, production combat, scripting, multiplayer, audio, seamless-world traversal, final save-game design, and production-volume content are outside the playable sandbox.
