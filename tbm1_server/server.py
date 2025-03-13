import queue
import random
import socket
import json
import threading

import websockets
from pynput import keyboard
import time

import asyncio

###################################
# Constants / Global Variables
###################################
HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 12346

# WebSocket server on separate port (e.g., 8765) for GUI
WS_HOST = '0.0.0.0'
WS_PORT = 8765

START_ID = 0x32
STOP_ID  = 0x33

KEY_SIGNAL = False
CHAR       = None

# We use 'esp_sending_json' (True/False) to know whether the ESP is currently
# sending JSON data. We'll set it to True as soon as any valid JSON is received,
# and set it to False if the ESP stops sending data for a while.
esp_sending_json = False

# Flags that instruct the main loop to keep sending start/stop messages.
send_start_flag = False
send_stop_flag  = False
# We'll send E-stop as a single message (once) when requested
estop_flag = False

# We'll keep a timestamp of when we last got any JSON data.
last_json_time = 0
JSON_TIMEOUT   = 3.0  # seconds - if needed to consider the ESP as "stopped"
commands_from_gui = queue.Queue()

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
# WebSocket Setup (for GUI)
###################################
connected_websockets = set()
ws_loop = None


async def ws_handler(websocket):
    """
    Handle incoming messages from a connected GUI WebSocket client.
    Also add the client to a set so we can broadcast new data to it.
    """
    global send_start_flag, send_stop_flag
    connected_websockets.add(websocket)
    try:
        async for message in websocket:
            # Handle incoming messages
            try:
                data = json.loads(message)
                cmd = data.get("command", "").lower()
                if cmd == "tbm_start":
                    print("[WEBSOCKET] TBM_start command received from GUI.")
                    commands_from_gui.put("start")
                elif cmd == "tbm_stop":
                    print("[WEBSOCKET] TBM_stop command received from GUI.")
                    send_stop_flag = True
                    send_start_flag = False
                else:
                    print("[WEBSOCKET] Unknown command from GUI:", data)
            except json.JSONDecodeError:
                print("[WEBSOCKET] Received non-JSON message from GUI:", message)
    except websockets.exceptions.ConnectionClosed:
        pass
    finally:
        connected_websockets.remove(websocket)


async def broadcast_to_gui(json_data: str):
    """
    Broadcast the given json string to all connected WebSocket clients.
    """
    if connected_websockets:
        # Gather tasks to send concurrently
        await asyncio.gather(*[ws.send(json_data) for ws in connected_websockets])

def start_websocket_server():
    """
    Runs an asyncio event loop for the WebSocket server in its own thread.
    """
    global ws_loop
    ws_loop = asyncio.new_event_loop()
    asyncio.set_event_loop(ws_loop)

    async def server_main():
        async with websockets.serve(ws_handler, WS_HOST, WS_PORT):
            print(f"[WEBSOCKET] WebSocket server started on ws://{WS_HOST}:{WS_PORT}")
            await asyncio.Future()  # Run forever

    ws_loop.run_until_complete(server_main())
    ws_loop.run_forever()

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
            print("we're in estop")
        #process commands from GUI if any
        try:
            while not estop_flag:
                cmd = commands_from_gui.get_nowait()
                if cmd == "start":
                    send_start_flag = True
                    send_stop_flag  = False
                elif cmd == "stop":
                    send_stop_flag  = True
                    send_start_flag = False
        except queue.Empty:
            pass
        # 1) Check if user pressed a key (global KEY_SIGNAL)
        if KEY_SIGNAL and not estop_flag:
            # We'll check CHAR to see if it was 's' or 't'
            if CHAR == 's':
                print("[SERVER] Start key pressed. Begin sending START msg.")
                send_start_flag = True
                send_stop_flag  = False  # Cancel any stop actions
            elif CHAR == 't':
                print("[SERVER] Stop key pressed. Begin sending STOP msg.")
                send_stop_flag  = True
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
                # We expect the first byte might be a "TBM_..." code.
                # If it's TBM_DATA (0x35) or TBM_INIT (0x31), there's JSON after that.
                estop_flag = False
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
                            # Broadcast actual ESP data to GUI
                            if ws_loop is not None:
                                asyncio.run_coroutine_threadsafe(
                                    broadcast_to_gui(json.dumps(parsed)), ws_loop
                                )
                            print("[SERVER] Broadcasted ESP data to GUI:", json.dumps(parsed, indent=2))

                        except json.JSONDecodeError:
                            print("[SERVER] Received invalid JSON. Data:", json_str)
                    else:
                        print("[SERVER] Could not find valid JSON in payload:", json_raw)
                else:
                    msg_id = payload[0]
                    if msg_id == 0x34:
                        estop_flag = True
                        #send message to gui indicating estop
                        estop_message = json.dumps({"state": "estop"})
                        if ws_loop is not None:
                            asyncio.run_coroutine_threadsafe(broadcast_to_gui(estop_message), ws_loop)
                    elif msg_id == 0x32:
                        estop_flag = False
                        #send message to gui indicating switch lifted and back in cofig mode
                        estop_message = json.dumps({"state": "config"})
                        if ws_loop is not None:
                            asyncio.run_coroutine_threadsafe(broadcast_to_gui(estop_message), ws_loop)
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
                    esp_sending_json = False
            else:
                if not send_stop_flag:
                    time_since_json = time.time() - last_json_time
                    if time_since_json > 10.0:
                        fail_msg = json.dumps({"state": "power_failure"})
                        if ws_loop is not None:
                            asyncio.run_coroutine_threadsafe(broadcast_to_gui(fail_msg), ws_loop)
                        print("power failure")
                    else:
                        fail_msg = json.dumps({"state": "comms_failure"})
                        if ws_loop is not None:
                            asyncio.run_coroutine_threadsafe(broadcast_to_gui(fail_msg), ws_loop)
                        print("comms failure")


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
    # 2) Start the WebSocket server in a separate thread (for the GUI)
    ws_thread = threading.Thread(target=start_websocket_server, daemon=True)
    ws_thread.start()

    # 3) Start the server (main thread blocks here)

    start_server()
