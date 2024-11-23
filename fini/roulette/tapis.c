#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_CELL_WIDTH 50
#define GRID_CELL_HEIGHT 50
#define FONT_SIZE 24

SDL_Color RED = {255, 0, 0, 255};
SDL_Color BLACK = {0, 0, 0, 255};
SDL_Color GREEN = {0, 128, 0, 255};
SDL_Color WHITE = {255, 255, 255, 255};  // WHITE avec alpha 255

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

    // Dessiner les sections "1st 12", "2nd 12", "3rd 12" sur la même ligne
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);

    // Définir la largeur de chaque section (12 colonnes)
    int sectionWidth = 4 * GRID_CELL_WIDTH;  // 12 colonnes de large pour chaque section
    int sectionHeight = GRID_CELL_HEIGHT;  // Une seule ligne pour les sections

    // "1st 12"
    SDL_Rect rectSection = {GRID_CELL_WIDTH, 3 * GRID_CELL_HEIGHT, sectionWidth, sectionHeight};
    SDL_RenderFillRect(renderer, &rectSection);
    textSurface = TTF_RenderText_Solid(font, "1st 12", BLACK);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect.x = rectSection.x + (sectionWidth - textSurface->w) / 2; // Centrer le texte
    textRect.y = rectSection.y + (sectionHeight - textSurface->h) / 2;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // "2nd 12"
    rectSection.x += sectionWidth;  // Décaler de la largeur d'une section
    SDL_RenderFillRect(renderer, &rectSection);
    textSurface = TTF_RenderText_Solid(font, "2nd 12", BLACK);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect.x = rectSection.x + (sectionWidth - textSurface->w) / 2; // Centrer le texte
    textRect.y = rectSection.y + (sectionHeight - textSurface->h) / 2;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // "3rd 12"
    rectSection.x += sectionWidth;  // Décaler de la largeur d'une section
    SDL_RenderFillRect(renderer, &rectSection);
    textSurface = TTF_RenderText_Solid(font, "3rd 12", BLACK);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect.x = rectSection.x + (sectionWidth - textSurface->w) / 2; // Centrer le texte
    textRect.y = rectSection.y + (sectionHeight - textSurface->h) / 2;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    
    
    // Dessiner les 6 cases sur la même ligne (1 à 18, PAIR, ROUGE, NOIR, IMPAIR, 19 à 36)
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);

    // Position Y des cases
    int startY = 4 * GRID_CELL_HEIGHT;  // Position Y pour la ligne des 6 cases

    // Textes à afficher dans chaque case
    const char* textes[] = {"1 a 18", "PAIR", "ROUGE", "NOIR", "IMPAIR", "19 a 36"};

    // Dessiner chaque case et le texte correspondant
    for (int i = 0; i < 6; i++) {
        // Position de la case
        SDL_Rect rectCase = {GRID_CELL_WIDTH + i * GRID_CELL_WIDTH*2, startY, GRID_CELL_WIDTH*2, GRID_CELL_HEIGHT};
        
        // Dessiner la case (noir)
        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
        SDL_RenderFillRect(renderer, &rectCase);

        SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
        SDL_RenderDrawRect(renderer, &rectCase);

        // Dessiner le texte centré dans la case
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, textes[i], BLACK);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {rectCase.x + (GRID_CELL_WIDTH*2 - textSurface->w) / 2,  // Centrer en X
                            rectCase.y + (GRID_CELL_HEIGHT - textSurface->h) / 2,  // Centrer en Y
                            textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Libérer les ressources utilisées pour le texte
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);



    // Dessiner les 3 cases à droite avec les textes "12 P", "12 M", "12 D"
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);

    // Largeur et hauteur des cases à droite
    int rightCaseWidth = GRID_CELL_WIDTH;  // Largeur fixe des cases
    int rightCaseHeight = GRID_CELL_HEIGHT ;  // Plus grandes cases verticalement

    // Position de départ (au bord droit de la grille)
    int startX = GRID_CELL_WIDTH * 13;  // Décalage après les 12 colonnes
    int startY = 0;

    // Textes des cases
    const char* rightTexts[] = {"P 12", "M 12", "D 12"};

    // Dessiner chaque case
    for (int i = 0; i < 3; i++) {
        // Définir les dimensions et la position de chaque case
        SDL_Rect rectCase = {startX, startY + i * rightCaseHeight, rightCaseWidth, rightCaseHeight};

        // Remplir la case (blanc)
        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
        SDL_RenderFillRect(renderer, &rectCase);

        SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
        SDL_RenderDrawRect(renderer, &rectCase);


        // Ajouter le texte centré
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, rightTexts[i], BLACK);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {rectCase.x + (rightCaseWidth - textSurface->w) / 2,  // Centrer en X
                            rectCase.y + (rightCaseHeight - textSurface->h) / 2,  // Centrer en Y
                            textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Libérer les ressources de texte
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    }


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

    // Attendre 5 secondes avant de fermer
    SDL_Delay(20000);

    // Libérer les ressources
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Quitter SDL_ttf et SDL
    TTF_Quit();
    SDL_Quit();

    return 0;
}
