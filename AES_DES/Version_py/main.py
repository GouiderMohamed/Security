"""
main.py — Tests des algorithmes AES-128 et DES

Vecteurs de test officiels :
    AES (FIPS-197, Annexe B) :
        Clé       : 2B7E151628AED2A6ABF7158809CF4F3C
        Plaintext : 3243F6A8885A308D313198A2E0370734
        CT attendu: 3925841D02DC09FBDC118597196A0B32

    DES (vecteur classique / FIPS-46) :
        Clé       : 133457799BBCDFF1
        Plaintext : 0123456789ABCDEF
        CT attendu: 85E813540F0AB405
"""

import sys
import aes
import des


def print_hex(label: str, data: bytes) -> None:
    print(f"  {label:<16}: {data.hex().upper()}")


# ------------------------------------------------------------------ #
#  Test AES-128                                                       #
# ------------------------------------------------------------------ #

def test_aes() -> bool:
    print("--- [ AES-128 ] ---")

    # Vecteur FIPS-197 Annexe B
    key         = bytes.fromhex("2B7E151628AED2A6ABF7158809CF4F3C")
    pt          = bytes.fromhex("3243F6A8885A308D313198A2E0370734")
    expected_ct = bytes.fromhex("3925841D02DC09FBDC118597196A0B32")

    ct = aes.aes_encrypt(pt,  key)
    rt = aes.aes_decrypt(ct,  key)

    print_hex("Plaintext",  pt)
    print_hex("Ciphertext", ct)
    print_hex("CT attendu", expected_ct)
    print_hex("Déchiffré",  rt)

    ct_ok = ct == expected_ct
    rt_ok = rt == pt

    print(f"  Vecteur CT  : {'OK' if ct_ok else 'ERREUR'}")
    print(f"  Roundtrip   : {'OK' if rt_ok else 'ERREUR'}")
    print()
    return ct_ok and rt_ok


# ------------------------------------------------------------------ #
#  Test DES                                                           #
# ------------------------------------------------------------------ #

def test_des() -> bool:
    print("--- [ DES ] ---")

    # Vecteur classique
    key         = bytes.fromhex("133457799BBCDFF1")
    pt          = bytes.fromhex("0123456789ABCDEF")
    expected_ct = bytes.fromhex("85E813540F0AB405")

    ct = des.des_process(pt, key, decrypt=False)
    rt = des.des_process(ct, key, decrypt=True)

    print_hex("Plaintext",  pt)
    print_hex("Ciphertext", ct)
    print_hex("CT attendu", expected_ct)
    print_hex("Déchiffré",  rt)

    ct_ok = ct == expected_ct
    rt_ok = rt == pt

    print(f"  Vecteur CT  : {'OK' if ct_ok else 'ERREUR'}")
    print(f"  Roundtrip   : {'OK' if rt_ok else 'ERREUR'}")
    print()
    return ct_ok and rt_ok


# ------------------------------------------------------------------ #
#  Point d'entrée                                                     #
# ------------------------------------------------------------------ #

if __name__ == "__main__":
    print("=== TEST DES ALGORITHMES DE CRYPTOGRAPHIE ===\n")

    results = [test_aes(), test_des()]
    failures = results.count(False)

    if failures == 0:
        print("Tous les tests ont réussi.")
    else:
        print(f"{failures} test(s) en ECHEC.")
        sys.exit(1)
