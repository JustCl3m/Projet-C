#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PONEY_COUNT 4
#define FINISH_LINE (WINDOW_WIDTH - 100)
#define OBSTACLE_COUNT 5

// Structure pour représenter un poney
typedef struct {
    SDL_Texture *texture[2]; // Deux textures pour l'animation
    SDL_Rect pos;
    int speed;
    int anim_state;
} Poney;

// Structure pour représenter un obstacle
typedef struct {
    SDL_Rect pos;
} Obstacle;

SDL_Texture *background_texture;
Mix_Chunk *win_sound;
TTF_Font *font;

void load_background_texture(SDL_Renderer *renderer) {
    background_texture = IMG_LoadTexture(renderer, "background.png");
    if (background_texture == NULL) {
        printf("Erreur de chargement de l'image de fond: %s\n", IMG_GetError());
        exit(1);
    }
}

void load_poney_textures(SDL_Renderer *renderer, Poney poneys[]) {
    const char *poney_images[PONEY_COUNT][2] = {
        {"poney1.png", "poney1.png"},
        {"poney2.png", "poney2.png"},
        {"poney3.png", "poney3.png"},
        {"poney4.png", "poney4.png"},
    };

    for (int i = 0; i < PONEY_COUNT; i++) {
        for (int j = 0; j < 2; j++) {
            poneys[i].texture[j] = IMG_LoadTexture(renderer, poney_images[i][j]);
            if (poneys[i].texture[j] == NULL) {
                printf("Erreur de chargement de l'image %s: %s\n", poney_images[i][j], IMG_GetError());
                exit(1);
            }
        }
        poneys[i].pos.x = 0;
        poneys[i].pos.y = i * 100 + 50;
        poneys[i].pos.w = 60;
        poneys[i].pos.h = 60;
        poneys[i].speed = 2 + rand() % 3;
        poneys[i].anim_state = 0;
    }
}

void init_obstacles(Obstacle obstacles[]) {
    for (int i = 0; i < OBSTACLE_COUNT; i++) {
        obstacles[i].pos.x = 200 + rand() % (WINDOW_WIDTH - 300);
        obstacles[i].pos.y = rand() % (WINDOW_HEIGHT - 60);
        obstacles[i].pos.w = 30;
        obstacles[i].pos.h = 30;
    }
}

void render_background(SDL_Renderer *renderer) {
    SDL_RenderCopy(renderer, background_texture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect finish_line = {FINISH_LINE, 0, 10, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &finish_line);
}

void render_obstacles(SDL_Renderer *renderer, Obstacle obstacles[]) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < OBSTACLE_COUNT; i++) {
        SDL_RenderFillRect(renderer, &obstacles[i].pos);
    }
}

int check_collision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void render_text(SDL_Renderer *renderer, const char *text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("Erreur d'initialisation de SDL_image: %s\n", IMG_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("Erreur d'initialisation de SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur d'initialisation de SDL_mixer: %s\n", Mix_GetError());
        return 1;
    }

    win_sound = Mix_LoadWAV("win.wav");
    if (!win_sound) {
        printf("Erreur de chargement du son: %s\n", Mix_GetError());
        return 1;
    }

    font = TTF_OpenFont("Roboto-Regular.ttf", 24);
    if (!font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Course de Poneys", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    load_background_texture(renderer);

    Poney poneys[PONEY_COUNT];
    load_poney_textures(renderer, poneys);

    Obstacle obstacles[OBSTACLE_COUNT];
    init_obstacles(obstacles);

    int running = 1;
    int finished = 0;
    int winner = -1;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        render_background(renderer);
        render_obstacles(renderer, obstacles);

        for (int i = 0; i < PONEY_COUNT; i++) {
            for (int j = 0; j < OBSTACLE_COUNT; j++) {
                if (check_collision(poneys[i].pos, obstacles[j].pos)) {
                    poneys[i].speed = 1;
                }
            }

            if (poneys[i].pos.x < FINISH_LINE) {
                poneys[i].pos.x += poneys[i].speed;
                poneys[i].anim_state = (poneys[i].anim_state + 1) % 2;
            } else if (!finished) {
                finished = 1;
                winner = i;
                Mix_PlayChannel(-1, win_sound, 0);
            }
            SDL_RenderCopy(renderer, poneys[i].texture[poneys[i].anim_state], NULL, &poneys[i].pos);
        }

        if (finished) {
            char winner_text[50];
            sprintf(winner_text, "Le gagnant est le Poney %d !", winner + 1);
            render_text(renderer, winner_text, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(50);
    }

    for (int i = 0; i < PONEY_COUNT; i++) {
        SDL_DestroyTexture(poneys[i].texture[0]);
        SDL_DestroyTexture(poneys[i].texture[1]);
    }
    SDL_DestroyTexture(background_texture);
    Mix_FreeChunk(win_sound);
    TTF_CloseFont(font);
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
