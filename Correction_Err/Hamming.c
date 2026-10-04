#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Calcule les bits de parité pour un message de 4 bits (d1, d2, d3, d4)
 * Indices Hamming (1-based) :
 * p1: bits 1, 3, 5, 7
 * p2: bits 2, 3, 6, 7
 * p3: bits 4, 5, 6, 7
 */
 /**
 * Logique du Code de Hamming  :
 * ---------------------------------
 * On place les bits de parité (p) aux puissances de 2 et les données (d) aux autres places.
 * Position (1-based) :  1    2    3    4    5    6    7
 * Type de bit        : p1   p2   d1   p3   d2   d3   d4
 * Indice C (0-based) : [0]  [1]  [2]  [3]  [4]  [5]  [6]
 *
 * Calcul des parités (basé sur la position binaire) :
 * - p1 (pos 1 : 001) surveille les positions avec le bit de poids faible à 1 :
 * Positions 1, 3, 5, 7  =>  p1 = d1 ^ d2 ^ d4
 * - p2 (pos 2 : 010) surveille les positions avec le 2ème bit à 1 :
 * Positions 2, 3, 6, 7  =>  p2 = d1 ^ d3 ^ d4
 * - p3 (pos 4 : 100) surveille les positions avec le 3ème bit à 1 :
 * Positions 4, 5, 6, 7  =>  p3 = d2 ^ d3 ^ d4
 */

void encoder_hamming(int data[4], int code[7]) {
    code[2] = data[0]; // d1
    code[4] = data[1]; // d2
    code[5] = data[2]; // d3
    code[6] = data[3]; // d4

    code[0] = code[2] ^ code[4] ^ code[6]; // p1
    code[1] = code[2] ^ code[5] ^ code[6]; // p2
    code[3] = code[4] ^ code[5] ^ code[6]; // p3
}

int main() {
    int data[4] = {1, 1, 0, 1}; // Message original
    int code[7];
    int recu[7];
    srand(time(NULL));

    // 1. Encodage
    encoder_hamming(data, code);
    printf("Message Recu : ");
    for(int i=0; i<7; i++) 
    {
        recu[i] = code[i];
        printf("%d",recu[i]);
    }
    printf("\n");
    // 2. Simulation d'erreurs (changez nb_erreurs à 2 pour tester)
    int nb_erreurs = 1; 
    printf("--- Simulation : %d erreur(s) ---\n", nb_erreurs);
    for(int i=0; i<nb_erreurs; i++) {
        int pos = rand() % 7;
        recu[pos] = !recu[pos];
        printf("Erreur insérée à l'indice : %d\n", pos);
    }
printf("Message avec erreur : ");
    for(int i=0; i<7; i++) printf("%d", recu[i]);
    printf("\n"); 
   // 3. Décodage et Syndrome
    int s1 = recu[0] ^ recu[2] ^ recu[4] ^ recu[6];
    int s2 = recu[1] ^ recu[2] ^ recu[5] ^ recu[6];
    int s3 = recu[3] ^ recu[4] ^ recu[5] ^ recu[6];

    int syndrome = s1 + (s2 * 2) + (s3 * 4);

    // 4. Correction et Observation
    printf("\nSyndrome calculé : %d (Binaire: %d%d%d)\n", syndrome, s3, s2, s1);

    if (syndrome == 0) {
        printf("Résultat : Aucune erreur détectée.\n");
    } else {
        printf("Résultat : Erreur détectée à la position %d.\n", syndrome);
        // Tenter la correction
        recu[syndrome - 1] = !recu[syndrome - 1];
        printf("Action : Bit inversé pour correction.\n");
    }

    // Comparaison finale
    printf("\nMessage Original : %d %d %d %d\n", data[0], data[1], data[2], data[3]);
    printf("Message Corrigé  : %d %d %d %d\n", recu[2], recu[4], recu[5], recu[6]);

    return 0;
}