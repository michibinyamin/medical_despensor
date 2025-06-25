import json, queue, threading
import customtkinter as ctk

from config import PILLS
from style import WINDOW_SIZE
from messages import MESSAGES
from screens import (
    Welcome, NameInput, SelectMed, Quantity,
    AddMore, Summary, Goodbye
)

TIMEOUT_MS = 30_000
GOODBYE_MS = 3_000

class SerialWorker(threading.Thread):
    def __init__(self, tx_q: queue.Queue):
        super().__init__(daemon=True)
        self.tx_q = tx_q
        try:
            import serial
            self.ser = serial.Serial(port="COM3", baudrate=115200, timeout=1)
        except Exception:
            self.ser = None

    def run(self):
        while True:
            cmd = self.tx_q.get()
            print(">>>", cmd)
            if self.ser:
                self.ser.write(cmd.encode() + b"\r\n")

class MDApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Medical Dispenser GUI")
        self.geometry(f"{WINDOW_SIZE[0]}x{WINDOW_SIZE[1]}")
        self.resizable(False, False)

        self.order: dict[str,int] = {}
        self.current_pill = None
        self.json_mode = ctk.BooleanVar(value=False)
        self._timer_id = None
        self.tx_q: queue.Queue[str] = queue.Queue()
        SerialWorker(self.tx_q).start()

        container = ctk.CTkFrame(self, fg_color="transparent")
        container.pack(fill="both", expand=True)

        self.screens = {}
        for Scr in (Welcome, NameInput, SelectMed, Quantity, AddMore, Summary, Goodbye):
            frame = Scr(parent=container, app=self)
            self.screens[Scr.__name__] = frame
            frame.grid(row=0, column=0, sticky="nsew")

        self.show("Welcome")

    def show(self, name: str):
        if self._timer_id:
            self.after_cancel(self._timer_id)
            self._timer_id = None
        goodbye = self.screens.get("Goodbye")
        if hasattr(goodbye, "cancel_timer"):
            goodbye.cancel_timer()
        if name == "NameInput":
            if hasattr(self.screens["NameInput"], "clear"):
                self.screens["NameInput"].clear()
        self.screens[name].tkraise()
        self._reset_timer()

    def add_item(self, code: str, qty: int):
        self.order[code] = self.order.get(code, 0) + qty

    def clear_order(self):
        self.order.clear()

    def build_serial(self) -> str:
        cells = [PILLS[c]["cell"] for c, q in self.order.items() for _ in range(q)]
        return f"get({','.join(cells)})"

    def build_json(self) -> str:
        return json.dumps({"command":"dispense","items":[{"pill":c,"quantity":q} for c,q in self.order.items()]}, separators=(",",":"))

    def send_order(self):
        cmd = self.build_json() if self.json_mode.get() else self.build_serial()
        self.tx_q.put(cmd)
        self.clear_order()
        self.show("Goodbye")

    def _reset_timer(self):
        if self._timer_id:
            self.after_cancel(self._timer_id)
        self._timer_id = self.after(TIMEOUT_MS, lambda: self.show("Welcome"))
