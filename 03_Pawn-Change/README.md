# 03 · Pawn Change

A multiplayer Blueprint project for the course *Postproducción Digital* (UDIT, 2023/24): players
spawn next to idle AI characters and take control of them by bumping into them.

<!-- MEDIA: GIF of a player switching into an AI character. -->

**Engine:** Unreal Engine 5.4 · **Tech:** Blueprints, replication (RepNotify) · **Report:** [Documentation_ES.pdf](Documentation_ES.pdf) (Spanish)

## Features

- **Tag-based spawning** — `BP_ThirdPersonGameMode` iterates the spawn points in the level, reads their tag and spawns either a player (and possesses it) or an AI. 2 players + 5 AIs; each AI goes to the next free point and a counter prevents over-spawning.
- **Replicated random colours** — every character gets a random colour stored in a **RepNotify** variable. The OnRep function updates a dynamic material instance (created in the construction script), so all clients see the same colours.
- **Pawn swapping** — colliding with an AI character transfers possession to it (`MyPlayerController`). You cannot take a pawn controlled by another player, and a failed attempt resets the possession system so you can keep swapping.

## How to try it

Open `Project/PawnChange.uproject` and play with **Number of Players = 2**, **Net Mode = Play As Listen Server**.

## Where my work is

- `Content/ThirdPerson/Blueprints/` — GameMode, character, player controller, AI Behavior Tree and Blackboard
- `Content/Maps/Scene_01`

Built on the Third Person template (the pawn-swap base came with the exercise; I extended and fixed it).
`Content/Characters/` contains Epic's mannequins.
