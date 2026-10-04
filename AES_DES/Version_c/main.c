/*
 * main.c — Tests des algorithmes AES-128 et DES
 *
 * Vecteurs de test officiels :
 *   AES (FIPS-197 Appendix B) :
 *     Clé       : 2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C
 *     Plaintext : 32 43 F6 A8 88 5A 30 8D 31 31 98 A2 E0 37 07 34
 *     Ciphertext: 39 25 84 1D 02 DC 09 FB DC 11 85 97 19 6A 0B 32  ← attendu
 *
 *   DES (vecteur classique) :
 *     Clé       : 13 34 57 79 9B BC DF F1
 *     Plaintext : 01 23 45 67 89 AB CD EF
 *     Ciphertext: 85 E8 13 54 0F 0A B4 05  ← attendu
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "aes.h"
#include "des.h"

/* ------------------------------------------------------------------ */
/*  Utilitaire d'affichage                                             */
/* ------------------------------------------------------------------ */

static void print_hex(const char *title, const uint8_t *data, int len) {
    printf("  %-14s: ", title);
    for (int i = 0; i < len; i++) printf("%02X ", data[i]);
    printf("\n");
}

/* ------------------------------------------------------------------ */
/*  Test AES-128                                                       */
/* ------------------------------------------------------------------ */

static int test_aes(void) {
    printf("--- [ AES-128 ] ---\n");

    /* Vecteur FIPS-197 Annexe B */
    const uint8_t key[16] = {
        0x2B,0x7E,0x15,0x16,0x28,0xAE,0xD2,0xA6,
        0xAB,0xF7,0x15,0x88,0x09,0xCF,0x4F,0x3C
    };
    const uint8_t pt[16] = {
        0x32,0x43,0xF6,0xA8,0x88,0x5A,0x30,0x8D,
        0x31,0x31,0x98,0xA2,0xE0,0x37,0x07,0x34
    };
    const uint8_t expected_ct[16] = {
        0x39,0x25,0x84,0x1D,0x02,0xDC,0x09,0xFB,
        0xDC,0x11,0x85,0x97,0x19,0x6A,0x0B,0x32
    };

    uint8_t ct[16], rt[16];

    aes_encrypt(pt,  key, ct);
    aes_decrypt(ct,  key, rt);

    print_hex("Plaintext",    pt,  16);
    print_hex("Ciphertext",   ct,  16);
    print_hex("CT attendu",   expected_ct, 16);
    print_hex("Déchiffré",    rt,  16);

    int ct_ok = (memcmp(ct, expected_ct, 16) == 0);
    int rt_ok = (memcmp(rt, pt,          16) == 0);

    printf("  Vecteur CT  : %s\n", ct_ok ? "OK" : "ERREUR");
    printf("  Roundtrip   : %s\n\n", rt_ok ? "OK" : "ERREUR");

    return ct_ok && rt_ok ? 0 : 1;
}

/* ------------------------------------------------------------------ */
/*  Test DES                                                           */
/* ------------------------------------------------------------------ */

static int test_des(void) {
    printf("--- [ DES ] ---\n");

    /* Vecteur classique (aussi présent dans FIPS-46 / Schneier) */
    const uint8_t key[8] = {
        0x13,0x34,0x57,0x79,0x9B,0xBC,0xDF,0xF1
    };
    const uint8_t pt[8] = {
        0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF
    };
    const uint8_t expected_ct[8] = {
        0x85,0xE8,0x13,0x54,0x0F,0x0A,0xB4,0x05
    };

    uint8_t ct[8], rt[8];

    des_process(pt,  key, ct, 0);   /* chiffrement */
    des_process(ct,  key, rt, 1);   /* déchiffrement */

    print_hex("Plaintext",  pt, 8);
    print_hex("Ciphertext", ct, 8);
    print_hex("CT attendu", expected_ct, 8);
    print_hex("Déchiffré",  rt, 8);

    int ct_ok = (memcmp(ct, expected_ct, 8) == 0);
    int rt_ok = (memcmp(rt, pt,          8) == 0);

    printf("  Vecteur CT  : %s\n", ct_ok ? "OK" : "ERREUR");
    printf("  Roundtrip   : %s\n\n", rt_ok ? "OK" : "ERREUR");

    return ct_ok && rt_ok ? 0 : 1;
}

/* ------------------------------------------------------------------ */
/*  Point d'entrée                                                     */
/* ------------------------------------------------------------------ */

int main(void) {
    printf("=== TEST DES ALGORITHMES DE CRYPTOGRAPHIE ===\n\n");

    int failures = 0;
    failures += test_aes();
    failures += test_des();

    if (failures == 0)
        printf("Tous les tests ont réussi.\n");
    else
        printf("%d test(s) en ECHEC.\n", failures);

    return failures;
}
