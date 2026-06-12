# PhaseDriftReactor — Player README

You are reading this because the Phase Drift Reactor mod is
installed in your SMM profile. Welcome.

## What does the mod add?

A new buildable called **Phase Drift Reactor** that generates
electrical power without any fuel input. It is unlocked by
finding a "PHASE DRIFT REACTOR" schematic in the AWESOME shop
once your HUB reaches Tier 4.

## Why does this exist?

FICSIT R&D was poking at some "coherence field" measurements
on the planet and accidentally invented a generator that runs
on factory activity. Don't ask. Just build it.

## Tiers

| Tier | Name                  | Cap (MW) | Hub requirement |
|------|-----------------------|----------|-----------------|
| T0   | Coherence Tap         | 50       | Tier 4          |
| T1   | Phase Anchor          | 200      | Tier 6          |
| T2   | Drift Coil            | 500      | Tier 8          |
| T3   | Entropic Resonator    | 1000     | AWESOME shop    |

Higher tiers cost more, output more.

## How does output work?

It's based on the operational entropy of nearby machinery.
Concretely:

- For every **running** production building within 96m of the
  PDR, the unit generates 0.5 MW. So 100 active machines =
  50 MW right there, before any bonuses.
- Conveyor belts that have moved in the last second count.
- Power poles count.
- The unit outputs at least 2 MW no matter what.
- Underground / enclosed PDRs (no line-of-sight to the sky)
  produce 1.6x more.
- Two or more PDRs within 256m of each other on the same power
  grid "phase-lock" and stack their output:
  - 2 PDRs = 1.45x combined
  - 3 PDRs = 1.85x
  - 4 PDRs = 2.20x
  - 5 PDRs = 2.40x
  - 6+ PDRs = 2.50x (cap)
- A Tier 3 PDR also gets a +25 event bonus (12.5 MW pre-cap)
  for each Time Crystal stored in any storage container within
  the 96m radius.

## Anti-cheese: the sole-producer ramp-down

If your PDR is the **only** producer on a power grid, it
gets a 30-minute grace period at full output. After 30 minutes,
it begins a 1-hour ramp-down to 0% output. The instant any
other generator (coal, fuel, nuclear, geothermal) joins the
grid, the timer resets and the PDR returns to full.

This is intentional. Don't cheese it. Build a real factory.

## Tier-3 "Entropic Resonator" perk

If you have a Time Crystal in a storage container within 96m
of a Tier 3 PDR, the unit generates an extra 12.5 MW of
baseline. The Time Crystal is NOT consumed.

## Visual feedback

The PDR is a tall thin pillar with a spinning glass ring at
the top. The ring's emissive brightness scales with output.
If the unit is dark (no LOS to sky), the ring glows violet.
If it's on a phase-locked grid, the ring has a 4-fold
symmetric pulse pattern.

## Mod compatibility

This mod does not require any other mod. It is compatible with
FicsitWiremod, RefinedPower, and PowerControlRoom (it produces
a normal FGPowerInfo signal that any consumer can read).
