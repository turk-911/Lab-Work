import socket
import select
import sys

# Server configuration
HOST = "127.0.0.1"
PORT = 12345

# Create a TCP socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))

print("Connected to the chat server! Type messages and press Enter to send.")

while True:
    # Use select to check if there's input from user or incoming message
    read_sockets, _, _ = select.select([sys.stdin, client_socket], [], [])

    for source in read_sockets:
        if source == client_socket:
            # Receiving message from server
            message = client_socket.recv(1024).decode('utf-8')
            if not message:
                print("Disconnected from server.")
                client_socket.close()
                sys.exit()
            print(f"\n{message}")
        else:
            # User input message
            message = sys.stdin.readline().strip()
            client_socket.send(message.encode('utf-8'))