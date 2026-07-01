# American States Quiz

This folder holds the first BitSlate Geography app prototype: `American States Quiz`.

Current step:

- Python desktop prototype only
- Located under `src/apps/geography/us_states_quiz/`
- Uses the reference USA map images in `src/apps/assets/images/geography/`

Prototype goals:

- 480x320 pygame window
- USA map scaled to fit the BitSlate screen
- Prompt such as `Tap Texas`
- Click/tap state regions on the map
- Correct state gets highlighted and the quiz advances
- Wrong state shows feedback and does not advance

Implemented in v0:

- 15-state working subset
- Polygon-based state regions
- Deterministic prompt order
- Correct-state highlighting
- Prompt/status UI
- Console logging of clicked states for debugging

Not implemented yet:

- All 50 states
- Perfect state borders
- Score history
- Difficulty modes
- Embedded LVGL app
- Portable C++ state-region core

Run locally:

```powershell
python src/apps/geography/us_states_quiz/python/test_us_states_model.py
python -m py_compile src/apps/geography/us_states_quiz/python/main.py src/apps/geography/us_states_quiz/python/us_states_model.py src/apps/geography/us_states_quiz/python/test_us_states_model.py
python src/apps/geography/us_states_quiz/python/main.py
```
