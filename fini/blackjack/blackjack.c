#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CARD_WIDTH 100
#define CARD_HEIGHT 150
#define MAX_CARDS 52

// Structure pour une carte
typedef struct {
    SDL_Texture *texture;
    int value;  // Valeur de la carte pour le calcul du score (10 pour J, Q, K, 11 pour Ace)
} Card;

SDL_Texture *loadTexture(const char *file, SDL_Renderer *renderer) {
    SDL_Surface *surface = IMG_Load(file);
    if (!surface) {
        printf("Erreur : Impossible de charger l'image %s : %s\n", file, IMG_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void shuffle(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// Fonction pour calculer le score total des cartes
int calculateScore(int *hand, Card *cards, int cardCount) {
    int score = 0;
    int aceCount = 0;

    // Ajouter les valeurs des cartes
    for (int i = 0; i < cardCount; i++) {
        int cardIndex = hand[i];
        if (cardIndex != -1) {
            score += cards[cardIndex].value;
            if (cards[cardIndex].value == 11) {
                aceCount++;
            }
        }
    }

    // Ajuster les As (11 => 1 si nécessaire)
    while (score > 21 && aceCount > 0) {
        score -= 10;
        aceCount--;
    }

    return score;
}

// Fonction pour afficher le texte
void renderText(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, int x, int y) {
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if (!textSurface) {
        printf("Erreur : Impossible de créer la surface du texte : %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_Rect destRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &destRect);
    SDL_DestroyTexture(textTexture);
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur : SDL n'a pas pu être initialisé : %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur : SDL_image n'a pas pu être initialisé : %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("Erreur : SDL_ttf n'a pas pu être initialisé : %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Blackjack", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur : La fenêtre n'a pas pu être créée : %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur : Le renderer n'a pas pu être créé : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Charger la police
    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 24);
    if (!font) {
        printf("Erreur : Impossible de charger la police : %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tableau des fichiers d'images
    const char *cardFiles[] = {
        "cards/cardClubs2.png", "cards/cardClubs3.png", "cards/cardClubs4.png",
        "cards/cardClubs5.png", "cards/cardClubs6.png", "cards/cardClubs7.png",
        "cards/cardClubs8.png", "cards/cardClubs9.png", "cards/cardClubs10.png",
        "cards/cardClubsJ.png", "cards/cardClubsQ.png", "cards/cardClubsK.png", "cards/cardClubsA.png",
        "cards/cardDiamonds2.png", "cards/cardDiamonds3.png", "cards/cardDiamonds4.png",
        "cards/cardDiamonds5.png", "cards/cardDiamonds6.png", "cards/cardDiamonds7.png",
        "cards/cardDiamonds8.png", "cards/cardDiamonds9.png", "cards/cardDiamonds10.png",
        "cards/cardDiamondsJ.png", "cards/cardDiamondsQ.png", "cards/cardDiamondsK.png", "cards/cardDiamondsA.png",
        "cards/cardHearts2.png", "cards/cardHearts3.png", "cards/cardHearts4.png",
        "cards/cardHearts5.png", "cards/cardHearts6.png", "cards/cardHearts7.png",
        "cards/cardHearts8.png", "cards/cardHearts9.png", "cards/cardHearts10.png",
        "cards/cardHeartsJ.png", "cards/cardHeartsQ.png", "cards/cardHeartsK.png", "cards/cardHeartsA.png",
        "cards/cardSpades2.png", "cards/cardSpades3.png", "cards/cardSpades4.png",
        "cards/cardSpades5.png", "cards/cardSpades6.png", "cards/cardSpades7.png",
        "cards/cardSpades8.png", "cards/cardSpades9.png", "cards/cardSpades10.png",
        "cards/cardSpadesJ.png", "cards/cardSpadesQ.png", "cards/cardSpadesK.png", "cards/cardSpadesA.png"
    };

    // Valeurs des cartes
    int cardValues[] = {
        2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11
    };

    // Charger les textures des cartes
    Card cards[MAX_CARDS];
    for (int i = 0; i < MAX_CARDS; i++) {
        cards[i].texture = loadTexture(cardFiles[i], renderer);
        cards[i].value = cardValues[i];
        if (!cards[i].texture) {
            printf("Erreur : Impossible de charger %s\n", cardFiles[i]);
            return 1;
        }
    }

    srand(time(NULL));

    int deck[MAX_CARDS];
    for (int i = 0; i < MAX_CARDS; i++) {
        deck[i] = i;
    }
    shuffle(deck, MAX_CARDS);

    int playerHand[5] = {-1}; // Initialisation vide
    int dealerHand[5] = {-1}; // Initialisation vide
    int playerIndex = 0, dealerIndex = 0;
    int playerScore = 0, dealerScore = 0;
    int playerTurn = 1; // 1 si c'est le tour du joueur, 0 si c'est le tour du dealer
    int gameOver = 0;

    while (!gameOver) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameOver = 1;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE && playerTurn && playerIndex < 5) {
                    // Tirer une carte pour le joueur
                    playerHand[playerIndex++] = deck[playerIndex + dealerIndex];
                } else if (event.key.keysym.sym == SDLK_RETURN && playerTurn) {
                    // Le joueur a fini son tour (stand)
                    playerTurn = 0;
                }
            }
        }

        // Calculer les scores
        playerScore = calculateScore(playerHand, cards, playerIndex);
        dealerScore = calculateScore(dealerHand, cards, dealerIndex);

        // Tirage du croupier (dealer) jusqu'à 17
        if (!playerTurn && dealerScore < 17 && dealerIndex < 5) {
            dealerHand[dealerIndex++] = deck[playerIndex + dealerIndex];
            dealerScore = calculateScore(dealerHand, cards, dealerIndex);
        }

        // Dessiner le fond
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
        SDL_RenderClear(renderer);

        // Afficher les cartes du joueur
        for (int i = 0; i < playerIndex; i++) {
            SDL_Rect dest = {50 + i * 120, 400, CARD_WIDTH, CARD_HEIGHT};
            SDL_RenderCopy(renderer, cards[playerHand[i]].texture, NULL, &dest);
        }

        // Afficher les cartes du dealer
        for (int i = 0; i < dealerIndex; i++) {
            SDL_Rect dest = {50 + i * 120, 100, CARD_WIDTH, CARD_HEIGHT};
            SDL_RenderCopy(renderer, cards[dealerHand[i]].texture, NULL, &dest);
        }

        // Afficher les scores
        char scoreText[100];
        snprintf(scoreText, sizeof(scoreText), "Joueur : %d | Croupier : %d", playerScore, dealerScore);
        renderText(scoreText, renderer, font, (SDL_Color){255, 255, 255, 255}, 50, 30);

        // Vérifier si quelqu'un a perdu
        if (playerScore > 21) {
            renderText("Le joueur a perdu!", renderer, font, (SDL_Color){255, 0, 0, 255}, 50, 150);
        } else if (dealerScore > 21) {
            renderText("Le croupier a perdu!", renderer, font, (SDL_Color){255, 0, 0, 255}, 50, 150);
        } else if (!playerTurn && dealerScore >= 17) {
            if (dealerScore > playerScore) {
                renderText("Le croupier gagne!", renderer, font, (SDL_Color){255, 0, 0, 255}, 50, 150);
            } else if (dealerScore < playerScore) {
                renderText("Le joueur gagne!", renderer, font, (SDL_Color){255, 0, 0, 255}, 50, 150);
            } else {
                renderText("Match nul!", renderer, font, (SDL_Color){255, 255, 255, 255}, 50, 150);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Libérer les ressources
    for (int i = 0; i < MAX_CARDS; i++) {
        SDL_DestroyTexture(cards[i].texture);
    }
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
