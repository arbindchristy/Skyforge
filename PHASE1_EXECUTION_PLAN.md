# SkyForge Phase 1 Execution Plan

## Current Objective

Build the first playable internal alpha for **SkyForge: Thunder Dominion** around the core loop:

1. control a unit directly
2. select a friendly unit
3. Soul-Switch into that unit in under 500 ms
4. leave the previous unit alive under AI control
5. fire prototype weapons and verify damage/death states

## Completed In This Pass

- Added reusable `ASkyForgeUnitPawn` with faction, health, selection eligibility, possession handoff, firing, and AI hooks.
- Converted `ASkyForgeTankPawn` into a ground unit derived from the shared unit base.
- Added `ASkyForgeAircraftPawn` as the first air-unit prototype class.
- Added `USkyForgeSoulSwitchComponent` with target validation, camera blend, time-dilation dive, cooldown, and possession transfer.
- Expanded `ASkyForgePlayerController` with friendly-unit registration, cursor selection, next-unit selection, Soul-Switch input, and command-view toggle.
- Expanded `ASkyForgeGameMode` so placed friendly units are registered and the first available friendly unit can be possessed.
- Added `ASkyForgeAIController` for simple post-switch handoff behavior.
- Added `ASkyForgeProjectile` for prototype weapon damage.
- Added legacy input mappings for immediate PIE testing while Enhanced Input assets are built in-editor.
- Added game/editor target files so UnrealBuildTool can compile the project directly.
- Added code-only prototype bootstrapping: if no authored level units exist, the GameMode spawns friendly tank/aircraft placeholders and an enemy tank.
- Added default placeholder meshes and projectile assignment so the prototype can be validated before custom art assets exist.
- Verified `SkyForgeThunderDominionEditor Win64 Development` builds successfully with UE 5.4.

## Build Note

This machine has Visual Studio MSVC `14.44.35211`; UE 5.4 prefers `14.38.33130`. The targets include a narrow compatibility override for the installed compiler:

- `bOverrideBuildEnvironment = true`
- `GlobalDefinitions.Add("__has_feature(x)=0")`

This avoids a UE shared-PCH failure in `ConcurrentLinearAllocator.h` on the newer compiler. If the build machine installs MSVC 14.38, this override can be removed.

## Unreal Editor Setup Required

The C++ fallback lets you validate the core loop before these binary assets exist. These steps are still required for the proper authored MVP map:

1. Open `SkyForgeThunderDominion/SkyForgeThunderDominion.uproject`.
2. Create `/Game/Maps/Desert_Canyon_Prototype`.
3. Place at least three friendly units:
   - one `SkyForgeTankPawn`
   - one `SkyForgeAircraftPawn`
   - one more `SkyForgeTankPawn` or aircraft
4. Place at least one enemy `SkyForgeTankPawn` and set `FactionTag` to `Faction.Enemy`.
5. Create a Blueprint subclass of `SkyForgeProjectile` for visible tank shells/missiles, or assign the C++ projectile class directly to each unit's `ProjectileClass`.
6. Set the map's GameMode override to `SkyForgeGameMode`.
7. Optionally update Project Settings > Maps & Modes to use `Desert_Canyon_Prototype` after the map exists.

## First Playtest Routine

Open `SkyForgeThunderDominion/SkyForgeThunderDominion.uproject`, press Play in the default map, then use:

- `W/S`: forward/back
- `A/D`: yaw
- `Q/E`: strafe for ground units
- `Left Mouse`: fire
- `Left Shift`: aircraft afterburner / secondary action
- `Right Mouse`: select friendly unit under cursor
- `Tab`: select next friendly unit
- `Space`: Soul-Switch to selected unit
- `C`: toggle command cursor mode

Expected result:

- Soul-Switch completes with a short camera dive.
- Previous unit receives an AI controller.
- Cooldown prevents immediate re-switching.
- Projectiles damage enemy units.
- Dead units stop accepting Soul-Switch.

## Next Build Slice

1. Replace legacy input with fully authored Enhanced Input assets.
2. Build `WBP_SoulSwitchWheel` and cooldown UI.
3. Add top-down command camera pawn/view.
4. Build Desert Canyon blockout with three control points.
5. Add replication annotations and dedicated-server smoke test.
6. Add automated C++ compile/build validation in CI.
