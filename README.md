# AltRebellion

AltRebellion is an Unreal Engine 5.6 C++ game project. The codebase contains core gameplay systems, character and AI logic, abilities, loot, UI, and prototype gameplay variants for twin-stick and strategy modes.

## Project Structure

- `Source/AltRebellion/Core` - game instance, game mode, player controller, HUD manager, and save game classes.
- `Source/AltRebellion/Characters` - base character, enemy, boss, AI controller, and character data assets.
- `Source/AltRebellion/Components` - reusable gameplay components for health, weapons, and abilities.
- `Source/AltRebellion/Abilities` - player and character abilities such as dash, burst, overheat, focus, traps, and barrage attacks.
- `Source/AltRebellion/AI` - behavior tree tasks and services.
- `Source/AltRebellion/Loot` - loot drop logic.
- `Source/AltRebellion/Variant_TwinStick` - twin-stick game mode, character, controller, projectiles, AI, and UI.
- `Source/AltRebellion/Variant_Strategy` - strategy game mode, pawn, units, controller, HUD, and UI.
- `Config` - Unreal project configuration.
- `Content` - Unreal assets, maps, blueprints, and editor content.
- `DataImports` - CSV data for gameplay tuning.

## Requirements

- Unreal Engine 5.6
- C++ toolchain supported by Unreal Engine on your platform
- Enabled project plugins:
  - Modeling Tools Editor Mode
  - StateTree
  - GameplayStateTree

## Getting Started

1. Open `AltRebellion.uproject` in Unreal Engine 5.6.
2. Let Unreal generate or refresh project files if prompted.
3. Build the `AltRebellionEditor` target.
4. Open the default map:
   `/Game/AltRebellion/Maps/MainMenu/L_MainMenu`
5. Press Play in the editor.

## Development Notes

- Main runtime module: `AltRebellion`.
- Default game mode is configured as `BP_AR_GameMode`.
- Input uses Unreal Enhanced Input.
- Gameplay systems use modules including `AIModule`, `NavigationSystem`, `StateTreeModule`, `GameplayTasks`, `GameplayStateTreeModule`, `Niagara`, `UMG`, and `Slate`.

