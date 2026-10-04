#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// Polynôme générateur CRC-8 : x^8 + x^2 + x + 1 (0x07)
#define POLYNOMIAL 0x07

uint8_t calculer_crc8(uint8_t data) {
    uint8_t crc = data;
    for (int i = 0; i < 8; i++) {
        if (crc & 0x80) { // Si le bit de poids fort est 1
            crc = (crc << 1) ^ POLYNOMIAL;
        } else {
            crc <<= 1;
        }
    }
    return crc;
}

int main() {
    srand(time(NULL));
    
    // 1. Message original (1 octet) + calcul du checksum
    uint8_t message = 0b11010010; // Exemple : 0xD2
    uint8_t checksum = calculer_crc8(message);
    
    printf("Message original : 0x%02X\n", message);
    printf("Checksum calculé : 0x%02X\n", checksum);

    // 2. Simulation d'une erreur
    uint8_t message_recu = message;
    int bit_erreur = rand() % 8;
    message_recu ^= (1 << bit_erreur); // Inversion d'un bit
    
    printf("\n--- Transmission avec 1 erreur ---\n");
    printf("Message reçu     : 0x%02X (Erreur au bit %d)\n", message_recu, bit_erreur);

    // 3. Vérification
    uint8_t verification = calculer_crc8(message_recu);
    
    printf("Nouveau checksum : 0x%02X\n", verification);
    
    if (verification != checksum) {
        printf("RESULTAT : Erreur DETECTEE par le CRC !\n");
    } else {
        printf("RESULTAT : Erreur NON DETECTEE (Collision).\n");
    }

    return 0;
}