from elements_data import ELEMENTS, UNKNOWN


class PeriodicTableModel:
    def __init__(self):
        self._elements = list(ELEMENTS)
        self._by_atomic_number = {
            element["atomic_number"]: element for element in self._elements
        }
        self._by_grid_position = {
            (element["grid_col"], element["grid_row"]): element
            for element in self._elements
        }
        self._selected_atomic_number = 6

    def all_elements(self):
        return list(self._elements)

    def get_element_by_atomic_number(self, atomic_number):
        return self._by_atomic_number.get(atomic_number)

    def get_element_by_grid_position(self, grid_col, grid_row):
        return self._by_grid_position.get((grid_col, grid_row))

    def select_element(self, atomic_number):
        if atomic_number in self._by_atomic_number:
            self._selected_atomic_number = atomic_number
            return True
        return False

    def selected_element(self):
        return self._by_atomic_number.get(self._selected_atomic_number)

    def format_value(self, value):
        if value is None or value == "":
            return UNKNOWN
        return str(value)

