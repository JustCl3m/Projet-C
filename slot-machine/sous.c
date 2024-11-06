// gcc sous.c -o sous -lSDL2 -lSDL2_image -lSDL2_ttf -lm

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>  // Inclure la bibliothèque SDL_ttf
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Définir la taille de la fenêtre et des slots
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

// Taille personnalisée pour l'image de fond
#define BACKGROUND_WIDTH 1600  // Par exemple, 1600 pixels
#define BACKGROUND_HEIGHT 950  // Par exemple, 900 pixels

#define SLOT_WIDTH 150
#define SLOT_HEIGHT 150
#define NUM_SLOTS 3
#define NUM_SYMBOLS 5
#define SPIN_DURATION 3000  // Durée du tirage en millisecondes

// Structure pour contenir les informations d'image
typedef struct {
    SDL_Texture *texture;
    int width, height;
} Image;

// Prototypes des fonctions
int init(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font);
Image loadImage(const char *path, SDL_Renderer *renderer);
void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height);
void cleanUp(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Image *background, Image *symbols, int numSymbols);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color);
void updateBalance(int *balance, int *slots);

// Fonction principale
int main() {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;

    // Initialisation de SDL et TTF
    if (!init(&window, &renderer, &font)) {
        printf("Erreur d'initialisation SDL\n");
        return -1;
    }

    // Charger les images
    Image background = loadImage("background.png", renderer);
    Image symbols[NUM_SYMBOLS]; // 5 symboles différents pour les rouleaux
    for (int i = 0; i < NUM_SYMBOLS; i++) {
        char path[64];
        snprintf(path, sizeof(path), "symbol_%d.png", i); // symbol_0.png, symbol_1.png, etc.
        symbols[i] = loadImage(path, renderer);
    }

    srand(time(NULL)); // Initialiser la génération aléatoire

    int balance = 1000;  // Solde initial
    int running = 1;
    int spinning = 0;  // Flag pour savoir si la machine est en train de tourner
    Uint32 spinStartTime = 0; // Moment du début du tirage
    int slots[NUM_SLOTS] = {0}; // Pour stocker les symboles actuels des rouleaux

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            // Ajouter un événement pour démarrer le tirage (par exemple, un clic de souris)
            if (e.type == SDL_MOUSEBUTTONDOWN && !spinning) {
                if (balance > 0) {  // Assurez-vous qu'il y a de l'argent
                    spinning = 1;
                    spinStartTime = SDL_GetTicks(); // Enregistrer le temps de départ du tirage
                    balance--;  // Déduire 1 unité de la balance pour jouer
                }
            }
        }

        // Effacer l'écran
        SDL_RenderClear(renderer);

        // Afficher l'arrière-plan avec la taille personnalisée
        int backgroundX = (WINDOW_WIDTH - BACKGROUND_WIDTH) / 2;  // Centrer horizontalement
        int backgroundY = (WINDOW_HEIGHT - BACKGROUND_HEIGHT) / 2 - 50;  // Centrer verticalement
        renderImage(renderer, background, backgroundX, backgroundY, BACKGROUND_WIDTH, BACKGROUND_HEIGHT);

        if (spinning) {
            // Calculer le temps écoulé
            Uint32 elapsedTime = SDL_GetTicks() - spinStartTime;

            // Si le tirage est toujours en cours, on fait défiler les rouleaux
            if (elapsedTime < SPIN_DURATION) {
                // Défilement des rouleaux (tous les 100ms, on change le symbole)
                if (elapsedTime % 100 < 50) {  // Contrôler la vitesse du défilement (ex: tous les 100ms)
                    for (int i = 0; i < NUM_SLOTS; i++) {
                        // Définir une probabilité plus élevée de gagner avec des symboles spécifiques
                        if (rand() % 100 < 50) {  // 20% de chance d'obtenir un symbole gagnant
                            slots[i] = 0;  // Choisir un symbole gagnant (par exemple, 0)
                        } else {
                            slots[i] = rand() % NUM_SYMBOLS;  // Sinon, choisir un symbole aléatoire
                        }
                    }
                }
            } else {
                // Arrêter le tirage après la durée spécifiée
                spinning = 0;
                updateBalance(&balance, slots);  // Mise à jour du solde en fonction du résultat
            }
        }

        // Afficher les rouleaux avec des symboles choisis, centrés horizontalement
        int startX = 640; // Calcul de la position de départ centrée

        for (int i = 0; i < NUM_SLOTS; i++) {
            int xPos = startX + i * (SLOT_WIDTH + 105);  // Position horizontale centrée
            int yPos = (WINDOW_HEIGHT - SLOT_HEIGHT) / 2;  // Position verticale centrée
            renderImage(renderer, symbols[slots[i]], xPos, yPos, SLOT_WIDTH, SLOT_HEIGHT);
        }

        // Affichage du solde en haut à gauche
        SDL_Color whiteColor = {255, 255, 255, 255};  // Couleur blanche
        char balanceText[64];
        snprintf(balanceText, sizeof(balanceText), "Solde: %d", balance);
        renderText(renderer, font, balanceText, 20, 20, whiteColor);  // Affichage du solde

        // Mettre à jour l'écran
        SDL_RenderPresent(renderer);

        // Attendre un petit moment pour simuler le temps de tirage
        SDL_Delay(50);  // Ajustez cette valeur pour le temps de réaction plus rapide ou plus lent
    }

    // Nettoyer et quitter
    cleanUp(window, renderer, font, &background, symbols, NUM_SYMBOLS);

    return 0;
}

// Fonction pour initialiser SDL, le renderer et la police
int init(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL ne s'est pas initialisé ! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    *window = SDL_CreateWindow("Slot Machine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!*window) {
        printf("La fenêtre ne peut pas être créée ! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        printf("Le renderer ne peut pas être créé ! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // Initialiser TTF pour le texte
    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation de TTF: %s\n", TTF_GetError());
        return 0;
    }

    *font = TTF_OpenFont("Roboto-Regular.ttf", 24);  // Charger une police (vérifiez le chemin de votre police)
    if (!*font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        return 0;
    }

    return 1;
}

// Fonction pour charger une image
Image loadImage(const char *path, SDL_Renderer *renderer) {
    Image img;
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        printf("Erreur de chargement de l'image : %s\n", SDL_GetError());
        img.texture = NULL;
        return img;
    }

    img.texture = SDL_CreateTextureFromSurface(renderer, surface);
    img.width = surface->w;
    img.height = surface->h;
    SDL_FreeSurface(surface);
    return img;
}

// Fonction pour afficher une image
void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height) {
    SDL_Rect dstRect = {x, y, width, height};
    SDL_RenderCopy(renderer, img.texture, NULL, &dstRect);
}

// Fonction pour afficher du texte
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Fonction pour mettre à jour le solde
void updateBalance(int *balance, int *slots) {
    // Par exemple, si tous les symboles sont les mêmes (gagnant), on double le solde
    if (slots[0] == slots[1] && slots[1] == slots[2]) {
        *balance += 10;  // Gain de 10 unités si le joueur gagne
    }
}

// Nettoyage de la mémoire et fermeture de SDL
void cleanUp(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Image *background, Image *symbols, int numSymbols) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}
