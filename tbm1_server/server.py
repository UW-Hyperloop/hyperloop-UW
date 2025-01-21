import socket

# Step 1: Create a TCP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

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
    # Step 4: Accept a connection from a client
    client_socket, client_address = server_socket.accept()
    print(f"Connection established with {client_address}")

    # Step 5: Receive and send data
    data = client_socket.recv(1024)  # Receive up to 1024 bytes
    print(f"Received: {data.decode()}")

    # Send a response
    response = "Hello from the server!"
    client_socket.send(response.encode())

    # Close the client connection
    client_socket.close()

