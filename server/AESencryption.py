
import pycksum
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Random import get_random_bytes
import zlib
from Crypto.Util.Padding import unpad, pad


def generate_aes_key():
    return get_random_bytes(16)


def rsa_encryption(data , public_key):
    cipher = PKCS1_OAEP.new(RSA.importKey(public_key))
    return cipher.encrypt(data)


def aes_encryption(public_key, key):
    cipher = AES.new(key, AES.MODE_EAX)
    data = cipher.encrypt(public_key)
    return cipher.encrypt(data)




def calculate_crc(filename):
    checkSum = pycksum.cksum(open(filename))
    return checkSum



def get_crc_sum(data: bytes):
     return zlib.crc32(data) & 0xffffffff




def aes_decryption(aes_key, content):
    iv = bytearray([0] * AES.block_size)
    cipher = AES.new(aes_key, AES.MODE_CBC, iv)
    return unpad(cipher.decrypt(content), AES.block_size)
