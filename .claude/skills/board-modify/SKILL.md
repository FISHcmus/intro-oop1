# Skill: board-modify

Modify 3D board model textures (GLB/glTF) to change grid patterns, dimensions, or visual appearance without re-modeling. Applies to any board game using baked grid textures.

## Prerequisites

- `npx @gltf-transform/cli` (Node.js) — GLB/glTF inspection, extraction, repacking
- Python 3 with `Pillow` (PIL) and `numpy` — texture analysis and generation

## Workflow

### 1. Extract GLB to editable files

```bash
mkdir -p /tmp/extracted
npx @gltf-transform/cli inspect model.glb          # see textures, meshes, materials
npx @gltf-transform/cli copy model.glb /tmp/extracted/model.gltf  # extract to directory
ls /tmp/extracted/  # baseColor_1.jpg, normal_1.png, model.bin, model.gltf, etc.
```

The base color texture (e.g., `baseColor_1.jpg`) contains the baked grid lines.

### 2. Analyze existing grid line positions

```python
from PIL import Image
import numpy as np

img = Image.open('/tmp/extracted/baseColor_1.jpg')
pixels = np.array(img)

# Scan for dark pixels (grid lines) in the grid region
region = pixels[:600, :500]  # adjust to where grid is visible
gray = np.mean(region, axis=2)

dark_threshold = 100  # grid lines are ~40-80, wood is ~150-200
dark_mask = gray < dark_threshold

# Count dark pixels per row/column to find line positions
row_dark = np.sum(dark_mask, axis=1)
col_dark = np.sum(dark_mask, axis=0)

# Group consecutive dark rows/columns (each line is 1-3 pixels wide)
def group_consecutive(arr, threshold=50, gap=5):
    indices = np.where(arr > threshold)[0]
    if len(indices) == 0: return []
    groups, start = [], indices[0]
    for i in range(1, len(indices)):
        if indices[i] - indices[i-1] > gap:
            groups.append((start, indices[i-1]))
            start = indices[i]
    groups.append((start, indices[-1]))
    return groups

h_lines = group_consecutive(row_dark)  # horizontal grid lines
v_lines = group_consecutive(col_dark)  # vertical grid lines
# h_lines and v_lines give pixel positions of each grid line
```

### 3. Erase old grid lines (median filter inpainting)

```python
from PIL import ImageFilter

# Widen bounds slightly to catch border pixels
gx0, gx1 = v_lines[0][0] - 3, v_lines[-1][1] + 3
gy0, gy1 = h_lines[0][0] - 3, h_lines[-1][1] + 3

arr = np.array(img)
region = arr[gy0:gy1, gx0:gx1]
gray = np.mean(region.astype(np.float32), axis=2)
dark_mask = gray < 130

# Median filter removes thin lines while preserving wood grain
blurred = Image.fromarray(region).filter(ImageFilter.MedianFilter(size=7))
blurred_arr = np.array(blurred)

# Replace only dark pixels with filtered version
region[dark_mask] = blurred_arr[dark_mask]
arr[gy0:gy1, gx0:gx1] = region

# Second pass for star points (larger dark spots)
gray2 = np.mean(arr[gy0:gy1, gx0:gx1].astype(np.float32), axis=2)
dot_mask = gray2 < 140
blurred2 = Image.fromarray(arr[gy0:gy1, gx0:gx1]).filter(ImageFilter.MedianFilter(size=11))
arr[gy0:gy1, gx0:gx1][dot_mask] = np.array(blurred2)[dot_mask]
```

**Key insight:** `MedianFilter(size=7)` erases 1-3 pixel wide lines while keeping the wood texture intact. Use `size=11` for larger features like star point dots. This is much faster than per-pixel neighbor averaging.

### 4. Draw new grid lines

```python
from PIL import ImageDraw

new_img = Image.fromarray(arr)
draw = ImageDraw.Draw(new_img)

grid_color = (40, 30, 20)  # dark brown, matches original
N = 15  # new grid size (N x N)

# Use the same pixel region as the original grid
x_step = (grid_x_end - grid_x_start) / (N - 1)
y_step = (grid_y_end - grid_y_start) / (N - 1)

# Inner lines
for i in range(N):
    y = int(round(grid_y_start + i * y_step))
    draw.line([(grid_x_start, y), (grid_x_end, y)], fill=grid_color, width=2)
for i in range(N):
    x = int(round(grid_x_start + i * x_step))
    draw.line([(x, grid_y_start), (x, grid_y_end)], fill=grid_color, width=2)

# Border (thicker)
for w in range(3):
    draw.rectangle([(grid_x_start - w, grid_y_start - w),
                     (grid_x_end + w, grid_y_end + w)], outline=grid_color)

# Star points (hoshi) — standard positions for NxN
hoshi_r = 4
for (row, col) in hoshi_positions:
    hx = int(round(grid_x_start + col * x_step))
    hy = int(round(grid_y_start + row * y_step))
    draw.ellipse([(hx - hoshi_r, hy - hoshi_r), (hx + hoshi_r, hy + hoshi_r)], fill=grid_color)

new_img.save('/tmp/extracted/baseColor_1.jpg', quality=95)
```

### 5. Repack into GLB

```bash
npx @gltf-transform/cli copy /tmp/extracted/model.gltf output.glb
```

## Standard star point (hoshi) positions

| Board | Positions |
|-------|-----------|
| 9x9   | (2,2), (2,6), (4,4), (6,2), (6,6) |
| 13x13 | (3,3), (3,6), (3,9), (6,3), (6,6), (6,9), (9,3), (9,6), (9,9) |
| 15x15 | (3,3), (3,7), (3,11), (7,3), (7,7), (7,11), (11,3), (11,7), (11,11) |
| 19x19 | (3,3), (3,9), (3,15), (9,3), (9,9), (9,15), (15,3), (15,9), (15,15) |

## Gotchas

- **GLB = binary glTF**: `gltf-transform copy` handles conversion both ways (GLB <-> glTF directory)
- **Texture format matters**: save as JPEG with `quality=95` to match original. PNG textures stay PNG.
- **UV mapping unchanged**: the grid lines must stay in the same UV region as the original. Don't move the grid area — only change line count/spacing within it.
- **Median filter size**: use odd numbers. Size 7 erases lines up to ~3px wide. Increase for thicker lines.
- **Model geometry stays the same**: only the texture changes. The mesh, normals, and other maps (normal, metallic-roughness, emissive) are untouched.
- **Scaling in renderer**: the code `scale = (Board::SIZE - 1) / MODEL_GRID_EXTENT` automatically handles different board sizes as long as the texture grid fills the same UV region.

## Tools

| Tool | Purpose |
|------|---------|
| `gltf-transform inspect` | See model structure (meshes, textures, materials) |
| `gltf-transform copy` | Extract GLB → glTF dir, or repack glTF dir → GLB |
| `gltf-transform prune` | Remove unused data from GLB |
| PIL `MedianFilter` | Erase thin lines from textures (fast inpainting) |
| PIL `ImageDraw` | Draw new grid lines, star points, borders |
| numpy | Fast pixel analysis (find dark pixels, compute thresholds) |
