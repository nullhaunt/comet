# Comet v0 playable sandbox roadmap

Plan version: `engine-v0-plan-1`

Comet advances only after the corresponding Doggo gate is accepted. Each gate remains a separate review boundary.

## Gate 0: engineering contract

- Add repository instructions and Comet architecture/build/testing documents.
- Lock the game/engine ownership boundary and planned target graph.
- Reference rather than duplicate Doggo ADRs and dependency decisions.

Acceptance: all links resolve, the documents agree with Doggo `engine-v0-plan-1`, and no build/runtime implementation is introduced.

## Gate 1: build and application composition

- Initialize the pinned Doggo submodule and Comet CMake project.
- Add shared components, gameplay, presentation, app, editor, and cooker target skeletons only as required by the gate.
- Boot minimal Windows and Switch applications using documented Doggo modules.

Acceptance: clean host configure/build/test, clean Switch cross-build, physical hardware boot/shutdown, and no RHI-dependent game code.

## Gate 2: shared render fixture

- Add the smallest Comet-owned scene inputs needed for Doggo's shared textured-geometry test.
- Verify target-specific shader and packaging outputs through the Comet build.

Acceptance: identical authored geometry appears through both backends without Comet using backend APIs.

## Gate 3: sandbox data and streaming

- Define the first reflected game components.
- Author one zone and two streamable subareas.
- Add valid and invalid cooking fixtures and exercise reload rollback.

Acceptance: both platforms load the same cooked scene, and deterministic/negative content tests pass.

## Gate 4: playable traversal

- Add player/camera control, collision configuration, one animated object, and traversal between subareas.
- Keep gameplay deliberately minimal; this gate validates engine integration rather than final game architecture.

Acceptance: the sandbox is playable on hardware at the locked simulation/presentation rates without monotonic memory growth.

## Gate 5: editor/live workflow

- Add Comet inspectors or panels only where generic Doggo UI is insufficient.
- Validate editor-authored scene changes and texture/shader reload against a running Switch.

Acceptance: editor and cooker use the same registrations; reconnect and failed transfers preserve valid state.

## Gate 6: representative qualification

- Expand the sandbox into a versioned representative traversal and stress variants.
- Tune content LOD, residency, lighting, and effect tiers within Doggo's fixed measurement procedure.

Acceptance: the vertical-slice thresholds pass, shipping graph inspection is clean, and all remaining compromises are documented.
