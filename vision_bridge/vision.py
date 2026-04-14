import cv2
import socket
import threading
import requests  # New: for ESP32 communication
from flask import Flask, jsonify, render_template
from ultralytics import YOLO

# --- CONFIGURATION ---
ESP32_IP = "http://192.168.1.XX"  # REPLACE WITH YOUR ESP32 IP FROM SERIAL MONITOR
CPP_BRAIN_ADDRESS = ('127.0.0.1', 8080)

app = Flask(__name__, template_folder='.')
model = YOLO('yolov8n.pt') 

current_count = 0
current_decision = "WAITING"

def update_hardware(decision):
    """ Sends the decision to the ESP32-S3 via Wi-Fi """
    try:
        # We send a simple GET request to the ESP32
        endpoint = "/ECO" if decision == "ECO-MODE" else "/NORMAL"
        requests.get(f"{ESP32_IP}{endpoint}", timeout=0.2)
    except:
        # If ESP32 is unplugged, we don't want the whole program to crash
        pass

def talk_to_brain(people_found):
    global current_decision
    try:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.settimeout(1)
        client.connect(CPP_BRAIN_ADDRESS)
        
        client.sendall(str(people_found).encode())
        response = client.recv(1024).decode()
        
        # Map decision and update hardware
        new_decision = "ECO-MODE" if response == "1" else "NORMAL MODE"
        
        if new_decision != current_decision:
            current_decision = new_decision
            # Only call the ESP32 if the decision actually changes to save bandwidth
            threading.Thread(target=update_hardware, args=(current_decision,)).start()
            
        client.close()
    except:
        current_decision = "BRAIN OFFLINE"

def vision_engine():
    global current_count
    cap = cv2.VideoCapture(0)
    while True:
        success, frame = cap.read()
        if success:
            results = model(frame, classes=[0], verbose=False)
            current_count = len(results[0].boxes)
            talk_to_brain(current_count)
            
            # Show vision for demo
            cv2.imshow("Vision Feed", results[0].plot())
            if cv2.waitKey(1) & 0xFF == ord('q'): break

@app.route('/')
def index(): return render_template('index.html')

@app.route('/data')
def get_data(): return jsonify(people=current_count, decision=current_decision)

if __name__ == '__main__':
    threading.Thread(target=vision_engine, daemon=True).start()
    app.run(host='0.0.0.0', port=5000, debug=False)
