import customtkinter as ctk
from typing import Sequence

ctk.set_appearance_mode("system")
ctk.set_default_color_theme("green")

WINDOW_SIZE = (750, 450)
MAIN_GREEN = "#35af4c"
HOVER_GREEN = "#56cf5b"
TEXT_GREEN = "#d0ffb2"
DEF_TEXT_COLOR = "#FFFFFF"
MAIN_GREY = "#222",
HOVER_GREY = "#444",
FONT_TITLE = ("Arial", 26, "bold")
FONT_SUBTITLE = ("Arial", 18)
FONT_NORMAL = ("Arial", 16)
FONT_MEDIUM = ("Arial", 12)
BTN_WIDTH = 180
BTN_HEIGHT = 48

def centered_container(
    frame: ctk.CTkFrame,
    row_weights: Sequence[int] = (0, 1),
    col_weights: Sequence[int] = (0,),
    sticky: str = "nsew"
) -> ctk.CTkFrame:
    frame.grid_rowconfigure(0, weight=1)
    frame.grid_columnconfigure(0, weight=1)

    center = ctk.CTkFrame(frame, fg_color="transparent")
    center.grid(row=0, column=0, sticky=sticky)

    # Make internal rows and columns expandable
    for r in row_weights:
        center.grid_rowconfigure(r, weight=1)
    for c in col_weights:
        center.grid_columnconfigure(c, weight=1)

    return center
