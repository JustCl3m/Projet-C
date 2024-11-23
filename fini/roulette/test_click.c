#include <SDL2/SDL.h>
#include <stdio.h>

// Structure pour un bouton
typedef struct {
    SDL_Rect rect;  // Position et taille du bouton
    SDL_Color color; // Couleur du bouton
    const char *label; // Texte du bouton
} Button;

// Fonction pour dessiner le bouton
void drawButton(SDL_Renderer *renderer, Button *button) {
    // Dessiner le rectangle du bouton
    SDL_SetRenderDrawColor(renderer, button->color.r, button->color.g, button->color.b, button->color.a);
    SDL_RenderFillRect(renderer, &button->rect);

    // Ajouter le texte du bouton (ici, pour l'exemple, on ne l'ajoute pas, mais vous pouvez utiliser SDL_ttf)
}

// Fonction pour vérifier si un clic est dans les limites du bouton
int isButtonClicked(Button *button, int x, int y) {
    return (x >= button->rect.x && x <= button->rect.x + button->rect.w &&
            y >= button->rect.y && y <= button->rect.y + button->rect.h);
}

int main(int argc, char *argv[]) {
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Créer la fenêtre
    SDL_Window *window = SDL_CreateWindow("SDL Button Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Créer le renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Définir un bouton
    Button button = {
        { 300, 250, 200, 50 }, // Position (x, y) et taille (largeur, hauteur)
        { 0, 255, 0, 255 }, // Couleur du bouton (vert)
        "Cliquez ici" // Texte du bouton (nous n'affichons pas ici, mais vous pouvez avec SDL_ttf)
    };

    // Boucle principale
    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                // Vérifier si l'utilisateur clique sur le bouton
                if (isButtonClicked(&button, e.button.x, e.button.y)) {
                    printf("Bouton cliqué !\n");
                }
            }
        }

        // Effacer l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dessiner le bouton
        drawButton(renderer, &button);

        // Actualiser l'écran
        SDL_RenderPresent(renderer);
    }

    // Nettoyage et fermeture
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
