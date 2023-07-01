import binascii
import secrets
import sqlite3
import uuid
import struct
import datetime
import function_request
from request import FileServerRequest
from response import FileServerResponse
import data
import AESencryption
import response
from AESencryption import generate_aes_key, aes_encryption

from db import CLIENT_TABLE
from db import FILE_TABLE

finish = 0
CLIENT_TABLE
FILE_TABLE
VERSION = 1


QUERY_UPDATE_LAST_SEEN = 'UPDATE Client SET last_seen = ? WHERE id=?'




def request(req):
    new_req = FileServerRequest.parse_binary_request(req)
    # print(new_req.code, new_req.client_id, new_req.payload, new_req.payload_size, new_req.version)
    COMMAND_DICTIONARY = {1100: Registration, 1101: send_public_key, 1102: reconnecting, 1103: Send_file,
                          1104: valid_crc,
                          1105: not_valid_crc, 1106: not_valid_crc_terminating}
    # open tabel_client
    conn = sqlite3.connect(r"server.db")
    cursor = conn.cursor()
    querry = "SELECT * FROM clients "
    cursor.execute(querry)
    querry = "SELECT * FROM files  "
    cursor.execute(querry)
    if new_req.code in COMMAND_DICTIONARY:
        print(new_req.code)
        output = COMMAND_DICTIONARY[new_req.code](new_req, cursor)
        conn.commit()
        cursor.close()
        return output


def Registration(req, cursor):
    name = req.payload[0:req.payload.find(b'\0')]  # Normalizing the input - removing the trailing NULL-BYTE
    client_exists = False
    for client in CLIENT_TABLE.values():
        if client[0] == name:
            client_exists = True
            break
    if client_exists:
        return FileServerResponse.generate_binary_request(response.FileServerResponse(VERSION, 2101, 0, None))


    user_uuid = uuid.uuid1()
    cursor.execute("INSERT INTO clients (ID, Name, LastSeen) VALUES (?, ?, ?)",
                   (user_uuid.bytes, name, datetime.datetime.now()))
    CLIENT_TABLE[user_uuid.bytes] = [name, None, datetime.datetime.now(), None, None]
    return FileServerResponse.generate_binary_request(response.FileServerResponse(VERSION, 2100, 16, user_uuid.bytes))


def send_public_key(req, cursor):
    payload = req.payload
    name = payload[:req.payload.find(b'\0')]
    if req.client_id not in CLIENT_TABLE:
        return FileServerResponse.generate_binary_request(response.FileServerResponse(VERSION, 2107, 0, None))
    public_key = payload[255:]
    CLIENT_TABLE.get(req.client_id)[1] = public_key
    generated_aes_key = generate_aes_key()
    encrypted_aes_key = AESencryption.rsa_encryption(generated_aes_key, public_key)
    cursor.execute("UPDATE clients SET PublicKey = ? ,AES = ?,AES_ENCRYPT = ? WHERE Name = ? ",
                   [public_key, generated_aes_key, encrypted_aes_key, name])
    CLIENT_TABLE.get(req.client_id)[3] = generated_aes_key
    CLIENT_TABLE.get(req.client_id)[4] = encrypted_aes_key
    cursor.execute('UPDATE clients SET LastSeen = ? WHERE id=?',
                   [datetime.datetime.now(), binascii.hexlify(req.client_id)])
    payload = struct.pack(f"16s{len(encrypted_aes_key)}s", req.client_id, encrypted_aes_key)
    return FileServerResponse.generate_binary_request(response.FileServerResponse(VERSION, 2102, len(payload), payload))


def reconnecting(req, cursor):
    if req.client_id not in CLIENT_TABLE:
        return response.FileServerResponse.generate_binary_request(response.FileServerResponse(VERSION, 2106, 0, None))
    cursor.execute('UPDATE clients SET LastSeen = ? WHERE id=?',
                   [datetime.datetime.now(), binascii.hexlify(req.client_id)])
    encrypted_aes_key = CLIENT_TABLE.get(req.client_id)[4]
    payload = struct.pack(f"16s{len(encrypted_aes_key)}s", req.client_id, encrypted_aes_key)
    # .decode('utf-8')
    return FileServerResponse.generate_binary_request(response.FileServerResponse(VERSION, 2105, len(payload), payload))


def Send_file(req, cursor):
    total_payload_headers_size = 259
    content_size, file_name = struct.unpack('<I255s', req.payload[:total_payload_headers_size])
    messege_content = req.payload[total_payload_headers_size:]
    file_name = file_name[:file_name.find(b'\0')]

    # file_name = data.unpad_binary(file_name, b'\xcc')
    cursor.execute("INSERT INTO files (ID, FileName, PathName, Verified) VALUES (?, ?, ?, ?)",
                   (req.client_id, file_name, "C:\Files", False))
    aes_key = CLIENT_TABLE.get(req.client_id)[3]
    cursor.execute('UPDATE clients SET LastSeen = ? WHERE id=?',
                   [datetime.datetime.now(), binascii.hexlify(req.client_id)])

    if not len(messege_content) % 16 == 0:
        messege_content = AESencryption.padding(messege_content)
    decrypted_file = AESencryption.aes_decryption(aes_key=aes_key, content=messege_content)
    data.save_file(path=f"C:\\Files\\ {file_name}", data=decrypted_file)
    format = '<'  # Little endian annotation
    format += '16s'  # client_id
    format += 'I'  # content_size
    format += '255s'  # file_name
    format += 'I'  # cksum

    payload = struct.pack(format, req.client_id, content_size, file_name, AESencryption.get_crc_sum(decrypted_file))
    return response.FileServerResponse.generate_binary_request(response.FileServerResponse(VERSION, 2103, 279, payload))


def valid_crc(req, cursor):
    file_name = req.payload[:req.payload.find(b'\0')]
    cursor.execute("UPDATE files SET Verified = True  WHERE ID = ?  and  FileName = ? ", [req.client_id, file_name])
    cursor.execute('UPDATE clients SET LastSeen = ? WHERE id=?',
                   [datetime.datetime.now(), binascii.hexlify(req.client_id)])
    function_request.finish = 1
    return response.FileServerResponse.generate_binary_request(
        response.FileServerResponse(VERSION, 2104, 16, req.client_id))


def not_valid_crc(req, cursor):
    cursor.execute('UPDATE clients SET LastSeen = ? WHERE id=?',
                   [datetime.datetime.now(), binascii.hexlify(req.client_id)])
    return response.FileServerResponse.generate_binary_request(response.FileServerResponse(VERSION, 2104, 0, None))


def not_valid_crc_terminating(req, cursor):
    global finish
    cursor.execute('UPDATE clients SET LastSeen = ? WHERE id=?',
                   [datetime.datetime.now(), binascii.hexlify(req.client_id)])
    finish = 1
    return response.FileServerResponse.generate_binary_request(response.FileServerResponse(VERSION, 2104, 0, None))
