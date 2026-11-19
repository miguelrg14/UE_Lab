# Automated Factory Resource Management Simulator — Report

> English translation of the original technical report (Spanish) for *Technologies III*, academic year 2024/2025 (UDIT).
> Asset names are those of the original submission; the project README lists the current ones.
> Author: Miguel Rodríguez Gallego. Original engine: Unreal Engine 5.4 / C++.
>
> The code has since been reviewed (thread safety, GC-safe references, a single update path, unit tests) and
> upgraded to Unreal Engine 5.8. Those changes are described in the [project README](../README.md); this
> document keeps the original design.

## 1. Introduction

This project simulates an automated factory in real time with three independent production lines. Each line updates its critical parameters on background threads and reports its state to the Central Manager through dynamic multicast delegates. When an emergency happens, an interactive minigame lets the user step in.

## 2. Project architecture

Main components:

- **AProductionLine** (Actor): one production line (A, B, C).
- **ACentralManager** (Actor): subscribes to the lines' events and coordinates the notifications sent to the UI.
- **UEmergencyMiniGame** (UUserWidget): interface that shows alerts and launches the minigame.
- **UUIFactoryHUD** (UUserWidget): main panel that displays the state of the lines.

## 3. Data flow

1. **Simulation start**
   - Every AProductionLine and the ACentralManager start their update loop in `BeginPlay`.
   - An asynchronous task runs every second to change the Efficiency and Resources values randomly.
2. **State evaluation**
   - On every iteration the line computes its new state (Operational, Warning, Critical) and compares it with the previous one.
   - If it changed, the main thread is notified through `Async` in `UpdateData()`.
3. **Event broadcast**
   - The state-changed delegate is broadcast and received by ACentralManager.
4. **Manager reaction**
   - The Central Manager updates the UI in real time.
   - If the new state is Critical, it shows the emergency widget and launches the minigame.
5. **Emergency minigame**
   - The user has to press a button in less than 10 seconds.
   - On success, the line is reset to safe values.
   - Otherwise the button disappears and the line can no longer be fixed, as if a system failure had been logged.

## 4. Module implementation

### 4.1. AProductionLine

- Simulates one production line.
- Parameters: Efficiency and Resources (0 to 100), and an identifier.
- State logic: Operational ≥ 75, Warning 50–74, Critical < 50.
- Multithreading: `Async` for the calculations, `AsyncTask` to talk back to the Game Thread.
- Events: a state-changed delegate notifies every change.

### 4.2. ACentralManager

- Actor placed in the level.
- Subscribes to every line's delegates and reacts to state changes.
- On Critical: shows the `EmergencyWarning` widget, launches the minigame and, if the player fails, logs the incident.

### 4.3. User interface (widgets)

**UUIFactoryHUD**

- Shows efficiency and resource bars and a state indicator (green / yellow / red).
- Updates automatically through the manager's events.

**UEmergencyMiniGame**

- Pop-up panel with instructions for the user and a reset button.
- 10-second timer.
- Resets the line to safe values if the button is pressed in time.

## 5. Techniques used

**Dynamic multicast delegates**

- Declared with `DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(...)`.
- Notify multiple listeners in both Blueprint and C++.

**Multithreading**

- `Async(EAsyncExecution::ThreadPool, ...)` for the parallel simulation.
- `AsyncTask(ENamedThreads::GameThread, ...)` to update the UI on the main thread.

**Timers**

- `FTimerHandle` with `GetWorld()->GetTimerManager()` for the UI and minigame timers.

## 6. Logging and debugging

- Events are shown with `GEngine->AddOnScreenDebugMessage`, which makes it easy to follow changes in real time during development.
- A persistent log could be added for the final evaluation (the reviewed version logs to the `LogFactory` category).

## 7. Evaluation and deliverables

Deliverables:

1. Complete C++ source code.
2. Technical report (this document).
3. A 5-minute presentation video showing the code, the Blueprints, the UI and the minigame, explained live by the author.

Evaluation criteria:

- Correct use of delegates and events.
- Multithreading implementation.
- Working UI updated in real time.
- Logical activation of the minigame.
- Quality and clarity of the code and documentation.

## 8. Summary

The simulator shows how Unreal Engine and C++ can model complex production environments with communication between systems, real-time visual feedback and multithreaded simulation. Events, reactive interfaces and parallel processes meet the goals of the assignment and give a complete, scalable base for simulated industrial environments.
