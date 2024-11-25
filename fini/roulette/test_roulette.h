#ifndef TEST_ROULETTE_H
#define TEST_ROULETTE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Constantes
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define ROULETTE_WIDTH 500
#define ROULETTE_HEIGHT 500
#define BALL_WIDTH 20
#define BALL_HEIGHT 20
#define ROULETTE_RADIUS 250
#define BALL_RADIUS_MAX 240
#define BALL_RADIUS_MIN 180
#define NUM_CASES 37  // Nombre de cases sur la roulette

// Structure pour une image SDL
typedef struct {
    SDL_Texture *texture;
    int width, height;
} Image;

// Variables globales (extern si utilisées ailleurs)
extern int rouletteValues[NUM_CASES];
extern int rouletteNumbers[NUM_CASES * 2];  // Pour les indices -36 à +36


void lancerRoulette();  // Fonction principale pour lancer la roulette


// Fonctions
void fillRoulette();  // Remplit le tableau `rouletteNumbers`

// Initialisation
int init(SDL_Window **window, SDL_Renderer **renderer);

// Gestion des images
Image loadImage(const char *path, SDL_Renderer *renderer);
void renderImage(SDL_Renderer *renderer, Image img, int x, int y, int width, int height, float angle);

// Nettoyage
void cleanUp(SDL_Window *window, SDL_Renderer *renderer, Image *roulette, Image *ball, TTF_Font *font);

// Déplacement de la balle
void moveBallTowardsCenter(float *ballAngle, float *ballSpeed, float *ballRadius, float *deceleration, int *stopped);
void followRoulette(float *ballAngle, float rouletteSpeed);

// Affichage
void drawAngle(SDL_Renderer *renderer, int centerX, int centerY, float angle, int length, SDL_Color color);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y);

#endif // TEST_ROULETTE_H
