# SkyForge: Thunder Dominion — 60-Minute Ready-to-Run Prototype Build (UE 5.4.4)

This is the **copy-paste-into-editor** build playbook for turning the current repo into a fully playable Phase-1 Soul-Switch prototype in Unreal Engine **5.4.4**.

---

## 1) Project Creation & Open Instructions

### 1.1 Clone/open repo and verify branch
1. Open terminal:
   ```bash
   cd /workspace
   git clone https://github.com/arbindchristy/Skyforge.git
   cd Skyforge
   git checkout main
   ```
2. Confirm scaffold exists:
   - `SkyForgeThunderDominion/SkyForgeThunderDominion.uproject`
   - `SkyForgeThunderDominion/Content`
   - `SkyForgeThunderDominion/Config`

### 1.2 Open Unreal and generate first real project files
1. Launch **Unreal Engine 5.4.4**.
2. Click **Browse…** and open:
   - `SkyForgeThunderDominion/SkyForgeThunderDominion.uproject`
3. If prompted for modules/project files, click **Yes** to generate.
4. Wait for project to finish opening.

### 1.3 Enable required plugins
Go to **Edit → Plugins** and enable:
- **Enhanced Input**
- **Niagara**
- **Chaos Destruction**
- **Chaos Vehicles** (prototype-ready, optional immediate use)
- **Modeling Tools Editor Mode** (for quick blockout)
- **Behavior Tree Editor** (AI workflow)

Click **Restart Now** when prompted.

### 1.4 Project settings baseline (PC + Mobile preview)
Go to **Edit → Project Settings**:

**Engine → Rendering**
- Dynamic Global Illumination Method: **Lumen**
- Reflection Method: **Lumen**
- Virtual Shadow Maps: **Enabled**
- Nanite: **Enabled**

**Platforms/Performance**
- Smooth Frame Rate: **On**
- Fixed Frame Rate (testing): **60.0**
- Mobile Previewer target: **Android Vulkan** (and ES3.1 fallback profile)

**Maps & Modes**
- Editor Startup Map: `/Game/Maps/Desert_Canyon_Prototype`
- Game Default Map: `/Game/Maps/Desert_Canyon_Prototype`
- Default GameMode: `/Game/Blueprints/Core/BP_SkyForgeGameMode`

### 1.5 Create authoritative content folders (exact)
In Content Browser, create:
- `/Game/Blueprints/Core`
- `/Game/Blueprints/Components`
- `/Game/Blueprints/Units`
- `/Game/Blueprints/AI`
- `/Game/Blueprints/Input`
- `/Game/Blueprints/UI`
- `/Game/Maps`
- `/Game/VFX`
- `/Game/Audio`
- `/Game/Materials`
- `/Game/Data`
- `/Game/Geometry`

---

## 2) Complete Blueprint Build Guide (node-by-node)

> Build order matters. Follow in sequence: Input → Base Pawn → Soul Component → Controller → GameMode → Units → UI → AI.

### 2.1 Enhanced Input assets

#### A) Input Actions (create all in `/Game/Blueprints/Input`)
Create each as **Input Action**:
1. `IA_TapSelect` (Value Type: Digital)
2. `IA_SoulWheelHold` (Digital)
3. `IA_SoulWheelRelease` (Digital)
4. `IA_CameraPan` (Axis2D)
5. `IA_CameraZoom` (Axis1D)
6. `IA_Move` (Axis2D)
7. `IA_Look` (Axis2D)
8. `IA_PrimaryFire` (Digital)
9. `IA_Afterburner` (Digital)

#### B) Input Mapping Contexts
Create:
- `IMC_CommandMode`
- `IMC_UnitMode`

**IMC_CommandMode mappings**
- Left Mouse Button / Touch1 Pressed → `IA_TapSelect`
- Right Mouse Button (Hold) → `IA_SoulWheelHold`
- Right Mouse Button (Released) → `IA_SoulWheelRelease`
- Mouse X,Y or Touch Delta → `IA_CameraPan`
- Mouse Wheel Axis or Pinch Axis → `IA_CameraZoom`

**IMC_UnitMode mappings**
- WASD + Left Stick → `IA_Move`
- Mouse Delta + Right Stick → `IA_Look`
- Left Mouse / Right Trigger → `IA_PrimaryFire`
- Left Shift / FaceButtonBottom(B) → `IA_Afterburner`

---

### 2.2 `BP_SkyForgePawn` (Base controllable unit)
**Path:** `/Game/Blueprints/Core/BP_SkyForgePawn`

#### Components
Add in order:
1. `DefaultSceneRoot`
2. `SkeletalMesh` (rename: `UnitMesh`)
3. `FloatingPawnMovement` (rename: `MoveComp`)
4. `Widget` (rename: `WorldBadgeWidget`)
5. `Niagara` (rename: `NS_SoulEntryFX`)
6. `Audio` (rename: `AC_SoulSwitchSFX`)
7. `Arrow` (rename: `SoulEntrySocketProxy`, forward axis)

#### Class defaults
- Auto Possess Player: Disabled
- Auto Possess AI: Disabled

#### Variables (exact)
- `FactionTag` (Name, default `Faction.Friendly`)
- `UnitClassTag` (Name, default `Class.None`)
- `Health` (Float, default 100)
- `MaxHealth` (Float, default 100)
- `bIsPlayerControlled` (Bool, default false)
- `AssignedAIControllerClass` (Class Reference → AIController)
- `bInvulnerable` (Bool, default false)

#### Function: `CanBeSoulSwitched`
Inputs: none. Output: `ReturnValue` (Bool)

Node chain:
1. `Health` → `>` (Float) with B=0.0
2. `Get Hidden` (self) → `NOT`
3. `FactionTag` == `Faction.Friendly`
4. `AND` (HealthAlive, NotHidden)
5. `AND` (Result, IsFriendly)
6. `ReturnNode(ReturnValue=AND2)`

#### Function: `OnPossessedByPlayer`
1. `Set bIsPlayerControlled = true`
2. `Set bInvulnerable = true`
3. `Set Timer by Event` (Time=0.4, Looping=false)
   - Event: `Set bInvulnerable = false`

#### Function: `OnReleasedToAI`
1. `Set bIsPlayerControlled = false`
2. `Spawn Default Controller`
3. `Get Controller` → `Cast to AIController`
4. `IsValid` branch; if true:
   - `Run Behavior Tree` (BT asset to be assigned in child or defaults)

---

### 2.3 `BP_SoulSwitchComponent`
**Path:** `/Game/Blueprints/Components/BP_SoulSwitchComponent` (Actor Component)

#### Variables
- `SwitchCooldown` Float = 8.0
- `DiveDuration` Float = 0.45
- `InvulnDuration` Float = 0.40
- `bSwitchReady` Bool = true
- `LastSwitchTimestamp` Float = -1000
- `OldPawn` (BP_SkyForgePawn Object Ref)
- `TargetPawn` (BP_SkyForgePawn Object Ref)
- `OwningPC` (BP_SkyForgePlayerController Object Ref)

#### Dispatcher
- `OnSoulSwitchCooldownPercent` (Float Percent)
- `OnSoulSwitchReady` ()

#### Timeline
Create Timeline `TL_SoulDive`:
- Length: 0.45
- Float Track: `Alpha` 0→1, Auto curve ease-in-out

#### Function: `RequestSoulSwitch(TargetIn)`
Nodes (exact execution order):
1. `Set TargetPawn = TargetIn`
2. `Branch` condition `bSwitchReady`
   - False: return
3. `IsValid(TargetPawn)` branch
   - False: return
4. `TargetPawn -> CanBeSoulSwitched` branch
   - False: return
5. `Get Owner` → `Cast to BP_SkyForgePlayerController` → `Set OwningPC`
6. `IsValid(OwningPC)` branch, false return
7. `OwningPC -> Get Controlled SkyForge Pawn` → `Set OldPawn`
8. `Set Global Time Dilation` = 0.35
9. `Play from Start` Timeline `TL_SoulDive`

#### Timeline Update (`Alpha`)
1. `OwningPC -> Get Player Camera Manager`
2. `OldPawn -> GetActorLocation` = A
3. `TargetPawn -> GetActorLocation` + `(0,0,250)` = B
4. `Lerp(Vector A,B,Alpha)` = CamLoc
5. `Set Camera Location` (PCM, CamLoc)

#### Timeline Finished
1. `OwningPC Possess(TargetPawn)`
2. `IsValid(OldPawn)` → true path:
   - `OldPawn -> OnReleasedToAI`
3. `TargetPawn -> OnPossessedByPlayer`
4. `TargetPawn.NS_SoulEntryFX -> Activate`
5. `TargetPawn.AC_SoulSwitchSFX -> Play`
6. `Set Global Time Dilation = 1.0`
7. `Set bSwitchReady = false`
8. `Set LastSwitchTimestamp = GetGameTimeInSeconds`
9. `Set Timer by Function Name`
   - Function Name: `ResetCooldown`
   - Time: `SwitchCooldown`
   - Looping: false

#### Function: `ResetCooldown`
1. `Set bSwitchReady = true`
2. `Call OnSoulSwitchReady`

---

### 2.4 `BP_SkyForgePlayerController`
**Path:** `/Game/Blueprints/Core/BP_SkyForgePlayerController`

#### Components
1. `SceneComponent` root
2. `SpringArm` (`TopDownBoom`) length 2800
3. `Camera` (`TopDownCamera`) attached to boom
4. `BP_SoulSwitchComponent` (`SoulSwitchComp`)

#### Variables
- `CurrentControlledUnit` (BP_SkyForgePawn Ref)
- `bInCommandView` Bool=true
- `CommandPanSpeed` Float=1800
- `CommandZoomSpeed` Float=200
- `MinZoom` Float=1200
- `MaxZoom` Float=4200
- `SoulWheelWidget` (WBP_SoulSwitchWheel Ref)
- `SelectedSoulSwitchTarget` (BP_SkyForgePawn Ref)

#### Function: `Get Controlled SkyForge Pawn`
- `Get Pawn` → `Cast BP_SkyForgePawn` → Return

#### Event BeginPlay graph
1. `Get Enhanced Input Local Player Subsystem`
2. `Add Mapping Context(IMC_CommandMode, Priority 0)`
3. `Add Mapping Context(IMC_UnitMode, Priority 1)`
4. `Create Widget (WBP_SoulSwitchWheel)` → `Set SoulWheelWidget`
5. `Add to Viewport`
6. `Set Visibility Hidden`

#### Input Event: `IA_TapSelect (Triggered)`
1. `Get Hit Result Under Cursor by Channel (Visibility)`
2. `Break Hit Result -> Hit Actor`
3. `Cast to BP_SkyForgePawn`
4. Branch `IsValid && FactionTag == Friendly`
5. True: `Set SelectedSoulSwitchTarget`

#### Input Event: `IA_SoulWheelHold (Started)`
1. `SoulWheelWidget -> SetVisibility(Visible)`
2. `SoulWheelWidget -> RefreshFromFriendlyUnits` (custom event)

#### Input Event: `IA_SoulWheelRelease (Completed)`
1. `SoulWheelWidget -> SetVisibility(Hidden)`
2. Branch `IsValid(SelectedSoulSwitchTarget)`
3. True: `SoulSwitchComp -> RequestSoulSwitch(SelectedSoulSwitchTarget)`

#### Input Event: `IA_CameraPan (Triggered)`
1. `Get Action Value (Vector2D)`
2. `Make Vector(X=Value.X, Y=Value.Y, Z=0)`
3. `Vector * CommandPanSpeed * GetWorldDeltaSeconds`
4. `Add Actor World Offset` (sweep false)

#### Input Event: `IA_CameraZoom (Triggered)`
1. `Get Action Value (Float)` = Axis
2. `TopDownBoom TargetArmLength - (Axis * CommandZoomSpeed)`
3. `Clamp(MinZoom, MaxZoom)`
4. `Set TargetArmLength`

---

### 2.5 `BP_SkyForgeGameMode`
**Path:** `/Game/Blueprints/Core/BP_SkyForgeGameMode`

#### Class Defaults
- Player Controller Class = `BP_SkyForgePlayerController`
- Default Pawn Class = `None`

#### Variables
- `FriendlyUnits` (Array of BP_SkyForgePawn Ref)

#### Event BeginPlay
1. `Get All Actors of Class(BP_SkyForgePawn)`
2. `ForEachLoop`
3. For each Actor:
   - `FactionTag == Faction.Friendly` → Branch
   - True: `Add` to `FriendlyUnits`
4. After loop completed:
   - `Get Player Controller 0` → Cast `BP_SkyForgePlayerController`
   - `Get(FriendlyUnits, Index=0)` as `InitialPawn`
   - `Possess(InitialPawn)`
   - `InitialPawn -> OnPossessedByPlayer`

---

### 2.6 Unit Blueprints

#### A) `BP_F15C_Eagle`
**Path:** `/Game/Blueprints/Units/BP_F15C_Eagle` parent `BP_SkyForgePawn`

Variables:
- `AfterburnerMultiplier` Float=1.8
- `AfterburnerDuration` Float=2.5
- `BaseMaxSpeed` Float=12000
- `AIM120Ammo` Int=6

Construction/Defaults:
- `UnitClassTag = Class.Air`
- MoveComp MaxSpeed = BaseMaxSpeed

Input nodes (Event Graph):
1. `IA_Move (Triggered)`
   - Value X → `Add Movement Input(GetActorRightVector, X)`
   - Value Y → `Add Movement Input(GetActorForwardVector, Y)`
2. `IA_Look (Triggered)`
   - Value X → `Add Controller Yaw Input(X * 1.8)`
   - Value Y → `Add Controller Pitch Input(Y * -1.3)`
3. `IA_Afterburner (Started)`
   - `Set MoveComp.MaxSpeed = BaseMaxSpeed * AfterburnerMultiplier`
   - `Set Timer by Event(AfterburnerDuration)` → Event sets back `BaseMaxSpeed`
4. `IA_PrimaryFire (Started)`
   - Branch `AIM120Ammo > 0`
   - True:
     - Spawn Actor `BP_AIM120_Placeholder` from muzzle socket transform
     - `AIM120Ammo = AIM120Ammo - 1`

#### B) `BP_M1A2_Tank`
**Path:** `/Game/Blueprints/Units/BP_M1A2_Tank` parent `BP_SkyForgePawn`

Components additional:
- `SceneComponent` `TurretPivot`
- `SceneComponent` `BarrelPivot`

Variables:
- `CannonReload` Float=8.0
- `bCannonReady` Bool=true
- `TurretYawSpeed` Float=40
- `BarrelPitchSpeed` Float=20
- `MinPitch` Float=-8
- `MaxPitch` Float=18

Input nodes:
1. `IA_Move (Triggered)`
   - X: right vector strafe 0.5 scale
   - Y: forward vector move 1.0 scale
2. `IA_Look (Triggered)`
   - Break Vector2D
   - `AddLocalRotation(TurretPivot, Yaw = X * TurretYawSpeed * DeltaSeconds)`
   - `CurrentPitch + (Y * BarrelPitchSpeed * DeltaSeconds)`
   - Clamp MinPitch/MaxPitch
   - SetRelativeRotation(BarrelPivot)
3. `IA_PrimaryFire (Started)`
   - Branch `bCannonReady`
   - True:
     - `Set bCannonReady=false`
     - Spawn Actor `BP_TankShell_Placeholder` at barrel muzzle
     - Activate Niagara muzzle flash
     - Play fire cue
     - Set Timer by Event(CannonReload) -> set `bCannonReady=true`

---

### 2.7 UI Widgets

#### `WBP_SoulSwitchWheel`
Path: `/Game/Blueprints/UI/WBP_SoulSwitchWheel`

Widget tree:
- `CanvasPanel`
  - `Image_Ring`
  - `UniformGridPanel_Units`
  - `ProgressBar_Cooldown`
  - `Text_Status`

Variables:
- `DisplayedUnits` (Array BP_SkyForgePawn)
- `OwningPC` (BP_SkyForgePlayerController)

Graph:
1. Event `Construct`
   - `Get Owning Player` → cast PC, cache
2. Custom Event `RefreshFromFriendlyUnits`
   - `Get GameMode` cast `BP_SkyForgeGameMode`
   - pull `FriendlyUnits` into `DisplayedUnits`
   - regenerate unit buttons (create child widgets or simple text buttons)
3. On unit button hovered/clicked:
   - `OwningPC.SelectedSoulSwitchTarget = HoveredUnit`

#### `WBP_HUDCombat`
Path: `/Game/Blueprints/UI/WBP_HUDCombat`
- Reticle image center
- HP/Ammo bars
- Crossfade when possessed pawn changes

---

### 2.8 AI hand-off (Behavior Tree)

1. Create `BB_CombatDummy` with keys:
   - `MoveTarget` (Vector)
   - `EnemyActor` (Object)
2. Create `BT_CombatDummy`:
   - Root → Selector
   - Sequence A: If EnemyActor IsSet → MoveTo EnemyActor → Wait(0.2)
   - Sequence B: Patrol random point around spawn (GetRandomReachablePointInRadius)
3. Create `BP_SkyForgeAIController`:
   - On Possess → RunBehaviorTree `BT_CombatDummy`

Assign `AssignedAIControllerClass` in both unit blueprints.

---

## 3) Desert_Canyon_Prototype Map Setup

### 3.1 Create map shell
1. **File → New Level → Open World**.
2. Save as `/Game/Maps/Desert_Canyon_Prototype`.
3. World Settings:
   - GameMode Override = `BP_SkyForgeGameMode`

### 3.2 Landscape
1. Landscape mode:
   - Section Size 63x63
   - 2x2 sections/component
   - 16x16 components
2. Sculpt:
   - High ridge north/south edges
   - Central canyon lane
   - One bridge choke in center
3. Material: temporary layered sand/rock material instance.

### 3.3 Place units for instant Soul-Switch test
Place actors:
- `BP_F15C_Eagle` x2 (friendly)
- `BP_M1A2_Tank` x2 (friendly)
- Duplicate one tank and set `FactionTag = Faction.Enemy` for AI target behavior.

Recommended transforms:
- Friendly Jet1: (0, -2000, 700)
- Friendly Jet2: (800, -2200, 720)
- Friendly Tank1: (-600, 400, 120)
- Friendly Tank2: (200, 550, 120)
- Enemy Tank: (2200, 300, 120)

### 3.4 Chaos destruction setup
1. Place a bridge static mesh in center.
2. Convert to Geometry Collection:
   - Right click mesh actor → **Create Geometry Collection**
3. Add `Field System Actor` nearby for fracture impulse test.
4. In PIE use radial force or projectile hit to break bridge.

### 3.5 Niagara explosions
Create quick systems:
- `NS_Explosion_Medium`
- `NS_SoulSwitch_Burst`

Attach:
- Tank shell impact → `NS_Explosion_Medium`
- Soul switch finish event → `NS_SoulSwitch_Burst`

---

## 4) Input, Polish & Cinematic Soul-Switch

### 4.1 Radial wheel behavior
- Hold `IA_SoulWheelHold` to display wheel.
- Hover/select updates `SelectedSoulSwitchTarget`.
- Release triggers `RequestSoulSwitch`.

### 4.2 <500ms dive
- Timeline length hard-set to **0.45s**.
- Global dilation 0.35 during dive.
- Restore to 1.0 on completion.

### 4.3 Slow-mo + audio + camera punch
At timeline start:
- Play whoosh SFX at camera.
- Optional post-process vignette increase.

At timeline finish:
- Play engine roar (jet) or armor slam (tank) based on `UnitClassTag`.
- Trigger camera shake (small, 0.2 intensity, 0.15 sec).

### 4.4 AI hand-off
- Old pawn always calls `OnReleasedToAI` after possession transfer.
- New pawn always calls `OnPossessedByPlayer`.

### 4.5 Cooldown lock (8s)
- `bSwitchReady=false` right after successful switch.
- `ResetCooldown` timer at 8.0 sec.
- HUD cooldown progress ring reads `(Now - LastSwitchTimestamp)/8` clamped 0..1.

---

## 5) “Spin Off the Application” Checklist

### 5.1 First playable validation (5 minutes)
1. Open map `Desert_Canyon_Prototype`.
2. Click **Play In Editor**.
3. You should start possessing first friendly unit.
4. Hold Soul wheel key/button, select other unit, release.
5. Confirm:
   - camera dive < 0.5s
   - slow-mo during dive
   - AI takes over previous unit
   - cooldown blocks immediate re-switch

### 5.2 Performance sanity
- PIE in **Mobile Preview (Vulkan)**
- `stat unit`, `stat fps`, `stat game`
- Verify target near 60 FPS with placeholder assets.

### 5.3 Packaging settings for Android + Windows
Project Settings → Packaging:
- Build Configuration: Development (prototype) / Shipping (later)
- Use Pak File: On
- Full Rebuild: Off for iteration

Android:
- Target SDK per installed toolchain
- Vulkan enabled, ES3.1 fallback

Windows:
- Default RHI DX12

### 5.4 One-button test routine
- Play → Soul-switch Jet↔Tank three times.
- Fire weapons from each.
- Destroy bridge test object.
- Verify no possession deadlock or stuck camera.

---

## 6) Commit-Ready File List

After finishing in-editor work, ensure these are present and committed:

### 6.1 Must-commit project files
- `SkyForgeThunderDominion/SkyForgeThunderDominion.uproject`
- `SkyForgeThunderDominion/Config/DefaultEngine.ini`
- `SkyForgeThunderDominion/Config/DefaultGame.ini`
- `SkyForgeThunderDominion/Config/DefaultInput.ini`

### 6.2 Must-commit gameplay Blueprints
- `SkyForgeThunderDominion/Content/Blueprints/Core/BP_SkyForgeGameMode.uasset`
- `SkyForgeThunderDominion/Content/Blueprints/Core/BP_SkyForgePlayerController.uasset`
- `SkyForgeThunderDominion/Content/Blueprints/Core/BP_SkyForgePawn.uasset`
- `SkyForgeThunderDominion/Content/Blueprints/Components/BP_SoulSwitchComponent.uasset`
- `SkyForgeThunderDominion/Content/Blueprints/Units/BP_F15C_Eagle.uasset`
- `SkyForgeThunderDominion/Content/Blueprints/Units/BP_M1A2_Tank.uasset`
- `SkyForgeThunderDominion/Content/Blueprints/AI/BP_SkyForgeAIController.uasset`
- `SkyForgeThunderDominion/Content/Blueprints/AI/BT_CombatDummy.uasset`
- `SkyForgeThunderDominion/Content/Blueprints/AI/BB_CombatDummy.uasset`

### 6.3 Must-commit UI/Map/VFX assets
- `SkyForgeThunderDominion/Content/Blueprints/UI/WBP_SoulSwitchWheel.uasset`
- `SkyForgeThunderDominion/Content/Blueprints/UI/WBP_HUDCombat.uasset`
- `SkyForgeThunderDominion/Content/Maps/Desert_Canyon_Prototype.umap`
- `SkyForgeThunderDominion/Content/VFX/NS_SoulSwitch_Burst.uasset`
- `SkyForgeThunderDominion/Content/VFX/NS_Explosion_Medium.uasset`
- Any imported meshes/audio used in prototype (`/Art`, `/Audio`, `/Geometry`)

### 6.4 Suggested git message and PR title
- Commit message:
  - `feat(phase1): implement playable soul-switch prototype with F-15C, M1A2, AI handoff, and Desert Canyon test map`
- PR title:
  - `Phase 1 Playable Prototype: Soul-Switch, Core Blueprints, Desert Canyon map`

---

## Done Criteria (Definition of Ready-to-Run)
You are done when:
1. PIE launches directly into Desert Canyon.
2. You can Soul-Switch between F-15C and M1A2 in <500ms.
3. Previous unit immediately continues under AI behavior.
4. Switch cooldown enforces 8 seconds.
5. Map includes destructible bridge + Niagara impact/switch FX.
6. Prototype runs in Mobile Preview without fatal errors.
