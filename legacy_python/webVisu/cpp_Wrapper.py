# ext_wrapper.py
from flask_app import create_app
from flask import current_app
import threading

# Wir müssen einen App-Context haben, bevor wir current_app nutzen.
_app = create_app()
_app_ctx = _app.app_context()
_app_ctx.push()

def set_image(image_bytes):
    """
    Wird aus C++ aufgerufen. Übergibt rohe PNG/JPEG-Bytes an den Manager.
    """
    current_app.img_manager.set_image_bytes(image_bytes)

def get_status():
    return current_app.img_manager.get_status()
