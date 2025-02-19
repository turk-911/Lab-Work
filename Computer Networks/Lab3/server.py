import socket
import select

# Server configuration
HOST = "127.0.0.1"
PORT = 12345

# Create a TCP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((HOST, PORT))
server_socket.listen(5)

# List of sockets to monitor
sockets_list = [server_socket]
clients = {}

print(f"Server started on {HOST}:{PORT}")

while True:
    # Use select to wait for sockets to be ready
    read_sockets, _, exception_sockets = select.select(sockets_list, [], sockets_list)

    for notified_socket in read_sockets:
        # New connection
        if notified_socket == server_socket:
            client_socket, client_address = server_socket.accept()
            sockets_list.append(client_socket)
            clients[client_socket] = client_address
            print(f"New connection from {client_address}")
        else:
            # Existing client sending message
            try:
                message = notified_socket.recv(1024).decode('utf-8')
                if not message:
                    raise ConnectionResetError
                print(f"Received from {clients[notified_socket]}: {message}")
                
                # Broadcast the message to all clients except sender
                for client in clients:
                    if client != notified_socket:
                        client.send(message.encode('utf-8'))
            except:
                # Remove the client if an error occurs
                print(f"Connection closed by {clients[notified_socket]}")
                sockets_list.remove(notified_socket)
                del clients[notified_socket]
                notified_socket.close()

    for notified_socket in exception_sockets:
        sockets_list.remove(notified_socket)
        del clients[notified_socket]
        notified_socket.close()