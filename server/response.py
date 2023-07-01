import struct



class FileServerResponse:
    def __init__(self, version, code, payload_size, payload):
        self.version = version
        self.code = code
        self.payload_size = payload_size
        self.payload = payload

    def generate_binary_request(self):


        try:

            format = '<'  # Little endian annotation
            format += 'c'  # version
            format += 'H'  # code
            format += 'I'  # unsigned payload size

            if self.payload:
                format += f"{self.payload_size}s"
                if type(self.payload) is str:
                    self.payload = self.payload.encode()


                return struct.pack(format, self.version.to_bytes(1 , 'little'), self.code, self.payload_size, self.payload)
            return struct.pack(format, self.version.to_bytes(1 , 'little'), self.code, self.payload_size)

        except Exception as e:
            print(e)
