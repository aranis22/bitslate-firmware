# LVGL Vector-Generated Map Plan

The embedded app will not draw Census vector polygons live and will not parse
shapefiles on-device. It will also avoid PNG state-map assets.

Pipeline:

1. Census Cartographic Boundary shapefile on desktop.
2. Python vector rasterizer/exporter.
3. Compact generated C++ grid data.
4. Portable C++ quiz core.
5. LVGL renders from generated arrays.

Memory-safe LVGL rendering:

- Logical grid: about `240x150`.
- Screen render: about `390x250`.
- RGB565 image buffer: `390 * 250 * 2 = 195 KB`, allocated in PSRAM.
- Use one LVGL image object for the rendered map.
- Do not create one LVGL object per state, cell, or border.

Draw order:

1. Fill the render buffer with the light background.
2. Draw `US_STATE_ID_GRID` cells as dark base fill.
3. Draw completed state IDs in their quiz colors.
4. Draw `US_BORDER_CELLS` in white.
5. Flush/invalidate the single LVGL image object.

Touch conversion:

```cpp
gridX = localX * US_GRID_W / MAP_W;
gridY = localY * US_GRID_H / MAP_H;
stateId = US_STATE_ID_GRID[gridY * US_GRID_W + gridX];
```

The portable quiz core owns prompt order, completed-state tracking, and tap
results. LVGL only maps touch coordinates to grid cells and redraws the image
buffer.

Later compression options:

- RLE rows for `US_STATE_ID_GRID`.
- Per-state cell spans.
- Border-cell RLE.
- Keep `uint8_t` IDs for raw grids because 4-bit state IDs cannot represent 48
  states.
