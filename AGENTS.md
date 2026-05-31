# Repository Guidelines

## Project Structure & Module Organization
The repository mixes weekly OOP exercises and a larger course project. Root folders such as `week1/`, `week2/`, `week3/`, `week7/`, `week8/`, and `week9/` contain weekly submissions; `week3/` is split into `header/` and `source/`. The main project lives in `doancaro/`: gameplay code is in `src/`, Catch2 tests are in `tests/`, runtime assets are in `assets/`, and supporting materials live in `report/`, `slides/`, and `storyline/`. Reference material and extracted notes live under `books/` and `extracted_content/`. Treat `build/`, `cmake-build-*`, `doancaro/slides/dist/`, and `node_modules/` as generated output.

## Project Facts
- Caro final-project group number: `13`

## Build, Test, and Development Commands
Use CMake from the repository root:

- `cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug` configures all weekly targets plus `doancaro`.
- `cmake --build cmake-build-debug --target week3 CaroGame CaroTests` builds a weekly exercise, the game, and the test binary.
- `./cmake-build-debug/doancaro/CaroGame` runs the game with copied assets from the build tree.
- `./cmake-build-debug/doancaro/CaroTests` runs the Catch2 logic suite for `Board` and `AIPlayer`.

For the Slidev deck:

- `cd doancaro/slides && bun install` installs presentation dependencies.
- `bun run dev` starts the local slide editor.
- `bun run build` creates the production deck in `dist/`.

## Coding Style & Naming Conventions
Use C++14, 4-space indentation, and K&R brace style (`int main() { ... }`) to match the existing code. Keep header/source pairs together (`Board.h` and `Board.cpp`). In `doancaro/src`, use PascalCase for classes and filenames, camelCase for methods, and descriptive enum names such as `CellState::PlayerX`. For slides, keep Vue components in PascalCase, for example `CoverSlide.vue`.

## Art Direction Rules
For any new image generation, background art, illustration replacement, or raster UI asset in `doancaro`, read the existing visual-system docs first, especially `CLAUDE.md`, `doancaro/src/Theme.{h,cpp}`, and `doancaro/storyline/story_image_prompt_pack.md`. Do not generate art from a generic fantasy prompt.

The required house style is the repo's `Wuxia Storm` system:

- Bright `wuxia painting cartoon`, not dark mythic realism
- 2D hand-painted look with ink linework plus watercolor / gouache wash
- Child-friendly mythic adventure tone
- Vietnamese folklore atmosphere and setting cues, not generic ancient-China fantasy
- Readable shapes, flatter painted values, soft paper texture
- No photorealism, no glossy anime, no horror, no gore, no poster/glamour composition

Color and motif direction must stay anchored to the existing theme tokens:

- Sơn Tinh side: earth, roots, stone, pine, warm gold-brown and jade accents
- Thủy Tinh side: turquoise, indigo, foam-white, storm-water motion
- Shared UI/art palette: `son_jade`, `son_bone`, `thuy_cyan`, `thuy_pearl`, `ink_sumi`, `gold_foil`

For menu or screen backgrounds, do not reuse arbitrary frames or unrelated wallpaper as placeholders when the request asks for real artwork. Generate or source a distinct composition per view with intentional negative space for the UI and explicit alignment to the `Wuxia Storm` style above.

## Testing Guidelines
Game tests live in `doancaro/tests/test_*.cpp`. When changing board logic, AI, or serialization, add or update focused Catch2 cases and reuse the current tag pattern such as `[board]`, `[win]`, and `[ai]`. No coverage gate is configured, so every logic change should at minimum rebuild `CaroTests` and run the binary locally.

## Commit & Pull Request Guidelines
Recent history uses short, imperative commit subjects with optional scope prefixes, for example `Story Mode: ...`, `Add ...`, or `Expand ...`. Keep each commit limited to one concern and avoid mixing weekly exercise changes with `doancaro` or slide work. For pull requests, summarize affected directories, list the commands you ran, and attach screenshots or GIFs for game UI and slide changes.
