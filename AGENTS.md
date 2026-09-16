# Comet repository instructions

## Purpose and authority

Comet is the game. It owns game components, systems, content, import registrations, editor extensions, and application composition. Reusable platform, rendering, asset, scene, physics, live-link, and editor-framework behavior belongs in Doggo.

Comet pins Doggo as a Git submodule at `lib/doggo`. The accepted game integration is [docs/architecture.md](docs/architecture.md), version `engine-v0-plan-1`. Architectural changes must update the applicable specification and add or supersede an ADR in Doggo when they alter an engine-wide decision.

## Required reading

Before changing Comet, read:

- [docs/architecture.md](docs/architecture.md)
- [docs/build.md](docs/build.md)
- [docs/testing.md](docs/testing.md)
- [docs/dependencies.md](docs/dependencies.md)
- [docs/roadmap/v0-playable-sandbox.md](docs/roadmap/v0-playable-sandbox.md)
- `lib/doggo/docs/architecture.md` and the relevant Doggo ADRs for engine-facing work, after the submodule is initialized in Gate 1

## Working rules

- Make only changes required by the active task and milestone. Do not perform unrelated cleanup or speculative abstraction.
- Put reusable mechanisms in Doggo and game policy/content in Comet. Do not fork or patch Doggo from Comet source directories.
- Portable game code is C++20 with exceptions and RTTI disabled. Use Doggo error, ID, math, asset, and world types at engine boundaries.
- Comet has no exported header API. Every Comet header is project-private, lives beneath `src/`, and follows the module-prefix naming convention in `docs/architecture.md`.
- Use only `.hpp` and `.cpp` for C++ files; files without a parent module use PascalCase names such as `Main.hpp` and `Main.cpp`.
- The repository `.clang-format` is authoritative. Format changed C++ files with it and never reformat unrelated code.
- Persistent entity and asset references use UUID-based Doggo IDs. Never serialize runtime ECS indices, pointers, or raw component memory.
- Game components that are editable or persistent must have explicit reflection/serialization registration shared by the runtime, editor, and cooker.
- Source assets are immutable cooker inputs. Generated and cooked output belongs under the build tree.
- Include Doggo only through headers beneath `include/doggo/` and link only documented `doggo::<module>` targets. Do not reach into Doggo source directories or backend implementations.
- Use Doggo's world and identity contracts. Never include or name EnTT in Comet source or serialized data; this keeps a future custom ECS possible without making it current roadmap work.
- Development-only panels and commands must not enter the shipping target graph.
- Stop at milestone gates and report acceptance evidence before beginning the next gate.

## Canonical commands

Gate 1 will make these presets executable; until then they are the locked command interface:

```powershell
cmake --preset windows-clang-dev
cmake --build --preset windows-clang-dev
ctest --preset windows-clang-dev

cmake --preset switch-dev
cmake --build --preset switch-dev
```

Initialize the pinned Doggo submodule before configuring. Run the narrowest relevant test first and the full milestone matrix before a gate review. Never claim Switch verification without a physical hardware run.

## Commit subjects

Use the repository's modified conventional format:

```text
(SCOPE) Concise description of the completed change
```

- Module scopes are uppercase: `(APP)`, `(COMPONENTS)`, `(GAMEPLAY)`, `(PRESENTATION)`, `(EDITOR)`, `(COOKER)`.
- A submodule appends an uppercase hyphenated suffix where useful: `(GAMEPLAY-COMBAT)`.
- Repository-wide/non-module scopes use Title Case: `(Docs)`, `(Build)`, `(Tests)`, `(Assets)`, `(Dependencies)`, `(Chore)`.
- Do not add a colon after the scope. Begin the subject with a capital letter, describe what the commit did, and omit the trailing period.
- Keep one primary scope per commit. Split unrelated changes instead of combining scopes.

Examples: `(Docs) Defined the Comet integration contract` and `(GAMEPLAY) Added player movement`.

## Definition of done

A Comet change is complete only when it stays within the engine/game boundary, relevant host tests pass, applicable cooked content is deterministic, required hardware behavior is verified, and the active milestone acceptance criteria remain satisfied.
