# SkyForge: Thunder Dominion — Production Development Specification (MVP→Live)

## 1) Game Design Document (2-page summary)

### Vision Pillars
**Game fantasy:** *“I am everywhere on the battlefield at once.”*

1. **Total Battlefield Presence** — Player fluidly commands a large-scale combined-arms war from overhead and instantly inhabits any unit via **Soul-Switch**.
2. **Cinematic Modern Warfare** — Photoreal jets, armored warfare, missile duels, and synchronized strikes that feel impossible on mobile.
3. **Skill-Forward Fairness** — Free-to-play with cosmetics and progression speedups only; no stat advantage purchases.
4. **Massive yet Readable Battles** — 200+ simulated entities with clear visual language, tactical UI hierarchy, and low-friction controls.
5. **Performance at Scale** — UE5.4 fidelity on high-end devices with aggressive scalability to hold 60 FPS targets on mid-range phones.

---

### Core Gameplay Loop
1. **Scan battlefield (Top-Down Command View)**
   - Capture points, identify threats, route armor and infantry, assign air support.
2. **Issue tactical orders**
   - Formation, focus-fire, hold/advance, artillery designation, SAM coverage arcs.
3. **Soul-Switch into a critical unit**
   - Dive into F-15/F-16 cockpit, tank commander seat, or infantry perspective to execute precision actions.
4. **Trigger high-impact abilities**
   - Thunder Strike, Ghost Protocol, EMP missile for momentum shifts.
5. **Exploit destruction + weather**
   - Collapse bridges, ignite fuel depots, use storms/night to deny sensors.
6. **Secure objectives + extract rewards**
   - XP, materials, cosmetics progress, and rank gains feed long-term progression.

---

### Session Design & Match Cadence
- **Campaign mission:** 12–18 minutes average (MVP first 12 missions).
- **Live Conquest 8v8:** 10–14 minutes, comeback mechanics via objective multipliers and timed command abilities.
- **Skirmish:** 5–25 minutes configurable.
- **Ranked Arena:** 8–12 minutes with strict MMR and anti-stall pacing.

---

### Player Modes

#### A. Campaign (Single-Player)
- **Launch scope:** First 12 of planned 25 missions.
- Story-delivered via pre-mission cinematics + in-mission radio chatter.
- Escalation arc:
  1. Tutorialization of command + Soul-Switch.
  2. Combined-arms pressure.
  3. EW and weather complexity.
  4. Mini-boss doctrine encounters (carrier task force elements, drone swarms).

#### B. Live Conquest (8v8 Multiplayer)
- Maps at launch: **Desert Canyon, Urban Ruins, Mountain Pass, Coastal Fortress**.
- Win condition: ticket depletion via control points and attrition.
- Force composition: mixed air/ground rosters with role constraints to prevent all-air abuse.

#### C. Skirmish
- Custom AI battles with selectable weather, time of day, unit caps, and reinforcements.

#### D. Ranked Arena
- Seasonal ladders, clan tags, cosmetic season rewards, ranked divisions and decay at upper tiers.

---

### Combat Systems

#### Soul-Switch (Signature Mechanic)
- Tap friendly unit icon or world-space badge to instantly transfer control.
- **Transition target:** <500 ms average, cinematic camera dive + short invulnerability frame (0.4s) to prevent spawn-kill feel-bad.
- Cooldown: 8s baseline (modifiable by commander perks, not paid items).
- Tactical AI retains previous unit behavior when player exits.

#### Weapon & Missile Simulation
- Guided missiles model acceleration curves, lock cones, and seeker behavior.
- Countermeasures: flares/chaff/ECM pods with timing windows and diminishing spam value.
- Blast effects:
  - Niagara particles
  - Shockwave impulse
  - Chaos debris generation
  - Material burn states and fire propagation in flammable biomes.

#### Special Abilities
- **Thunder Strike:** synchronized F-15/F-16 attack lane saturation.
- **Ghost Protocol:** temporary radar/visual stealth profile reduction for one aircraft.
- **EMP Missile:** 8-second electronics disable (locks, HUD modules, active radar, drone uplink).

---

### Controls & UX
- **Top-down:** pinch zoom, two-finger drag, tap-select + drag command line.
- **Flight:** left virtual stick + right look/roll pad, optional aim assist, lead reticle.
- **Ground:** dual-stick move/look, contextual action button, tap/hold fire logic.
- **Command wheel:** quick-select weapons, stance, formation, target priorities.
- **Controller support:** Xbox/PlayStation mappings and remap presets.

---

### Meta, Progression, Monetization
- Unit unlocks through gameplay progression paths (Operations tree).
- **Battle Pass:** cosmetics, emotes, profile banners, non-stat boosters.
- **Store:** skins only + convenience unlock acceleration.
- **Hard rule:** no monetized stat boosts in PvP.

---

### Technical Architecture
- **Engine:** Unreal Engine 5.4+.
- **Visual stack:** Nanite (environment), Lumen fallback tiers, VSM, Niagara.
- **Physics/destruction:** Chaos Geometry Collections + deterministic destruction replication budget.
- **Platforms:** iOS 15+, Android 10+, cross-play matchmaking pools by input type.
- **Netcode:** dedicated authoritative servers + rollback-assisted reconciliation for high-tempo engagements.
- **Scale target:** 200+ units/entities active with LOD + relevance culling.
- **AI:** offline commander AI with behavior trees + utility scoring.

---

### Accessibility & Compliance
- Color-blind presets (Protanopia/Deuteranopia/Tritanopia).
- One-handed mode (auto-throttle + radial command simplification).
- Subtitle system with speaker labels, size presets, and background opacity.
- Haptics intensity slider and motion reduction camera options.

---

### MVP Launch Content (Must Ship)
- 4 maps
- 8 playable units
- Campaign missions 1–12
- Live Conquest 8v8
- Tutorial/onboarding teaching Soul-Switch within first 90 seconds

---

## 2) Detailed Unit Stats Table (Launch 8 Units)

> Values are balance-start targets for first playable and should be tuned via telemetry (TTK, win contribution, ability usage, and retention).

| Unit | Class | Health (HP/EHP) | Speed | Armor / Evasion | Primary Weapons | Secondary / Utility | Ability Cooldowns |
|---|---|---:|---|---|---|---|---|
| **F-15C Eagle** | Air Superiority Fighter | 1,600 HP | Max Mach 2.5 (combat cap Mach 1.8) | High evasion, medium armor | 20 mm cannon (900 RPM, 240 rounds), AIM-120 x6 (active radar) | Flares x2 charges, Chaff x2, High-G turn burst | Soul-Switch 8s, Afterburner Burst 14s, Countermeasure reload 18s/charge |
| **F-16C Viper** | Multirole Strike Fighter | 1,350 HP | Max Mach 2.0 (combat cap Mach 1.6) | Medium evasion, medium-low armor | 20 mm cannon (800 RPM, 300 rounds), AGM-65 x4, Cluster bomb x2 | LGB precision strike designator, ECM pod | Soul-Switch 8s, Precision Run 20s, ECM 26s |
| **MQ-9 Reaper** | UAV Support / Loiter | 900 HP | 480 km/h | Low armor, low signature | Laser-guided Hellfire x4, EO/IR recon ping | Loitering munition deploy x2 | Soul-Switch 8s, Target Paint 16s, Loiter Deploy 28s |
| **M1A2-equivalent MBT** | Heavy Armor | 3,800 EHP | 55 km/h | Very high frontal armor | 120 mm cannon (8s reload), coax MG | Smoke launcher, short-range AA missile x2 | Soul-Switch 8s, Reactive Armor Pulse 24s, Smoke 18s |
| **T-90M-equivalent MBT** | Assault Armor | 3,500 EHP | 60 km/h | High armor, better lateral profile | 125 mm cannon (7.5s reload), coax MG | ATGM tube-launch x2, smoke | Soul-Switch 8s, ATGM Volley 22s, Smoke 18s |
| **Infantry Fireteam Alpha** | Anti-Armor Squad (4-man) | 1,000 squad HP | 5.5 m/s sprint | Cover-based mitigation | M4 carbines (burst/auto), Javelin x2 | Drone spotter micro-UAV, frag grenades | Soul-Switch 6s, Spotter Drone 30s, Javelin reload 20s |
| **Infantry Fireteam Bravo** | Air Defense Squad (4-man) | 950 squad HP | 5.8 m/s sprint | Cover-based mitigation | M4 carbines, Stinger x3 | NV/Thermal toggle, suppression call | Soul-Switch 6s, MANPADS lock boost 18s, Suppression 24s |
| **Mobile SAM/Artillery Hybrid** | Area Denial Vehicle | 2,400 EHP | 42 km/h | Medium armor | SAM battery (6-cell), HE barrage (3-round burst) | Radar sweep, relocation booster | Soul-Switch 8s, Radar Sweep 20s, Barrage 26s |

### Global Weapon/System Rules
- **EMP missile effect duration:** 8.0s hard disable on affected electronics.
- **Thunder Strike:** 90s cooldown, 2 attack waves over 6s lane.
- **Ghost Protocol:** 14s duration, 75s cooldown, lock-on resistance + visual signature dampening.
- Countermeasure immunity window: 1.2s post-deploy.

---

## 3) Sample Level Design — “Desert Canyon”

### High Concept
A vast red-rock valley with chokepoint bridges, dry grass basins, and elevated SAM plateaus. Designed for dramatic air-to-ground interplay and destructive route reshaping.

### Layout (8v8 Conquest)
- **Size:** 2.2 km x 2.2 km.
- **Control points:**
  - **A: Ridge Relay** (high-altitude radar node)
  - **B: Canyon Bridge** (central high-traffic choke)
  - **C: Refinery Flats** (fuel tanks, explosive chain reactions)
  - **D: Wadi Outpost** (cover-rich infantry lanes)
  - **E: Wind Farm Spur** (open, high-risk air corridor)
- **Spawns:** asymmetric mirrored bases with anti-spawn shelling safe zones.

### Tactical Lanes
1. **Air lane (high altitude canyon spine):** best for fighter interception and beyond-visual-range exchanges.
2. **Armor lane (bridge-to-refinery):** MBT push/pull with destructible overpass options.
3. **Infantry lane (wadi trenches):** anti-armor ambush and SAM denial pockets.

### Destruction Script Opportunities
- Destroy **Canyon Bridge** to split armored rush.
- Ignite **Refinery fuel depots** causing lingering fire zones and vision shimmer.
- Collapse **cliffside facades** to create fresh cover and block line-of-sight.

### Weather Events
- **Sandstorm cycle** (every ~4 min, 45s duration):
  - radar range reduced 35%
  - lock-on acquisition time +40%
  - drone spotting effectiveness reduced
- **Night variant:** thermal and flare usage become core to target acquisition.

### Match Flow Targets
- Opening (0:00–2:00): rapid multi-point contest and scouting.
- Midgame (2:00–8:00): bridge control and refinery denial become decisive.
- Endgame (8:00+): ticket bleed race with high-value Thunder Strike windows.

### Performance Budget Notes
- Dynamic debris cap by platform tier.
- HLOD swap for distant mesas.
- Particle budget clamps during overlapping bombardments.

---

## 4) UI Mockup Descriptions (HUD)

### A. Top-Down Command HUD
- **Top-left:** mission timer, ticket bars (blue/red), weather icon + countdown.
- **Top-right:** mini objective panel (active goals, bonus tasks).
- **Bottom-left:** tactical minimap (pinch-expand), friendly/enemy pings, capture rings.
- **Bottom-center:** command strip (hold, assault, flank, regroup), formation state chip.
- **Bottom-right:** Soul-Switch wheel button + ability cluster (Thunder/Ghost/EMP).

### B. Soul-Switch Wheel
- Radial wheel appears on long-press or tap button:
  - segments by class (Air, Armor, Infantry, Support)
  - unit portrait + HP + ammo + threat markers
  - swipe to select, release to dive camera transition
- Safety affordances:
  - red warning ring if target is critically threatened
  - cooldown tint overlay

### C. Cockpit / Ground Combat HUD
- **Center:** adaptive reticle with lead indicator and lock confidence.
- **Left:** movement/throttle control.
- **Right:** aim/look + fire button + alt-fire.
- **Upper-center:** target card (distance, armor class, debuff status).
- **Lower-center:** weapon rack with ammo counts and reload arcs.
- **Right edge:** contextual quick-chat + radio command pips.

### D. Kill Feed & Combat Readability
- Vertical feed right side with icon language:
  - missile, cannon, collision, ability kill types
  - assist tag and multi-kill callouts
- Important events (point captured, EMP hit, Thunder Strike ready) use short, non-blocking banner pulses.

### E. Accessibility UI Variants
- high-contrast objective rings
- larger touch zones preset
- one-handed mirrored HUD layout
- subtitle pane with speaker portrait and radio filter styling

---

## 5) Monetization Store Layout (Fair F2P)

### Store IA (Information Architecture)
1. **Featured**
   - seasonal bundles, limited-time cosmetics, crossover packs
2. **Aircraft Cosmetics**
   - F-15/F-16 skins, contrail colors, nose art
3. **Ground Cosmetics**
   - tank camos, infantry uniforms, weapon wraps
4. **Pilot Identity**
   - helmets, visors, banners, announcer packs, emblems
5. **Battle Pass**
   - free/premium tracks preview with claim history
6. **Currency**
   - premium currency packs, starter value bundle, first-purchase bonus

### Purchase Design Rules
- Every item clearly labeled **COSMETIC ONLY**.
- Side-by-side preview in diorama (day/night/weather toggle).
- No random stat-affecting loot boxes.
- Duplicate protection for owned cosmetics.

---

## 6) Phase 1 Implementation Plan (Foundation Milestone)

### Phase 1 Goal
Deliver a production-ready vertical foundation for *SkyForge: Thunder Dominion* that proves the core fantasy loop:
**command from top-down → Soul-Switch into unit → execute combat action → return to command flow**.

### Scope (In)
1. **Core Playable Loop**
   - Top-down command camera with map navigation and unit selection.
   - Soul-Switch transition pipeline targeting sub-500ms handoff.
   - Return-to-command flow from possessed unit.
2. **Initial Unit Set (4 of 8 launch units)**
   - F-15C Eagle
   - F-16C Viper
   - M1A2-equivalent MBT
   - Infantry Fireteam Alpha
3. **Combat Baseline**
   - Primary weapons, lock-on flow, countermeasures, and damage model v1.
   - EMP missile gameplay effect implementation (8s disable rules).
4. **Map + Objectives**
   - Desert Canyon blockout to art-pass-ready alpha.
   - Three-point Conquest ruleset (A/B/C subset from full design).
5. **UI/HUD v1**
   - Top-down command HUD skeleton.
   - Soul-Switch radial selector.
   - In-unit reticle, ammo rack, and target card.
6. **Technical Foundation**
   - Dedicated server session bootstrap for internal multiplayer tests.
   - Replication relevance/LOD scaffolding for 200-entity scale target.
   - Telemetry hooks for TTK, ability usage, and switch frequency.

### Scope (Out)
- Full 8v8 live operations.
- Ranked Arena and seasonal systems.
- Monetization storefront production wiring.
- Campaign mission cinematics and full narrative stack.
- Final art polish and full destruction fidelity pass.

### Exit Criteria (Definition of Done)
- 10-minute internal Conquest match playable end-to-end without blockers.
- Soul-Switch success rate ≥ 98% with median transition time under 500ms.
- Stable dedicated-server session for 16 players in test environment.
- 60 FPS on target high-end device profile, scalable profile operational on mid-tier.
- Core telemetry events emitted and visible in analytics dashboard.

### Phase 1 Deliverables
- Playable internal alpha build (`Phase1-Alpha`).
- Technical test report (network performance + frame pacing).
- Balance snapshot for the initial 4-unit roster.
- Updated risk register and scope recommendation for Phase 2.

### Suggested Launch SKUs
- Starter Pack ($4.99): 1 pilot skin + 1 jet skin + currency.
- Seasonal Pass ($9.99): 50 tiers cosmetics + boosters.
- Elite Pass Bundle ($19.99): +15 tier skips + exclusive finisher VFX.
- Currency packs from $0.99 to $99.99 with transparent value table.

### Retention-linked Economy Hooks
- Daily ops (non-intrusive) grant soft currency and pass XP.
- Weekly challenge chains unlock premium-currency trickle.
- Clan milestone cosmetics encourage social return loops.

---

## 6) 30-Second Marketing Hook Trailer Script

### Title Card
**“SKYFORGE: THUNDER DOMINION”**

### 0:00–0:05 (Establish Spectacle)
- Aerial shot: sunrise over burning canyon. F-15 streaks overhead, afterburners glowing, contrails carving the sky.
- SFX: thunderous jet roar + distant artillery.
- VO (calm, intense): “One battlefield. A hundred wars.”

### 0:06–0:11 (Air Superiority)
- Cockpit view lock tone rises. Player fires AIM-120; missile trails through cloud vapor and detonates near enemy fighter.
- UI flashes “AIRSPACE SECURED.”
- Music ramps with heavy percussion.

### 0:12–0:17 (Signature Mechanic Reveal)
- Freeze-frame micro-beat. Player taps Soul-Switch icon.
- Camera violently dives from sky to ground into an Abrams-equivalent turret sight.
- Immediate 120 mm shot obliterates a bridge column; enemy convoy falls into dust and fire.
- VO: “Don’t pick a class. Become the whole army.”

### 0:18–0:24 (Combined-Arms Chaos)
- Quick cuts:
  - Infantry team paints target with drone.
  - F-16 drops cluster bombs in canyon pass.
  - EMP wave pulses, enemy HUDs flicker out.
  - Thunder Strike saturates refinery with synchronized detonations.
- On-screen text: “REAL-TIME COMMAND + DIRECT CONTROL.”

### 0:25–0:30 (Call to Action)
- 8v8 battle tableau at night: tracers, flares, collapsing skyscraper silhouette.
- Logo slam + platform badges (iOS / Android).
- VO (hard close): “SkyForge: Thunder Dominion. Command everything.”
- Super: “Pre-register now. Season Zero incoming.”

---

## Production Notes & Next Steps (Internal)
- Run 6-week closed alpha focused on Soul-Switch readability and mobile thermal performance.
- Instrument telemetry for switch frequency, mode engagement, and churn at tutorial step 3.
- Balance milestone: aircraft dominance cap (ensure ground counterplay >42% successful denial rate in objective zones).
