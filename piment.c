#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // Pour la fonction sleep

#define SYMBOLS 5
#define JACKPOT_CHANCE 100

// Couleurs ANSI pour le terminal
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"

// Structure pour représenter le joueur
typedef struct {
    float balance; // Solde en euros
    float jackpot; // Jackpot progressif en euros
} Player;

// Fonction pour générer un symbole aléatoire
char generateSymbol() {
    char symbols[] = {'$', '#', '@', '*', '&'};
    return symbols[rand() % SYMBOLS];
}

// Fonction pour afficher la balance avec des couleurs
void displayBalance(Player *player) {
    printf(CYAN "Votre solde actuel : %.2f euros\n" RESET, player->balance);
    printf(YELLOW "Jackpot actuel : %.2f euros\n" RESET, player->jackpot);
}

// Fonction pour sauvegarder la balance et le jackpot du joueur dans un fichier
void saveGame(Player *player) {
    FILE *file = fopen("sauvegarde.txt", "w");
    if (file) {
        fprintf(file, "%.2f %.2f\n", player->balance, player->jackpot);
        fclose(file);
        printf(GREEN "Jeu sauvegardé !\n" RESET);
    } else {
        printf(RED "Erreur lors de la sauvegarde du jeu.\n" RESET);
    }
}

// Fonction pour charger la balance et le jackpot du joueur depuis un fichier
void loadGame(Player *player) {
    FILE *file = fopen("sauvegarde.txt", "r");
    if (file) {
        fscanf(file, "%f %f", &player->balance, &player->jackpot);
        fclose(file);
        printf(GREEN "Jeu chargé !\n" RESET);
    } else {
        printf(YELLOW "Aucune sauvegarde trouvée. Veuillez entrer un solde initial.\n" RESET);
        player->balance = 0;
        player->jackpot = 0;
    }
}

// Fonction pour compter les symboles identiques
int countMatches(char slots[], int size) {
    int counts[SYMBOLS] = {0};
    char symbols[] = {'$', '#', '@', '*', '&'};

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < SYMBOLS; j++) {
            if (slots[i] == symbols[j]) {
                counts[j]++;
            }
        }
    }

    int maxCount = 0;
    for (int i = 0; i < SYMBOLS; i++) {
        if (counts[i] > maxCount) {
            maxCount = counts[i];
        }
    }

    return maxCount;
}

// Fonction pour simuler les rouleaux avec une animation
void spinAnimation() {
    char symbols[] = {'$', '#', '@', '*', '&'};
    for (int i = 0; i < 10; i++) {
        printf("\r| %c | %c | %c | %c | %c |", symbols[rand() % SYMBOLS], symbols[rand() % SYMBOLS], symbols[rand() % SYMBOLS], symbols[rand() % SYMBOLS], symbols[rand() % SYMBOLS]);
        fflush(stdout);
        usleep(100000); // Pause de 100ms
    }
    printf("\n");
}

// Fonction pour jouer un tour de machine à sous
int playSlotMachine(Player *player, float bet) {
    if (bet > player->balance) {
        printf(RED "Mise trop élevée !\n" RESET);
        return 0;
    }

    // Ajout de la mise au jackpot
    player->jackpot += bet * 0.1;

    printf("Résultats :\n");
    spinAnimation();

    char slots[5];
    for (int i = 0; i < 5; i++) {
        slots[i] = generateSymbol();
    }

    printf("| %c | %c | %c | %c | %c |\n", slots[0], slots[1], slots[2], slots[3], slots[4]);

    int maxMatches = countMatches(slots, 5);
    float winnings = 0;

    // Calcul des gains
    if (maxMatches == 2) {
        winnings = bet;
        printf(YELLOW "Deux symboles identiques ! Vous récupérez votre mise : %.2f euros\n" RESET, winnings);
    } else if (maxMatches == 3) {
        winnings = bet * 3;
        printf(GREEN "Trois symboles identiques ! Vous gagnez %.2f euros\n" RESET, winnings);
    } else if (maxMatches == 4) {
        winnings = bet * 5;
        printf(GREEN "Quatre symboles identiques ! Vous gagnez %.2f euros\n" RESET, winnings);
    } else if (maxMatches == 5) {
        winnings = bet * 10;
        printf(GREEN "Jackpot du jeu ! Cinq symboles identiques ! Vous gagnez %.2f euros\n" RESET, winnings);
    } else {
        printf(RED "Désolé, vous avez perdu.\n" RESET);
        player->balance -= bet;
        return 1;
    }

    player->balance += winnings;

    // Vérifie si le joueur gagne le jackpot progressif
    if ((rand() % JACKPOT_CHANCE) == 0) {
        printf(GREEN "Félicitations ! Vous avez gagné le jackpot progressif de %.2f euros !\n" RESET, player->jackpot);
        player->balance += player->jackpot;
        player->jackpot = 0;
    } else {
        printf("Pas de jackpot cette fois-ci.\n");
    }

    return 1;
}

int main() {
    srand(time(NULL));

    Player player;
    float bet;

    // Chargement de la sauvegarde
    loadGame(&player);

    if (player.balance == 0) {
        printf("Entrez votre solde initial en euros : ");
        scanf("%f", &player.balance);
    }

    displayBalance(&player);
    printf("Bienvenue dans le jeu de Machine à Sous avec Jackpot Progressif !\n");

    while (player.balance > 0) {
        printf("Entrez votre mise en euros (0 pour quitter) : ");
        scanf("%f", &bet);

        if (bet == 0) {
            saveGame(&player);
            break;
        }

        if (!playSlotMachine(&player, bet)) {
            printf("Erreur lors du pari. Réessayez.\n");
        }

        displayBalance(&player);
    }

    printf("Merci d'avoir joué ! Votre solde final : %.2f euros\n", player.balance);
    return 0;
}
