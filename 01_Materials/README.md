# 01 · Materials

Material-graph exercises for the course *Tecnologías del Videojuego y Entornos Virtuales II* (ESNE).
The goal was to understand how the Material Editor nodes work by building materials from scratch,
reverse-engineering given materials and applying them to themed scenes.

<!-- MEDIA: screenshots of the scenes (Scenes/1_Materiales, 3_Oficina, 4_Spaceship) go here. -->

**Engine:** Unreal Engine 5.4 · **Tech:** Material Editor · **Report:** [Documentation_ES.pdf](Documentation_ES.pdf) (Spanish)

## What's inside

| Map (`Content/Scenes/`) | Content |
|-------------------------|---------|
| `1_Materiales` | Material exercises 1–12: physically based surfaces (gold, copper, aluminium, granite, glass, diamond, ice), opacity / translucency, two-sided materials, masks, lerp/add/multiply blends, fresnel, decals |
| `2_IngenieriaInversaMateriales` | Three reverse-engineered materials (see below) |
| `3_Oficina` | The materials applied to an office scene |
| `4_Spaceship` | The materials applied to a spaceship interior |

### Reverse-engineered materials

1. **Pulsing emissive** — `Time` → `Sine` → `Clamp` drives the emissive intensity. Use cases: flickering lights in horror games, beacons, flashlights.
2. **Scrolling texture** — `Panner` + `Texture Sample` moves the base colour over time. Use cases: rivers, lava, animated screens.
3. **Distance-based colour** — `Distance(CameraPosition, ObjectPosition)` / range → `Lerp` from blue (far) to red (near). Use cases: highlighting nearby enemies or pickups (combined with fresnel).

Master materials with parameters and several material instances (`1_Materiales/12/`) show the
parent/instance workflow.

## Where my work is

- `Content/MaterialesEjercicios/` — all the materials
- `Content/Scenes/` — the four maps

`Content/StarterContent/` (Epic Games) and `Content/SpaceshipInterior/` (environment pack from Fab)
are third-party content used as textures, meshes and set dressing.
