import customtkinter as ctk
from config import PILLS
from style import FONT_NORMAL, FONT_SUBTITLE, MAIN_GREEN


def render_order(
    parent: ctk.CTkFrame,
    order: dict[str, int],
    show_cmd: bool = False,
    cmd: str = "",
    allow_delete: bool = False,
    on_delete: callable = None
):
    """
    Отображает список заказа с опциями:
    - show_cmd: показать json/serial строку
    - allow_delete: показать кнопку удаления рядом с каждой строкой
    - on_delete(code): коллбек при нажатии на удаление
    """

    for w in parent.winfo_children():
        w.destroy()

    total = 0.0

    for i, (code, qty) in enumerate(order.items(), 1):
        name = PILLS[code]["name"]
        price = PILLS[code].get("price", 0)
        subtotal = price * qty
        total += subtotal

        row = i - 1

        ctk.CTkLabel(
            parent,
            text=f"{i}. {name}",
            font=FONT_NORMAL,
            anchor="w",
            width=200
        ).grid(row=row, column=0, sticky="w")

        ctk.CTkLabel(
            parent,
            text=f"×{qty}",
            font=FONT_NORMAL,
            width=40
        ).grid(row=row, column=1)

        ctk.CTkLabel(
            parent,
            text=f"{subtotal:.2f} ₪",
            font=FONT_NORMAL,
            width=80
        ).grid(row=row, column=2)

        if allow_delete and on_delete:
            ctk.CTkButton(
                parent,
                text="✕",
                width=28, height=28,
                fg_color="#911",
                hover_color="#c33",
                font=("Arial", 16),
                command=lambda c=code: on_delete(c)
            ).grid(row=row, column=3, padx=(6, 0))

    # Итого
    if total > 0:
        ctk.CTkLabel(
            parent,
            text=f"{total:.2f} ₪",
            font=FONT_SUBTITLE,
            text_color=MAIN_GREEN
        ).grid(row=i, column=0, columnspan=2, pady=10)

    if show_cmd:
        ctk.CTkLabel(
            parent,
            text=cmd,
            font=FONT_MONO,
            anchor="center"
        ).grid(row=i + 1, column=0, columnspan=4, pady=6)
