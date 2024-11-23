#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define ROULETTE_WIDTH 500
#define ROULETTE_HEIGHT 500
#define BALL_WIDTH 20
#define BALL_HEIGHT 20
#define ROULETTE_RADIUS 250
#define BALL_RADIUS_MAX 240
#define BALL_RADIUS_MIN 180
#define NUM_CASES 37  // Définir le nombre de cases

// Tableau des numéros de la roulette
int rouletteValues[NUM_CASES] = {
    0, 32, 15, 19, 4, 21, 2, 25, 17, 34, 6, 27, 13, 36, 11, 30, 8, 23, 10, 5,
    24, 16, 33, 1, 20, 14, 31, 9, 22, 18, 29, 7, 28, 12, 35, 3, 26
};


// Tableau avec des indices allant de -36 à +36, rempli avec les valeurs de la roulette
int rouletteNumbers[NUM_CASES*2];

// Fonction pour remplir le tableau avec des indices de -36 à +36
void fillRoulette() {
    // Remplir les indices de 0 à 36 dans le tableau correspondant à l'index de -36 à +36
    for (int i = 0; i < NUM_CASES; i++) {
        rouletteNumbers[i + NUM_CASES] = rouletteValues[i];  // Les indices de 0 à 36 sont décalés de +36
    }

    // Remplir les indices négatifs de -1 à -36
    for (int i = 1; i <= NUM_CASES; i++) {
        rouletteNumbers[NUM_CASES - i] = rouletteValues[NUM_CASES - i];  // Mappage inverse pour les indices négatifs
    }
}

#define START_NUM 34

typedef struct {
    SDL_Texture *texture;
    int width, height;
} Image;

int init(SDL_Window **window, SDL_Renderer **renderer);
Image loadImage(const char *path, SDL_Renderer *renderer);
void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height, float angle);
void cleanUp(SDL_Window *window, SDL_Renderer *renderer, Image *roulette, Image *ball, TTF_Font *font);
void moveBallTowardsCenter(float *ballAngle, float *ballSpeed, float *ballRadius, float *deceleration, int *stopped);
void followRoulette(float *ballAngle, float rouletteSpeed);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y);

// Fonction pour dessiner une ligne représentant un angle
void drawAngle(SDL_Renderer *renderer, int centerX, int centerY, float angle, int length, SDL_Color color);

int main() {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!init(&window, &renderer)) {
        printf("Erreur d'initialisation SDL\n");
        return -1;
    }

    if (TTF_Init() == -1) {
    printf("Erreur d'initialisation SDL_ttf: %s\n", TTF_GetError());
    return 0;
    }

    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 24);  // Vous pouvez changer "arial.ttf" et la taille (ici 24)
    if (!font) {
    printf("Erreur de chargement de la police: %s\n", TTF_GetError());
    return 0;
    }


    Image roulette = loadImage("roulette.png", renderer);
    Image ball = loadImage("ball.png", renderer);
    if (roulette.texture == NULL || ball.texture == NULL) {
        cleanUp(window, renderer, &roulette, &ball, font);
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


        fillRoulette();

        for (int i = -36; i <= 36; i++) {
        printf("Index %d: %d\n", i, rouletteNumbers[i + NUM_CASES]);  // Décaler de +36 pour accéder à l'indice de -36 à +36
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

        // Dessiner les angles graphiquement
        int centerX = WINDOW_WIDTH / 2;
        int centerY = WINDOW_HEIGHT / 2;

        // Affichage des informations dans la console
        int rouletteAngledegree = ((int)(rouletteAngle) % 360);
        int ballAngledegree = (((int)(ballAngle)) % 360);

        printf("Roulette Angle: %.2d°, Ball Angle: %.2d°\n", rouletteAngledegree, ballAngledegree);

        // Calcul de la section de la roulette (ajustée)
        int section = round((int)(rouletteAngledegree-ballAngledegree) / 9.72972973);
        printf("Section: %d\n", section);

        int caseNum = rouletteNumbers[(9-section)];
        printf("Case: %d\n", caseNum);
    
        char numberText[100];
        sprintf(numberText, "La case est :%d", caseNum);  // Exemple de numéro
        renderText(renderer, font, numberText, 500, 0);  // Affiche le texte au centre de l'écran



        SDL_Color ballColor = {255, 0, 0, 255}; // Rouge pour la balle
        drawAngle(renderer, centerX, centerY, ballAngledegree, ROULETTE_RADIUS, ballColor);

        SDL_Color rouletteColor = {0, 255, 0, 255}; // Vert pour un angle fixe (exemple)
        drawAngle(renderer, centerX, centerY, 0, ROULETTE_RADIUS, rouletteColor);

        SDL_Color rouletteAxisColor = {0, 0, 255, 255}; // Bleu pour l'axe de la roulette
        drawAngle(renderer, centerX, centerY, rouletteAngle, ROULETTE_RADIUS, rouletteAxisColor);

        renderImage(renderer, ball, screenBallX, screenBallY, BALL_WIDTH, BALL_HEIGHT, 0);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    cleanUp(window, renderer, &roulette, &ball, font);
    return 0;
}

// Fonction pour initialiser la fenêtre et le renderer
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

// Fonction pour charger une image et la convertir en texture SDL
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

// Fonction pour rendre une image à une position donnée
void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height, float angle) {
    SDL_Rect dstRect = {x, y, width, height};
    SDL_RenderCopyEx(renderer, img.texture, NULL, &dstRect, angle, NULL, SDL_FLIP_NONE);
}

// Fonction pour libérer la mémoire
void cleanUp(SDL_Window *window, SDL_Renderer *renderer, Image *roulette, Image *ball, TTF_Font *font) {
    if (roulette->texture) SDL_DestroyTexture(roulette->texture);
    if (ball->texture) SDL_DestroyTexture(ball->texture);
    SDL_DestroyRenderer(renderer);
    if (font) TTF_CloseFont(font);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

// Fonction pour déplacer la balle vers le centre
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

// Fonction pour faire suivre la roulette à la balle
void followRoulette(float *ballAngle, float rouletteSpeed) {
    *ballAngle -= rouletteSpeed;
}

// Fonction pour dessiner un angle
void drawAngle(SDL_Renderer *renderer, int centerX, int centerY, float angle, int length, SDL_Color color) {
    int endX = centerX + (int)(cos(angle * M_PI / 180.0) * length);
    int endY = centerY + (int)(sin(angle * M_PI / 180.0) * length);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, centerX, centerY, endX, endY);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y) {
    SDL_Color textColor = {255, 255, 255, 255};  // Blanc
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    if (!textSurface) {
        printf("Erreur de création de la surface de texte: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);  // Libérer la surface après la conversion en texture

    if (!textTexture) {
        printf("Erreur de création de la texture de texte: %s\n", SDL_GetError());
        return;
    }

    int textWidth = 0, textHeight = 0;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    SDL_Rect textRect = {x, y, textWidth, textHeight};

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);  // Libérer la texture
}
