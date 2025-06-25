import customtkinter as ctk
from config import PILLS
from messages import MESSAGES
from style import FONT_SUBTITLE, FONT_NORMAL, BTN_WIDTH, BTN_HEIGHT, MAIN_GREEN, HOVER_GREEN, centered_container

class Quantity(ctk.CTkFrame):


    def __init__(self, parent, app):
        super().__init__(parent, fg_color="transparent")
        self.app = app
        self.qty = ctk.IntVar(value=1)

        center = centered_container(self, row_weights=(0, 1, 2, 3))

        self.label = ctk.CTkLabel(
            center,
            text="",
            font=FONT_SUBTITLE,
            anchor="center"
        )
        self.label.grid(row=0, column=0, pady=(0, 16), sticky="s")

        counter = ctk.CTkFrame(center, fg_color="transparent")
        counter.grid(row=1, column=0)

        counter.grid_columnconfigure((0, 1, 2), weight=1)

        ctk.CTkButton(
            counter,
            text=MESSAGES.minus_button,
            width=60,
            height=60,
            font=FONT_NORMAL,
            fg_color="#333",
            hover_color="#555",
            command=lambda: self._inc(-1)
        ).grid(row=0, column=0, padx=16)

        self.qty_label = ctk.CTkLabel(
            counter,
            textvariable=self.qty,
            font=("Arial", 22, "bold"),
            width=60,
        )
        self.qty_label.grid(row=0, column=1)

        ctk.CTkButton(
            counter,
            text=MESSAGES.plus_button,
            width=60, height=60,
            font=FONT_NORMAL,
            fg_color="#444",
            hover_color="#666",
            command=lambda: self._inc(+1)
        ).grid(row=0, column=2, padx=16)

        ctk.CTkButton(
            center,
            text=MESSAGES.ok_button,
            width=BTN_WIDTH, height=BTN_HEIGHT,
            font=FONT_NORMAL,
            fg_color=MAIN_GREEN,
            hover_color=HOVER_GREEN,
            command=self._confirm
        ).grid(row=2, column=0, padx=24)

    def tkraise(self, *args, **kwargs):
        super().tkraise(*args, **kwargs)
        pill_code = self.app.current_pill
        self.label.configure(text=f"{MESSAGES.quantity_title}\n{PILLS[pill_code]['name']}")
        self.qty.set(1)

    def _inc(self, delta):
        val = max(1, self.qty.get() + delta)
        self.qty.set(val)

    def _confirm(self):
        self.app.add_item(self.app.current_pill, self.qty.get())
        self.app.show("AddMore")
