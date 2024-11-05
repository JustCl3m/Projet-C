#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PONEY_COUNT 8
#define FINISH_LINE (WINDOW_WIDTH - 100)

// Structure pour représenter un poney
typedef struct {
    SDL_Texture *texture;
    SDL_Rect pos;
    int speed;
} Poney;

void load_poney_textures(SDL_Renderer *renderer, Poney poneys[]) {
    for (int i = 0; i < PONEY_COUNT; i++) {
        poneys[i].texture = IMG_LoadTexture(renderer, "poney.png");
        poneys[i].pos.x = 0; // Position de départ
        poneys[i].pos.y = i * (WINDOW_HEIGHT / PONEY_COUNT); // Espacement vertical pour chaque poney
        poneys[i].pos.w = 80; // Largeur de l'image
        poneys[i].pos.h = 60; // Hauteur de l'image
        poneys[i].speed = rand() % 3 + 2; // Vitesse aléatoire entre 2 et 4
    }
}

void render_background(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // Vert pour l'hippodrome
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Ligne d'arrivée blanche
    SDL_Rect finish_line = {FINISH_LINE, 0, 10, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &finish_line);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    // Initialisation de SDL et création de la fenêtre
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Course de Poneys", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_PNG);

    // Initialisation des poneys
    Poney poneys[PONEY_COUNT];
    load_poney_textures(renderer, poneys);

    int running = 1;
    int finished = 0;
    int winner = -1;
    
    // Boucle de jeu
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        render_background(renderer); // Affiche l'hippodrome et la ligne d'arrivée

        // Déplacement des poneys
        for (int i = 0; i < PONEY_COUNT; i++) {
            if (poneys[i].pos.x < FINISH_LINE) {
                poneys[i].pos.x += poneys[i].speed;
            } else if (!finished) {
                finished = 1;
                winner = i;
            }
            SDL_RenderCopy(renderer, poneys[i].texture, NULL, &poneys[i].pos);
        }

        SDL_RenderPresent(renderer); // Mettre à jour la fenêtre
        SDL_Delay(50);

        if (finished) {
            SDL_Delay(2000);
            printf("Le gagnant est le Poney %d !\n", winner + 1);
            running = 0;
        }
    }

    // Libération des ressources
    for (int i = 0; i < PONEY_COUNT; i++) {
        SDL_DestroyTexture(poneys[i].texture);
    }

    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
