#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>



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


int player_tokens = MAX_BETS;  // Jetons du joueur
int bets[max_rows][max_cols] = {0};  // Tableau pour stocker les mises sur chaque case


// Structure pour stocker les mises
typedef struct {
    int montant;   // Montant de la mise
    int numero;    // Le numéro sur lequel on a misé
    char couleur;  // 'R' pour Rouge, 'B' pour Noir, 'V' pour Vert (0)
} Mise;

Mise pari[N][M];




int available_bets[] = {1, 5, 25, 50, 100};  // Options de mise possibles
int selected_bet_index = 0;  // L'index de la mise actuellement sélectionnée
int player_balance = 500;  // Solde initial du joueur (à ajuster en fonction du jeu)



SDL_Color RED = {255, 0, 0, 255};
SDL_Color BLACK = {0, 0, 0, 255};
SDL_Color GREEN = {0, 128, 0, 255};
SDL_Color WHITE = {255, 255, 255, 255};  // WHITE avec alpha 255
SDL_Color BLUE = {0, 0, 255, 255};  // Couleur de la case sélectionnée


// Roulette 

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


void lancerRoulette(SDL_Window *window, SDL_Renderer *renderer) {

        TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 24);  // Vous pouvez changer "arial.ttf" et la taille (ici 24)
        if (!font) {
            printf("Erreur de chargement de la police: %s\n", TTF_GetError());
            return;
        }

        Image roulette = loadImage("roulette.png", renderer);
        Image ball = loadImage("ball.png", renderer);
        if (roulette.texture == NULL || ball.texture == NULL) {
            cleanUp(window, renderer, &roulette, &ball, font);
            return;
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

            // for (int i = -36; i <= 36; i++) {
            //     printf("Index %d: %d\n", i, rouletteNumbers[i + NUM_CASES]);  // Décaler de +36 pour accéder à l'indice de -36 à +36
            // }

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

            //printf("Roulette Angle: %.2d°, Ball Angle: %.2d°\n", rouletteAngledegree, ballAngledegree);

            // Calcul de la section de la roulette (ajustée)
            int section = round((int)(rouletteAngledegree - ballAngledegree) / 9.72972973);
            //printf("Section: %d\n", section);

            int caseNum = rouletteNumbers[(9 - section)];
           // printf("Case: %d\n", caseNum);

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

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <time.h>




void draw_chips(SDL_Renderer* renderer, SDL_Texture* chipTextures[], int x, int y, int numChips, int cellWidth, int cellHeight);



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
void draw_table(SDL_Renderer* renderer, TTF_Font* font, int selected_row, int selected_col, SDL_Texture** chipTextures) {
    int x, y;
    char numStr[3];
    SDL_Surface* textSurface = NULL;
    SDL_Texture* textTexture = NULL;
    SDL_Rect textRect;


// Afficher le solde
    char balanceStr[50];
    snprintf(balanceStr, sizeof(balanceStr), "Solde : %d €", player_balance);
    SDL_Surface* balanceTextSurface = TTF_RenderText_Solid(font, balanceStr, (SDL_Color){255, 255, 255});
    SDL_Texture* balanceTextTexture = SDL_CreateTextureFromSurface(renderer, balanceTextSurface);
    SDL_Rect balanceTextRect = {20, 20, balanceTextSurface->w, balanceTextSurface->h};
    SDL_RenderCopy(renderer, balanceTextTexture, NULL, &balanceTextRect);
    
    // Afficher les options de mise et surligner la mise sélectionnée
    for (int i = 0; i < sizeof(available_bets) / sizeof(int); i++) {
        char betStr[50];
        snprintf(betStr, sizeof(betStr), "%d €", available_bets[i]);

        SDL_Surface* betTextSurface = TTF_RenderText_Solid(font, betStr, (SDL_Color){255, 255, 255});
        SDL_Texture* betTextTexture = SDL_CreateTextureFromSurface(renderer, betTextSurface);

        // Surligner la mise sélectionnée
        SDL_Color betTextColor = (i == selected_bet_index) ? (SDL_Color){255, 0, 0} : (SDL_Color){255, 255, 255}; // Rouge pour la mise sélectionnée
        betTextSurface = TTF_RenderText_Solid(font, betStr, betTextColor);
        betTextTexture = SDL_CreateTextureFromSurface(renderer, betTextSurface);

        SDL_Rect betTextRect = {20, 300 + (i * 40), betTextSurface->w, betTextSurface->h};
        SDL_RenderCopy(renderer, betTextTexture, NULL, &betTextRect);

        SDL_FreeSurface(betTextSurface);
        SDL_DestroyTexture(betTextTexture);
    }


    // Dessiner la case "0" fusionnée sur 3 cases de hauteur
    x = 0;
    y = 0;

    // Déterminer la couleur en fonction de la sélection
    SDL_Color cell_color = (selected_col == -1) ? BLUE : GREEN;

    // Appliquer la couleur de la case
    SDL_SetRenderDrawColor(renderer, cell_color.r, cell_color.g, cell_color.b, cell_color.a);


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

            // Vérifier si cette case est la case sélectionnée
            SDL_Color cell_color = (selected_row == i +1 && selected_col == j) ? BLUE : (is_red(number) ? RED : BLACK);

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



    // Définir la couleur pour les sections


// Définir la largeur de chaque section (12 colonnes)
int sectionWidth = 4 * GRID_CELL_WIDTH;  // Chaque section fait la largeur de 4 cases (12 colonnes)
int sectionHeight = GRID_CELL_HEIGHT;  // Une seule ligne pour chaque section

// Tableau des titres des sections
const char* sections[] = {"1st 12", "2nd 12", "3rd 12"};

// Dessiner chaque section avec le texte centré
for (int i = 0; i < 3; i++) {  // Trois sections

    SDL_Color cell2_color = (selected_row == 4 && selected_col ==i) ? BLUE : WHITE;  // Vérifie si une section est sélectionnée
    // Calculer la position de la section
    SDL_Rect rectSection = {GRID_CELL_WIDTH + i * sectionWidth, 3 * GRID_CELL_HEIGHT, sectionWidth, sectionHeight};
    
    // Dessiner la case pour la section
    SDL_SetRenderDrawColor(renderer, cell2_color.r, cell2_color.g, cell2_color.b, cell2_color.a);
    SDL_RenderFillRect(renderer, &rectSection);

    // Dessiner le texte de la section, centré dans la case
    SDL_Surface* sectionTextSurface = TTF_RenderText_Solid(font, sections[i], BLACK);
    SDL_Texture* sectionTextTexture = SDL_CreateTextureFromSurface(renderer, sectionTextSurface);
    SDL_Rect sectionTextRect = {rectSection.x + (sectionWidth - sectionTextSurface->w) / 2,  // Centrer en X
                                rectSection.y + (sectionHeight - sectionTextSurface->h) / 2,  // Centrer en Y
                                sectionTextSurface->w, sectionTextSurface->h};
    SDL_RenderCopy(renderer, sectionTextTexture, NULL, &sectionTextRect);
    

    // Libérer les ressources du texte
    SDL_FreeSurface(sectionTextSurface);
    SDL_DestroyTexture(sectionTextTexture);
}
    

    // Position Y des cases
    int startY = 4 * GRID_CELL_HEIGHT;  // Position Y pour la ligne des 6 cases

    // Textes à afficher dans chaque case
    const char* textes[] = {"1 a 18", "PAIR", "ROUGE", "NOIR", "IMPAIR", "19 a 36"};

    // Dessiner chaque case et le texte correspondant
    for (int i = 0; i < 6; i++) {


        // Position de la case
        SDL_Rect rectCase = {GRID_CELL_WIDTH + i * GRID_CELL_WIDTH*2, startY, GRID_CELL_WIDTH*2, GRID_CELL_HEIGHT};
        
        // Dessiner la case (noir)

        // Vérifier si cette case est la case sélectionnée
        SDL_Color cell_color = (selected_row == 5 && selected_col == i) ? BLUE : WHITE;

        SDL_SetRenderDrawColor(renderer, cell_color.r, cell_color.g, cell_color.b, cell_color.a);
        SDL_RenderFillRect(renderer, &rectCase);

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


        // Vérifier si cette case est la case sélectionnée
        SDL_Color cell_color = (selected_row == i && selected_col == 12) ? BLUE : WHITE;        


        // Remplir la case (blanc)
        SDL_SetRenderDrawColor(renderer, cell_color.r, cell_color.g, cell_color.b, cell_color.a);
        SDL_RenderFillRect(renderer, &rectCase);



        // Ajouter le texte centré
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, rightTexts[i], BLACK);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {rectCase.x + (rightCaseWidth - textSurface->w) / 2,  // Centrer en X
                            rectCase.y + (rightCaseHeight - textSurface->h) / 2,  // Centrer en Y
                            textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);




for (int j = -1; j < 13; j++) {  // Colonnes
    for (int i = 0; i < 5; i++) {  // Rangées
        // Calcul des coordonnées pour chaque case
        int x = j * GRID_CELL_WIDTH + OFFSET_X;
        int y = i * GRID_CELL_HEIGHT + OFFSET_Y;

        int cellWidth = GRID_CELL_WIDTH;
        int cellHeight = GRID_CELL_HEIGHT;

        // Si nous sommes dans la ligne 4 (ou une autre ligne nécessitant un décalage)
        if (i == 3) {
            // Agrandir la taille des cases pour la ligne 4
            x += j * 150;  // Augmente l'écart horizontal pour la ligne 4
            cellWidth = 190;  // Largeur de la case pour la ligne 4
        }

        if (i == 4) {
            // Agrandir la taille des cases pour la ligne 4
            x += j * 50;  // Augmente l'écart horizontal pour la ligne 4
            cellWidth = 100;  // Largeur de la case pour la ligne 4
        }


       // if (i == 12) {
            // Agrandir la taille des cases pour la ligne 4
          //  y += j * 100;  // Augmente l'écart horizontal pour la ligne 4
            //cellWidth = 100;  // Largeur de la case pour la ligne 4
      //  }


        if (j== 12) {
            
            if (bets[i][j] > 0) { // Assurez-vous que bets[i + 1][j] est correctement indexé
// Afficher les jetons
            int numChips = bets[i][j]; // Nombre de jetons dans la case
            int chipValue = bets[i][j]; // Exemple : définir une valeur de jeton par rapport à la mise
            SDL_Texture* chipTexture = NULL;

            // Sélectionner la bonne texture en fonction de la valeur
            if (chipValue >= 100) {
                chipTexture = chipTextures[4]; // Jeton de 100
            } else if (chipValue >= 50) {
                chipTexture = chipTextures[3]; // Jeton de 50
            } else if (chipValue >= 25) {
                chipTexture = chipTextures[2]; // Jeton de 25
            } else if (chipValue >= 5) {
                chipTexture = chipTextures[1]; // Jeton de 5
            } else if (chipValue >= 1) {
                chipTexture = chipTextures[0]; // Jeton de 1
            }

            // Dessiner les jetons
            draw_chips(renderer, chipTextures, x, y, numChips, GRID_CELL_WIDTH, GRID_CELL_HEIGHT);
            }
        }

        
        else  {

        // Dessiner les jetons empilés si une mise est présente
        if (bets[i+1][j] > 0) { // Assurez-vous que bets[i + 1][j] est correctement indexé
           // Afficher les jetons
            int numChips = bets[i+1][j]; // Nombre de jetons dans la case
            int chipValue = bets[i+1][j]; // Exemple : définir une valeur de jeton par rapport à la mise
            SDL_Texture* chipTexture = NULL;

            // Sélectionner la bonne texture en fonction de la valeur
            if (chipValue >= 100) {
                chipTexture = chipTextures[4]; // Jeton de 100
            } else if (chipValue >= 50) {
                chipTexture = chipTextures[3]; // Jeton de 50
            } else if (chipValue >= 25) {
                chipTexture = chipTextures[2]; // Jeton de 25
            } else if (chipValue >= 5) {
                chipTexture = chipTextures[1]; // Jeton de 5
            } else if (chipValue >= 1) {
                chipTexture = chipTextures[0]; // Jeton de 1
            }

            // Dessiner les jetons
            draw_chips(renderer, chipTextures, x, y, numChips, GRID_CELL_WIDTH, GRID_CELL_HEIGHT);
            }
        }
    }
}
//  printf("Bets:\n");
// for (int i = 0; i < max_rows; i++) {
//     for (int j = 0; j < max_cols; j++) {
//         printf("%d ", bets[i][j]);
//     }
//     printf("\n");
// }
}
}
}




void draw_chips(SDL_Renderer* renderer, SDL_Texture* chipTextures[], int x, int y, int numChips, int cellWidth, int cellHeight) {
    // Si la texture est NULL, il y a une erreur
    if (chipTextures == NULL || chipTextures[0] == NULL) {
        printf("Erreur : Texture NULL dans draw_chips\n");
        return;
    }

    // Déclaration des jetons à dessiner
    int jetonsRestants = numChips;

    int jetonsDe100 = 0;
    int jetonsDe50 = 0;
    int jetonsDe25 = 0;
    int jetonsDe5 = 0;
    int jetonsDe1 = 0;

    // Calcul des jetons à afficher

    if (jetonsRestants >= 100) {
        jetonsDe100 = jetonsRestants/100;  // Ajouter un jeton de 100€
        jetonsRestants -= jetonsDe100*100;  // Réduire le nombre de jetons restants
    }

    if (jetonsRestants >= 50) {
        jetonsDe50 = jetonsRestants/50;  // Ajouter un jeton de 5€
        jetonsRestants -= jetonsDe50*50;  // Réduire le nombre de jetons restants
    }

    if (jetonsRestants >= 25) {
        jetonsDe25 = jetonsRestants/25;  // Ajouter un jeton de 5€
        jetonsRestants -= jetonsDe25*25;  // Réduire le nombre de jetons restants
    }
    if (jetonsRestants >= 5) {
        jetonsDe5 = jetonsRestants/5 ; // Ajouter des jetons de 2€
        jetonsRestants -= jetonsDe5 *5;  // Réduire le nombre de jetons restants
    }
    jetonsDe1 = jetonsRestants;

    // Définir les positions de départ pour chaque type de jeton
    SDL_Rect destRect;
    destRect.w = cellWidth / 2;
    destRect.h = cellHeight / 2;
    destRect.x = x + (cellWidth - destRect.w) / 2;
    destRect.y = y + (cellHeight - destRect.h) / 2;

    
        // Dessiner les jetons de 5€
    for (int i = 0; i < jetonsDe100; i++) {
        SDL_RenderCopy(renderer, chipTextures[4], NULL, &destRect);  // chipTextures[3] = jeton de 5€
        destRect.y -= 5;  // Décalage pour empiler les jetons
    }



    
    // Dessiner les jetons de 5€
    for (int i = 0; i < jetonsDe50; i++) {
        SDL_RenderCopy(renderer, chipTextures[3], NULL, &destRect);  // chipTextures[3] = jeton de 5€
        destRect.y -= 5;  // Décalage pour empiler les jetons
    }
    
    
    // Dessiner les jetons de 5€
    for (int i = 0; i < jetonsDe25; i++) {
        SDL_RenderCopy(renderer, chipTextures[2], NULL, &destRect);  // chipTextures[3] = jeton de 5€
        destRect.y -= 5;  // Décalage pour empiler les jetons
    }

    // Dessiner les jetons de 2€
    for (int i = 0; i < jetonsDe5; i++) {
        SDL_RenderCopy(renderer, chipTextures[1], NULL, &destRect);  // chipTextures[2] = jeton de 2€
        destRect.y -= 5;  // Décalage pour empiler les jetons
    }

    // Dessiner les jetons de 1€
    for (int i = 0; i < jetonsDe1; i++) {
        SDL_RenderCopy(renderer, chipTextures[0], NULL, &destRect);  // chipTextures[0] = jeton de 1€
        destRect.y -= 5;  // Décalage pour empiler les jetons
    }
}


// Fonction principale
void tapis(SDL_Window *window, SDL_Renderer *renderer) {

    // Charger la police de caractères
    TTF_Font* font = TTF_OpenFont("Roboto-Regular.ttf", FONT_SIZE);  // Assurez-vous que le chemin de la police est correct
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        //return 1;
    }


    // Déclaration des textures des jetons
    SDL_Texture* chipTextures[5]; // 5 types de jetons (1€, 5€, 25€, 50€, 100€)

    // Charger les images des jetons
    SDL_Surface* chipSurface_1 = IMG_Load("jeton_1.png");
    SDL_Surface* chipSurface_5 = IMG_Load("jeton_5.png");
    SDL_Surface* chipSurface_25 = IMG_Load("jeton_25.png");
    SDL_Surface* chipSurface_50 = IMG_Load("jeton_50.png");
    SDL_Surface* chipSurface_100 = IMG_Load("jeton_100.png");

    // Vérifier si les surfaces sont chargées correctement
    if (!chipSurface_1 || !chipSurface_5 || !chipSurface_25 || !chipSurface_50 || !chipSurface_100) {
        printf("Erreur : Impossible de charger une ou plusieurs images de jetons.\n");
        printf("Détails de l'erreur : %s\n", SDL_GetError());
        //return -1; // Sortir si une image n'est pas chargée
    }



    // Charger les textures dans chipTextures[]
    // Assurez-vous que vous chargez bien vos textures dans ce tableau comme ceci :
    chipTextures[0] = SDL_CreateTextureFromSurface(renderer, chipSurface_1); // Jeton de 1€
    chipTextures[1] = SDL_CreateTextureFromSurface(renderer, chipSurface_5); // Jeton de 5€
    chipTextures[2] = SDL_CreateTextureFromSurface(renderer, chipSurface_25); // Jeton de 25€
    chipTextures[3] = SDL_CreateTextureFromSurface(renderer, chipSurface_50); // Jeton de 50€
    chipTextures[4] = SDL_CreateTextureFromSurface(renderer, chipSurface_100); // Jeton de 100€

    // Vérifier si les textures sont correctement créées
    for (int i = 0; i < 5; i++) {
        if (!chipTextures[i]) {
            printf("Erreur : Impossible de créer la texture pour le jeton %d. SDL_Error: %s\n", i + 1, SDL_GetError());
            //return -1; // Sortir si une texture n'est pas créée
        }
    }

    // Libérer les surfaces
    SDL_FreeSurface(chipSurface_1);
    SDL_FreeSurface(chipSurface_5);
    SDL_FreeSurface(chipSurface_25);
    SDL_FreeSurface(chipSurface_50);
    SDL_FreeSurface(chipSurface_100);



    // Initialiser la position de la case sélectionnée
    int selected_row = 1;
    int selected_col = 1;


    int is_betting_allowed = 0;  // Initialement, le joueur ne peut pas miser
    int is_betting_locked = 0;  // Une fois verrouillé, les mises ne sont plus possibles

    SDL_Event e;
    int quit = 0;
    
   while (!quit) {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = 1;
        }

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_p) {
                if (!is_betting_locked) {
                    is_betting_allowed = 1;  // Activer la possibilité de miser
                    printf("Les paris sont maintenant ouverts.\n");
                }
            } else if (e.key.keysym.sym == SDLK_v) {
                if (is_betting_allowed) {
                    is_betting_locked = 1;  // Verrouiller les paris
                    printf("Les paris sont maintenant fermés.\n");
                }
            }
        }




        if (is_betting_allowed && !is_betting_locked) {

        // Exemple de gestion de l'entrée utilisateur pour le curseur
        if (e.type == SDL_KEYDOWN) {
            // Déplacement du curseur dans la ligne 4 (ligne indexée 4) : de 0 à 2
            if (selected_row == 4) {
                if (e.key.keysym.sym == SDLK_UP && selected_row > 0) {
                    selected_row--;
                }
                if (e.key.keysym.sym == SDLK_DOWN && selected_row < 5) {
                    selected_row++;
                }
                if (e.key.keysym.sym == SDLK_LEFT && selected_col > 0 && selected_col <= 2) {
                    selected_col--;
                }
                if (e.key.keysym.sym == SDLK_RIGHT && selected_col < 2) {
                    selected_col++;
                }
            }
            // Déplacement du curseur dans la ligne 5 (ligne indexée 5) : de 0 à 5
            else if (selected_row == 5) {
                if (e.key.keysym.sym == SDLK_UP && selected_row > 0) {
                    selected_row--;
                }
                if (e.key.keysym.sym == SDLK_DOWN && selected_row < 5) {
                    selected_row++;
                }
                if (e.key.keysym.sym == SDLK_LEFT && selected_col > 0 && selected_col <= 5) {
                    selected_col--;
                }
                if (e.key.keysym.sym == SDLK_RIGHT && selected_col < 5) {
                    selected_col++;
                }
            }


            else if (selected_row == -1) {
            // Si selected_row est -1, empêcher le mouvement vers le bas (vers i++)
            if (e.key.keysym.sym == SDLK_DOWN) {
            // Ne rien faire, empêcher de descendre lorsque selected_row est -1
    }

            }

            // Déplacement du curseur dans toutes les autres lignes (normales)
            else {
                if (e.key.keysym.sym == SDLK_UP && selected_row > 0) {
                    selected_row--;
                }
                if (e.key.keysym.sym == SDLK_DOWN && selected_row < max_rows - 1) {
                    selected_row++;
                }
                if (e.key.keysym.sym == SDLK_LEFT && selected_col > -1) {
                    selected_col--;
                }
                if (e.key.keysym.sym == SDLK_RIGHT && selected_col < max_cols ) {
                    selected_col++;
                }
            }
        
         if (e.key.keysym.sym == SDLK_d) {
            // Augmenter la mise (passer à la suivante)
            if (selected_bet_index < sizeof(available_bets) / sizeof(int) - 1) {
                selected_bet_index++;
            }
        } else if (e.key.keysym.sym == SDLK_a) {
            // Diminuer la mise (passer à la précédente)
            if (selected_bet_index > 0) {
                selected_bet_index--;
            }
        } else if (e.key.keysym.sym == SDLK_RETURN) {
            // Confirmer la mise
            int selected_bet = available_bets[selected_bet_index];

            if (player_balance >= selected_bet) {
                player_balance -= selected_bet; // Déduire la mise du solde
                bets[selected_row][selected_col]+= selected_bet; // Ajouter la mise à la case sélectionnée
                printf("Mise de %d € placée. Solde restant : %d €\n", selected_bet, player_balance);
            } else {
                printf("Solde insuffisant pour miser %d €\n", selected_bet);
            }
        }

        int selected_bet = available_bets[selected_bet_index];

        if (e.key.keysym.sym == SDLK_BACKSPACE) {
            
            if (player_balance >=0 ) {
                player_balance += selected_bet; // Déduire la mise du solde
            }

                if (bets[selected_row][selected_col] > 0) {  // Vérifie qu'une mise existe
                    bets[selected_row][selected_col]-= selected_bet;     // Réduit la mise
                    printf("Removed bet from (%d, %d). Remaining bets: %d\n", selected_row, selected_col, bets[selected_row][selected_col]);
                }
        }
            }

        

          }


        }


        pari[0][1] = (Mise) {bets[0][0], 1, 'R'};
        pari[0][2] = (Mise) {bets[0][1], 2, 'N'};
        pari[0][3] = (Mise) {bets[0][2], 3, 'R'};
        pari[1][1] = (Mise) {bets[1][1], 4, 'N'};
        pari[2][1] = (Mise) {bets[2][1], 5, 'R'};
        pari[3][1] = (Mise) {bets[3][1], 6, 'N'};
        pari[1][2] = (Mise) {bets[1][2], 7, 'R'};
        pari[2][2] = (Mise) {bets[2][2], 8, 'N'};
        pari[3][2] = (Mise) {bets[3][2], 9, 'R'};
        pari[1][3] = (Mise) {bets[1][3], 10, 'N'};
        pari[2][3] = (Mise) {bets[2][3], 11, 'N'};
        pari[3][3] = (Mise) {bets[3][3], 12, 'R'};
        pari[1][4] = (Mise) {bets[1][4], 13, 'N'};
        pari[2][4] = (Mise) {bets[2][4], 14, 'R'};
        pari[3][4] = (Mise) {bets[3][4], 15, 'N'};
        pari[1][5] = (Mise) {bets[1][5], 16, 'R'};
        pari[2][5] = (Mise) {bets[2][5], 17, 'N'};
        pari[3][5] = (Mise) {bets[3][5], 18, 'R'};
        pari[1][6] = (Mise) {bets[1][6], 19, 'R'};
        pari[2][6] = (Mise) {bets[2][6], 20, 'N'};
        pari[3][6] = (Mise) {bets[3][6], 21, 'R'};
        pari[1][7] = (Mise) {bets[1][7], 22, 'N'};
        pari[2][7] = (Mise) {bets[2][7], 23, 'R'};
        pari[3][7] = (Mise) {bets[3][7], 24, 'N'};
        pari[1][8] = (Mise) {bets[1][8], 25, 'R'};
        pari[2][8] = (Mise) {bets[2][8], 26, 'N'};
        pari[3][8] = (Mise) {bets[3][8], 27, 'R'};
        pari[1][9] = (Mise) {bets[1][9], 28, 'N'};
        pari[2][9] = (Mise) {bets[2][9], 29, 'N'};
        pari[3][9] = (Mise) {bets[3][9], 30, 'R'};
        pari[1][10] = (Mise) {bets[1][10], 31, 'N'};
        pari[2][10] = (Mise) {bets[2][10], 32, 'R'};
        pari[3][10] = (Mise) {bets[3][10], 33, 'N'};
        pari[1][11] = (Mise) {bets[1][11], 34, 'R'};
        pari[2][11] = (Mise) {bets[2][11], 35, 'N'};
        pari[3][11] = (Mise) {bets[3][11], 36, 'R'};

        // Définir le résultat de la roulette (par exemple, 17)
        const int resultat = 17;


    if (is_betting_locked) {

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 12; j++) {
            // Vérifier si le numéro correspond au résultat et si le montant est positif
            if (pari[i][j].numero == resultat && pari[i][j].montant > 0) {
                printf("Gagnant! pari[%d][%d] correspond au résultat %d avec un montant de %d\n", i, j, resultat, pari[i][j].montant);
            }
            }
        }
    }

        






    // Nettoyer l'écran
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Noir
    SDL_RenderClear(renderer);





    // Dessiner la table de roulette
    draw_table(renderer, font, selected_row, selected_col, chipTextures);

    // Mettre à jour l'affichage
    SDL_RenderPresent(renderer);
}

// Libérer la mémoire et quitter SDL
TTF_CloseFont(font);

for (int i = 0; i < 5; i++) {
    SDL_DestroyTexture(chipTextures[i]);
}

TTF_Quit();

}


void lancerRouletteEtTapis(SDL_Window *window, SDL_Renderer *renderer) {




int main() {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Initialisation de SDL_ttf
    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Création de la fenêtre
    window = SDL_CreateWindow("Roulette et Tapis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Création du renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Charger une police pour l'affichage
    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 24);
    if (!font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Boucle principale
    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Effacer l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Noir
        SDL_RenderClear(renderer);

        // Afficher la roulette et le tapis
        lancerRouletteEtTapis(window, renderer);

        // Mettre à jour l'écran
        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}