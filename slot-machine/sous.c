#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Définir la taille de la fenêtre et des slots
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
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
int init(SDL_Window **window, SDL_Renderer **renderer);
Image loadImage(const char *path, SDL_Renderer *renderer);
void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height);
void cleanUp(SDL_Window *window, SDL_Renderer *renderer, Image *background, Image *symbols, int numSymbols);

int main() {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Initialisation de SDL
    if (!init(&window, &renderer)) {
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
                spinning = 1;
                spinStartTime = SDL_GetTicks(); // Enregistrer le temps de départ du tirage
            }
        }

        // Effacer l'écran
        SDL_RenderClear(renderer);

        // Afficher l'arrière-plan
        renderImage(renderer, background, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        if (spinning) {
            // Calculer le temps écoulé
            Uint32 elapsedTime = SDL_GetTicks() - spinStartTime;

            // Si le tirage est toujours en cours, on fait défiler les rouleaux
            if (elapsedTime < SPIN_DURATION) {
                // Défilement des rouleaux (tous les 100ms, on change le symbole)
                if (elapsedTime % 100 < 50) {  // Contrôler la vitesse du défilement (ex: tous les 100ms)
                    for (int i = 0; i < NUM_SLOTS; i++) {
                        slots[i] = rand() % NUM_SYMBOLS;  // Sélectionner un symbole aléatoire
                    }
                }
            } else {
                // Arrêter le tirage après la durée spécifiée
                spinning = 0;
            }
        }

        // Afficher les rouleaux avec des symboles choisis, centrés horizontalement
        int startX = (WINDOW_WIDTH - (NUM_SLOTS * SLOT_WIDTH + (NUM_SLOTS - 1) * 30)) / 2; // Calcul de la position de départ centrée

        for (int i = 0; i < NUM_SLOTS; i++) {
            int xPos = startX + i * (SLOT_WIDTH + 30);  // Position horizontale centrée
            int yPos = (WINDOW_HEIGHT - SLOT_HEIGHT) / 2;  // Position verticale centrée
            renderImage(renderer, symbols[slots[i]], xPos, yPos, SLOT_WIDTH, SLOT_HEIGHT);
        }

        // Mettre à jour l'écran
        SDL_RenderPresent(renderer);

        // Attendre un petit moment pour simuler le temps de tirage
        SDL_Delay(50);  // Ajustez cette valeur pour le temps de réaction plus rapide ou plus lent
    }

    // Nettoyer et quitter
    cleanUp(window, renderer, &background, symbols, NUM_SYMBOLS);

    return 0;
}

int init(SDL_Window **window, SDL_Renderer **renderer) {
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

    return 1;
}

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

// Modification pour permettre le redimensionnement à SLOT_WIDTH x SLOT_HEIGHT
void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height) {
    SDL_Rect dstRect = {x, y, width, height};
    SDL_RenderCopy(renderer, img.texture, NULL, &dstRect);
}

void cleanUp(SDL_Window *window, SDL_Renderer *renderer, Image *background, Image *symbols, int numSymbols) {
    SDL_DestroyTexture(background->texture);
    for (int i = 0; i < numSymbols; i++) {
        SDL_DestroyTexture(symbols[i].texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
