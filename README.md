# UE Lab — Unreal Engine 5 study projects

A collection of five Unreal Engine 5 projects I built while studying Game Design & Development
(ESNE and UDIT, 2022–2025). Each one focuses on a different engine system: materials,
networked gameplay, pawn possession and replication, AI with Behavior Trees + EQS, and a
gameplay simulation written in C++.

<!-- MEDIA: add a short GIF of each project here (e.g. Media/factory.gif) — recruiters look at this first. -->

| # | Project | What it shows | Tech | Engine |
|---|---------|---------------|------|--------|
| 01 | [Materials](01_Materials/) | Material graph authoring: animated emissives, panners, distance-based colour, fresnel, master materials + instances, reverse-engineered materials | Material Editor | 5.4 |
| 02 | [Multiplayer FPS](02_Multiplayer/) | Listen-server FPS: server/multicast RPCs, replicated aim pitch, two weapon modes, damage types with immunity, Chaos destruction, Enhanced Input | Blueprints | 5.6 |
| 03 | [Pawn Change](03_Pawn-Change/) | Multiplayer pawn swapping, tag-based spawning from the GameMode, random colours replicated with RepNotify | Blueprints | 5.4 |
| 04 | [Farming EQS](04_Environment-Query-System/) | Autonomous NPCs that find, collect and deliver resources using Behavior Trees, Blackboard, EQS queries and custom contexts/tasks | Blueprints, AI | 5.4 |
| 05 | [Factory](05_Factory/) | Real-time factory simulation: actors + dynamic multicast delegates, thread-pool tasks marshalled back to the game thread, UMG HUD driven from C++ | **C++**, UMG | 5.4 |

## Skills covered

- **Gameplay framework**: GameMode, PlayerController, Pawn/Character, possession, spawning.
- **Networking**: replicated variables, RepNotify, Run on Server / Multicast / Owning Client events, what each client sees (1st-person arms vs 3rd-person body).
- **AI**: Behavior Trees, Blackboards, EQS queries, contexts and custom tasks, EQS debugging.
- **C++**: `UCLASS`/`UPROPERTY`/`UFUNCTION` exposed to Blueprints, dynamic multicast delegates, timers, `Async` + `AsyncTask` to the game thread, weak object pointers, UMG widgets in C++, custom log category.
- **Rendering / tech art**: material graphs, material instances and parameters.
- **Input & UI**: Enhanced Input (mapping contexts + actions), UMG HUDs.
- **Physics**: Chaos destruction (geometry collections + fields), radial damage and impulses.

## Repository layout

```
NN_ProjectName/
├── Project/                 # The Unreal project (open the .uproject)
├── Documentation_ES.pdf     # Original technical report (Spanish)
└── README.md                # Summary in English
```

## Opening the projects

1. Install the engine version listed in the table (Epic Games Launcher).
2. Open `NN_ProjectName/Project/<Name>.uproject`.
3. **Factory (C++)**: right-click `factory.uproject` → *Generate Visual Studio project files*, then build
   the `factoryEditor` target (Development Editor) or just open the `.uproject` and let the editor compile it.

The repository is large (several GB) because it contains the full `Content/` folders. To download a
single project without cloning everything you can use a sparse checkout:

```bash
git clone --filter=blob:none --sparse https://github.com/miguelrg14/UE_Lab.git
cd UE_Lab
git sparse-checkout set 05_Factory
```

## Third-party content

These projects use Epic Games template content (First Person, Third Person and Top Down templates,
Starter Content, mannequins) and free packs from Fab (e.g. *Animation Starter Pack*, *Spaceship
Interior* environment). That content belongs to its authors and is subject to their licences; only
the original work in this repository is covered by the [LICENSE](LICENSE). Each project README
lists which folders are mine.

## Author

**Miguel Rodríguez Gallego** — Game developer (Unity / C#, Unreal Engine / C++), Vigo, Spain.
<!-- Add links: portfolio, LinkedIn, itch.io -->
