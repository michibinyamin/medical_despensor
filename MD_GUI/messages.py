# messages.py
from types import SimpleNamespace

_MESSAGES = {
    # Welcome / Name
    "welcome_title": "Hello! I'm your medication assistant robot",
    "start_btn": "Next",
    "name_title": "Please write your full name\nor insert ID card",
    "name_placeholder": "Full name",
    "name_next": "Next",
    "back_main": "Back to main",
    "json_mode": "JSON mode",

    # SelectMed
    "select_med_title": "Which medication do you need?",

    "summary_button": "Next",

    # Quantity
    "quantity_title": "How many units do you need?",
    "ok_button": "Next",
    "minus_button": "-",
    "plus_button": "+",

    # AddMore
    "add_more_title": "Would you like another medication?",
    "add_more_yes": "Yes",
    "add_more_no": "No",

    # Summary
    "summary_title": "Summary of dispensed medications:",
    "send_btn": "Next",
    "cancel_btn": "Back to main",

    # Goodbye
    "goodbye_title": "Goodbye and thank you\nfor using our service ☺",

    # Warnings / info
    "empty_warning_title": "Empty",
    "empty_warning_body": "Please select at least one medication.",
    "timeout_message": "Session reset due to inactivity",
}

MESSAGES = SimpleNamespace(**_MESSAGES)
