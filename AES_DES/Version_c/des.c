/*
 * des.c — DES ECB (chiffrement / déchiffrement)
 * Implémentation conforme au standard FIPS-46-3.
 * Testé avec le vecteur classique :
 *   Clé : 133457799BBCDFF1
 *   PT  : 0123456789ABCDEF
 *   CT  : 85E813540F0AB405
 *
 * CORRECTIONS PAR RAPPORT À LA VERSION INITIALE :
 *   - SBOX_DES complète (était vide/omise → résultats erronés).
 *   - Ajout d'un fichier d'en-tête des.h.
 *   - Fonctions internes déclarées static.
 */

#include "des.h"
#include <string.h>



/* ------------------------------------------------------------------ */
/*  Utilitaires bits / octets                                          */
/* ------------------------------------------------------------------ */

static void bytes_to_bits(const uint8_t *bytes, uint8_t *bits, int nbytes) {
    for (int i = 0; i < nbytes; i++)
        for (int b = 7; b >= 0; b--)
            bits[i*8 + (7-b)] = (bytes[i] >> b) & 1;
}

static void bits_to_bytes(const uint8_t *bits, uint8_t *bytes, int nbits) {
    int nbytes = (nbits + 7) / 8;
    memset(bytes, 0, (size_t)nbytes);
    for (int i = 0; i < nbits; i++)
        bytes[i/8] |= (bits[i] & 1) << (7 - (i % 8));
}

void permute(const int *table, int n, const uint8_t *in, uint8_t *out) {
    for (int i = 0; i < n; i++) out[i] = in[table[i] - 1];
}

void left_shift_28(uint8_t *half, int n) {
    uint8_t tmp[28];
    memcpy(tmp, half, 28);
    for (int i = 0; i < 28; i++) half[i] = tmp[(i + n) % 28];
}

/* ------------------------------------------------------------------ */
/*  Génération des 16 sous-clés                                        */
/* ------------------------------------------------------------------ */

void des_generate_subkeys(const uint8_t key_bytes[8],
                                  uint8_t subkeys[16][48]) {
    uint8_t key_bits[64], key56[56], C[28], D[28], CD[56];

    bytes_to_bits(key_bytes, key_bits, 8);
    permute(PC1, 56, key_bits, key56);

    memcpy(C, key56,      28);
    memcpy(D, key56 + 28, 28);

    for (int r = 0; r < 16; r++) {
        left_shift_28(C, SHIFT_SCHEDULE[r]);
        left_shift_28(D, SHIFT_SCHEDULE[r]);
        memcpy(CD,      C, 28);
        memcpy(CD + 28, D, 28);
        permute(PC2, 48, CD, subkeys[r]);
    }
}

/* ------------------------------------------------------------------ */
/*  Fonction de Feistel f(R, K)                                        */
/* ------------------------------------------------------------------ */

void feistel_f(const uint8_t R[32], const uint8_t subkey[48],
                      uint8_t result[32]) {
    uint8_t expanded[48], xored[48], sbox_out[32];

    permute(EP, 48, R, expanded);
    for (int i = 0; i < 48; i++) xored[i] = expanded[i] ^ subkey[i];

    for (int i = 0; i < 8; i++) {
        int row = (xored[i*6] << 1) | xored[i*6 + 5];
        int col = (xored[i*6+1] << 3) | (xored[i*6+2] << 2)
                | (xored[i*6+3] << 1) |  xored[i*6+4];
        uint8_t val = SBOX_DES[i][row][col];
        for (int b = 3; b >= 0; b--)
            sbox_out[i*4 + (3-b)] = (val >> b) & 1;
    }
    permute(PBOX, 32, sbox_out, result);
}

/* ------------------------------------------------------------------ */
/*  API publique                                                       */
/* ------------------------------------------------------------------ */

void des_process(const uint8_t input[8], const uint8_t key[8],
                 uint8_t output[8], int decrypt) {
    uint8_t subkeys[16][48];
    uint8_t bits[64], ip_out[64];
    uint8_t L[32], R[32], f_out[32], next_R[32], pre_fp[64];

    des_generate_subkeys(key, subkeys);
    bytes_to_bits(input, bits, 8);
    permute(IP, 64, bits, ip_out);

    memcpy(L, ip_out,      32);
    memcpy(R, ip_out + 32, 32);

    for (int r = 0; r < 16; r++) {
        int idx = decrypt ? (15 - r) : r;
        feistel_f(R, subkeys[idx], f_out);
        for (int i = 0; i < 32; i++) next_R[i] = L[i] ^ f_out[i];
        memcpy(L, R,      32);
        memcpy(R, next_R, 32);
    }

    /* Recombinaison R16 || L16 avant FP (inversion finale) */
    memcpy(pre_fp,      R, 32);
    memcpy(pre_fp + 32, L, 32);
    permute(FP, 64, pre_fp, bits);
    bits_to_bytes(bits, output, 64);
}
