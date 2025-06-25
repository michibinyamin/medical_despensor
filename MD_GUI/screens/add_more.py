import customtkinter as ctk
from config import PILLS
from messages import MESSAGES
from style import (
    FONT_NORMAL, FONT_SUBTITLE,
    BTN_WIDTH, BTN_HEIGHT,
    MAIN_GREEN, HOVER_GREEN, centered_container, MAIN_GREY, HOVER_GREY
)
from screens.components.order_preview import render_order



class AddMore(ctk.CTkFrame):

    def __init__(self, parent, app):
        super().__init__(parent, fg_color="transparent")
        self.app = app

        center = centered_container(self, row_weights=(0, 1, 2, 3))

        ctk.CTkLabel(
            center,
            text=MESSAGES.add_more_title,
            font=FONT_SUBTITLE,
            anchor="center"
        ).grid(row=0, column=0, pady=(0, 8))

        self.box = ctk.CTkFrame(center, fg_color="transparent")
        self.box.grid(row=1, column=0, pady=4)

        btns = ctk.CTkFrame(center, fg_color="transparent")
        btns.grid(row=2, column=0, pady=10)

        ctk.CTkButton(
            btns,
            text=MESSAGES.add_more_yes,
            width=BTN_WIDTH,
            height=BTN_HEIGHT,
            font=FONT_NORMAL,
            fg_color=MAIN_GREEN,
            hover_color=HOVER_GREEN,
            command=lambda: app.show("SelectMed")
        ).grid(row=0, column=1, padx=20)

        ctk.CTkButton(
            btns,
            text=MESSAGES.add_more_no,
            width=BTN_WIDTH,
            height=BTN_HEIGHT,
            font=FONT_NORMAL,
            fg_color="#222",
            hover_color="#444",
            command=lambda: app.show("Summary")
        ).grid(row=0, column=0, padx=20)

        # preview of output (json or serial)
        self.cmd_label = ctk.CTkLabel(center, text="", font=FONT_SUBTITLE)
        self.cmd_label.grid(row=3, column=0, pady=6)


    def tkraise(self, *args, **kwargs):
        super().tkraise(*args, **kwargs)
        cmd = (
            self.app.build_json()
            if self.app.json_mode.get()
            else self.app.build_serial()
        )
        render_order(self.box, self.app.order, show_cmd=True, cmd=cmd)