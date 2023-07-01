import os

PORT_INFO = "port.info.txt"
PORT = 1234
MAX_PORT = 65535




def save_file(path: str, data: bytearray):
    with open(path, 'wb') as f:
        f.write(data)


def unpad_binary(data: bytearray, padding: bytearray):
    return data.strip(padding)


def read_port():
    try:
        with open("port.info.txt", "r") as file:
            PORT = file.read()
            print(PORT)
        if not PORT.isnumeric() or MAX_PORT < int(PORT) or int(PORT) < 0:
            PORT = 1234
    except:
        print("The file does not exist")
        PORT = 1234
    return PORT


def create_files_dir():
    path_file = "C:\\Files"
    if not os.path.exists(path_file):
        os.makedirs(path_file, exist_ok=True)