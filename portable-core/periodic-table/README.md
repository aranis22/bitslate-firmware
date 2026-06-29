# Periodic Table Portable Core

This folder contains the portable C++ core for the BitSlate Periodic Table app.

The core is independent of Arduino, LVGL, and pygame. It stores a static table of 118 elements and supports selection by atomic number or periodic-grid position.

## Model

- Static `PeriodicElement` array with 118 entries
- Default selected element: Carbon
- Lookup by atomic number
- Lookup by grid column / row
- Selection by atomic number
- Selection by grid column / row

## Desktop Test

```powershell
g++ -std=c++17 portable-core/periodic-table/PeriodicTableModel.cpp portable-core/periodic-table/test_periodic_table_model.cpp -o portable-core/periodic-table/test_periodic_table_model.exe
portable-core/periodic-table/test_periodic_table_model.exe
```
