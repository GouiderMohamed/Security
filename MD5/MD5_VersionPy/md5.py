# MD5 (Message-Digest Algorithm 5)
# Copyright Victor Breder 2024
# SPDX-License-Identifier: MIT

# Implemented from reference RFC 1321 available at
# <https://www.ietf.org/rfc/rfc1321.txt>

import ctypes
import struct

# --- md5.h equivalent ---

class MD5Context:
    def __init__(self):
        self.a: int = 0
        self.b: int = 0
        self.c: int = 0
        self.d: int = 0

# --- md5.c equivalent ---

# from RFC 1321, Section 3.4:
def F(X, Y, Z): return ((X & Y) | ((~X) & Z)) & 0xFFFFFFFF
def G(X, Y, Z): return ((X & Z) | (Y & (~Z))) & 0xFFFFFFFF
def H(X, Y, Z): return (X ^ Y ^ Z) & 0xFFFFFFFF
def I(X, Y, Z): return (Y ^ (X | (~Z))) & 0xFFFFFFFF

def rotl(x: int, s: int) -> int:
    x &= 0xFFFFFFFF
    return ((x << s) | (x >> (32 - s))) & 0xFFFFFFFF

def STEP(OP, a, b, c, d, k, s, i, X):
    a = (b + rotl((a + OP(b, c, d) + X[k] + i) & 0xFFFFFFFF, s)) & 0xFFFFFFFF
    return a

def TO_I32(data: bytes, i: int) -> int:
    return struct.unpack_from('<I', data, i)[0]

def md5_block(ctx: MD5Context, m: bytes) -> None:
    assert ctx is not None
    assert len(m) == 64

    X = [TO_I32(m, i * 4) for i in range(16)]

    a = ctx.a
    b = ctx.b
    c = ctx.c
    d = ctx.d

    a = STEP(F, a, b, c, d,  0,  7, 0xd76aa478, X)
    d = STEP(F, d, a, b, c,  1, 12, 0xe8c7b756, X)
    c = STEP(F, c, d, a, b,  2, 17, 0x242070db, X)
    b = STEP(F, b, c, d, a,  3, 22, 0xc1bdceee, X)
    a = STEP(F, a, b, c, d,  4,  7, 0xf57c0faf, X)
    d = STEP(F, d, a, b, c,  5, 12, 0x4787c62a, X)
    c = STEP(F, c, d, a, b,  6, 17, 0xa8304613, X)
    b = STEP(F, b, c, d, a,  7, 22, 0xfd469501, X)
    a = STEP(F, a, b, c, d,  8,  7, 0x698098d8, X)
    d = STEP(F, d, a, b, c,  9, 12, 0x8b44f7af, X)
    c = STEP(F, c, d, a, b, 10, 17, 0xffff5bb1, X)
    b = STEP(F, b, c, d, a, 11, 22, 0x895cd7be, X)
    a = STEP(F, a, b, c, d, 12,  7, 0x6b901122, X)
    d = STEP(F, d, a, b, c, 13, 12, 0xfd987193, X)
    c = STEP(F, c, d, a, b, 14, 17, 0xa679438e, X)
    b = STEP(F, b, c, d, a, 15, 22, 0x49b40821, X)
    a = STEP(G, a, b, c, d,  1,  5, 0xf61e2562, X)
    d = STEP(G, d, a, b, c,  6,  9, 0xc040b340, X)
    c = STEP(G, c, d, a, b, 11, 14, 0x265e5a51, X)
    b = STEP(G, b, c, d, a,  0, 20, 0xe9b6c7aa, X)
    a = STEP(G, a, b, c, d,  5,  5, 0xd62f105d, X)
    d = STEP(G, d, a, b, c, 10,  9, 0x02441453, X)
    c = STEP(G, c, d, a, b, 15, 14, 0xd8a1e681, X)
    b = STEP(G, b, c, d, a,  4, 20, 0xe7d3fbc8, X)
    a = STEP(G, a, b, c, d,  9,  5, 0x21e1cde6, X)
    d = STEP(G, d, a, b, c, 14,  9, 0xc33707d6, X)
    c = STEP(G, c, d, a, b,  3, 14, 0xf4d50d87, X)
    b = STEP(G, b, c, d, a,  8, 20, 0x455a14ed, X)
    a = STEP(G, a, b, c, d, 13,  5, 0xa9e3e905, X)
    d = STEP(G, d, a, b, c,  2,  9, 0xfcefa3f8, X)
    c = STEP(G, c, d, a, b,  7, 14, 0x676f02d9, X)
    b = STEP(G, b, c, d, a, 12, 20, 0x8d2a4c8a, X)
    a = STEP(H, a, b, c, d,  5,  4, 0xfffa3942, X)
    d = STEP(H, d, a, b, c,  8, 11, 0x8771f681, X)
    c = STEP(H, c, d, a, b, 11, 16, 0x6d9d6122, X)
    b = STEP(H, b, c, d, a, 14, 23, 0xfde5380c, X)
    a = STEP(H, a, b, c, d,  1,  4, 0xa4beea44, X)
    d = STEP(H, d, a, b, c,  4, 11, 0x4bdecfa9, X)
    c = STEP(H, c, d, a, b,  7, 16, 0xf6bb4b60, X)
    b = STEP(H, b, c, d, a, 10, 23, 0xbebfbc70, X)
    a = STEP(H, a, b, c, d, 13,  4, 0x289b7ec6, X)
    d = STEP(H, d, a, b, c,  0, 11, 0xeaa127fa, X)
    c = STEP(H, c, d, a, b,  3, 16, 0xd4ef3085, X)
    b = STEP(H, b, c, d, a,  6, 23, 0x04881d05, X)
    a = STEP(H, a, b, c, d,  9,  4, 0xd9d4d039, X)
    d = STEP(H, d, a, b, c, 12, 11, 0xe6db99e5, X)
    c = STEP(H, c, d, a, b, 15, 16, 0x1fa27cf8, X)
    b = STEP(H, b, c, d, a,  2, 23, 0xc4ac5665, X)
    a = STEP(I, a, b, c, d,  0,  6, 0xf4292244, X)
    d = STEP(I, d, a, b, c,  7, 10, 0x432aff97, X)
    c = STEP(I, c, d, a, b, 14, 15, 0xab9423a7, X)
    b = STEP(I, b, c, d, a,  5, 21, 0xfc93a039, X)
    a = STEP(I, a, b, c, d, 12,  6, 0x655b59c3, X)
    d = STEP(I, d, a, b, c,  3, 10, 0x8f0ccc92, X)
    c = STEP(I, c, d, a, b, 10, 15, 0xffeff47d, X)
    b = STEP(I, b, c, d, a,  1, 21, 0x85845dd1, X)
    a = STEP(I, a, b, c, d,  8,  6, 0x6fa87e4f, X)
    d = STEP(I, d, a, b, c, 15, 10, 0xfe2ce6e0, X)
    c = STEP(I, c, d, a, b,  6, 15, 0xa3014314, X)
    b = STEP(I, b, c, d, a, 13, 21, 0x4e0811a1, X)
    a = STEP(I, a, b, c, d,  4,  6, 0xf7537e82, X)
    d = STEP(I, d, a, b, c, 11, 10, 0xbd3af235, X)
    c = STEP(I, c, d, a, b,  2, 15, 0x2ad7d2bb, X)
    b = STEP(I, b, c, d, a,  9, 21, 0xeb86d391, X)

    ctx.a = (ctx.a + a) & 0xFFFFFFFF
    ctx.b = (ctx.b + b) & 0xFFFFFFFF
    ctx.c = (ctx.c + c) & 0xFFFFFFFF
    ctx.d = (ctx.d + d) & 0xFFFFFFFF

def md5_init(ctx: MD5Context) -> None:
    assert ctx is not None
    # initialization values from RFC 1321, Section 3.3:
    ctx.a = 0x67452301
    ctx.b = 0xEFCDAB89
    ctx.c = 0x98BADCFE
    ctx.d = 0x10325476

def md5_digest(ctx: MD5Context, buffer: bytes) -> None:
    size = len(buffer)
    message_bits = size * 8
    rem_size = size
    offset = 0

    while rem_size > 64:
        md5_block(ctx, buffer[offset:offset + 64])
        offset += 64
        rem_size -= 64

    scratch = bytearray(64)
    scratch[:rem_size] = buffer[offset:offset + rem_size]

    if rem_size == 64:
        md5_block(ctx, bytes(scratch))
        scratch = bytearray(64)
        scratch[0] = 0x80
    else:
        scratch[rem_size] = 0x80
        if 64 - (rem_size + 1) < 8:
            md5_block(ctx, bytes(scratch))
            scratch = bytearray(64)

    struct.pack_into('<Q', scratch, 56, message_bits)
    md5_block(ctx, bytes(scratch))

def md5_output(ctx: MD5Context) -> bytes:
    return struct.pack('<IIII', ctx.a, ctx.b, ctx.c, ctx.d)
