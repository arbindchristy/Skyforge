# SkyForge: Thunder Dominion — Phase 1 Final Production Implementation (UE 5.4.4)

> Goal: build a **fully playable Phase 1 prototype in under 60 minutes** from this repo’s UE5.4 scaffold, with a cinematic Soul-Switch between an F-15C and M1A2.

---

## 1. One-Click Project Open Instructions

### 1.1 Open the existing project from this repo
1. Launch **Unreal Engine 5.4.4** from Epic Games Launcher.
2. Click **Browse…** and open:
   - `SkyForgeThunderDominion/SkyForgeThunderDominion.uproject`
3. If prompted to rebuild modules, click **Yes**.
4. Wait for shader compilation to finish before creating Blueprints.

### 1.2 Enable required plugins (first launch only)
Open **Edit → Plugins** and enable these plugins, then restart editor once:
- **Enhanced Input** (Input)
- **Niagara** (FX)
- **Chaos Vehicles** (Physics)
- **Chaos Destruction** + **Geometry Collection** (Physics)
- **Behavior Tree Editor** / AI support plugins if disabled
- **Modeling Tools Editor Mode** (optional but useful for quick prototype meshes)

### 1.3 Project settings for Phase 1 target (Mobile + PC preview @ 60 FPS)
Open **Edit → Project Settings** and apply:

**Maps & Modes**
- Default GameMode: `BP_SkyForgeGameMode`
- Editor Startup Map: `Desert_Canyon_Prototype`
- Game Default Map: `Desert_Canyon_Prototype`

**Engine → General Settings**
- Framerate → Use Fixed Frame Rate: **Off** (for editor), Smoothed Range max 60 for quick validation

**Engine → Input**
- Default Player Input Class: Enhanced Input (default in UE5)

**Platforms/Rendering**
- **Nanite** enabled for static environment meshes
- **Lumen GI/Reflections** enabled for PC Preview profile
- Mobile profile uses scalability fallback (Medium/Low shadows, no heavy post)

**Physics**
- Chaos as default physics (UE5 default)

### 1.4 Content folder structure to create now
Inside `Content/`, create:

```text
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
Geometry/
```

---

## 2. Full Blueprint Implementation (expanded, exact build order)

> Follow this exact order for dependency safety:
> 1) Input assets → 2) Base pawn → 3) Player controller → 4) Soul switch component → 5) Unit child blueprints → 6) Widgets → 7) AI

---

### 2.1 Enhanced Input assets (create before Blueprints)

Create these **Input Actions** in `/Game/Blueprints/Input/`:
- `IA_TapSelect` (Digital Bool)
- `IA_SoulWheelHold` (Digital Bool)
- `IA_SoulConfirm` (Digital Bool)
- `IA_CameraPan` (Axis2D)
- `IA_CameraZoom` (Axis1D)
- `IA_Move` (Axis2D)
- `IA_Look` (Axis2D)
- `IA_PrimaryFire` (Digital Bool)
- `IA_Afterburner` (Digital Bool)

Create **Input Mapping Contexts**:

#### `IMC_CommandMode`
Bindings:
- Mouse Left Click / Touch1 Pressed → `IA_TapSelect`
- Mouse Middle Drag / Touch Delta2 → `IA_CameraPan`
- Mouse Wheel Axis / Pinch Axis → `IA_CameraZoom`
- Gamepad Left Stick → `IA_CameraPan`
- Gamepad Right Stick Y → `IA_CameraZoom`
- Keyboard Tab (Hold) / Gamepad Left Trigger (Hold) → `IA_SoulWheelHold`

#### `IMC_UnitMode`
Bindings:
- WASD / Left Stick / Left Virtual Stick → `IA_Move`
- Mouse Delta / Right Stick / Right Virtual Stick → `IA_Look`
- Left Mouse / Right Trigger / Touch Fire Button → `IA_PrimaryFire`
- Left Shift / Gamepad B / Touch Boost Button → `IA_Afterburner`
- Tab (Hold) / Left Trigger (Hold) → `IA_SoulWheelHold`

---

### 2.2 `BP_SkyForgePawn` (base class with possession logic)
**Create:** `/Game/Blueprints/Core/BP_SkyForgePawn` (Parent: `Pawn`)

#### Components
1. `DefaultSceneRoot` (Root)
2. `PawnMesh` (Static Mesh; replace with Skeletal Mesh in child units if desired)
3. `FloatingPawnMovement`
4. `SwitchFX` (Niagara Component, Auto Activate: false)
5. `SwitchAudio` (Audio Component, Auto Activate: false)
6. `UnitBadge` (Widget Component)
7. `TargetLockPoint` (Scene Component, attach above mesh)

#### Variables (exact)
- `Faction` (Name) default `Friendly`
- `UnitType` (Name) default `Ground`
- `Health` (Float) default `100`
- `MaxHealth` (Float) default `100`
- `bIsAlive` (Bool) default `true`
- `bIsPlayerControlled` (Bool) default `false`
- `AssignedAIControllerClass` (Class Reference: AIController)
- `BehaviorTreeAsset` (Behavior Tree Object Reference)
- `SoulEntrySocketName` (Name) default `SoulEntry`

#### Function: `CanBeSoulSwitched` (Pure)
**Inputs:** none
**Output:** `CanSwitch` (Bool)

Node chain:
1. `Get bIsAlive` → Branch
2. True path:
   - `Get Hidden In Game` (self) → `NOT`
   - `Get Faction` == `Friendly`
   - `Boolean AND` (Alive AND NotHidden)
   - `Boolean AND` with Friendly check
   - Return as `CanSwitch`
3. False path: Return false

#### Function: `OnPossessedByPlayer`
Node chain:
1. `Set bIsPlayerControlled = true`
2. `Set Faction = Friendly` (optional safeguard)
3. `SwitchFX` → `Activate`
4. `SwitchAudio` → `Play`

#### Function: `OnReleasedToAI`
Node chain:
1. Branch (`AssignedAIControllerClass IsValid?`)
2. If true:
   - `Spawn Default Controller`
   - `Get Controller` → Cast to `AIController`
   - Branch (`BehaviorTreeAsset IsValid?`)
   - True: `Run Behavior Tree`
3. Set `bIsPlayerControlled = false`

#### Event Graph
- `Event BeginPlay`
  - `Set Health = MaxHealth`
  - `Set bIsAlive = true`

- `Event AnyDamage`
  - `Health - Damage` → `Set Health`
  - Branch (`Health <= 0`)
    - True: `Set bIsAlive = false`, `Set Actor Hidden In Game(true)`, `Set Actor Enable Collision(false)`

---

### 2.3 `BP_SkyForgePlayerController` (top-down camera + input)
**Create:** `/Game/Blueprints/Core/BP_SkyForgePlayerController` (Parent: `PlayerController`)

#### Components
1. `CommandViewRoot` (Scene)
2. `TopDownCameraBoom` (SpringArm; Length 3000; Rotation -65 pitch)
3. `TopDownCamera` (Camera attached to boom)

#### Variables
- `CurrentControlledUnit` (BP_SkyForgePawn Object Ref)
- `SoulSwitchComponent` (BP_SoulSwitchComponent Object Ref)
- `bInCommandView` (Bool, default true)
- `PanSpeed` (Float, default 2500)
- `ZoomStep` (Float, default 200)
- `MinZoom` (Float, default 1200)
- `MaxZoom` (Float, default 5000)
- `HUDWidget` (WBP_SkyForgeHUD Ref)
- `SoulWheelWidget` (WBP_SoulSwitchWheel Ref)
- `CachedWheelSelection` (BP_SkyForgePawn Ref)

#### BeginPlay graph (exact sequence)
1. `Event BeginPlay`
2. `Create Widget (WBP_SkyForgeHUD)` → `Promote to HUDWidget`
3. `Add to Viewport`
4. `Add Component by Class` → class `BP_SoulSwitchComponent` → promote to `SoulSwitchComponent`
5. `Get Enhanced Input Local Player Subsystem`
6. `Add Mapping Context (IMC_CommandMode, Priority 0)`
7. `Add Mapping Context (IMC_UnitMode, Priority 1)`
8. `Set Show Mouse Cursor = true`
9. `Set Input Mode Game and UI`

#### Input action events

##### `IA_TapSelect (Triggered)`
1. `Get Hit Result Under Cursor by Channel`
2. `Break Hit Result` → `Hit Actor`
3. Cast to `BP_SkyForgePawn`
4. Branch `IsValid`
5. True: set `CachedWheelSelection = HitPawn`

##### `IA_SoulWheelHold (Started)`
1. Branch (`SoulWheelWidget IsValid?`)
2. If false: `Create Widget (WBP_SoulSwitchWheel)` → promote to `SoulWheelWidget`
3. `SoulWheelWidget -> SetOwningController(self)` (custom widget function)
4. `Add To Viewport`
5. `SoulWheelWidget -> RefreshUnits` (custom function)

##### `IA_SoulWheelHold (Completed)`
1. Branch `SoulWheelWidget IsValid`
2. `SoulWheelWidget -> GetSelectedPawn` → promote to local `Target`
3. Branch `Target IsValid`
4. True: `SoulSwitchComponent -> RequestSoulSwitch(Target)`
5. `Remove From Parent (SoulWheelWidget)`

##### `IA_CameraPan (Triggered)`
1. Branch (`bInCommandView`)
2. True:
   - `Get Action Value (Vector2D)` → Break
   - `Get Control Rotation` → `Get Forward Vector` + `Get Right Vector`
   - `Add Movement Input` style equivalent for controller anchor:
     - `CommandViewRoot AddWorldOffset(Forward*Y*PanSpeed*Delta)`
     - `CommandViewRoot AddWorldOffset(Right*X*PanSpeed*Delta)`

##### `IA_CameraZoom (Triggered)`
1. `Get Action Value (Float)`
2. `TopDownCameraBoom TargetArmLength - (Value*ZoomStep)`
3. `Clamp (MinZoom, MaxZoom)`
4. Set `TargetArmLength`

#### Function: `InitializeControlledUnit` (Input: `NewUnit` BP_SkyForgePawn)
1. Branch `NewUnit IsValid`
2. `Possess(NewUnit)`
3. `Set CurrentControlledUnit = NewUnit`
4. `NewUnit -> OnPossessedByPlayer`
5. `Set bInCommandView = false`

---

### 2.4 `BP_SoulSwitchComponent` (complete soul-switch system)
**Create:** `/Game/Blueprints/Components/BP_SoulSwitchComponent` (Parent: `ActorComponent`)

#### Variables
- `SwitchCooldown` (Float) = `8.0`
- `DiveDuration` (Float) = `0.45`
- `InvulnDuration` (Float) = `0.40`
- `bSwitchReady` (Bool) = `true`
- `LastSwitchTimestamp` (Float) = `-1000`
- `OldPawn` (BP_SkyForgePawn Ref)
- `TargetPawn` (BP_SkyForgePawn Ref)
- `OwningPC` (BP_SkyForgePlayerController Ref)
- `OriginalTimeDilation` (Float) = `1.0`

#### Custom Events
- `EVT_BeginSoulSwitch`
- `EVT_EndSoulSwitch`
- `EVT_ResetCooldown`

#### Timeline: `TL_SoulDive`
- Length: `0.45`
- Float Track: `Alpha` (0→1, ease in/out auto tangents)

##### TL Update pin connections
1. `Get Player Camera Manager`
2. `Get Camera Location` as Start
3. `TargetPawn -> GetActorLocation + (0,0,120)` as End
4. `Lerp (Vector, Start, End, Alpha)`
5. `Set Camera Location` (Camera Manager)
6. Optional: `Set FOV` from curve (70→82→70)

##### TL Finished pin connections
1. `OwningPC -> Possess(TargetPawn)`
2. `OldPawn -> OnReleasedToAI`
3. `TargetPawn -> OnPossessedByPlayer`
4. `Set Global Time Dilation (1.0)`
5. `Set bSwitchReady = false`
6. `Get Game Time in Seconds` → set `LastSwitchTimestamp`
7. `Set Timer by Event (Time = SwitchCooldown, Looping false)` → Event `EVT_ResetCooldown`
8. Broadcast to HUD: cooldown begin (custom dispatcher optional)

#### Event: `EVT_ResetCooldown`
- `Set bSwitchReady = true`
- Broadcast cooldown finished to HUD

#### Function: `RequestSoulSwitch(TargetInput: BP_SkyForgePawn)`
Node-by-node (exact):
1. `Get Owner` → Cast to `BP_SkyForgePlayerController` → set `OwningPC`
2. Branch `bSwitchReady`
   - False: return
3. Branch `TargetInput IsValid`
   - False: return
4. `TargetInput -> CanBeSoulSwitched` → Branch
   - False: return
5. `OwningPC -> Get Pawn` → Cast to `BP_SkyForgePawn` → set `OldPawn`
6. `Set TargetPawn = TargetInput`
7. Branch (`OldPawn == TargetPawn`)
   - True: return
8. `Set Global Time Dilation = 0.25`
9. Play Niagara at location (`NS_SoulSwitch_DiveTrail`) at old pawn location
10. Play Sound 2D (`SC_SoulWhoosh`)
11. `Play TL_SoulDive from Start`

#### AI hand-off detail
Inside `OldPawn->OnReleasedToAI`, ensure:
- if no controller then Spawn Default Controller
- if AIController valid + BT assigned then Run BT

#### Radial wheel integration
- `RequestSoulSwitch` is called only by PlayerController after wheel release.
- Wheel remains visible while hold is active.
- On complete, wheel closes and cooldown ring animates for 8 sec.

---

### 2.5 `BP_SkyForgeGameMode`
**Create:** `/Game/Blueprints/Core/BP_SkyForgeGameMode` (Parent: `GameModeBase`)

#### Class defaults
- Default Pawn Class: `None`
- Player Controller Class: `BP_SkyForgePlayerController`
- HUD Class: `None` (HUD spawned from controller)

#### Variables
- `FriendlyUnits` (Array of BP_SkyForgePawn)

#### Event Graph
1. `Event BeginPlay`
2. `Get All Actors Of Class (BP_SkyForgePawn)`
3. ForEachLoop:
   - Cast to BP_SkyForgePawn
   - Branch (`Faction == Friendly`)
   - True: `Add` to FriendlyUnits
4. `Get Player Controller 0` → Cast BP_SkyForgePlayerController
5. Branch (`FriendlyUnits Length > 0`)
6. True: `Get FriendlyUnits[0]` → `InitializeControlledUnit`

---

### 2.6 `BP_F15C_Eagle` (flight + afterburner + missile placeholder)
**Create:** `/Game/Blueprints/Units/BP_F15C_Eagle` (Parent: `BP_SkyForgePawn`)

#### Components
- Replace/augment `PawnMesh` with `JetMesh` (Skeletal or Static)
- `AfterburnerFX` (Niagara, Auto Activate false)
- `VaporConeFX` (Niagara, Auto Activate false)
- `MissileSpawn_L` (Scene)
- `MissileSpawn_R` (Scene)

#### Variables
- `BaseSpeed` Float = `6000`
- `CurrentSpeed` Float = `6000`
- `AfterburnerMultiplier` Float = `1.8`
- `AfterburnerDuration` Float = `2.5`
- `AIM120Ammo` Integer = `6`
- `bAfterburnerActive` Bool = `false`
- `RollRate` Float = `55`
- `PitchRate` Float = `35`

#### BeginPlay
- Set `FloatingPawnMovement Max Speed = BaseSpeed`
- Set `UnitType = Air`

#### Input handling (inside Event Graph)

##### `IA_Move (Triggered)`
1. `Get Action Value Vector2D` (X=Yaw/Roll, Y=Pitch/Throttle bias)
2. `Add Actor Local Rotation`:
   - Pitch += Y * PitchRate * Delta
   - Roll  += X * RollRate * Delta
3. `Add Movement Input (Forward Vector, 1.0)` continuously for jet forward thrust

##### `IA_Afterburner (Started)`
1. Branch `bAfterburnerActive`
2. False path:
   - `Set bAfterburnerActive = true`
   - `CurrentSpeed = BaseSpeed * AfterburnerMultiplier`
   - Set `FloatingPawnMovement Max Speed = CurrentSpeed`
   - Activate `AfterburnerFX` and `VaporConeFX`
   - `Set Timer by Event(AfterburnerDuration)` -> Event `StopAfterburner`

##### Custom Event `StopAfterburner`
1. `Set bAfterburnerActive = false`
2. `CurrentSpeed = BaseSpeed`
3. Set movement max speed to base
4. Deactivate afterburner/vapor systems

##### `IA_PrimaryFire (Started)`
1. Branch (`AIM120Ammo > 0`)
2. True:
   - Choose spawn point (alternate L/R with bool flip)
   - `Spawn Actor from Class BP_AIM120_Placeholder`
   - Set missile forward direction from jet forward
   - `AIM120Ammo = AIM120Ammo - 1`
   - Play `NS_MissileTrail` on missile actor

---

### 2.7 `BP_M1A2_Tank` (movement + turret + cannon + chaos hit)
**Create:** `/Game/Blueprints/Units/BP_M1A2_Tank` (Parent: `BP_SkyForgePawn`)

#### Components
- `HullMesh`
- `TurretPivot` (Scene)
- `BarrelPivot` (Scene attached to TurretPivot)
- `MuzzlePoint` (Scene attached to Barrel)
- `MuzzleFX` (Niagara)

#### Variables
- `MoveSpeed` Float = `1200`
- `TurnSpeed` Float = `60`
- `TurretYawSpeed` Float = `45`
- `BarrelPitchSpeed` Float = `20`
- `MinBarrelPitch` Float = `-8`
- `MaxBarrelPitch` Float = `20`
- `CannonReload` Float = `8.0`
- `bCannonReady` Bool = `true`

#### BeginPlay
- Set `FloatingPawnMovement Max Speed = MoveSpeed`
- Set `UnitType = Ground`

#### `IA_Move (Triggered)`
1. Get Vector2D input
2. `Add Movement Input (Forward, Y)`
3. `Add Actor Local Rotation (Yaw = X * TurnSpeed * Delta)`

#### `IA_Look (Triggered)`
1. Get Vector2D look
2. `TurretPivot AddLocalRotation (Yaw = X * TurretYawSpeed * Delta)`
3. Current barrel pitch + (Y * BarrelPitchSpeed * Delta)
4. Clamp between min/max
5. Set BarrelPivot relative rotation

#### `IA_PrimaryFire (Started)`
1. Branch `bCannonReady`
2. True:
   - Set `bCannonReady = false`
   - Spawn `BP_TankShell_Placeholder` at `MuzzlePoint` transform
   - Activate `MuzzleFX`
   - Play `SC_TankCannon`
   - Camera shake (small impulse)
   - Set Timer by Event (`CannonReload`) → Event `ResetCannon`

#### Custom Event `ResetCannon`
- `Set bCannonReady = true`

#### Chaos destruction hook (on shell hit)
In shell blueprint (simple actor):
- On Hit:
  - `Apply Radial Damage` (Radius 450, Damage 60)
  - If hit actor is GeometryCollection:
    - `Apply Physics Field` / radial impulse
  - Spawn `NS_Explosion_Medium`
  - Destroy self

---

### 2.8 UI Widgets

### `WBP_SkyForgeHUD`
**Create:** `/Game/Blueprints/UI/WBP_SkyForgeHUD`

#### Designer hierarchy
- `CanvasPanel Root`
  - `WBP_Minimap` anchored top-right
  - `SoulSwitchButton` (mobile) anchored bottom-right
  - `CooldownRingImage` around button
  - `UnitInfoPanel` bottom-left (name, HP, ammo)

#### Variables
- `CooldownProgress` Float (0..1)
- `OwningPC` BP_SkyForgePlayerController Ref

#### Graph
- `Event Construct`
  - `Get Owning Player` cast to BP_SkyForgePlayerController -> set `OwningPC`
  - Start timer every 0.05 sec -> `UpdateCooldownUI`

- Function `UpdateCooldownUI`
  - Get `SoulSwitchComponent`
  - If `bSwitchReady` true → `CooldownProgress = 1`
  - Else compute: `(Now - LastSwitchTimestamp) / SwitchCooldown` clamped 0..1
  - Update dynamic material scalar `CooldownAlpha`

### `WBP_SoulSwitchWheel`
**Create:** `/Game/Blueprints/UI/WBP_SoulSwitchWheel`

#### Variables
- `CandidateUnits` (Array BP_SkyForgePawn)
- `SelectedIndex` Int
- `OwningController` BP_SkyForgePlayerController Ref

#### Functions
- `SetOwningController(InController)` set variable
- `RefreshUnits`
  1. Clear array
  2. `Get All Actors of Class BP_SkyForgePawn`
  3. Filter `Faction == Friendly && CanBeSoulSwitched`
  4. Add to `CandidateUnits`
  5. Build radial slots dynamically (Uniform angle 360/Count)

- `GetSelectedPawn` (pure)
  - Return `CandidateUnits[SelectedIndex]` if valid else none

#### Event Tick (optional for hover)
- Get mouse/touch position relative center
- `Atan2` to angle
- Convert angle to segment index
- Set highlight image for selected segment

### `WBP_Minimap`
**Create:** `/Game/Blueprints/UI/WBP_Minimap`

#### Setup
- Add `SceneCapture2D` actor in map with top-down orthographic capture to `RT_Minimap`
- In widget, Image brush uses `RT_Minimap` material

#### Graph
- `Event Construct`: cache control point refs + friendly/enemy units
- Timer every 0.1 sec updates icon overlays positions

---

## 3. Test Map Finalization

## 3.1 Create `Desert_Canyon_Prototype.umap`
**Path:** `/Game/Maps/Desert_Canyon_Prototype`

### Landscape
1. File → New Level → Empty.
2. Add Landscape:
   - Section Size 63
   - 2x2 sections/component
   - 16x16 components (approx 2km world scale)
3. Sculpt long canyon with two ridges and a mid-valley combat lane.
4. Paint 3 layers: Sand, Rock, DryDirt.

### Key set pieces
1. Add static bridge mesh at center valley (X=0,Y=0)
2. Convert bridge to Geometry Collection:
   - Right click mesh → Create Geometry Collection
   - Enable fracture with 40–80 pieces
3. Place 4 capture points:
   - `BP_ControlPoint_A` at (-60000, -20000)
   - `BP_ControlPoint_B` at ( 60000, -20000)
   - `BP_ControlPoint_C` at (-60000,  30000)
   - `BP_ControlPoint_D` at ( 60000,  30000)
4. Add NavMeshBoundsVolume covering ground lanes.

### Unit placement
Place exactly 8 units:

**Friendly (Faction = Friendly)**
- `BP_F15C_Eagle_Friendly_01`
- `BP_F15C_Eagle_Friendly_02`
- `BP_M1A2_Tank_Friendly_01`
- `BP_M1A2_Tank_Friendly_02`

**Enemy (Faction = Enemy)**
- `BP_F15C_Eagle_Enemy_01`
- `BP_F15C_Eagle_Enemy_02`
- `BP_M1A2_Tank_Enemy_01`
- `BP_M1A2_Tank_Enemy_02`

For enemy units:
- Set `AssignedAIControllerClass = BP_SkyForgeAIController`
- Set `BehaviorTreeAsset = BT_CombatDummy`

---

## 4. Input & Polish

## 4.1 Mobile touch + gamepad completion

### Mobile UMG controls
In `WBP_SkyForgeHUD`:
- Add left virtual joystick (move)
- Add right swipe pad (look)
- Add fire button
- Add afterburner button
- Add soul-wheel hold button

Bind each widget interaction to corresponding Enhanced Input action by using `Inject Input For Action` or direct controller events.

### Gamepad profile
- Left stick: move/pan
- Right stick: look/zoom depending mode
- LT hold: Soul wheel
- RT: Fire
- B: Afterburner

## 4.2 Cinematic transition polish

### VFX
- `NS_SoulSwitch_DiveTrail`: spiral ribbon from old pawn to target pawn
- `NS_SoulSwitch_Burst`: short radial burst at target possession frame
- `NS_Afterburner_Jet`: cone + heat distortion sprite
- `NS_VaporCone`: toggled when speed > 1.5x base

### Audio cues
Create and assign:
- `SC_SoulWhoosh` (switch start)
- `SC_SoulImpact` (switch complete)
- `SC_JetEngineLoop` (F15 loop)
- `SC_AfterburnerBurst`
- `SC_TankEngineLoop`
- `SC_TankCannon`

### Camera polish
In SoulSwitch timeline:
- Total transition = **0.45 sec**
- At 0.00s set time dilation 0.25
- At 0.40s restore to 1.0
- Add light camera shake at 0.45s

### Performance guardrails (important)
- Niagara systems max particles capped for mobile variants
- Lumen quality medium in editor test, low for mobile preview
- Use simple collision on prototype meshes
- Keep tick-heavy widget logic on timers (0.05–0.1 sec), not every frame where possible

---

## 5. "Spin Off the Application" — Make It Ready

## 5.1 Build & Run checklist (zero-error path)
1. Compile Blueprints: **Tools → Refresh All Nodes**, then **Compile All Blueprints**.
2. Save all assets.
3. Confirm **Project Settings → Maps & Modes** points to `BP_SkyForgeGameMode` and `Desert_Canyon_Prototype`.
4. Open `Desert_Canyon_Prototype`.
5. PIE in **Selected Viewport** first.
6. Validate initial possession:
   - Player starts in first friendly unit (or command camera then first select).
7. Hold Soul Wheel input:
   - Wheel appears with friendly units.
8. Select F-15 → release:
   - Camera dive ≈ 400–450 ms
   - Whoosh audio + burst FX
   - Possession changes to F-15
9. Hold wheel again, select Tank:
   - Same cinematic transition
   - Old unit continues under AI
10. Attempt immediate re-switch:
   - Cooldown ring blocks switch for 8 seconds
11. Fire cannon at bridge:
   - Fracture/destruction response visible

## 5.2 iOS/Android preview packaging settings

### Android
Project Settings → Platforms → Android:
- Minimum SDK per local toolchain (commonly 26+)
- Target SDK latest supported by UE5.4 setup
- Graphics: Vulkan primary, ES3.1 fallback
- Texture format: ASTC

### iOS
Project Settings → Platforms → iOS:
- Minimum iOS: 15.0
- Metal enabled
- Mobile HDR off if performance dips

### Packaging
- Build Configuration: Development (prototype)
- Full Rebuild: On (first package)
- For Distribution: Off (Phase 1)

## 5.3 Final How-to-Test (screenshots-style verification)

1. **Launch PIE** → You see canyon map with HUD + minimap.
2. **Hold Soul key/button** → radial wheel appears center-screen showing all 4 friendly units.
3. **Hover jet icon and release** → camera dives in ~0.45s, whoosh plays, you now control F-15.
4. **Press Afterburner** → jet speed jumps, afterburner trail + vapor cone appear.
5. **Press Fire** → missile placeholder launches with Niagara trail.
6. **Soul-switch to tank** → same cinematic transition, tank becomes controllable.
7. **Aim turret + fire** → muzzle flash, cannon sound, shell impact explosion.
8. **Hit bridge** → chaos fracture chunks break away.
9. **Try immediate switch again** → denied until cooldown ring refills.
10. **After 8s** → switch becomes available and wheel segments return normal color.

If all ten pass, Phase 1 is complete and ready for Phase 2.

---

## 6. Commit-Ready Package

## 6.1 New files/folders the user must create and commit
At minimum, after implementation they should commit:

```text
SkyForgeThunderDominion/Content/Blueprints/Core/BP_SkyForgeGameMode.uasset
SkyForgeThunderDominion/Content/Blueprints/Core/BP_SkyForgePlayerController.uasset
SkyForgeThunderDominion/Content/Blueprints/Core/BP_SkyForgePawn.uasset
SkyForgeThunderDominion/Content/Blueprints/Components/BP_SoulSwitchComponent.uasset
SkyForgeThunderDominion/Content/Blueprints/Units/BP_F15C_Eagle.uasset
SkyForgeThunderDominion/Content/Blueprints/Units/BP_M1A2_Tank.uasset
SkyForgeThunderDominion/Content/Blueprints/Units/BP_AIM120_Placeholder.uasset
SkyForgeThunderDominion/Content/Blueprints/Units/BP_TankShell_Placeholder.uasset
SkyForgeThunderDominion/Content/Blueprints/UI/WBP_SkyForgeHUD.uasset
SkyForgeThunderDominion/Content/Blueprints/UI/WBP_SoulSwitchWheel.uasset
SkyForgeThunderDominion/Content/Blueprints/UI/WBP_Minimap.uasset
SkyForgeThunderDominion/Content/Blueprints/AI/BP_SkyForgeAIController.uasset
SkyForgeThunderDominion/Content/Blueprints/AI/BT_CombatDummy.uasset
SkyForgeThunderDominion/Content/Maps/Desert_Canyon_Prototype.umap
SkyForgeThunderDominion/Content/VFX/NS_SoulSwitch_DiveTrail.uasset
SkyForgeThunderDominion/Content/VFX/NS_SoulSwitch_Burst.uasset
SkyForgeThunderDominion/Content/VFX/NS_Afterburner_Jet.uasset
SkyForgeThunderDominion/Content/VFX/NS_VaporCone.uasset
SkyForgeThunderDominion/Content/VFX/NS_Explosion_Medium.uasset
SkyForgeThunderDominion/Content/Audio/SC_SoulWhoosh.uasset
SkyForgeThunderDominion/Content/Audio/SC_SoulImpact.uasset
SkyForgeThunderDominion/Content/Audio/SC_JetEngineLoop.uasset
SkyForgeThunderDominion/Content/Audio/SC_AfterburnerBurst.uasset
SkyForgeThunderDominion/Content/Audio/SC_TankEngineLoop.uasset
SkyForgeThunderDominion/Content/Audio/SC_TankCannon.uasset
SkyForgeThunderDominion/Content/Materials/M_SoulCooldownRing.uasset
SkyForgeThunderDominion/Content/Data/DA_UnitStats.uasset
```

Also commit this updated guide:
- `PHASE1_IMPLEMENTATION_GUIDE.md`

## 6.2 Suggested git commit message

```text
feat(phase1): complete UE5.4 Soul-Switch playable prototype implementation guide

- Add production-ready step-by-step Blueprint build instructions
- Define exact input mappings, HUD/wheel/minimap setup, and Soul-Switch timeline
- Document F-15 and M1A2 playable behaviors with AI handoff and cooldown logic
- Finalize Desert_Canyon_Prototype map setup, chaos destruction, and test checklist
- Add packaging/test validation for mobile + PC preview
```

---

## Prime-Time Outcome
By following this document exactly, the project becomes a **jaw-dropping, buttery-smooth Phase 1 playable** where Soul-Switching feels magical, cinematic, and reliable on both PC preview and mobile-targeted settings—fully ready to advance into Phase 2 combat systems.
