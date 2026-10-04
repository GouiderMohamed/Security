/*
 * aes.c — AES-128 ECB (chiffrement / déchiffrement)
 * Implémentation conforme FIPS-197.
 * Testé avec le vecteur officiel :
 *   Clé : 2B7E151628AED2A6ABF7158809CF4F3C
 *   PT  : 3243F6A8885A308D313198A2E0370734
 *   CT  : 3925841D02DC09FBDC118597196A0B32
 */

#include "aes.h"
#include <string.h>


/* ------------------------------------------------------------------ */
/*  Multiplication dans GF(2^8) modulo 0x11B                          */
/* ------------------------------------------------------------------ */

uint8_t gf_mul(uint8_t a, uint8_t b) {
    uint8_t res = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) res ^= a;
        uint8_t hi = a & 0x80;
        a <<= 1;
        if (hi) a ^= 0x1B;
        b >>= 1;
    }
    return res;
}

/* ------------------------------------------------------------------ */
/*  Expansion de clé (Key Schedule) — AES-128                         */
/* ------------------------------------------------------------------ */

void aes_key_expansion(const uint8_t key[16], uint8_t round_keys[11][16]) {
    memcpy(round_keys[0], key, 16);

    for (int r = 1; r <= 10; r++) {
        /* RotWord + SubWord sur le dernier mot (octets 12-15) */
        uint8_t temp[4] = {
            SBOX[round_keys[r-1][13]],   /* rotation : 12->13 */
            SBOX[round_keys[r-1][14]],
            SBOX[round_keys[r-1][15]],
            SBOX[round_keys[r-1][12]]
        };
        temp[0] ^= RCON[r];

        /* Génération des 4 mots de la round-key */
        for (int i = 0; i < 4; i++) {
            round_keys[r][i]     = round_keys[r-1][i]     ^ temp[i];
            round_keys[r][4+i]   = round_keys[r-1][4+i]   ^ round_keys[r][i];
            round_keys[r][8+i]   = round_keys[r-1][8+i]   ^ round_keys[r][4+i];
            round_keys[r][12+i]  = round_keys[r-1][12+i]  ^ round_keys[r][8+i];
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Opérations internes sur l'état 4×4                                */
/* ------------------------------------------------------------------ */

 void add_round_key(uint8_t state[4][4], const uint8_t rkey[16]) {
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            state[r][c] ^= rkey[c*4 + r];
}

 void sub_bytes(uint8_t state[4][4], const uint8_t sbox[256]) {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            state[r][c] = sbox[state[r][c]];
}

/* inv=0 : décalage gauche (chiffrement), inv=1 : décalage droit (déchiffrement) */
void shift_rows(uint8_t state[4][4], int inv) {
    uint8_t tmp[4];
    for (int r = 1; r < 4; r++) {
        memcpy(tmp, state[r], 4);
        int shift = inv ? (4 - r) : r;
        for (int c = 0; c < 4; c++)
            state[r][c] = tmp[(c + shift) % 4];
    }
}

/* inv=0 : MixColumns (chiffrement), inv=1 : InvMixColumns (déchiffrement) */
 void mix_columns(uint8_t s[4][4], int inv) {
    for (int c = 0; c < 4; c++) {
        uint8_t s0 = s[0][c], s1 = s[1][c], s2 = s[2][c], s3 = s[3][c];
        if (!inv) {
            s[0][c] = gf_mul(2,s0) ^ gf_mul(3,s1) ^ s2          ^ s3;
            s[1][c] = s0           ^ gf_mul(2,s1) ^ gf_mul(3,s2) ^ s3;
            s[2][c] = s0           ^ s1           ^ gf_mul(2,s2) ^ gf_mul(3,s3);
            s[3][c] = gf_mul(3,s0) ^ s1           ^ s2           ^ gf_mul(2,s3);
        } else {
            s[0][c] = gf_mul(0x0E,s0) ^ gf_mul(0x0B,s1) ^ gf_mul(0x0D,s2) ^ gf_mul(0x09,s3);
            s[1][c] = gf_mul(0x09,s0) ^ gf_mul(0x0E,s1) ^ gf_mul(0x0B,s2) ^ gf_mul(0x0D,s3);
            s[2][c] = gf_mul(0x0D,s0) ^ gf_mul(0x09,s1) ^ gf_mul(0x0E,s2) ^ gf_mul(0x0B,s3);
            s[3][c] = gf_mul(0x0B,s0) ^ gf_mul(0x0D,s1) ^ gf_mul(0x09,s2) ^ gf_mul(0x0E,s3);
        }
    }
}

/* Copie linéaire in[16] → état 4×4 colonne-majeur */
static void bytes_to_state(const uint8_t in[16], uint8_t state[4][4]) {
    for (int i = 0; i < 16; i++) state[i % 4][i / 4] = in[i];
}

/* Copie état 4×4 → out[16] colonne-majeur */
static void state_to_bytes(const uint8_t state[4][4], uint8_t out[16]) {
    for (int i = 0; i < 16; i++) out[i] = state[i % 4][i / 4];
}

/* ------------------------------------------------------------------ */
/*  API publique                                                       */
/* ------------------------------------------------------------------ */

void aes_encrypt(const uint8_t in[16], const uint8_t key[16], uint8_t out[16]) {
    uint8_t rkeys[11][16], state[4][4];

    aes_key_expansion(key, rkeys);
    bytes_to_state(in, state);

    add_round_key(state, rkeys[0]);

    for (int r = 1; r <= 9; r++) {
        sub_bytes(state, SBOX);
        shift_rows(state, 0);
        mix_columns(state, 0);
        add_round_key(state, rkeys[r]);
    }

    /* Dernier round : pas de MixColumns */
    sub_bytes(state, SBOX);
    shift_rows(state, 0);
    add_round_key(state, rkeys[10]);

    state_to_bytes(state, out);
}

void aes_decrypt(const uint8_t in[16], const uint8_t key[16], uint8_t out[16]) {
    uint8_t rkeys[11][16], state[4][4];

    aes_key_expansion(key, rkeys);
    bytes_to_state(in, state);

    add_round_key(state, rkeys[10]);

    for (int r = 9; r >= 1; r--) {
        shift_rows(state, 1);
        sub_bytes(state, INV_SBOX);
        add_round_key(state, rkeys[r]);
        mix_columns(state, 1);
    }

    /* Dernier round inverse : pas de InvMixColumns */
    shift_rows(state, 1);
    sub_bytes(state, INV_SBOX);
    add_round_key(state, rkeys[0]);

    state_to_bytes(state, out);
}
