# SkyForge: Thunder Dominion — Phase 1 Implementation (UE 5.4.4)

This repository now includes a UE5 project skeleton and the exact build plan for implementing the first playable Soul-Switch prototype from `SKYFORGE_THUNDER_DOMINION_SPEC.md`.

---

## 1) Full Project Setup Instructions

## 1.1 Create the UE project
1. Open **Unreal Engine 5.4.4**.
2. New Project → **Games** → **Blank**.
3. Set:
   - Blueprint + C++ (starter module already scaffolded in this repo)
   - Target Platform: **Mobile**
   - Quality Preset: **Scalable**
   - Starter Content: **Off**
   - Project Name: `SkyForgeThunderDominion`
4. Point location to this repo root so the generated project reuses:
   - `SkyForgeThunderDominion/SkyForgeThunderDominion.uproject`
   - `SkyForgeThunderDominion/Config/*`
   - `SkyForgeThunderDominion/Source/*`

## 1.2 Required plugins (enable in Edit → Plugins)
- Enhanced Input
- Niagara
- Chaos Vehicles
- Chaos Destruction (Geometry Collection)
- AI Module / Behavior Tree Editor

## 1.3 Recommended project settings
- Rendering:
  - Nanite: Enabled for environment meshes
  - Lumen GI + Reflections enabled for PC Preview, with mobile scalability fallback
  - Virtual Shadow Maps enabled (PC preview), mobile fallback via scalability buckets
- Framerate:
  - Mobile target 60 FPS; lock test profiles at 60.
- Packaging:
  - Android Vulkan + ES3.1 fallback
  - iOS minimum 15

## 1.4 Git LFS and repo hygiene
Run from repo root:
```bash
git lfs install
git lfs track "*.uasset" "*.umap" "*.ubulk" "*.uexp" "*.fbx" "*.wav"
```
`.gitattributes` is already included and preconfigured.

## 1.5 Folder structure (authoritative)

```text
SkyForgeThunderDominion/
  Config/
  Content/
    Blueprints/
      AI/
      Components/
      Core/
      Input/
      UI/
      Units/
    Maps/
    VFX/
    Audio/
    Materials/
    Data/
    Art/
    Geometry/
  Source/
    SkyForgeThunderDominion/
```

---

## 2) Core Game Framework Blueprints

## 2.1 `BP_SkyForgeGameMode`
**Path:** `/Game/Blueprints/Core/BP_SkyForgeGameMode`

- Parent: `GameModeBase`
- Defaults:
  - Default Pawn Class: `None`
  - Player Controller Class: `BP_SkyForgePlayerController`
  - HUD Class: `BP_SkyForgeHUD`
- Event BeginPlay:
  1. `Get All Actors Of Class (BP_SkyForgePawn)`
  2. ForEach: if GameplayTag == `Faction.Friendly`, add to array `FriendlyUnits`
  3. Select first alive friendly → call `InitializeControlledUnit` on PlayerController

## 2.2 `BP_SkyForgePlayerController`
**Path:** `/Game/Blueprints/Core/BP_SkyForgePlayerController`

Components/vars:
- `SoulSwitchComponent` (Actor Component)
- `CurrentControlledUnit` (`BP_SkyForgePawn` ref)
- `TopDownCameraBoom` + `TopDownCamera` (for commander view)
- `bInCommandView` bool (default true)

Enhanced Input flow:
- Event BeginPlay:
  1. `Get Local Player Subsystem (EnhancedInput)`
  2. `Add Mapping Context IMC_CommandMode Priority 0`
  3. `Add Mapping Context IMC_UnitMode Priority 1`
- Input Actions:
  - `IA_TapSelect` → line trace under touch/mouse, select friendly unit
  - `IA_SoulWheelHold` (Hold) → show `WBP_SoulSwitchWheel`
  - `IA_SoulWheelRelease` → resolve highlighted candidate and call `SoulSwitchComponent->RequestSoulSwitch`
  - `IA_CameraPan` (2-finger drag / mouse edge drag)
  - `IA_CameraZoom` (pinch / mouse wheel)

## 2.3 `BP_SkyForgePawn` (base)
**Path:** `/Game/Blueprints/Core/BP_SkyForgePawn`

Components:
- Root Scene
- Mesh/SkeletalMesh
- `FloatingPawnMovement`
- `WidgetComponent` (world badge)
- `NiagaraComponent` (switch entry FX)
- `AudioComponent` (switch roar)

Variables:
- `FactionTag` (GameplayTag)
- `UnitClassTag` (Air/Ground)
- `Health`, `MaxHealth`
- `bIsPlayerControlled`
- `AssignedAIControllerClass`

Functions:
- `CanBeSoulSwitched` → alive + friendly + not hidden
- `OnPossessedByPlayer` → set flags, enable player input profile
- `OnReleasedToAI` → spawn/assign AI controller and run BT

## 2.4 `BP_SoulSwitchComponent`
**Path:** `/Game/Blueprints/Components/BP_SoulSwitchComponent`

Variables:
- `SwitchCooldown` = 8.0
- `DiveDuration` = 0.45
- `InvulnDuration` = 0.40
- `bSwitchReady` bool = true
- `LastSwitchTimestamp` float

Function `RequestSoulSwitch(TargetPawn)`:
1. Branch `bSwitchReady`
2. Validate target `CanBeSoulSwitched`
3. Cache `OldPawn = PlayerController->GetPawn`
4. `Set Global Time Dilation = 0.25`
5. Start Timeline `TL_SoulDive` length 0.45 (ease in/out)
   - Track updates camera from top-down pivot to target socket `SoulEntry`
6. On timeline finished:
   - `PlayerController Possess(TargetPawn)`
   - Call `OldPawn->OnReleasedToAI`
   - Call `TargetPawn->OnPossessedByPlayer`
   - Play Niagara + sound
   - Apply invulnerability tag for 0.4s
   - `Set Global Time Dilation = 1.0`
   - Set `bSwitchReady=false`
   - `Set Timer by Event` 8s → `ResetCooldown`

Function `ResetCooldown`:
- `bSwitchReady = true`
- Broadcast UI event for cooldown fill completion

---

## 3) Soul-Switch Mechanic – Fully Working Prototype

## 3.1 Camera and transition (<500 ms)
- Implement in `BP_SoulSwitchComponent` timeline (`0.45s`).
- Curve: Fast start + soft settle (`Auto` tangents).
- During dive:
  - Motion blur off on mobile profile
  - Optional FOV pulse: 65→80→70
  - Camera shake (small amplitude) at possession frame

## 3.2 AI hand-off
- `BP_SkyForgePawn::OnReleasedToAI`:
  1. If controller is player, `UnPossess`
  2. `Spawn Default Controller`
  3. Cast to `BP_SkyForgeAIController`
  4. `Run Behavior Tree BT_CombatDummy`
  5. Set blackboard `LastKnownTarget` from prior player aim target if present

## 3.3 Cooldown + visual feedback
- `WBP_SoulSwitchCooldownRing`
  - Material parameter `CooldownAlpha`
  - Tick/Timer reads `Remaining / 8.0`
- When switching unavailable, wheel entries tinted and pulse red border.

## 3.4 Mobile + gamepad input mapping

Create Input Actions under `/Game/Blueprints/Input/`:
- `IA_TapSelect` (Digital)
- `IA_SoulWheelHold` (Digital Hold)
- `IA_SoulConfirm` (Digital)
- `IA_CameraPan` (Axis2D)
- `IA_CameraZoom` (Axis1D)
- `IA_Move` (Axis2D)
- `IA_Look` (Axis2D)
- `IA_PrimaryFire` (Digital)
- `IA_Afterburner` (Digital)

Create Mapping Contexts:
- `IMC_CommandMode`:
  - Touch1 Pressed → TapSelect
  - Touch2 Delta → CameraPan
  - Pinch Axis → CameraZoom
  - Gamepad Left Stick → CameraPan
  - Gamepad Right Stick Y → CameraZoom
  - Gamepad Left Trigger Hold → SoulWheelHold
- `IMC_UnitMode`:
  - Left stick / virtual stick → Move
  - Right stick / virtual look pad → Look
  - RT / touch fire button → PrimaryFire
  - B / touch boost button → Afterburner

---

## 4) First Two Playable Units

## 4.1 `BP_F15C_Eagle`
**Path:** `/Game/Blueprints/Units/BP_F15C_Eagle`

Parent: `BP_SkyForgePawn`

Setup:
- Movement: custom flight controls via `FloatingPawnMovement`
  - MaxSpeed 12000
  - Accel high, decel moderate
- Variables:
  - `AfterburnerMultiplier = 1.8`
  - `AfterburnerDuration = 2.5`
  - `AIM120Ammo = 6`
- Graph:
  - `IA_Move` updates forward/strafe pitch roll blend
  - `IA_Afterburner` sets temporary speed multiplier and Niagara trail
  - `IA_PrimaryFire` spawns placeholder missile actor `BP_AIM120_Placeholder`

## 4.2 `BP_M1A2_Tank`
**Path:** `/Game/Blueprints/Units/BP_M1A2_Tank`

Parent: `BP_SkyForgePawn`

Setup:
- Components:
  - Hull mesh
  - Turret scene pivot
  - Barrel scene pivot
- Movement:
  - Ground movement using `FloatingPawnMovement` (prototype) or Chaos Vehicle pawn (optional upgrade)
- Variables:
  - `CannonReload = 8.0`
  - `CannonReady` bool
- Graph:
  - `IA_Move` drives hull movement
  - `IA_Look` rotates turret yaw and barrel pitch with clamps
  - `IA_PrimaryFire` checks reload → spawn `BP_TankShell_Placeholder` + Niagara muzzle FX + camera shake

Both units must keep inherited Soul-Switch compatibility and faction tags.

---

## 5) Minimal Test Map

## 5.1 `Desert_Canyon_Prototype`
**Path:** `/Game/Maps/Desert_Canyon_Prototype`

Build steps:
1. Landscape 2 km x 2 km (2017x2017 or equivalent), sculpt canyon ridges.
2. Add bridge static mesh at map center (set up as Geometry Collection for Chaos fracture).
3. Place four control point actors (`BP_ControlPoint_A/B/C/D`).
4. Spawn units:
   - Friendly: 2x F-15, 2x M1A2
   - Enemy: 2x dummy aircraft, 2x dummy tanks (reuse base pawn with red materials)
5. NavMeshBoundsVolume covering ground lanes.
6. Niagara systems:
   - `NS_Explosion_Medium`
   - `NS_SoulSwitch_DiveTrail`

## 5.2 Basic destruction + explosions
- Convert bridge and one fuel tank cluster into Geometry Collections.
- On shell/missile hit:
  - `Apply Radius Damage`
  - `Apply Radial Impulse`
  - Spawn Niagara explosion + decal.

---

## 6) Widgets (HUD/minimap/wheel)

## 6.1 `WBP_SkyForgeHUD`
**Path:** `/Game/Blueprints/UI/WBP_SkyForgeHUD`
- Contains:
  - Minimap panel (RenderTarget mini scene capture)
  - Soul-Switch button
  - Cooldown ring widget

## 6.2 `WBP_SoulSwitchWheel`
**Path:** `/Game/Blueprints/UI/WBP_SoulSwitchWheel`
- Radial entries generated from friendly units array.
- Each segment shows icon + HP bar + class tag.
- On release, returns selected pawn reference to PlayerController.

## 6.3 `WBP_Minimap`
**Path:** `/Game/Blueprints/UI/WBP_Minimap`
- Friendly/Enemy icons with color coding.
- Control points displayed with capture state.

---

## 7) Node-by-node blueprint quick build checklist

Use this order to stand up playable prototype in <1 day:
1. Build `BP_SkyForgePawn` base and possession/AI handoff events.
2. Build `BP_SoulSwitchComponent` with `RequestSoulSwitch` timeline.
3. Build `BP_SkyForgePlayerController` with Enhanced Input hooks.
4. Build `WBP_SoulSwitchWheel` and cooldown ring.
5. Create F-15 and M1A2 child pawns.
6. Place actors in `Desert_Canyon_Prototype`; set default map.
7. PIE test: switch chain between 4 friendly units repeatedly.

---

## How to Test
1. Open map `/Game/Maps/Desert_Canyon_Prototype`.
2. Press Play.
3. In command view, tap a friendly unit badge.
4. Hold Soul-Wheel input, select another unit, release.
5. Verify:
   - Camera dive completes in ~0.45s
   - Possession transferred
   - Previous pawn continues via AI behavior
   - Cooldown ring locks out for 8s
6. Repeat between F-15 and M1A2 to confirm cross-class switching.
