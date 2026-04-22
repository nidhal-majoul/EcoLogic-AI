import cv2
import socket
import threading
import requests
from flask import Flask, jsonify, render_template
from ultralytics import YOLO

# --- 1. NETWORK CONFIGURATION ---
# The IP address of your ESP32-S3 on your phone's hotspot
ESP32_IP = "" 
# The local address where your C++ Perceptron server is listening
CPP_ADDRESS = (,) 

# --- 2. INITIALIZATION ---
# Initialize Flask web server
app = Flask(__name__, template_folder='.')
# Load the pre-trained YOLOv8 Nano model (efficient for real-time)
model = YOLO('yolov8n.pt')

# Global variables to store real-time state for the web dashboard
current_count = 0
current_decision = "INITIALIZING"

def update_hardware(decision):
    """
    Communicates with the ESP32 via HTTP GET requests.
    This acts as the 'Actuator' part of the Cyber-Physical system.
    """
    def send_cmd():
        try:
            # Map the AI decision to the specific ESP32 URL endpoints
            endpoint = "/ECO" if decision == "ECO-MODE" else "/NORMAL"
            # We use a short timeout so the program doesn't freeze if the ESP32 disconnects
            requests.get(f"{ESP32_IP}{endpoint}", timeout=0.5)
        except Exception:
            # If the ESP32 is unreachable, we fail silently to keep the video feed smooth
            pass
            
    # Running this in a separate thread prevents the camera feed from stuttering
    threading.Thread(target=send_cmd).start()

def talk_to_brain(count):
    """
    Sends the number of detected people to the C++ Perceptron engine.
    This bridge connects the Python Vision to the C++ Logic.
    """
    global current_decision
    try:
        # Establish a TCP/IP socket connection to the C++ Brain
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(0.2)
        s.connect(CPP_ADDRESS)
        
        # Send the occupancy count as a string
        s.send(str(count).encode())
        
        # Receive the AI decision: '1' for Normal Mode, '0' for Eco Mode
        resp = s.recv(1024).decode()
        new_dec = "NORMAL MODE" if resp == "1" else "ECO-MODE"
        
        # State Machine Logic: Only trigger the hardware if the mode actually changes
        if new_dec != current_decision:
            current_decision = new_dec
            update_hardware(current_decision)
            
        s.close()
    except Exception:
        # If the C++ program isn't running, update the status for the dashboard
        current_decision = "BRAIN OFFLINE"

def vision_loop():
    """
    The main Computer Vision thread. Handles camera input and person detection.
    """
    global current_count
    # Open the default webcam (0)
    cap = cv2.VideoCapture(0)
    
    while cap.isOpened():
        success, frame = cap.read()
        if not success:
            break
            
        # Perform detection using YOLOv8. 
        # class 0 = 'person'. verbose=False keeps the console clean.
        results = model(frame, classes=[0], verbose=False)
        
        # Count how many bounding boxes (people) were detected in the current frame
        current_count = len(results[0].boxes)
        
        # Send the count to the C++ AI for processing
        talk_to_brain(current_count)
        
        # Annotate the frame with YOLO detection boxes and show it
        cv2.imshow("EcoLogic AI - Computer Vision", results[0].plot())
        
        # Allow closing the window by pressing the 'q' key
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
            
    cap.release()
    cv2.destroyAllWindows()

# --- 3. WEB DASHBOARD ROUTES ---

@app.route('/')
def index():
    """Renders the HTML dashboard interface."""
    return render_template('index.html')

@app.route('/data')
def get_data():
    """API endpoint used by JavaScript to refresh the dashboard numbers every second."""
    return jsonify(people=current_count, decision=current_decision)

# --- 4. MAIN EXECUTION ---
if __name__ == '__main__':
    print(f"--- EcoLogic AI System Starting ---")
    print(f"Targeting Hardware at: {ESP32_IP}")
    
    # Run the Computer Vision logic in a 'daemon' thread so it closes when the app stops
    threading.Thread(target=vision_loop, daemon=True).start()
    
    # Start the Flask web server on all local network interfaces
    app.run(host='0.0.0.0', port=5000)
