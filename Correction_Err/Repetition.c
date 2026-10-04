#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/**
 * Logique de correction par vote majoritaire
 * Si 2 ou 3 bits sont identiques, on considère que c'est le bit original.
 */
int vote_majoritaire(int b1, int b2, int b3) {
    return (b1 + b2 + b3 >= 2) ? 1 : 0;
}

int main() {
    // 0. Configuration
    int message_original[] = {1, 0, 1, 1, 0};
    int taille = sizeof(message_original) / sizeof(message_original[0]);
    /*printf("%d\n",sizeof(message_original) );
    printf("%d\n",taille);*/
    int message_transmis[taille * 3];
    int message_recu[taille * 3];
    int message_corrige[taille];
    /* * Initialise le générateur de nombres pseudo-aléatoires.
 * Utilise l'heure actuelle (time) comme graine (seed) pour garantir 
 * une séquence de nombres différente à chaque exécution du programme.
 c'est mieux que le rand car rand donne la meme chaine a chaque exécution
 */
    srand(time(NULL));

    // 1. Transmission : Répéter chaque bit 3 fois
    /* * Exemple de fonctionnement :
 * Si message_original = {1, 0, ...}
 * i = 0 : message_transmis[0]=1, message_transmis[1]=1, message_transmis[2]=1
 * i = 1 : message_transmis[3]=0, message_transmis[4]=0, message_transmis[5]=0
 * Résultat final : {1, 1, 1, 0, 0, 0, ...}
 */
    for (int i = 0; i < taille; i++) {
        message_transmis[i * 3]     = message_original[i];
        message_transmis[i * 3 + 1] = message_original[i];
        message_transmis[i * 3 + 2] = message_original[i];
    }

    // Copie pour la réception avant erreurs
    memcpy(message_recu, message_transmis, sizeof(message_transmis));

    // 2. Simulation d'une erreur de 1 bit par bloc
    printf("--- Simulation du Canal (1 erreur par bloc) ---\n");
    for (int i = 0; i < taille; i++) {
        int position_erreur = rand() % 3; // Choisit l'index 0, 1 ou 2 dans le bloc
        int index_global = (i * 3) + position_erreur;
        
        // Inversion du bit (0->1 ou 1->0)
        message_recu[index_global] = !message_recu[index_global];
        printf("Bloc %d : Erreur insérée à l'index local %d\n", i, position_erreur);
    }

    // 3. Décodage et Correction
    for (int i = 0; i < taille; i++) {
        message_corrige[i] = vote_majoritaire(
            message_recu[i * 3], 
            message_recu[i * 3 + 1], 
            message_recu[i * 3 + 2]
        );
    }

    // 4. Comparaison et Affichage des résultats
    printf("\n--- Résultats ---\n");
    printf("Original : ");
    for(int i=0; i<taille; i++) printf("%d ", message_original[i]);
    
    printf("\nReçu (err): ");
    for(int i=0; i<taille*3; i++) printf("%d", message_recu[i]);

    printf("\nCorrigé  : ");
    int erreurs_finales = 0;
    for(int i=0; i<taille; i++) {
        printf("%d ", message_corrige[i]);
        if (message_corrige[i] != message_original[i]) erreurs_finales++;
    }

    printf("\n\nStatut : %s\n", (erreurs_finales == 0) ? "SUCCÈS (Toutes les erreurs ont été corrigées)" : "ÉCHEC");

    return 0;
}