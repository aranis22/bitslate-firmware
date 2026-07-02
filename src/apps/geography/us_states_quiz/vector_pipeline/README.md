# U.S. States Vector Pipeline

The PNG/raster border extraction attempt was removed because it produced specks,
broken boundaries, and noisy fragments after downsampling to the 168x108 quiz
grid.

The next approach is vector-first:

- Primary source: U.S. Census Cartographic Boundary Files, States, 1:20m.
- Fallback source: Natural Earth Admin-1 States/Provinces or boundary lines.
- Initial target: contiguous U.S. state borders rendered into a clean 390x250
  preview.
- Later targets: clean state border cells, full state masks, and compact C++
  arrays for the LVGL quiz app.

The scripts in this folder are staged utilities. They should write downloaded
or extracted source data into `data/`, which is intentionally ignored by git,
and should not overwrite LVGL generated data until the vector preview is
visually confirmed.
