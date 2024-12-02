#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

// Structure pour un piment
typedef struct {
    char nom[20];
    int triche;  // Niveau de triche (0-10)
    int revisions;  // Niveau de révisions (0-10)
    int chance;  // Niveau de chance (0-10)
    int score;  // Score final
} Piment;

// Fonction pour initialiser les piments
void initialiser_piments(Piment piments[], int nombre) {
    // Liste de piments prédéfinis
    Piment piments_predefinis[] = {
        {"Quentin", 8, 5, 6, 0},
        {"Alan", 3, 9, 4, 0},
        {"Sterenn", 5, 9, 5, 0},
    };

    // Copie les piments prédéfinis dans le tableau principal
    for (int i = 0; i < nombre; i++) {
        piments[i] = piments_predefinis[i];
    }
}

// Fonction pour afficher les piments
void afficher_piments(Piment piments[], int nombre) {
    printf("\nVoici les piments disponibles :\n");
    for (int i = 0; i < nombre; i++) {
        printf("%d. %s (Triche: %d, Révisions: %d, Chance: %d)\n",
               i + 1, piments[i].nom, piments[i].triche, piments[i].revisions, piments[i].chance);
    }
}

// Fonction d'évaluation des piments
void evaluation(Piment *piment) {
    int evenement = rand() % 100;

    // Triche détectée (30% de chance si triche > 5)
    if (evenement < 30 && piment->triche > 5) {
        printf("%s s'est fait prendre en train de tricher et perd des points!\n", piment->nom);
        piment->score -= 3;  // Perte plus significative
        if (piment->score < 0) piment->score = 0;  // Evite les scores négatifs
        sleep(1);
    } 

    // Utilisation des fiches pour réussir (30% de chance si révisions > 5)
    if (evenement < 60 && piment->revisions > 5) {
        printf("%s utilise sa fiche pour réussir l'exercice et perd des points!\n", piment->nom);
        piment->score -= 2;
        if (piment->score < 0) piment->score = 0;
        sleep(1);
    } 

    // Aide inattendue et gain de points (20% de chance si chance > 7)
    if (evenement < 40 && piment->chance > 7) {
        printf("%s a eu une aide inattendue et gagne des points!\n", piment->nom);
        piment->score += 3;
        sleep(1);
    }
    
    // Bonus chanceux pour un score de base (10% de chance)
    if (evenement < 20) {
        printf("%s a eu une chance extraordinaire et gagne des points!\n", piment->nom);
        piment->score += 4;
        sleep(1);
    }

    // Score de base
    piment->score += piment->revisions + (rand() % (piment->chance + 1));
    if (piment->score < 0) piment->score = 0;  // Eviter les scores négatifs
}

// Demander à l'utilisateur de parier sur un piment
int demander_pari(int nombre_piments) {
    int choix;
    while (1) {
        printf("Sur quel piment voulez-vous parier ? (1-%d): ", nombre_piments);
        if (scanf("%d", &choix) != 1 || choix < 1 || choix > nombre_piments) {
            printf("Entrée invalide, veuillez réessayer.\n");
            while (getchar() != '\n');  // Nettoyer le buffer
        } else {
            return choix - 1;  // Index des piments
        }
    }
}

// Afficher les résultats finaux
void afficher_resultats(Piment piments[], int nombre) {
    printf("\nRésultats finaux:\n");
    for (int i = 0; i < nombre; i++) {
        printf("%s: %d points\n", piments[i].nom, piments[i].score);
    }
}

// Mini-jeu pour aider le piment parié
void mini_jeu(Piment *piment) {
    int reponse;
    printf("\nMini-jeu : %s ne sait pas répondre à cette question : 5+7 = ? \nAide ton piment et sois discret !\n", piment->nom);
    scanf("%d", &reponse);

    if (reponse == 12) {
        printf("Bonne réponse ! %s gagne 5 points supplémentaires.\n", piment->nom);
        piment->score += 5;
    } else {
        printf("Mauvaise réponse ! %s perd des points sur l'exercice !\n", piment->nom);
        piment->score -= 3;  // Plus de malus pour une mauvaise réponse
        if (piment->score < 0) piment->score = 0;  // Eviter les scores négatifs
    }
}

// Fonction principale du jeu
int main() {
    srand(time(NULL));
    const int nombre_piments = 3;
    Piment piments[nombre_piments];

    initialiser_piments(piments, nombre_piments);
    printf("Bienvenue dans le jeu des Piments !\n");
    afficher_piments(piments, nombre_piments);

    int pari = demander_pari(nombre_piments);
    printf("Vous avez parié sur %s.\n\n", piments[pari].nom);

    printf("Les piments commencent leurs évaluations...\n");
    sleep(2);
    if (pari >= 0 && pari < nombre_piments) {
        mini_jeu(&piments[pari]); // Permet d'aider le piment favori
    }

    for (int i = 0; i < nombre_piments; i++) {
        evaluation(&piments[i]);
        sleep(1);
    }

    afficher_resultats(piments, nombre_piments);

    // Déterminer le gagnant
    int gagnant = 0;
    for (int i = 1; i < nombre_piments; i++) {
        if (piments[i].score > piments[gagnant].score) {
            gagnant = i;
        }
    }

    printf("\nLe gagnant est %s !\n", piments[gagnant].nom);
    sleep(1);
    if (gagnant == pari) {
        printf("Félicitations ! Vous avez gagné votre pari.\n");
    } else {
        printf("Dommage, vous avez perdu votre pari.\n");
    }

    return 0;
}
