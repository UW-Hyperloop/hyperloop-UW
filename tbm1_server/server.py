#  ESP
# Arduino IDE
# Pull up Repositories

import socket
import json
import threading
import keyboard

###################################
#Detecting Keystroke to Send Data to ESP or Stop the connection with ESP
KEY_SIGNAL = False
CHAR = None

def listen_for_keys():
    global CHAR
    global KEY_SIGNAL
    while True:
        key = keyboard.read_event()  # Wait for any key press
        if key.event_type == keyboard.KEY_DOWN:  # Detect key press
            KEY_SIGNAL = True  # Set flag
            CHAR = key.name
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



# Socket is a combination of IP address, port, and protocol
  # Server Socket is for listening to incoming connections from clients like a reception desk to handle connection requests
  # Client Socket is for handling with a specific client such as sending or receiving data to/from a client

# Step 1: Create TBM object and TCP socket
tbm = TBM()
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  #AF_INET implies IPv4

# Step 2: Bind the socket to an IP address and port
host = '0.0.0.0'  # Accept connections from any IP
port = 12345      # Choose a port (make sure it's not in use)
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
            print("Json Format")
            # Strip start and end bytes
            json_data = ""
            for data in received_data[2:460]:
                json_data += chr(data)

#Broken Pipe

            # json_data = chr(received_data[2:460]) # Remove start/end bytes
            received_refined_data = json.loads(json_data)  # Parse JSON
            tbm.TBMupdate(received_refined_data)    #Passing the data to the TBM's function to update TBM
            print(received_refined_data)
        else:
            print("Not Jason Format")
            print(f"Received: {received_data.decode()}")

        
        ###################################
        if KEY_SIGNAL:
            #Stop Connecting with ESP
            if CHAR == 's':
                break
            if CHAR == 't':
                client_socket.send("DATA".encode())
        ###################################

    except Exception as e:
        print(f"Error receiving or processing data: {e}")

t.join()

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
