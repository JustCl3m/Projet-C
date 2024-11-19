#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CELL_SIZE 40
#define FPS 60
#define DELAY_TIME (1000 / FPS) // Temps entre chaque image en millisecondes
#define TOKEN_RADIUS 8 // Rayon du jeton
#define MAX_TOKENS 5 // Nombre maximal de jetons empilés
#define CIRCLE_RADIUS 15 // Rayon du cercle où les jetons seront disposés

// Structure représentant une case de la grille
typedef struct {
    int x, y;
    int selected; // 1 si la case est sélectionnée, 0 sinon
    int tokenCount; // Nombre de jetons actuellement empilés
} Cell;

// Fonction pour charger une texture
SDL_Texture* load_texture(const char* filepath, SDL_Renderer* renderer) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, filepath);
    if (!texture) {
        printf("Unable to load image %s! SDL_image Error: %s\n", filepath, IMG_GetError());
    }
    return texture;
}

// Fonction pour dessiner la grille
void draw_grid(SDL_Renderer* renderer, SDL_Texture* backgroundTexture, Cell** grid, int rows, int cols, int selectedX, int selectedY) {
    // Afficher l'image de fond
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    // Dessiner les cases
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Dessiner les bordures des cases uniquement (sans remplir la case)
            if (grid[i][j].selected) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Bordure rouge pour la case sélectionnée
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Bordure noire pour les autres cases
            }

            SDL_Rect cellRect = {i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_RenderDrawRect(renderer, &cellRect); // Bordure des cases

            // Dessiner les jetons sous forme de cercle
            if (grid[i][j].tokenCount > 0) {
                // Calculer l'angle entre chaque jeton
                float angleStep = 2 * M_PI / grid[i][j].tokenCount;
                for (int k = 0; k < grid[i][j].tokenCount; k++) {
                    // Calculer la position du jeton sur le cercle
                    float angle = k * angleStep;
                    int centerX = i * CELL_SIZE + CELL_SIZE / 2;
                    int centerY = j * CELL_SIZE + CELL_SIZE / 2;
                    int tokenX = centerX + (int)(CIRCLE_RADIUS * cos(angle));
                    int tokenY = centerY + (int)(CIRCLE_RADIUS * sin(angle));

                    // Dessiner le jeton (cercle noir)
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Couleur noire pour le jeton
                    for (int w = -TOKEN_RADIUS; w < TOKEN_RADIUS; w++) {
                        for (int h = -TOKEN_RADIUS; h < TOKEN_RADIUS; h++) {
                            if (w * w + h * h <= TOKEN_RADIUS * TOKEN_RADIUS) {
                                SDL_RenderDrawPoint(renderer, tokenX + w, tokenY + h);
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialiser SDL_image
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Récupérer la résolution de l'écran
    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0) {
        printf("SDL_GetDesktopDisplayMode Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    int screenWidth = displayMode.w;
    int screenHeight = displayMode.h;

    // Calculer le nombre de lignes et de colonnes basés sur la résolution
    int cols = screenWidth / CELL_SIZE;  // Nombre de colonnes
    int rows = screenHeight / CELL_SIZE; // Nombre de lignes

    // Créer la fenêtre avec les dimensions calculées
    SDL_Window* window = SDL_CreateWindow("Grille avec Jetons et Fond",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Charger le fond d'écran
    SDL_Texture* backgroundTexture = load_texture("tapis-roulette.png", renderer);
    if (!backgroundTexture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialisation de la grille
    Cell** grid = (Cell**)malloc(rows * sizeof(Cell*));
    for (int i = 0; i < rows; i++) {
        grid[i] = (Cell*)malloc(cols * sizeof(Cell));
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j].x = i;
            grid[i][j].y = j;
            grid[i][j].selected = 0;
            grid[i][j].tokenCount = 0;
        }
    }

    int selectedX = 0, selectedY = 0;

    // Boucle principale
    int quit = 0;
    SDL_Event e;
    Uint32 startTime;
    while (!quit) {
        startTime = SDL_GetTicks(); // Temps avant l'événement

        // Gestion des événements
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }

            // Gestion des entrées clavier
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (selectedY > 0) selectedY--;
                        break;
                    case SDLK_DOWN:
                        if (selectedY < rows - 1) selectedY++;
                        break;
                    case SDLK_LEFT:
                        if (selectedX > 0) selectedX--;
                        break;
                    case SDLK_RIGHT:
                        if (selectedX < cols - 1) selectedX++;
                        break;
                    case SDLK_RETURN: // Touche Entrée
                        // Placer un jeton (cercle noir) dans la case sélectionnée
                        if (grid[selectedX][selectedY].tokenCount < MAX_TOKENS) {
                            grid[selectedX][selectedY].tokenCount++; // Incrémenter le nombre de jetons
                        }
                        break;
                    case SDLK_BACKSPACE: // Touche Backspace
                        // Retirer un jeton de la case sélectionnée si possible
                        if (grid[selectedX][selectedY].tokenCount > 0) {
                            grid[selectedX][selectedY].tokenCount--; // Décrémenter le nombre de jetons
                        }
                        break;
                }
            }
        }

        // Mise à jour de la grille en fonction de la sélection
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                grid[i][j].selected = 0; // Désélectionner toutes les cases
            }
        }
        grid[selectedX][selectedY].selected = 1; // Sélectionner la case actuelle

        // Rendu
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Fond blanc (ne sera pas utilisé ici)
        SDL_RenderClear(renderer);

        draw_grid(renderer, backgroundTexture, grid, rows, cols, selectedX, selectedY);

        SDL_RenderPresent(renderer);

        // Calculer le temps passé et ajuster la boucle pour maintenir une fréquence constante (FPS)
        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        if (elapsedTime < DELAY_TIME) {
            SDL_Delay(DELAY_TIME - elapsedTime); // Délai pour atteindre 60 FPS
        }
    }

    // Libération des ressources
    for (int i = 0; i < rows; i++) {
        free(grid[i]);
    }
    free(grid);

    SDL_DestroyTexture(backgroundTexture); // Libérer la texture du fond
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
