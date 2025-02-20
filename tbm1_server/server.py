#  ESP
# Arduino IDE
# Pull up Repositories

import socket
import json
import threading
import keyboard
import websockets  # needs pip install websockets?
###################################
#Detecting Keystroke to Send Data to ESP or Stop the connection with ESP
KEY_SIGNAL = False
CHAR = None
START_ID = 0x32
STOP_ID = 0x33

def listen_for_keys():
   global CHAR
   global KEY_SIGNAL
   while True:
       key = keyboard.read_event()  # Wait for any key press
       if key.event_type == keyboard.KEY_DOWN:  # Detect key press
           #print(f"Key pressed: {key.name}")  # Debug: Print the key pressed
           KEY_SIGNAL = True  # Set flag
           CHAR = key.name
           #print(f"KEY_SIGNAL set to {KEY_SIGNAL}, CHAR set to {CHAR}")  # Debug: Print the state of KEY_SIGNAL and CHAR
###################################

#Thread to respond to keystroke anytime
t = threading.Thread(target=listen_for_keys, daemon=True)
t.start()

#TBM Class
class TBM:

    def __init__(self):
        #list[]: There are 3 elements inside -> active, value, timestamp
        self.state = None      #"String describing state as either “config”, “stop”, “running”, or “error”"
        self.motor_temp = [] #<Sensor> type value
        self.pump_temp =  []   #<Sensor> type value
        self.flow_in = []   #<Sensor> type value
        self.flow_out = []    #<Sensor> type value
        self.motor_power = [] #<Sensor> type value. 1 or 0 indicating power flowing to motor
        self.pump_power = [] #<Sensor> type value. 1 or 0 indicating power flowing to motor
        self.bentonite_power = [] #<Sensor> type value. 1 or 0 indicating power to bentonite pump
        self.estop_pressed = []   #<Sensor> type value. 1 or 0 indicating whether e-stop has been pressed
        self.global_time  = []   #Time since “TBM_START” command in ms

        self.active = []    #1 or 0 representing whether we are currently using this sensor. (Is it installed and working)
        self.value = []           #
        self.timestamp = None   #time of sampling sensor since “TBM_START” in ms

        #Function to update TBM given data from ESP (Client)
    def TBMupdate(self, data):
        #Update state and global_time and Delete the state and global_time value from data
        self.state = data["state"]
        del data["state"]
        self.global_time = data["global_time"]
        del data["global_time"]

        for key, value in data.items():
            if hasattr(self, key):  # Check if the key exists
                attr = getattr(self, key)  # Get the attribute (list)
                attr[:] = list(value.values())  # Update list with dictionary values
                setattr(self, key, attr)  # Save updated list

    def to_dict(self):
        # Returns a dictionary containing all TBM attributes.
        # used to send to GUI client
        return {
            "state": self.state,
            "motor_temp": self.motor_temp,
            "pump_temp": self.pump_temp,
            "flow_in": self.flow_in,
            "flow_out": self.flow_out,
            "motor_power": self.motor_power,
            "pump_power": self.pump_power,
            "bentonite_power": self.bentonite_power,
            "estop_pressed": self.estop_pressed,
            "global_time": self.global_time,
            "active": self.active,
            "value": self.value,
            "timestamp": self.timestamp
        }


# Socket is a combination of IP address, port, and protocol
  # Server Socket is for listening to incoming connections from clients like a reception desk to handle connection requests
  # Client Socket is for handling with a specific client such as sending or receiving data to/from a client

# Step 1: Create TBM object and TCP socket
tbm = TBM()
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  #AF_INET implies IPv4

# Step 2: Bind the socket to an IP address and port
host = '0.0.0.0'  # Accept connections from any IP
port = 12346      # Choose a port (make sure it's not in use)
server_socket.bind((host, port))  #Use "bind" to associate the server with an IP address and port

# Step 3: Listen for incoming connections
server_socket.listen(5)  # The number '5' specifies the backlog size (maximum number of pending connections, which is 5 here!)
                         # To be more specific, the server can queue up to 5 client connections while
                         # waiting for the server to call "accept" and process them

print(f"Server is listening on {host}:{port}...")

while True:
    try:
        client_socket, client_address = server_socket.accept()
        print(f"Connection established with {client_address}")
        break
    except Exception as e:
        print(f"Error accepting connection: {e}")
        break  # Exit early if no connection is established

while True:
    try:
        # Construct the message in the structured way.
        
        #################################################
        #Handling Broken Pipe
        if client_socket.fileno() != -1:  # -1 means socket is closed
            client_socket.sendall(b"Hello, Client!")
        #################################################
        
        word = "HEllo"
        client_socket.send(word.encode())

        # Receiving Data from Client
        # Data format from Client: [start byte, Message Type, Json, End byte]
        received_data = client_socket.recv(1024)

        if received_data.startswith(b'\x02') and received_data.endswith(b'\x03'):
            #print("Json Format")
            # Strip start and end bytes and convert to string
            json_data = received_data[2:-1].decode().strip()
            
            # Find the start and end of the JSON data
            json_start = json_data.find('{')
            json_end = json_data.rfind('}') + 1  # Include the closing brace
            
            if json_start != -1 and json_end != -1:
                json_data = json_data[json_start:json_end]
                
            #print(f"Cleaned JSON: {json_data}")
            received_refined_data = json.loads(json_data)  # Parse JSON
            tbm.TBMupdate(received_refined_data)    #Passing the data to the TBM's function to update TBM
            print(received_refined_data)
        else:
            print("Not Jason Format")
            print(f"Received: {received_data.decode()}")

        
        ###################################
        if KEY_SIGNAL:
            #print('KEY_SIGNAL was true')
            #Stop Connecting with ESP
            if CHAR == 's': #start
                print("sending start")
                client_socket.sendall(bytes([0x02, START_ID, 0x03]))
                KEY_SIGNAL = 0
            if CHAR == 't': #stop
                print("sending stop")
                client_socket.sendall(bytes([0x02, STOP_ID, 0x03]))
                KEY_SIGNAL = 0
        ###################################

    except Exception as e:
        print(f"Error receiving or processing data: {e}")

t.join()
""" 
async def broadcast_loop(tbm: TBM, connected_clients: Set[websockets.WebSocketServerProtocol]):
    ###
    Periodically broadcast the TBM state to all connected WebSocket clients.
    ###
    while True:
        await asyncio.sleep(0.5)  # 500 ms interval

        data_dict = tbm.to_dict()
        message = json.dumps(data_dict)

        # Broadcast to all current clients
        to_remove = []
        for ws in connected_clients:
            try:
                await ws.send(message)
            except websockets.exceptions.ConnectionClosed:
                to_remove.append(ws)

        # Clean up any disconnected websockets
        for ws in to_remove:
            connected_clients.remove(ws)


async def websocket_handler(websocket: websockets.WebSocketServerProtocol, path: str,
                            tbm: TBM, connected_clients: Set[websockets.WebSocketServerProtocol]):
    ###
    Handler for each new WebSocket connection.
    ###
    connected_clients.add(websocket)
    print(f"[WS] New client connected. Total clients: {len(connected_clients)}")
    try:
        async for msg in websocket:
            # Handle messages from client if needed
            print(f"[WS] Received from client: {msg}")
    finally:
        # On disconnect, remove from the set
        connected_clients.remove(websocket)
        print(f"[WS] Client disconnected. Total clients: {len(connected_clients)}")


def start_websocket_server(tbm: TBM, ws_host: str = "0.0.0.0", ws_port: int = 8001):
    ###
    Start the WebSocket server in an asyncio event loop.
    Runs the `broadcast_loop` indefinitely, so this function blocks.
    ###
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)

    connected_clients: Set[websockets.WebSocketServerProtocol] = set()

    # Create the server
    start_server = websockets.serve(
        lambda ws, path: websocket_handler(ws, path, tbm, connected_clients),
        ws_host,
        ws_port
    )

    # Run the server
    server = loop.run_until_complete(start_server)
    print(f"[WS] WebSocket server listening on {ws_host}:{ws_port}")

    # Run broadcast loop forever
    try:
        loop.run_until_complete(broadcast_loop(tbm, connected_clients))
    finally:
        server.close()
        loop.run_until_complete(server.wait_closed())
        loop.close()


def main():
    ###
    Main entry point. Creates the TBM object, then starts:
      1) TCP server (in one thread)
      2) WebSocket server (in another thread)

    Both share the same TBM instance.
    ###
    tbm = TBM()

    # Thread for the TCP server that talks to the ESP
    tcp_thread = threading.Thread(
        target=start_tcp_server,
        args=(tbm,),  # pass our shared TBM
        daemon=True
    )

    # Thread for the WebSocket server that talks to React/Next.js clients
    ws_thread = threading.Thread(
        target=start_websocket_server,
        args=(tbm,),
        daemon=True
    )

    tcp_thread.start()
    ws_thread.start()

    # Keep the main thread alive
    # You could do other tasks here or just wait for threads to exit.
    tcp_thread.join()
    ws_thread.join()


if __name__ == "__main__":
    main()
"""
# import socket
#
#
# def thing():
#     HOST = "0.0.0.0"  # Listen on all interfaces
#     PORT = 5001  # Must match the port in the ESP32 code
#
#     # Create socket
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.bind((HOST, PORT))
#     server_socket.listen(1)
#
#     print(f"Server listening on {HOST}:{PORT}")
#
#     # Accept client connections
#     conn, addr = server_socket.accept()
#     print(f"Connection from {addr}")
#
#     while True:
#         data = conn.recv(1024)
#         if not data:
#             break
#         print(f"Received: {data.decode()}")
#         conn.sendall(b"Message received!")  # Echo response
#
#     conn.close()
#     server_socket.close()
#
#
# # Press the green button in the gutter to run the script.
# if __name__ == '__main__':
#     thing()
