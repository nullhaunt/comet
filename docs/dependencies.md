# Comet dependency policy

Status: Accepted  
Plan version: `engine-v0-plan-1`

Comet has one first-party dependency: Doggo, pinned as a Git submodule at `lib/doggo`. The Gate 1 pin is Doggo revision `2fbe5d80bba4db39b101370e0675ddb093d3bfb6`.

Doggo owns the shared third-party dependency lock at `lib/doggo/docs/dependencies.md`. Comet does not add duplicate copies or alternate versions of those libraries.

`external/` is reserved for a dependency uniquely required by the game. Adding one requires:

- A concrete game-owned use that does not belong in Doggo.
- Compatible license review.
- An exact tag and commit hash.
- A named owning target and private/public boundary decision.
- Host and applicable Switch verification.
- An ADR if the dependency changes the game architecture.

Normal configure and build are offline. Dependencies are initialized explicitly as setup and are never fetched implicitly by CMake.
