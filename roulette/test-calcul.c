#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define RADIUS 150
#define BALL_RADIUS 20
#define NUM_PARTS 10
#define CENTER_X (WINDOW_WIDTH / 2)
#define CENTER_Y (WINDOW_HEIGHT / 2)
#define FPS 60
#define NUM_FRAMES 3600 // Durée de l'animation en nombre de frames

// Prototype de la fonction pour dessiner un cercle rempli
void SDL_RenderFillCircle(SDL_Renderer *renderer, int x, int y, int radius);

// Fonction pour dessiner un cercle découpé en 10 parties
void drawCircleWithParts(SDL_Renderer *renderer, int angleOffset) {
    int i;
    for (i = 0; i < NUM_PARTS; i++) {
        float angle1 = (i * 360 / NUM_PARTS) + angleOffset;
        float angle2 = ((i + 1) * 360 / NUM_PARTS) + angleOffset;

        int x1 = CENTER_X + RADIUS * cos(angle1 * M_PI / 180.0);
        int y1 = CENTER_Y + RADIUS * sin(angle1 * M_PI / 180.0);

        int x2 = CENTER_X + RADIUS * cos(angle2 * M_PI / 180.0);
        int y2 = CENTER_Y + RADIUS * sin(angle2 * M_PI / 180.0);

        SDL_SetRenderDrawColor(renderer, (i * 25) % 255, (i * 50) % 255, (i * 100) % 255, 255); // Couleurs dynamiques
        SDL_RenderDrawLine(renderer, CENTER_X, CENTER_Y, x1, y1);
        SDL_RenderDrawLine(renderer, CENTER_X, CENTER_Y, x2, y2);
    }
}

// Fonction pour dessiner la boule
void drawBall(SDL_Renderer *renderer, int angleOffset) {
    int ballX = CENTER_X + (RADIUS + BALL_RADIUS) * cos(angleOffset * M_PI / 180.0);
    int ballY = CENTER_Y + (RADIUS + BALL_RADIUS) * sin(angleOffset * M_PI / 180.0);

    // Dessiner la boule
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Couleur rouge
    SDL_RenderFillCircle(renderer, ballX, ballY, BALL_RADIUS); // Utilisation correcte
}

// Fonction pour dessiner un cercle rempli (équivalent de fillellipse)
void SDL_RenderFillCircle(SDL_Renderer *renderer, int x, int y, int radius) {
    int dx, dy, d;
    int xi = radius, yi = 0;

    // Remplir le cercle avec des pixels
    d = 3 - 2 * radius;
    while (xi >= yi) {
        for (dy = y - yi; dy <= y + yi; dy++) {
            SDL_RenderDrawPoint(renderer, x + xi, dy);
            SDL_RenderDrawPoint(renderer, x - xi, dy);
        }
        for (dx = x - xi; dx <= x + xi; dx++) {
            SDL_RenderDrawPoint(renderer, dx, y + yi);
            SDL_RenderDrawPoint(renderer, dx, y - yi);
        }

        yi++;
        if (d > 0) {
            xi--;
            d = d + 4 * (yi - xi) + 10;
        } else {
            d = d + 4 * yi + 6;
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Cercle et Boule en Rotation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int circleAngle = 0;
    int ballAngle = 0;
    int frameCount = 0;

    // Boucle principale
    int running = 1;
    while (running && frameCount < NUM_FRAMES) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Effacer l'écran avec un fond noir
        SDL_RenderClear(renderer);

        drawCircleWithParts(renderer, circleAngle); // Dessiner le cercle avec les parties
        drawBall(renderer, ballAngle); // Dessiner la boule

        SDL_RenderPresent(renderer);

        // Incrémenter les angles pour la prochaine frame
        circleAngle++; // Tourner le cercle dans le sens horaire
        ballAngle--; // Tourner la boule dans le sens anti-horaire

        SDL_Delay(1000 / FPS); // Limiter la vitesse de l'animation (FPS)
        frameCount++;
    }

    // Libérer les ressources et quitter SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
