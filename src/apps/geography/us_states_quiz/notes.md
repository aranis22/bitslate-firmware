# American States Quiz Notes

## Layout

- Window: `480x320`
- Map area: scaled from the reference USA bitmap into a centered play area
- Prompt/status: compact text above the map

## Reference images

Uses the existing reference images in:

`src/apps/assets/images/geography/`

Files used:

- `usa-map-unselected.png` for the visible map
- `usa-map-full.png` as a visual reference only

## Region strategy

The old overlay-polygon-on-top-of-PNG direction is now paused.

Current direction:

- use the reference PNG only as source material
- reconstruct a standalone BitSlate-style USA map as generated data
- render generated cell/polygon data directly in pygame
- avoid shipping the visible PNG as the actual quiz surface
- derive per-state grid-cell masks from the colored reference map
- use generated state masks for hit detection instead of guessed regions or manual polygon tracing

## Next steps

## Generated mask checkpoint

- Current generated mask set is intentionally limited to six states:
  - California
  - Texas
  - Washington
  - South Dakota
  - Alaska
  - Missouri
- Boundary-constrained color flood fill remains the active extraction method.
- Manual polygon tracing remains paused.
- Final cleanup pass adjusted only Alaska, Washington, and Texas:
  - Alaska uses `min_hits=1`, seed `(16, 88)`, and cleanup `(4, 2)` to keep one filled connected Alaska inset region.
  - Washington uses seed `(15, 6)`, default `min_hits=2`, and cleanup `(6, 2)` to smooth the mask while keeping it inside Washington.
  - Texas uses `min_hits=3`, seed `(70, 83)`, and cleanup `(5, 2)` to reduce northwest/top edge leakage and jagged artifacts.
- California, South Dakota, and Missouri were left at the default generated-mask settings.
- Preview controls: `O` toggles the colored source overlay, `B` toggles borders/labels, `D` toggles debug text, and Esc exits.

## Next steps

1. build the next desktop quiz pass on top of generated masks
2. port the compact grid-cell state model to portable C++
3. build the LVGL BitSlate version later
