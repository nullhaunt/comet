# Comet verification contract

Comet follows Doggo's engine-level verification contract at `lib/doggo/docs/testing.md` and adds game integration checks. That path becomes available when Gate 1 initializes the submodule.

## Host automation

Host tests cover:

- Component registration uniqueness, defaults, schema versions, and source/cooked round trips.
- Fixed system ordering and deterministic behavior for controlled inputs where promised.
- Zone/subarea dependency declarations and soft-reference behavior.
- Application composition with development services both enabled and disabled.
- Editor and cooker registration parity.
- Deterministic cooking of the sandbox fixture.
- C++ file-extension, private-header location/naming, and `.clang-format` policy checks.
- A `.clang-tidy` naming-contract test with both compliant and deliberately invalid fixtures.
- Absence of EnTT types, headers, and serialized identities in Comet code.

Tests must not reach through Doggo abstractions to manipulate EnTT, Jolt, Vulkan, deko3d, SDL, or libnx directly.

## Content validation

The sandbox fixture includes:

- One zone with at least two independently streamable subareas.
- Static collision and one dynamic or animated object.
- A controllable camera/player proxy.
- At least one reloadable texture and shader.
- Deliberately invalid fixtures for missing references and incompatible component versions.

Clean repeated cooks must be byte-identical. Cooked assets and test output remain beneath `out/`.

## Hardware verification

At applicable gates, record:

- Comet and Doggo revisions.
- Preset and devkitPro package manifest.
- Cold boot, shutdown, handheld/docked transition, and suspend/resume results.
- Controller and development keyboard/mouse behavior.
- Zone traversal, unload/reload, and memory behavior.
- Live-link reconnect and failed-reload rollback.
- CPU, GPU, simulation, streaming, and memory percentile data for the representative traversal.

The v0 performance thresholds are inherited from Doggo: 33.33 ms presentation and 16.67 ms fixed simulation at the 99th percentile after warm-up, with total runtime committed memory below 85% of measured application-mode allocation.

## Gate evidence

Each gate report identifies exactly what was run, separates host from hardware evidence, lists known failures, and confirms that the change contains no work from the next gate.
