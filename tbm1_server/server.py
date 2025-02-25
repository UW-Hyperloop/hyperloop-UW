import socket
import json
import threading
from pynput import keyboard
import time

###################################
# Constants / Global Variables
###################################
HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 12346

START_ID = 0x32
STOP_ID = 0x33

KEY_SIGNAL = False
CHAR = None

# We use 'esp_sending_json' (True/False) to know whether the ESP is currently
# sending JSON data. We'll set it to True as soon as any valid JSON is received,
# and set it to False if the ESP stops sending data for a while.
esp_sending_json = False
estop_flag = False

# Flags that instruct the main loop to keep sending start/stop messages.
send_start_flag = False
send_stop_flag = False

# We'll keep a timestamp of when we last got any JSON data.
last_json_time = 0
JSON_TIMEOUT = 3.0  # seconds - if needed to consider the ESP as "stopped"

# We will store the client socket globally once connected.
client_socket = None

###################################
# Keyboard Handling
###################################
def on_press(key):
    """
    Called whenever a key is pressed. We'll set global flags accordingly.
    """
    global KEY_SIGNAL, CHAR

    # Filter only normal character keys
    try:
        # e.g. key.char might be 's' or 't'
        if key.char:
            CHAR = key.char.lower()
            KEY_SIGNAL = True
    except AttributeError:
        # Special keys (CTRL, SHIFT, etc.) - ignore them for now
        pass

def listen_for_keys():
    """
    This thread runs a permanent pynput listener in the background.
    """
    with keyboard.Listener(on_press=on_press) as listener:
        listener.join()

###################################
# Server Setup
###################################
def start_server():
    global client_socket, send_start_flag, send_stop_flag, estop_flag
    global esp_sending_json, last_json_time
    global KEY_SIGNAL, CHAR

    # Create TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen(1)
    print(f"[SERVER] Listening on {HOST}:{PORT}")

    # Accept client connection (from the ESP)
    while True:
        try:
            client_socket, client_address = server_socket.accept()
            print(f"[SERVER] Connected by {client_address}")
            client_socket.settimeout(0.5)  # Non-blocking or short-blocking reads
            break
        except Exception as e:
            print(f"[SERVER] Error accepting connection: {e}")
            time.sleep(1)

    # Main server loop: handle sending & receiving
    while True:
        if estop_flag:
            print("server in estop")
        # 1) Check if user pressed a key (global KEY_SIGNAL)
        if KEY_SIGNAL and not estop_flag:
            # We'll check CHAR to see if it was 's' or 't'
            if CHAR == 's':
                print("[SERVER] Start key pressed. Begin sending START msg.")
                send_start_flag = True
                send_stop_flag = False  # Cancel any stop actions
            elif CHAR == 't':
                print("[SERVER] Stop key pressed. Begin sending STOP msg.")
                send_stop_flag = True
                send_start_flag = False
            # Reset KEY_SIGNAL so we only process once per press
            KEY_SIGNAL = False

        # 2) If we are in "send_start_flag" mode and the ESP is NOT sending JSON,
        #    keep sending [0x02, START_ID, 0x03].
        if send_start_flag and not esp_sending_json:
            # Keep sending the start message
            msg = bytes([0x02, START_ID, 0x03])
            try:
                client_socket.sendall(msg)
                # Optional: add a small sleep to avoid spamming too fast
                time.sleep(0.1)
            except BrokenPipeError:
                print("[SERVER] Broken pipe on sending START. Will retry after reconnect.")
                # TODO: Implement reconnect logic if needed
            except Exception as e:
                print(f"[SERVER] Error sending START: {e}")

        # 3) If we are in "send_stop_flag" mode and the ESP *is* sending JSON,
        #    keep sending [0x02, STOP_ID, 0x03].
        #    Once the ESP stops sending JSON, we can disable the flag.
        if send_stop_flag and esp_sending_json:
            msg = bytes([0x02, STOP_ID, 0x03])
            try:
                client_socket.sendall(msg)
                time.sleep(0.1)
            except BrokenPipeError:
                print("[SERVER] Broken pipe on sending STOP. Will retry after reconnect.")
            except Exception as e:
                print(f"[SERVER] Error sending STOP: {e}")

        # 4) Receive data from ESP (non-blocking or short-block).
        #    If we see valid JSON framed by [0x02 ... 0x03], parse it and update flags.
        try:
            received_data = client_socket.recv(1024)
        except socket.timeout:
            received_data = b''
        except Exception as e:
            print(f"[SERVER] Error receiving data: {e}")
            received_data = b''

        if received_data:
            # Check if it's a framed message [0x02 ... 0x03]
            if received_data.startswith(b'\x02') and received_data.endswith(b'\x03'):
                # Extract the inner payload
                payload = received_data[1:-1].strip()  # remove start 0x02 and end 0x03
                estop_flag = False
                # We expect the first byte might be a "TBM_..." code.
                # If it's TBM_DATA (0x35) or TBM_INIT (0x31), there's JSON after that.
                if len(payload) > 1:
                    msg_id  = payload[0]
                    json_raw = payload[1:].decode(errors='ignore').strip()

                    # Attempt to parse as JSON
                    # Normally you'd look for '{' and '}' but let's do a quick parse:
                    json_start = json_raw.find('{')
                    json_end = json_raw.rfind('}')
                    if json_start != -1 and json_end != -1:
                        json_str = json_raw[json_start:json_end+1]
                        try:
                            parsed = json.loads(json_str)
                            # We have valid JSON from the ESP
                            esp_sending_json = True
                            last_json_time   = time.time()
                            # If we were sending start messages, we can stop once we see JSON
                            if send_start_flag:
                                send_start_flag = False

                            # Print or store the JSON data
                            print("[SERVER] Received JSON from ESP:")
                            print(json.dumps(parsed, indent=2))

                        except json.JSONDecodeError:
                            print("[SERVER] Received invalid JSON. Data:", json_str)
                    else:
                        print("[SERVER] Could not find valid JSON in payload:", json_raw)
                else:
                    msg_id = payload[0]
                    if msg_id == 0x34:
                        estop_flag = True
                    elif msg_id == 0x32:
                        estop_flag = False
                    else:
                        print("[SERVER] Message with no meaningful payload:", payload)
            else:
                # Not a framed message or doesn't match your expected format
                print("[SERVER] Non-JSON or unframed data:", received_data)
        else:
            # If we got no data in this loop iteration, check how long since last JSON
            # This is how we detect the ESP has stopped sending data.
            # If you want to require that for the STOP logic, do it here.
            if esp_sending_json:
                if send_stop_flag:
                    time_since_json = time.time() - last_json_time
                    if time_since_json > 1.0:
                        # We can assume the ESP has stopped sending JSON
                        esp_sending_json = False
                        send_stop_flag = False
                        print("[SERVER] Stopped receiving JSON. STOP cycle complete.")
                else:
                    # print("its an estop or error. no msg coming back")
                    esp_sending_json = False


        # Optional: If you want to detect “if JSON stops unexpectedly, resume sending start”:
        # if time.time() - last_json_time > JSON_TIMEOUT and esp_sending_json:
        #     # It's been more than 3s since last JSON
        #     esp_sending_json = False
        #     # We could set send_start_flag = True here if we want to force re-start
        #     # send_start_flag = True
        #     print("[SERVER] JSON timed out; re-enabling start. (Commented out by default)")

        # Quick idle so we don't spin too tight
        time.sleep(0.05)

###################################
# MAIN
###################################
if __name__ == "__main__":
    # Thread to detect keystrokes
    keyboard_thread = threading.Thread(target=listen_for_keys, daemon=True)
    keyboard_thread.start()

    # Start the server (main thread blocks here)
    start_server()
