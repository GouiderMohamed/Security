# 🔐 Sécurité des Systèmes Embarqués — AES, DES, MD5 & Correction d'Erreurs

Projet de TP réalisé dans le cadre du module **Sécurité des systèmes embarqués**.
Il regroupe des implémentations maison (en **C** et en **Python**) d'algorithmes
cryptographiques, de mécanismes de **détection/correction d'erreurs**, ainsi que
**deux démonstrations complètes** : un scénario d'attaque *Man-in-the-Middle*
conteneurisé avec Docker, et un protocole de *défi cryptographique* entre une
carte **ESP32** et un serveur Python.

> 🎓 **Auteur** : Gouider Mohamed — Option *Systèmes Embarqués & IOT*
> 📄 Le rapport complet est disponible dans [`Rapport_Finale.pdf`](Rapport_Finale.pdf)

---

## 📑 Table des matières

- [Objectifs pédagogiques](#-objectifs-pédagogiques)
- [Algorithmes implémentés](#-algorithmes-implémentés)
- [Vecteurs de test](#-vecteurs-de-test)
- [Architecture du dépôt](#-architecture-du-dépôt)
- [Prérequis](#-prérequis)
- [Démo 1 — Attaque Man-in-the-Middle (Docker)](#-démo-1--attaque-man-in-the-middle-docker)
- [Démo 2 — Défi cryptographique ESP32 ↔ Serveur](#-démo-2--défi-cryptographique-esp32--serveur)
- [Lancer les tests directement](#-lancer-les-tests-directement)
- [Démonstrations vidéo](#-démonstrations-vidéo)
- [📄 Documentation](#-documentation)
- [⚠️ Avertissements de sécurité](#️-avertissements-de-sécurité)
- [Structure du code](#-structure-du-code)
- [Licence](#-licence)

---

## 🎯 Objectifs pédagogiques

Ce TP met en pratique les notions suivantes :

| Domaine | Contenu |
|---|---|
| **Confidentialité** | Chiffrement symétrique **AES-128** (FIPS-197) et **DES** (FIPS-46-3) |
| **Intégrité** | Empreinte **MD5** (RFC 1321) |
| **Détection d'erreurs** | **CRC-8** (polynôme `0x07`) |
| **Correction d'erreurs** | **Code de Hamming (7,4)** et **Code de répétition (3)** |
| **Réseau / IoT** | Client HTTP sur **ESP32**, serveur REST en Python |
| **Virtualisation** | Orchestration de 3 conteneurs avec **Docker Compose** |
| **Attaque** | Simulation d'un **MitM** par injection d'erreur binaire |

---

## 🔐 Algorithmes implémentés

### AES-128 (Advanced Encryption Standard)

Implémentation complète des 10 rondes de l'algorithme, sans aucune dépendance
externe (aucune utilisation de `pycryptodome` / `openssl`).

**Version C** — `AES_DES/Version_c/aes.c`

| Fonction | Rôle |
|---|---|
| `gf_mul()` | Multiplication dans le corps de Galois `GF(2⁸)` |
| `aes_key_expansion()` | Expansion de la clé en 11 clés de ronde |
| `add_round_key()` | Addition de la clé de ronde (XOR) |
| `sub_bytes()` | Substitution via la S-Box |
| `shift_rows()` | Décalage des lignes (paramétrable pour l'inverse) |
| `mix_columns()` | Mélange des colonnes (matrices de MDS) |
| `aes_encrypt()` / `aes_decrypt()` | Chiffrement / Déchiffrement |

**Version Python** — `AES_DES/Version_py/aes.py` (`aes_encrypt` / `aes_decrypt`)

### DES (Data Encryption Standard)

Structure de **Feistel** en 16 rondes avec S-Boxes et tables de permutation
issues de la norme **FIPS-46-3**.

| Fonction | Rôle |
|---|---|
| `des_generate_subkeys()` | Génération des 16 sous-clés de 48 bits |
| `permute()` | Application d'une table de permutation |
| `left_shift_28()` | Décalage à gauche du registre de 28 bits |
| `feistel_f()` | Fonction de Feistel (expansion + S-Box + permutation) |
| `des_process()` | Chiffrement **et** déchiffrement (`idx = decrypt ? (15 - r) : r`) |

### MD5 (Message-Digest Algorithm 5)

Hachage 128 bits — `MD5/MD5_versionC/md5.c` et `MD5/MD5_VersionPy/md5.py`,
décomposé en `md5_init()`, `md5_digest()` et `md5_output()`.

### Correction / Détection d'erreurs

| Fichier | Principe | Capacité |
|---|---|---|
| `Correction_Err/CRC.c` | CRC-8, polynôme `0x07` | **Détecte** toute erreur simple |
| `Correction_Err/Hamming.c` | Hamming (7,4) — `encoder_hamming()` + calcul du *syndrome* | **Corrige** 1 bit, **détecte** 2 bits |
| `Correction_Err/Repetition.c` | Répétition ×3 + `vote_majoritaire()` | **Corrige** 1 bit par bloc |

---

## 🧪 Vecteurs de test

Les implémentations sont validées contre les vecteurs officiels des normes.

### AES-128 — FIPS-197, Annexe B

```
Clé        : 2B7E151628AED2A6ABF7158809CF4F3C
Plaintext  : 3243F6A8885A308D313198A2E0370734
Ciphertext : 3925841D02DC09FBDC118597196A0B32   ← attendu
```

### DES — vecteur classique (FIPS-46-3)

```
Clé        : 133457799BBCDFF1
Plaintext  : 0123456789ABCDEF
Ciphertext : 85E813540F0AB405   ← attendu
```

### Sortie attendue

```
=== TEST DES ALGORITHMES DE CRYPTOGRAPHIE ===

--- [ AES-128 ] ---
  Plaintext     : 32 43 F6 A8 88 5A 30 8D 31 31 98 A2 E0 37 07 34
  Ciphertext    : 39 25 84 1D 02 DC 09 FB DC 11 85 97 19 6A 0B 32
  CT attendu    : 39 25 84 1D 02 DC 09 FB DC 11 85 97 19 6A 0B 32
  Déchiffré     : 32 43 F6 A8 88 5A 30 8D 31 31 98 A2 E0 37 07 34
  Vecteur CT  : OK
  Roundtrip   : OK

--- [ DES ] ---
  ...
Tous les tests ont réussi.
```

---

## 📂 Architecture du dépôt

```
Securité/
├── AES_DES/                       # Chiffrement symétrique
│   ├── Version_c/                  # Implémentation C
│   │   ├── aes.c / aes.h
│   │   ├── des.c / des.h
│   │   └── main.c                  # Tests des vecteurs FIPS
│   └── Version_py/                 # Implémentation Python
│       ├── aes.py
│       ├── des.py
│       └── main.py                 # Tests des vecteurs FIPS
│
├── MD5/                            # Hachage / intégrité
│   ├── MD5_versionC/
│   │   ├── md5.c / md5.h
│   │   └── md5_test.c
│   └── MD5_VersionPy/
│       ├── md5.py
│       └── md5_test.py
│
├── Correction_Err/                 # Détection & correction d'erreurs
│   ├── CRC.c                       # CRC-8
│   ├── Hamming.c                   # Hamming (7,4)
│   └── Repetition.c                # Répétition ×3
│
├── Esp32/
│   └── Esp32.ino                   # Client HTTP du protocole de défi
│
├── attaquant.py                    # Script d'injection d'erreur (MitM)
├── emetteur_app.py                 # Émetteur (chiffrement + MD5)
├── serveur_jeu.py                  # Serveur HTTP de déchiffrement (port 8082)
│
├── docker-compose.yml              # Réseau + 3 conteneurs isolés
├── run.bat                         # Automatisation de la Démo 1
│
├── Rapport_Finale.pdf              # Rapport complet du TP
├── Structuration de rapport securite.txt
├── Un_guide_utile_test.txt         # Guide de déploiement détaillé
├── vid_Docker.mp4                  # Capture de la Démo 1
└── vid_ESP32.mp4                   # Capture de la Démo 2
```

---

## 🛠️ Prérequis

| Outil | Version | Usage |
|---|---|---|
| **Python** | ≥ 3.8 | Serveur HTTP et implémentations Python |
| **Docker** + **Docker Compose** | récent | Démo 1 (3 conteneurs) |
| **GCC** / MinGW-w64 | ≥ 8 | Compilation des sources C |
| **Arduino IDE** | ≥ 2.x | Programmation de la carte ESP32 |
| **Moniteur Série** | 115200 bauds | Visualisation des logs ESP32 |

> Aucune dépendance Python externe : le projet n'utilise que la bibliothèque
> standard (`http.server`, `json`, `sys`, `os`).

---

## 🐳 Démo 1 — Attaque Man-in-the-Middle (Docker)

### Principe

Trois conteneurs partagent le volume du projet via un réseau bridge isolé
`crypto_net` :

| Conteneur | Rôle | Action |
|---|---|---|
| `emetteur` | Émet la donnée | Écrit l'octet `0xD2` dans `trame_saine.bin` |
| `attaquant` | Intercepte & corrompt | Applique `XOR 0x01` → `0xD3`, écrit `trame_corrompue.bin` |
| `recepteur` | Contrôle l'intégrité | Compile `CRC.c` et calcule le CRC-8 pour détecter l'erreur |

### Exécution automatisée (recommandé)

Depuis un terminal **CMD** à la racine du dépôt :

```bash
run.bat
```

Le script enchaîne automatiquement : `docker-compose up -d` → émission →
interception → compilation + CRC → `docker-compose down`.

### Exécution manuelle (étape par étape)

```bash
# 1. Démarrer l'infrastructure
docker-compose up -d

# 2. L'émetteur génère la trame saine
docker exec -it emetteur python -c "with open('trame_saine.bin', 'wb') as f: f.write(bytearray([0xD2]))"

# 3. L'attaquant intercepte et corrompt la trame
docker exec -it attaquant python attaquant.py

# 4. Le récepteur compile et vérifie le CRC-8
docker exec -it recepteur sh -c "gcc Correction_Err/CRC.c -o verif_crc && ./verif_crc"

# 5. Nettoyer
docker-compose down
```

### Résultat attendu

```
[ATTAQUANT] Interception de la trame. Donnée originale : 0xd2
[ATTAQUANT] Injection d'erreur réussie. Nouvelle donnée : 0xd3

Message original : 0xD2
Checksum calculé : 0xAD
--- Transmission avec 1 erreur ---
Message reçu     : 0xD3 (Erreur au bit 0)
Nouveau checksum : 0xAC
RESULTAT : Erreur DETECTEE par le CRC !
```

---

## 📡 Démo 2 — Défi cryptographique ESP32 ↔ Serveur

### Principe

1. L'**ESP32** sélectionne aléatoirement l'un des 3 blocs chiffrés stockés en
   mémoire flash et l'envoie en **HTTP POST** (JSON).
2. Le **serveur Python** déchiffre le bloc avec l'implémentation `aes.py`
   (clé maîtresse `2B7E151628AED2A6ABF7158809CF4F3C`) et renvoie le résultat en
   hexadécimal.
3. L'**ESP32** compare la réponse au bloc clair attendu et affiche
   `GAGNÉ !` ou `ÉCHEC !`.

### Étape 1 — Récupérer l'adresse IP du PC

```bash
ipconfig
```
Notez votre IPv4 locale, par exemple `192.168.1.36`.

### Étape 2 — Lancer le serveur

```bash
python serveur_jeu.py
```

> Le serveur écoute sur le port **8082** (`0.0.0.0:8082`).
> Si votre pare-feu le demande, autorisez Python sur le réseau *privé*.

### Étape 3 — Configurer et flasher l'ESP32

Dans [`Esp32/Esp32.ino`](Esp32/Esp32.ino), modifiez les paramètres réseau :

```cpp
const char* ssid     = "VOTRE_RESEAU_WIFI";
const char* password = "VOTRE_MOT_DE_PASSE";
const char* serverUrl = "http://192.168.1.36:8082/";  // IP du PC + port du serveur
```

Puis :
1. Ouvrir le sketch dans l'**Arduino IDE**.
2. **Vérifier** la carte sélectionnée.
3. **Téléverser** (upload).
4. Ouvrir le **Moniteur Série** à **115200 bauds**.

### Étape 4 — Validation

Sortie attendue côté **serveur** :

```
[SERVEUR] Défi Ciphertext reçu : 60A90FBE6C6F2B8D8F492E38DFE1097F
[SERVEUR] Bloc déchiffré avec votre code (HEX) : '0072008582483DF4F4E59486E6B51035'
```

Sortie attendue côté **ESP32** :

```
[ESP32] Transmission du bloc chiffré : 60A90FBE6C6F2B8D8F492E38DFE1097F
[ESP32] Réponse brute du serveur : '0072008582483DF4F4E59486E6B51035'
[ESP32] RÉSULTAT : GAGNÉ ! Le serveur a déchiffré correctement avec votre code AES-128.
```

Le défi est rejoué toutes les **6 secondes** avec un bloc différent.

### Test du serveur sans carte (curl / PowerShell)

```powershell
Invoke-RestMethod -Uri "http://localhost:8082/" -Method Post `
  -Body '{"ciphertext":"60A90FBE6C6F2B8D8F492E38DFE1097F"}' `
  -ContentType "application/json"
# -> { status: success, decrypted_word: 0072008582483DF4F4E59486E6B51035 }
```

---

## 🧪 Lancer les tests directement

### Implémentations Python

```bash
cd AES_DES/Version_py
python main.py            # Tests AES-128 + DES

cd ../../MD5/MD5_VersionPy
python md5_test.py        # Tests MD5
```

### Implémentations C

```bash
# AES + DES
gcc AES_DES/Version_c/main.c AES_DES/Version_c/aes.c AES_DES/Version_c/des.c -o test_crypto -I AES_DES/Version_c
./test_crypto

# MD5
gcc MD5/MD5_versionC/md5_test.c MD5/MD5_versionC/md5.c -o test_md5 -I MD5/MD5_versionC
./test_md5

# Codes de correction d'erreurs
gcc Correction_Err/CRC.c        -o verif_crc   && ./verif_crc
gcc Correction_Err/Hamming.c    -o verif_ham   && ./verif_ham
gcc Correction_Err/Repetition.c -o verif_rep   && ./verif_rep
```

> Sous **VS Code**, une tâche de build GCC est déjà configurée dans
> [`.vscode/tasks.json`](.vscode/tasks.json) (`Ctrl+Shift+B`).

---

## 🎬 Démonstrations vidéo

| Fichier | Contenu |
|---|---|
| [`vid_Docker.mp4`](vid_Docker.mp4) | Scénario MitM + détection CRC dans Docker |
| [`vid_ESP32.mp4`](vid_ESP32.mp4) | Protocole de défi ESP32 ↔ serveur Python |

---

## 📄 Documentation

| Document | Description |
|---|---|
| [`Rapport_Finale.pdf`](Rapport_Finale.pdf) | Rapport complet du TP (analyse, listings, captures) |
| [`Un_guide_utile_test.txt`](Un_guide_utile_test.txt) | Guide de déploiement pas-à-pas |
| [`Structuration de rapport securite.txt`](Structuration%20de%20rapport%20securite.txt) | Plan et notes de rédaction |

---

## ⚠️ Avertissements de sécurité

> Ce dépôt est un **projet pédagogique**. Veuillez lire ces avertissements
> avant tout redéploiement.

- 🔑 **Identifiants Wi-Fi codés en dur** — `Esp32/Esp32.ino` contient un SSID et
  un mot de passe réels (`ssid = "HG"`, `password = "12345678"`).
  **Changez-les et갹ez-les dans un fichier `secrets.h` ignoré par Git** si vous
  publiez ce dépôt publiquement.
- 🔐 **Clé AES en clair** — la clé maîtresse `2B7E151628AED2A6ABF7158809CF4F3C`
  est présente dans `serveur_jeu.py` et dans le sketch ESP32. En production,
  utilisez un échange de clé (Diffie-Hellman / TLS) au lieu d'une clé statique.
- 🌍 **HTTP en clair** — l'échange ESP32 → serveur n'est pas chiffré en transit.
  Utilisez `HTTPS` (TLS) pour tout déploiement réel.
- 🧪 **Algorithmes à usage pédagogique uniquement** — DES (clé 56 bits) et MD5
  sont aujourd'hui **considérés comme cassés**. AES-128 et les codes de
  correction d'erreurs restent valides, mais n'utilisez pas MD5 pour de la
  signature ou de l'authentification (préférez SHA-256).
- 🧱 **`docker-compose.yml` monte le projet en lecture/écriture dans les
  conteneurs** — uniquement pour les besoins de la démo, jamais en production.

### Fichiers qui ne devraient pas être versionnés

Un `.gitignore` est fourni ; il exclut notamment :

```
__pycache__/
*.pyc
*.exe
trame_saine.bin
trame_corrompue.bin
```

---

## 🧱 Structure du code

```
AES_DES/Version_c/aes.c
├── gf_mul()                multiplication GF(2⁸)
├── aes_key_expansion()     11 clés de ronde
├── aes_encrypt()           10 rondes : AddRoundKey → SubBytes → ShiftRows → MixColumns
└── aes_decrypt()           10 rondes inversées + MixColumns inverse

AES_DES/Version_c/des.c
├── des_generate_subkeys()  16 sous-clés via left_shift_28()
├── feistel_f()             expansion R + S-Box + permutation
└── des_process()           Feistel 16 rondes (chiffrement et déchiffrement)

serveur_jeu.py
├── SHARED_KEY              clé AES-128 maîtresse
├── do_POST()               reception JSON {"ciphertext": "..."}
└── aes.aes_decrypt()       déchiffrement + réponse {"decrypted_word": "..."}
```

---

## 📜 Licence

Projet à vocation **éducative**.

```
MIT License

Copyright (c) 2025 Gouider Mohamed

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

> Remplacez cette section par la licence de votre choix (MIT, Apache-2.0,
> GPL-3.0…) et ajoutez un fichier `LICENSE` correspondant.

---

## 📚 Références

| Standard | Objet |
|---|---|
| **FIPS-197** | Advanced Encryption Standard (AES) |
| **FIPS-46-3** | Data Encryption Standard (DES) |
| **RFC 1321** | MD5 Message-Digest Algorithm |
| **Hamming, 1950** | Codes correcteurs d'erreurs |
| Documentation Arduino | `WiFi.h`, `HTTPClient.h` |