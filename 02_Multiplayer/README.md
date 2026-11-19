# 02 · Multiplayer FPS

A networked first-person shooter prototype made entirely with Blueprints for the course
*Postproducción Digital* (UDIT, 2023/24). The focus was learning Unreal's gameplay framework and
replication: what runs on the server, what is multicast, and what each client sees.

<!-- MEDIA: GIF of two clients fighting (Play → Net Mode: Play As Listen Server, 2 players). -->

**Engine:** Unreal Engine 5.8 · **Tech:** Blueprints, Enhanced Input, Chaos Destruction · **Report:** [Docs/Report.md](Docs/Report.md)

## Features

- **Replicated aiming** — the camera pitch is calculated on the server (*Run on Server*) and multicast to every client, driving a 1D aim offset built from three poses so other players see where you are looking.
- **First/third-person visibility** — the local player only sees their arms; everybody else sees the full body.
- **Two fire modes**
  - *Direct projectile* (LMB): spawned on the server, 10 damage, +10 energy, uses ammo, 1.5 s cooldown.
  - *Area shot* (RMB, costs 70 energy): physics-driven projectile that applies radial damage, impulses and a transient field to break **Chaos geometry collections**.
- **Damage types** — physical, fire and frost (mouse wheel or 1–3). Repeated hits with the same type build **immunity**: 100% → 50% → 25% → 0% damage.
- **Health, potion and death** — health bar, potion that heals when below 70% (60 s cooldown), death screen and 10 s respawn lock.
- **Ammo & magazines** with reload (R), plus a "hit combo" that rewards a magazine.
- **Combat log** on screen: who was hit, damage, type, cause and instigator.
- **Enhanced Input** with two mapping contexts: `IMC_Default` (movement, health) and `IMC_Weapons` (shooting, energy, damage type).

## How to try it

Open `Project/MultiplayerFPS.uproject`, then in the Play options set **Number of Players = 2** and
**Net Mode = Play As Listen Server**.

## Where my work is

- `Content/FirstPerson/Blueprints/` — character, projectiles, rifle, damage types, player controller, HUD widget
- `Content/FirstPerson/Input/` — input actions and mapping contexts
- `Content/Poses/` — aim poses and the aim offset
- `Content/Destructive/`, `Content/Destructive_Greometry/` — destructible geometry collections

Built on the First Person template. `AnimStarterPack`, `FPWeapon` and `FirstPersonArms` are Epic / Fab content.
