# Choco Survivor
A game made by Team Choco Latte. This is a game project for DigiPen subject CSD1401.

Team members:
1. Zong Han (zonghan.law@digipen.edu)
2. Fadzly (muhammadnurfadzly.b@digipen.edu)
3. Dalton (d.koh@digipen.edu)

## Codebase Guidelines
---

### Naming Conventions
- **Variables**:
 - Descriptive, meaningful names. Comment if names may lead to confusion.
 - `camelCase`:
    - Global variables (if static, prefix with a `_` e.g., `_camelCase`)
    - Local variables (flexible, as long as it is not conflicting with other variable names)
    - Parameters (same as local variables)
 - `snake_case`:
    - Parameters (same, read above)
 - `PascalCase`:
    - Structs (including `typedef`)
    - Enums

- **Functions**:
  - Use `snake_case` (e.g., `calculate_sum`, `update_config`).
  - Start function names with verbs to indicate action (e.g., `process_data`, `read_file`).
  - Unless it's for debugging, prefix with `debug_` (e.g., `debug_render_graphic`).

- **Constants**:
  - Use all uppercase with underscores for constants and macros (e.g., `MAX_BUFFER_SIZE`).

- **File Names**:
  - Use lowercase with no whitespace and underscore (e.g., `mainmenu.c`)

---