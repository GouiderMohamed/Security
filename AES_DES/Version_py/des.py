"""
des.py — DES ECB (chiffrement / déchiffrement)
Implémentation conforme FIPS-46-3. Aucune dépendance externe.

Vecteur de test classique :
    Clé       : 133457799BBCDFF1
    Plaintext : 0123456789ABCDEF
    Ciphertext: 85E813540F0AB405


"""

# ------------------------------------------------------------------ #
#  Tables de constantes (FIPS-46-3)                                  #
# ------------------------------------------------------------------ #

PC1 = [
    57,49,41,33,25,17, 9, 1,58,50,42,34,26,18,
    10, 2,59,51,43,35,27,19,11, 3,60,52,44,36,
    63,55,47,39,31,23,15, 7,62,54,46,38,30,22,
    14, 6,61,53,45,37,29,21,13, 5,28,20,12, 4,
]

PC2 = [
    14,17,11,24, 1, 5, 3,28,15, 6,21,10,
    23,19,12, 4,26, 8,16, 7,27,20,13, 2,
    41,52,31,37,47,55,30,40,51,45,33,48,
    44,49,39,56,34,53,46,42,50,36,29,32,
]

IP = [
    58,50,42,34,26,18,10, 2,60,52,44,36,28,20,12, 4,
    62,54,46,38,30,22,14, 6,64,56,48,40,32,24,16, 8,
    57,49,41,33,25,17, 9, 1,59,51,43,35,27,19,11, 3,
    61,53,45,37,29,21,13, 5,63,55,47,39,31,23,15, 7,
]

FP = [
    40, 8,48,16,56,24,64,32,39, 7,47,15,55,23,63,31,
    38, 6,46,14,54,22,62,30,37, 5,45,13,53,21,61,29,
    36, 4,44,12,52,20,60,28,35, 3,43,11,51,19,59,27,
    34, 2,42,10,50,18,58,26,33, 1,41, 9,49,17,57,25,
]

EP = [
    32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
     8, 9,10,11,12,13,12,13,14,15,16,17,
    16,17,18,19,20,21,20,21,22,23,24,25,
    24,25,26,27,28,29,28,29,30,31,32, 1,
]

PBOX = [
    16, 7,20,21,29,12,28,17, 1,15,23,26, 5,18,31,10,
     2, 8,24,14,32,27, 3, 9,19,13,30, 6,22,11, 4,25,
]

SHIFT_SCHEDULE = [1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1]

SBOX_DES = [
    # S1
    [[14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7],
     [ 0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8],
     [ 4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0],
     [15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13]],
    # S2
    [[15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10],
     [ 3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5],
     [ 0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15],
     [13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9]],
    # S3
    [[10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8],
     [13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1],
     [13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7],
     [ 1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12]],
    # S4
    [[ 7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15],
     [13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9],
     [10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4],
     [ 3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14]],
    # S5
    [[ 2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9],
     [14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6],
     [ 4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14],
     [11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3]],
    # S6
    [[12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11],
     [10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8],
     [ 9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6],
     [ 4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13]],
    # S7
    [[ 4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1],
     [13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6],
     [ 1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2],
     [ 6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12]],
    # S8
    [[13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7],
     [ 1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2],
     [ 7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8],
     [ 2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11]],
]

# ------------------------------------------------------------------ #
#  Utilitaires bits / octets                                          #
# ------------------------------------------------------------------ #

def _bytes_to_bits(data: bytes) -> list[int]:
    """Convertit des octets en liste de bits (MSB en premier)."""
    bits = []
    for byte in data:
        for i in range(7, -1, -1):
            bits.append((byte >> i) & 1)
    return bits


def _bits_to_bytes(bits: list[int]) -> bytes:
    """Convertit une liste de bits en octets (MSB en premier)."""
    result = bytearray()
    for i in range(0, len(bits), 8):
        byte = 0
        for bit in bits[i:i + 8]:
            byte = (byte << 1) | bit
        result.append(byte)
    return bytes(result)


def _permute(table: list[int], data: list[int]) -> list[int]:
    """Applique une permutation (index 1-based)."""
    return [data[i - 1] for i in table]


def _left_shift(bits: list[int], n: int) -> list[int]:
    """Rotation à gauche de n positions."""
    return bits[n:] + bits[:n]

# ------------------------------------------------------------------ #
#  Génération des 16 sous-clés                                        #
# ------------------------------------------------------------------ #

def _des_generate_subkeys(key_bytes: bytes) -> list[list[int]]:
    key_bits = _bytes_to_bits(key_bytes)
    key56 = _permute(PC1, key_bits)

    C = key56[:28]
    D = key56[28:]

    subkeys = []
    for r in range(16):
        C = _left_shift(C, SHIFT_SCHEDULE[r])
        D = _left_shift(D, SHIFT_SCHEDULE[r])
        subkeys.append(_permute(PC2, C + D))
    return subkeys

# ------------------------------------------------------------------ #
#  Fonction de Feistel f(R, K)                                        #
# ------------------------------------------------------------------ #

def _feistel_f(R: list[int], subkey: list[int]) -> list[int]:
    expanded = _permute(EP, R)
    xored = [expanded[i] ^ subkey[i] for i in range(48)]

    sbox_out = []
    for i in range(8):
        block = xored[i * 6:(i + 1) * 6]
        row = (block[0] << 1) | block[5]
        col = (block[1] << 3) | (block[2] << 2) | (block[3] << 1) | block[4]
        val = SBOX_DES[i][row][col]
        for b in range(3, -1, -1):
            sbox_out.append((val >> b) & 1)

    return _permute(PBOX, sbox_out)

# ------------------------------------------------------------------ #
#  API publique                                                       #
# ------------------------------------------------------------------ #

def des_process(input_bytes: bytes, key_bytes: bytes, decrypt: bool = False) -> bytes:
  
    if len(input_bytes) != 8:
        raise ValueError(
            f"DES opère sur des blocs de 8 octets exactement (reçu {len(input_bytes)})"
        )
    if len(key_bytes) != 8:
        raise ValueError(
            f"La clé DES doit faire 8 octets (reçu {len(key_bytes)})"
        )
    subkeys = _des_generate_subkeys(key_bytes)
    bits    = _bytes_to_bits(input_bytes)
    ip_bits = _permute(IP, bits)
    L = ip_bits[:32]
    R = ip_bits[32:]
    for r in range(16):
        idx = (15 - r) if decrypt else r
        f_res  = _feistel_f(R, subkeys[idx])
        next_R = [L[i] ^ f_res[i] for i in range(32)]
        L = R
        R = next_R
    final_bits = _permute(FP, R + L)
    return _bits_to_bytes(final_bits)
