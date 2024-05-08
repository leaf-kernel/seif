import struct

SEIF_ENCODING_RGBA = 0x01
SEIF_FLAG_ALPHA = 1 << 0
SEIF_FLAG_COMPRESSED = 1 << 1

class SEIF_Meta:
    def __init__(self):
        self.signature = b"PYTHSEIF"
        self.width = 0
        self.height = 0

class SEIF_Header:
    def __init__(self):
        self.magic = b"SEIF"
        self.flags = SEIF_FLAG_ALPHA
        self.encoding = SEIF_ENCODING_RGBA
        self.meta = SEIF_Meta()
        self.chunk_count = 1
        self.chunk_size = 128*128

class SEIF_ChunkHeader:
    def __init__(self, i):
        self.idx = i 

def write_header(file, header):
    file.write(header.magic)
    file.write(struct.pack('BB', header.flags, header.encoding))
    file.write(header.meta.signature)
    file.write(struct.pack('II', header.meta.width, header.meta.height))
    file.write(struct.pack('II', header.chunk_count, header.chunk_size))

def write_chunk(file, chunk_header):
    file.write(struct.pack('I', chunk_header.idx))
    
    encoding_size = 4   # RGBA
    for _ in range((128*128) * encoding_size):
        file.write(struct.pack('B', 255))

def generate_seif_file(filename):
    with open(filename, 'wb') as file:
        header = SEIF_Header()
        write_header(file, header)
        
        for i in range(header.chunk_count):
            chunk_header = SEIF_ChunkHeader(i + 1)
            write_chunk(file, chunk_header)

if __name__ == "__main__":
    generate_seif_file("out.seif")
