# World Map LVGL Port Plan

The ESP32 port should use the generated `WorldMapData` arrays and the portable `ZoomableWorldMapCore` renderer. There should be no shapefile parser and no PNG world map on-device.

LVGL can own one fixed RGB565 viewport buffer in PSRAM, sized initially around `430x240` or `390x230`. On zoom, pan, reset, or toggle changes, call `ZoomableWorldMapCore::renderMapRgb565()` into that buffer and invalidate the image object.

Capital dots, country labels, and ocean labels should be LVGL overlay objects positioned from `gridToScreen()` after each camera change. Country labels become visible at `zoom >= 1.6`, capital labels at `zoom >= 2.2`, and ocean labels are visible when their projected point is inside the viewport.

Initial controls can be `+`, `-`, reset, grid toggle, and labels toggle. These controls should update the core camera/options first, then refresh the RGB565 image and overlay objects.
