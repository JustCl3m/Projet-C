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

#define ROULETTE_RADIUS 250   // Rayon de la roulette
#define BALL_RADIUS_MAX 240   // Rayon initial pour la boule (proche du bord de la roulette)
#define BALL_RADIUS_MIN 180   // Rayon minimum pour que la balle ne se rapproche pas trop du centre

typedef struct {
    SDL_Texture *texture;
    int width, height;
} Image;

// Ordre des numéros sur une roulette française
int rouletteSectors[37] = {
    0, 32, 15, 19, 4, 21, 2, 25, 17, 34, 6, 27, 13, 36, 11, 30, 8, 23, 10, 5, 
    24, 16, 33, 1, 20, 14, 31, 9, 22, 18, 29, 7, 28, 12, 35, 3, 26
};

int init(SDL_Window **window, SDL_Renderer **renderer);
Image loadImage(const char *path, SDL_Renderer *renderer);
void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height, float angle);
void cleanUp(SDL_Window *window, SDL_Renderer *renderer, Image *roulette, Image *ball);
void moveBallTowardsCenter(float *ballAngle, float *ballSpeed, float *ballRadius, float *deceleration, int *stopped);
void moveBallWithRouletteSpeed(float *ballAngle, float rouletteSpeed);
int getSectorFromAngle(float ballAngle);

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

    int running = 1;
    int stopped = 0;
    float ballAngle = 0;           // Angle de la balle
    float rouletteAngle = 0;       // Angle de la roulette
    float ballSpeed = 3.0;         // Vitesse initiale de la balle
    float rouletteSpeed = 5.0;     // Vitesse initiale de la roulette (plus élevée pour un arrêt plus long)
    float deceleration = 0.01;     // Décélération de la balle
    float rouletteDeceleration = 0.005; // Décélération plus lente pour la roulette
    float ballRadius = BALL_RADIUS_MAX;  // Commence au bord et descend vers le centre

    // Décalage nécessaire pour aligner correctement les secteurs
    float angleOffset = 10.0f; // Décalage en degrés

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && stopped) {
                // Relancer la roulette et la boule
                stopped = 0;
                ballSpeed = 4.0 + (rand() % 3);  // Vitesse de départ aléatoire
                rouletteSpeed = 5.0;             // Réinitialise la vitesse de la roulette
                ballRadius = BALL_RADIUS_MAX;    // Repart du bord extérieur
            }
        }

        SDL_RenderClear(renderer);

        int rouletteX = (WINDOW_WIDTH - ROULETTE_WIDTH) / 2;
        int rouletteY = (WINDOW_HEIGHT - ROULETTE_HEIGHT) / 2;

        // Rotation de la roulette avec décélération
        if (rouletteSpeed > 0) {
            rouletteAngle -= rouletteSpeed;
            rouletteSpeed -= rouletteDeceleration;  // Applique la décélération lente
            if (rouletteSpeed < 0) rouletteSpeed = 0;  // Assure que la vitesse ne devient pas négative
        }

        renderImage(renderer, roulette, rouletteX, rouletteY, ROULETTE_WIDTH, ROULETTE_HEIGHT, rouletteAngle);

        if (!stopped) {
            moveBallTowardsCenter(&ballAngle, &ballSpeed, &ballRadius, &deceleration, &stopped);
        } else {
            // Une fois que la balle s'est arrêtée, elle prend la vitesse de la roulette
            moveBallWithRouletteSpeed(&ballAngle, rouletteSpeed);
            ballRadius = ROULETTE_RADIUS - 80 ;  // La boule reste à un rayon fixe une fois arrêtée
        }

        // Calcul de la position de la boule en fonction du rayon actuel
        int ballX = WINDOW_WIDTH / 2 + cos(ballAngle) * ballRadius - BALL_WIDTH / 2;
        int ballY = WINDOW_HEIGHT / 2 + sin(ballAngle) * ballRadius - BALL_HEIGHT / 2;
        renderImage(renderer, ball, ballX, ballY, BALL_WIDTH, BALL_HEIGHT, 0);

        // Si la balle s'est arrêtée, déterminer le secteur (calcul uniquement après l'arrêt)
        if (stopped) {
            int sector = getSectorFromAngle(ballAngle + angleOffset); // Appliquer le décalage d'angle
            printf("La balle est dans le secteur %d\n", rouletteSectors[sector]);
        }

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
        *ballAngle += *ballSpeed * 0.05;
        *ballSpeed -= *deceleration * log(*ballSpeed + 15);
        if (*ballSpeed < 0.05) {
            *ballSpeed = 0.05;
        }
        *ballRadius -= 0.3;
        if (*ballRadius <= BALL_RADIUS_MIN) {
            *ballRadius = BALL_RADIUS_MIN;
            *stopped = 1;
        }
    }
}

void moveBallWithRouletteSpeed(float *ballAngle, float rouletteSpeed) {
    *ballAngle -= rouletteSpeed * 0.01752;
}

int getSectorFromAngle(float ballAngle) {
    float angleInDegrees = fmod(ballAngle * (180 / M_PI), 360.0f);
    if (angleInDegrees < 0) angleInDegrees += 360.0f;

    // Chaque secteur mesure 360 / 37 degrés
    float degreesPerSector = 360.0f / 37.0f;

    // Calculer l'index du secteur
    int sectorIndex = (int)(angleInDegrees / degreesPerSector);
    return sectorIndex;
}
