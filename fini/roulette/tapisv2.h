#ifndef TAPISV2_H
#define TAPISV2_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_CELL_WIDTH 50
#define GRID_CELL_HEIGHT 50
#define FONT_SIZE 24
#define max_rows 6
#define max_cols 14

#define CHIP_WIDTH 30
#define CHIP_HEIGHT 30

#define OFFSET_X 50
#define OFFSET_Y 0

#define MAX_BETS 10  // Nombre maximum de jetons qu'un joueur peut avoir

#define N 6
#define M 14

// Declare global variables as extern
extern int player_tokens; // Declaring the global variable
extern int bets[max_rows][max_cols];  // Declaring the global variable

// Structure pour stocker les mises
typedef struct {
    int montant;   // Montant de la mise
    int numero;    // Le numéro sur lequel on a misé
    char couleur;  // 'R' pour Rouge, 'B' pour Noir, 'V' pour Vert (0)
} Mise;

extern Mise pari[N][M]; // Declaring the global array

extern int available_bets[];  // Declaring the global array
extern int selected_bet_index;  // Declaring the global variable
extern int player_balance;  // Declaring the global variable

// Color definitions
extern SDL_Color RED;
extern SDL_Color BLACK;
extern SDL_Color GREEN;
extern SDL_Color WHITE;
extern SDL_Color BLUE;

// Function declarations
void draw_chips(SDL_Renderer* renderer, SDL_Texture* chipTextures[], int x, int y, int numChips, int cellWidth, int cellHeight);
void tapis();  // Corrected function declaration

#endif // TAPISV2_H
