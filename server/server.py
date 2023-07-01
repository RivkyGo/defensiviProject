'''maman 15 defensivi rivka goldberg'''

import socket
import db
import function_request
from data import read_port
from function_request import request
from db import CLIENT_TABLE
from db import FILE_TABLE
import threading

CLIENT_TABLE
FILE_TABLE


def handle_client(client_socket):

    data = bytearray()
    batch = 20000
    packet = client_socket.recv(batch)
    data.extend(packet)
    response = request(data)
    client_socket.send(response)
    if function_request.finish:
        client_socket.close()


def main():
    try:
        try:
            db.open_db()
        except:
            db.load_client_db()
            db.load_file_db()
        port = read_port()
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind(("0.0.0.0", int(port)))
        server_socket.listen()
        print("Server is up and running")
        while True:
            (client_socket, client_address) = server_socket.accept()
            client_thread = threading.Thread(target=handle_client, args=(client_socket,))
            client_thread.start()

        server_socket.close()
    except:
        client_socket.close()
        server_socket.close()


if __name__ == '__main__':
    main()




