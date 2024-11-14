#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define ROULETTE_WIDTH 500
#define ROULETTE_HEIGHT 500
#define BALL_WIDTH 20
#define BALL_HEIGHT 20
#define ROULETTE_RADIUS 250
#define BALL_RADIUS_MAX 240
#define BALL_RADIUS_MIN 180

typedef struct {
    SDL_Texture *texture;
    int width, height;
} Image;

int init(SDL_Window **window, SDL_Renderer **renderer);
Image loadImage(const char *path, SDL_Renderer *renderer);
void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height, float angle);
void cleanUp(SDL_Window *window, SDL_Renderer *renderer, Image *roulette, Image *ball);
void moveBallTowardsCenter(float *ballAngle, float *ballSpeed, float *ballRadius, float *deceleration, int *stopped);
void followRoulette(float *ballAngle, float rouletteSpeed);

int main() {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!init(&window, &renderer)) {
        printf("Erreur d'initialisation SDL\n");
        return -1;
    }

    Image roulette = loadImage("roulette.png", renderer);
    Image ball = loadImage("ball.png", renderer);
    if (roulette.texture == NULL || ball.texture == NULL) {
        cleanUp(window, renderer, &roulette, &ball);
        return -1;
    }

    srand(time(NULL));

    int running = 1, stopped = 0;
    float ballAngle = 0, rouletteAngle = 0;
    float ballSpeed = 3.0, rouletteSpeed = 5.0;
    float deceleration = 0.02;
    float ballRadius = BALL_RADIUS_MAX;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && stopped) {
                stopped = 0;
                ballSpeed = 4.0 + (rand() % 10);
                rouletteSpeed = 5.0 + (rand() % 3);
                ballRadius = BALL_RADIUS_MAX;
            }
        }

        SDL_RenderClear(renderer);

        int rouletteX = (WINDOW_WIDTH - ROULETTE_WIDTH) / 2;
        int rouletteY = (WINDOW_HEIGHT - ROULETTE_HEIGHT) / 2;

        if (rouletteSpeed > 0) {
            rouletteAngle -= rouletteSpeed;
            rouletteSpeed -= 0.01;
            if (rouletteSpeed < 0) rouletteSpeed = 0;
        }

        renderImage(renderer, roulette, rouletteX, rouletteY, ROULETTE_WIDTH, ROULETTE_HEIGHT, rouletteAngle);

        if (!stopped) {
            moveBallTowardsCenter(&ballAngle, &ballSpeed, &ballRadius, &deceleration, &stopped);
        } else {
            followRoulette(&ballAngle, rouletteSpeed);
        }

        // Calcul des coordonnées de la balle
        int ballX = (int)(cos(ballAngle * M_PI / 180.0) * ballRadius);
        int ballY = (int)(sin(ballAngle * M_PI / 180.0) * ballRadius);
        int screenBallX = WINDOW_WIDTH / 2 + ballX - BALL_WIDTH / 2;
        int screenBallY = WINDOW_HEIGHT / 2 + ballY - BALL_HEIGHT / 2;



        int rouletteAngledegree = abs((int)(rouletteAngle) % 360);
        int BallAngledegree = abs(((int)((ballAngle+90)) % 360));

        int section = 360/37;
        int secteur = (rouletteAngledegree - BallAngledegree)/section;

        printf("Roulette Angle: %.2d°, Ball Angle: %.2d°\n", rouletteAngledegree, BallAngledegree);
        printf("Secteur de la roulette: %.2d°", secteur);

        renderImage(renderer, ball, screenBallX, screenBallY, BALL_WIDTH, BALL_HEIGHT, 0);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    cleanUp(window, renderer, &roulette, &ball);
    return 0;
}

int init(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return 0;
    }

    *window = SDL_CreateWindow("Roulette", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!*window) {
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

Image loadImage(const char *path, SDL_Renderer *renderer) {
    Image img;
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        printf("Erreur de chargement de l'image %s: %s\n", path, SDL_GetError());
        img.texture = NULL;
        return img;
    }

    img.texture = SDL_CreateTextureFromSurface(renderer, surface);
    img.width = surface->w;
    img.height = surface->h;
    SDL_FreeSurface(surface);
    return img;
}

void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height, float angle) {
    SDL_Rect dstRect = {x, y, width, height};
    SDL_RenderCopyEx(renderer, img.texture, NULL, &dstRect, angle, NULL, SDL_FLIP_NONE);
}

void cleanUp(SDL_Window *window, SDL_Renderer *renderer, Image *roulette, Image *ball) {
    if (roulette->texture) SDL_DestroyTexture(roulette->texture);
    if (ball->texture) SDL_DestroyTexture(ball->texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void moveBallTowardsCenter(float *ballAngle, float *ballSpeed, float *ballRadius, float *deceleration, int *stopped) {
    if (*ballSpeed > 0) {
        *ballAngle += *ballSpeed;
        *ballSpeed -= *deceleration;
        if (*ballSpeed < 0.05) *ballSpeed = 0.05;
        *ballRadius -= 0.3;
        if (*ballRadius <= BALL_RADIUS_MIN) {
            *ballRadius = BALL_RADIUS_MIN;
            *stopped = 1;
        }
    }
}

void followRoulette(float *ballAngle, float rouletteSpeed) {
    *ballAngle -= rouletteSpeed;
}
