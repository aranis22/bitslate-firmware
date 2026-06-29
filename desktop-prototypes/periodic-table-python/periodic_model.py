class PeriodicTableModel:
    """Placeholder model for the Periodic Table desktop prototype."""

    def __init__(self):
        self.selected_atomic_number = None

    def select_element(self, atomic_number):
        # TODO: Look up an element record by atomic number.
        self.selected_atomic_number = atomic_number

    def selected_element(self):
        # TODO: Return the selected element record.
        return None

    def format_value(self, value):
        # TODO: Use this for missing/unknown values in the info panel.
        return "-" if value is None else str(value)

