import struct

# Seif Flags
SEIF_FLAG_ALPHA = (1 << 0)
SEIF_FLAG_META = (1 << 1)
SEIF_FLAG_COMPRESSED = (1 << 2)

# Seif Encoding
SEIF_ENCODING_RGBA = 0x01
SEIF_ENCODING_RGB = 0x02
SEIF_ENCODING_ARGB = 0x03

# Generation of SEIF
SEIF_MAGIC = b"SEIF"
SEIF_FLAGS  = SEIF_FLAG_ALPHA | SEIF_FLAG_META
SEIF_ENCODING = SEIF_ENCODING_RGBA

LEAF_SEIF_SIGNATURE = b"PY SEIF"

SEIF_Meta = struct.Struct("<8s")
SEIF_Header = struct.Struct("<4sBB8s")

SEIF_META = SEIF_Meta.pack(
        LEAF_SEIF_SIGNATURE
)

header_data = SEIF_Header.pack(
    SEIF_MAGIC,
    SEIF_FLAGS,
    SEIF_ENCODING,
    SEIF_META
)

with open("out.seif", "wb") as f:
    f.write(header_data)

