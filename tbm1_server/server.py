import socket
import json

# Socket is a combination of IP address, port, and protocal
  # Server Socket is for listening to incoming connections from clients like a reception desk to handle connection requests
  # Client Socket is for handling with a specific client such as sending or receiving data to/from a client

# Step 1: Create a TCP socket
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


##########################################################
## Receiving and Sending Data between Server and Client
# while True:
#     # Step 4: Accept a connection from a client
#     client_socket, client_address = server_socket.accept()   #Processing a client one by one
#     print(f"Connection established with {client_address}")

#     # Step 5: Receive and send data
#     data = client_socket.recv(1024)  # Receive data upto 1024 bytes from the client
#     print(f"Received: {data.decode()}") # Decode is for converting bytes back to string

#     # Send a response
#     response = "Hello from the server!"
#     client_socket.send(response.encode())  # Send data to the client
#                                            # Encode is for converting string to bytes

#     # Close the client connection
#     client_socket.close()
#     server_socket.close()

###############################################

while True:
    try:
        client_socket, client_address = server_socket.accept()
        print(f"Connection established with {client_address}")
    except Exception as e:
        print(f"Error accepting connection: {e}")
        break  # Exit early if no connection is established

    try:
        # Construct the message in the structured way
        client_socket.send("Hello")

        # Receiving Data from Client
        received_data = client_socket.recv(1024)
        if received_data.startswith(b'\x02') and received_data.endswith(b'\x03'):
            print("Json Format")
            # Strip start and end bytes
            json_data = received_data[1:-1].decode()  # Remove start/end bytes
            received_refined_data = json.loads(json_data)  # Parse JSON
            print(received_refined_data)
        else:
            print("Not Jason Format")
            received_data = client_socket.recv(1024)
            print(f"Received: {received_data.decode()}")

    except Exception as e:
        print(f"Error receiving or processing data: {e}")

    finally:
        client_socket.close()
        server_socket.close()
        
  
