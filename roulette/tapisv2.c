#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_CELL_WIDTH 50
#define GRID_CELL_HEIGHT 50
#define FONT_SIZE 24

SDL_Color RED = {255, 0, 0, 255};
SDL_Color BLACK = {0, 0, 0, 255};
SDL_Color GREEN = {0, 128, 0, 255};
SDL_Color WHITE = {255, 255, 255, 255};  // WHITE avec alpha 255

// Structure représentant un jeton placé
typedef struct {
    int x, y;
    SDL_Color color;
} Token;

// Tableau dynamique pour stocker les jetons
Token* tokens = NULL;
int tokenCount = 0;

// Fonction pour déterminer si le numéro est rouge (selon les règles de la roulette européenne)
int is_red(int number) {
    int red_numbers[] = {1, 3, 5, 7, 9, 12, 14, 16, 18, 19, 21, 23, 25, 27, 30, 32, 34, 36};
    int n = sizeof(red_numbers) / sizeof(red_numbers[0]);
    for (int i = 0; i < n; i++) {
        if (number == red_numbers[i]) {
            return 1;  // C'est une case rouge
        }
    }
    return 0;  // C'est une case noire
}

// Fonction pour dessiner la table de roulette
void draw_table(SDL_Renderer* renderer, TTF_Font* font) {
    int x, y;
    char numStr[3];
    SDL_Surface* textSurface = NULL;
    SDL_Texture* textTexture = NULL;
    SDL_Rect textRect;

    // Dessiner la case "0" fusionnée sur 3 cases de hauteur
    x = 0;
    y = 0;
    SDL_SetRenderDrawColor(renderer, GREEN.r, GREEN.g, GREEN.b, GREEN.a);
    SDL_Rect rect = {x, y, GRID_CELL_WIDTH, 3 * GRID_CELL_HEIGHT};  // Fusionner 3 cases verticalement
    SDL_RenderFillRect(renderer, &rect);

    // Dessiner le texte du "0"
    snprintf(numStr, sizeof(numStr), "0");
    textSurface = TTF_RenderText_Solid(font, numStr, WHITE);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect.x = x + (GRID_CELL_WIDTH - textSurface->w) / 2;
    textRect.y = y + (3 * GRID_CELL_HEIGHT - textSurface->h) / 2;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Dessiner les cases pour les autres numéros organisés en colonnes
    for (int j = 0; j < 12; j++) {  // 12 colonnes
        for (int i = 0; i < 3; i++) {  // 3 rangées
            int number = j * 3 + i + 1;
            x = (j + 1) * GRID_CELL_WIDTH;  // Décalage de 1 colonne pour les numéros
            y = i * GRID_CELL_HEIGHT;

            SDL_Color cell_color = is_red(number) ? RED : BLACK;

            // Dessiner la case
            SDL_SetRenderDrawColor(renderer, cell_color.r, cell_color.g, cell_color.b, cell_color.a);
            SDL_Rect rect = {x, y, GRID_CELL_WIDTH, GRID_CELL_HEIGHT};
            SDL_RenderFillRect(renderer, &rect);

            // Dessiner le numéro dans la case
            snprintf(numStr, sizeof(numStr), "%d", number);
            textSurface = TTF_RenderText_Solid(font, numStr, WHITE);
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            textRect.x = x + 10;
            textRect.y = y + 10;
            textRect.w = textSurface->w;
            textRect.h = textSurface->h;
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
    }

    // Dessiner les jetons sur les cases
    for (int i = 0; i < tokenCount; i++) {
        SDL_SetRenderDrawColor(renderer, tokens[i].color.r, tokens[i].color.g, tokens[i].color.b, tokens[i].color.a);
        SDL_Rect tokenRect = {tokens[i].x + GRID_CELL_WIDTH / 4, tokens[i].y + GRID_CELL_HEIGHT / 4, GRID_CELL_WIDTH / 2, GRID_CELL_HEIGHT / 2};
        SDL_RenderFillRect(renderer, &tokenRect);
    }
}

// Fonction pour gérer l'ajout d'un jeton sur la case cliquée
void place_token(int x, int y) {
    // Calculer la position de la case cliquée
    int gridX = x / GRID_CELL_WIDTH;
    int gridY = y / GRID_CELL_HEIGHT;
    
    // Calculer le numéro de la case (en fonction de la disposition de la table)
    int number = gridX * 3 + gridY + 1;

    // Déterminer la couleur du jeton (rouge ou noir)
    SDL_Color tokenColor = is_red(number) ? RED : BLACK;

    // Ajouter le jeton à la liste des jetons
    tokens = realloc(tokens, (tokenCount + 1) * sizeof(Token));
    tokens[tokenCount].x = gridX * GRID_CELL_WIDTH;
    tokens[tokenCount].y = gridY * GRID_CELL_HEIGHT;
    tokens[tokenCount].color = tokenColor;
    tokenCount++;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Créer la fenêtre SDL
    SDL_Window* window = SDL_CreateWindow("Roulette Table", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Créer le renderer SDL
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Charger la police de caractères
    TTF_Font* font = TTF_OpenFont("Roboto-Regular.ttf", FONT_SIZE);  // Assurez-vous que le chemin de la police est correct
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Réglage de la couleur de fond
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Dessiner la table de roulette
    draw_table(renderer, font);

    // Afficher le rendu
    SDL_RenderPresent(renderer);

    SDL_Event e;
    int quit = 0;

    // Boucle principale d'événements
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    place_token(e.button.x, e.button.y);
                    SDL_RenderClear(renderer);
                    draw_table(renderer, font);
                    SDL_RenderPresent(renderer);
                }
            }
        }
    }

    // Libérer les ressources
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Quitter SDL_ttf et SDL
    TTF_Quit();
    SDL_Quit();

    // Libérer la mémoire des jetons
    free(tokens);

    return 0;
}
