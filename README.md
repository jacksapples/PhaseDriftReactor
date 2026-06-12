# Phase Drift Reactor — Build & Install

This mod is a SML 3.x (Satisfactory Mod Loader) content + plugin
mod that adds a new fuel-less power generator to the game.

## Concept

The Phase Drift Reactor (PDR) is a stationary electrical generator
that produces power in direct proportion to the operational entropy
of nearby factory machinery. There is no fuel input. Power scales
with:

- Number of running production buildings within 96m
- Active conveyor belts within 96m
- Power poles within 96m
- A 1.6x "darkness bonus" when the unit has no direct line of
  sight to the open sky (underground / enclosed bases get more)
- A phase-locking bonus when 2+ PDRs are within 256m of each
  other and connected to the same power circuit (up to 2.5x
  with 6+ units)
- A 25-event "resonance" bonus for Tier 3 when a Time Crystal
  is in any storage within the radius
- A 30-minute grace period before the device begins to ramp
  down if it is the SOLE producer on its grid (anti-cheese)

The mod ships in 4 tiers with output caps of 50 / 200 / 500 /
1000 MW, gated by Hub progression and an AWESOME shop unlock.

## What this mod is NOT

- Not a void-style "free power, no thought" mod. PDRs are
  strongest when paired with active machinery. Standalone PDRs
  produce at most 2 MW and begin a 30-minute ramp-down.
- Not a cheat. Costs and tier caps are deliberate. Endgame
  output of a 6-PDR T3 cluster caps at ~15 GW; vanilla T3/T4
  nuclear setups produce 2-3 GW. PDR is competitive, not broken.

## Files in this project

- FactoryGame.uplugin                SML 3.x manifest
- Config/Config.ini                   SML config
- Source/PhaseDriftReactor/           C++ runtime (compile via UAT)
- Content/FactoryGame/Buildable/...  Buildable, hologram, item
                                      descriptors, recipes
- Content/FactoryGame/Schematics/    Schematic unlock
- Resources/                          Algorithm spec, lore memo

## Install steps (manual dev workflow)

1. Open the project in SatisfactoryEditor.exe (Editor mode).
2. Compile C++ via "Build" > "Compile" inside the editor.
3. Right-click FactoryGame.uproject > "Package Mod".
4. SML produces a .smmod file in Saved/StagedBuilds.
5. Copy the .smmod to:
     C:\Users\Jonathan\AppData\Roaming\SatisfactoryModManager\profiles\development\
   Or use the Satisfactory Mod Manager UI to install it
   into the "development" profile.

## Development tools

- SatisfactoryEditor.exe (Unreal 5.x with Satisfactory source)
- Alpine SDK 1.2 (the Satisfactory modding fork of UE)
- Visual Studio 2022 with C++ workload
- SML 3.12.0+ Satisfactory Mod Loader
- SMM 3.1.0+ Satisfactory Mod Manager

## What works WITHOUT a compiled build

The Content/ folder contains .building, .recipe, .schematic,
and .itemdesc descriptors that SML picks up at load time. If
the C++ runtime is not compiled, the building will appear in
the build menu but will not produce power (or will produce
0 MW; safe, no crashes). Compiling the C++ is what makes
it actually tick.

## Lore

See Resources/PDR_Lore_Memo.txt for the in-universe R&D memo
from Dr. Halvar Kølle, FICSIT Phlogiston Division.
