#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define BUTTON_WIDTH 850/2
#define BUTTON_HEIGHT 1193/2
#define BUTTON_WIDTH2 400
#define BUTTON_HEIGHT2 100
#define BUTTON_SPACING 20

typedef struct {
    char nom[20];
    int triche;
    int revisions;
    int chance;
    int score;
} Piment;

Piment piments[] = {
    {"Quentin", 8, 5, 6, 0},
    {"Alan", 3, 9, 4, 0},
    {"Sterenn", 5, 9, 5, 0},
};

int nombre_piments = sizeof(piments) / sizeof(Piment);
int joueur_choisi = -1; // Index du joueur choisi

// Variable globale pour stocker l'action du joueur (0 pour quitter, 1 pour rejouer)
int action = 2;  // 2 pour indiquer qu'aucune action n'a encore été choisie

// Fonction pour dessiner un bouton
void draw_button(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Rect rect, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect textRect = {
        rect.x + (rect.w - textWidth) / 2,
        rect.y + (rect.h - textHeight) / 2,
        textWidth,
        textHeight
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void choisir_joueur(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color bgColor = {0, 0, 0, 255};  // Fond noir
    SDL_Rect buttons[3];

    // Charger les images des boutons
    SDL_Texture *buttonTextures[3];
    buttonTextures[0] = IMG_LoadTexture(renderer, "Piment1.png"); // Remplacez par vos images
    buttonTextures[1] = IMG_LoadTexture(renderer, "Piment2.png");
    buttonTextures[2] = IMG_LoadTexture(renderer, "Piment3.png");

    if (!buttonTextures[0] || !buttonTextures[1] || !buttonTextures[2]) {
        printf("Erreur de chargement des images : %s\n", SDL_GetError());
        return;
    }

    // Définir la position et la taille des boutons
    for (int i = 0; i < 3; i++) {
        buttons[i].x = 100 + i * (BUTTON_HEIGHT + BUTTON_SPACING);
        buttons[i].y = 200;
        buttons[i].w = BUTTON_WIDTH;
        buttons[i].h = BUTTON_HEIGHT;
    }

    int running = 1;
    while (running) {
        SDL_Event event;
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(renderer);

        // Affichage du texte d'accueil
        SDL_Surface *textSurface = TTF_RenderText_Blended(font, "Choisissez un joueur", (SDL_Color){255, 255, 255, 255});
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {WINDOW_WIDTH / 4, 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // Dessiner les boutons avec les images
        for (int i = 0; i < 3; i++) {
            // Dessiner chaque image sur le bouton
            SDL_RenderCopy(renderer, buttonTextures[i], NULL, &buttons[i]);
        }

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                // Vérifier si un bouton est cliqué
                for (int i = 0; i < 3; i++) {
                    if (mouseX >= buttons[i].x && mouseX <= buttons[i].x + BUTTON_WIDTH &&
                        mouseY >= buttons[i].y && mouseY <= buttons[i].y + BUTTON_HEIGHT) {
                        joueur_choisi = i; // Enregistrer le joueur choisi
                        running = 0; // Quitter la boucle
                        break;
                    }
                }
            }
        }
    }

    // Libérer les textures des boutons
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(buttonTextures[i]);
    }
}

// Fonction pour afficher le mini-jeu
void mini_jeu(SDL_Renderer *renderer, TTF_Font *font, Piment *piment) {
    SDL_Color bgColor = {0, 0, 0, 255};  // Fond noir
    SDL_Color textColor = {255, 255, 255, 255};  // Texte en blanc
    SDL_Color buttonColor = {0, 128, 255, 255};  // Couleur des boutons
    SDL_Color correctButtonColor = {0, 128, 255, 255};  // Couleur correcte (vert)
    SDL_Color incorrectButtonColor = {0, 128, 255, 255};  // Couleur incorrecte (rouge)

    char question[50];
    sprintf(question, "%s, 5 + 7 = ?", piment->nom);

    int correct_answer = 12;
    int responses[] = {11, 12, 13}; // Les réponses possibles
    SDL_Rect buttons[3];

    for (int i = 0; i < 3; i++) {
        buttons[i].x = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
        buttons[i].y = 200 + i * (BUTTON_HEIGHT2 + BUTTON_SPACING);
        buttons[i].w = BUTTON_WIDTH2;
        buttons[i].h = BUTTON_HEIGHT2;
    }

    // Afficher le message d'introduction

    SDL_Texture *backgroundTexture = IMG_LoadTexture(renderer, "background.png");
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, "L'évaluation commence...", textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {100, 100, textSurface->w, textSurface->h};
    
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(backgroundTexture);

    SDL_RenderPresent(renderer);
    SDL_Delay(2000);  // Attendre avant de commencer le mini-jeu

    // Boucle pour gérer les événements du mini-jeu
    int selected = -1; // Réponse sélectionnée
    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                for (int i = 0; i < 3; i++) {
                    if (mouseX >= buttons[i].x && mouseX <= buttons[i].x + BUTTON_WIDTH2 &&
                        mouseY >= buttons[i].y && mouseY <= buttons[i].y + BUTTON_HEIGHT2) {
                        selected = i;
                        running = 0;
                        break;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(renderer);

        // Afficher la question
        textSurface = TTF_RenderText_Blended(font, question, textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = (SDL_Rect){100, 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // Dessiner les boutons de réponses
        for (int i = 0; i < 3; i++) {
            char response_text[4];
            sprintf(response_text, "%d", responses[i]);

            SDL_Color currentButtonColor = (responses[i] == correct_answer) ? correctButtonColor : buttonColor;
            draw_button(renderer, font, response_text, buttons[i], currentButtonColor);
        }

        SDL_RenderPresent(renderer);
    }

    // Vérification de la réponse et mise à jour du score
    if (responses[selected] == correct_answer) {

        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_Surface *textSurface = TTF_RenderText_Blended(font, "Bonne reponse", textColor);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {100, 100, textSurface->w, textSurface->h};
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(2000); 
        
        piment->score += 5;
    } else {

        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_Surface *textSurface = TTF_RenderText_Blended(font, "Mauvaise reponse", textColor);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {100, 100, textSurface->w, textSurface->h};
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(2000); 

        piment->score -= 3;
        if (piment->score < 0) piment->score = 0;
        
    }
}

void evenement_bonus_malus(SDL_Renderer *renderer, TTF_Font *font, Piment *piment) {
    SDL_Color bgColor = {0, 0, 0, 255};  // Fond noir
    SDL_Color textColor = {255, 255, 255, 255};  // Texte en blanc

    // Générer un événement aléatoire
    int evenement = rand() % 80; // Un nombre entre 0 et 99
    printf("Evenement: %d\n", evenement);
    char message[200]; // Assez grand pour contenir tous les messages possibles

    // Événement 1 : Triche détectée (probabilité 20% si triche > 5)
    if (evenement < 40 && piment->triche > 4) {
        sprintf(message, "%s s'est fait prendre en train de tricher et perd 3 points !", piment->nom);
        piment->score -= 3; 
        if (piment->score < 0) piment->score = 0; // Eviter les scores négatifs
    } 
    // Événement 2 : Révisions utiles (probabilité 20% si révisions > 5)
    else if (evenement < 70 && piment->revisions > 5) {
        sprintf(message, "%s utilise ses révisions et gagne 2 points supplémentaires !", piment->nom);
        piment->score += 2;
    }
    // Événement 3 : Chance incroyable (probabilité 20% si chance > 7)
    else if (evenement < 80 && piment->chance > 7) {
        sprintf(message, "%s bénéficie d'une aide inattendue et gagne 3 points !", piment->nom);
        piment->score += 3;
    }
    // Événement 4 : Bonus chanceux universel (probabilité 10%)
    else if (evenement < 10) {
        sprintf(message, "%s a eu une chance extraordinaire et gagne 4 points !", piment->nom);
        piment->score += 4;
    }
    else {
        sprintf(message, "%s bloque sur l'exercice, sans doute un manque de connaissance.", piment->nom);
    }

    // Calculer un score de base aléatoire basé sur ses compétences
    int base_bonus = piment->revisions + (rand() % (piment->chance + 1));
    piment->score += base_bonus;
    if (piment->score < 0) piment->score = 0; // Toujours éviter les scores négatifs

    // Afficher le message sur l'écran

    SDL_Texture *backgroundTexture = IMG_LoadTexture(renderer, "background.png");
if (!backgroundTexture) {
    printf("Erreur de chargement de l'image de fond : %s\n", IMG_GetError());
    return;
}

// Effacer l'écran avant de dessiner
SDL_RenderClear(renderer);

// Afficher l'image de fond (recouvre toute la fenêtre)
SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

// Créer la surface et la texture du texte
SDL_Surface *textSurface = TTF_RenderText_Blended(font, message, textColor);
if (!textSurface) {
    printf("Erreur de création de la surface de texte : %s\n", TTF_GetError());
    SDL_DestroyTexture(backgroundTexture);
    return;
}

SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
if (!textTexture) {
    printf("Erreur de création de la texture de texte : %s\n", SDL_GetError());
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(backgroundTexture);
    return;
}

// Définir la position du texte
SDL_Rect textRect = {100, 100, textSurface->w, textSurface->h};  // Ajustez les coordonnées si nécessaire

// Dessiner un rectangle noir derrière le texte
SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
SDL_RenderFillRect(renderer, &textRect);

// Afficher le texte sur l'écran
SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

// Libérer les ressources utilisées pour le texte
SDL_FreeSurface(textSurface);
SDL_DestroyTexture(textTexture);

// Libérer la texture de fond
SDL_DestroyTexture(backgroundTexture);

// Afficher tout à l'écran
SDL_RenderPresent(renderer);

// Attendre 3 secondes pour afficher le message
SDL_Delay(5000);

}


// Fonction utilitaire pour rendre du texte et calculer la taille
void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, SDL_Rect *rect) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Mettre à jour les dimensions du rectangle
    rect->w = surface->w;
    rect->h = surface->h;

    // Copier le texte dans le renderer
    SDL_RenderCopy(renderer, texture, NULL, rect);

    // Libérer les ressources
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}



// Fonction pour réinitialiser les données du jeu
void reset_game(Piment piments[], int nombre_piments) {
    // Réinitialiser les scores
    for (int i = 0; i < nombre_piments; i++) {
        piments[i].score = 0;
    }
}

// Fonction pour redémarrer le jeu
void restart_game(SDL_Renderer *renderer, TTF_Font *font, Piment piments[], int nombre_piments) {
    // Réinitialiser les scores, les compétences et les données du jeu
    reset_game(piments, nombre_piments);

    // Re-demander au joueur de choisir son personnage
    joueur_choisi = -1; // Réinitialiser le joueur choisi
}

int afficher_resultat_joueur(SDL_Renderer *renderer, TTF_Font *font, Piment piments[], int nombre_piments, Piment *joueur_choisi, int *action) {
    // Couleurs
    SDL_Color bgColor = {0, 0, 0, 255};      // Fond noir
    SDL_Color textColor = {255, 255, 255, 255}; // Texte blanc
    SDL_Color winColor = {0, 255, 0, 255};   // Vert pour "gagné"
    SDL_Color loseColor = {255, 0, 0, 255};  // Rouge pour "perdu"
    SDL_Color buttonColor = {0, 128, 255, 255}; // Couleur du bouton

    // Trouver le score maximum
    int score_max = 0;
    for (int i = 0; i < nombre_piments; i++) {
        if (piments[i].score > score_max) {
            score_max = piments[i].score;
        }
    }

    // Déterminer si le joueur choisi a gagné
    int joueur_gagne = (joueur_choisi->score == score_max);

    // Effacer l'écran
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);

    // Affichage des scores des participants
    int y_offset = 50; // Point de départ vertical
    char message[200]; // Buffer pour les messages

    for (int i = 0; i < nombre_piments; i++) {
        sprintf(message, "%s : %d points", piments[i].nom, piments[i].score);

        // Définir la couleur selon le score max
        SDL_Color currentColor = (piments[i].score == score_max) ? winColor : textColor;

        // Afficher le texte
        SDL_Rect textRect = {100, y_offset, 0, 0};
        render_text(renderer, font, message, currentColor, &textRect);

        y_offset += 50; // Décalage pour le prochain participant
    }

    // Message spécifique pour le joueur choisi
    sprintf(message, joueur_gagne
                    ? "%s : Félicitations, vous avez GAGNÉ avec %d points !"
                    : "Désolé, %s, vous avez PERDU avec %d points.",
            joueur_choisi->nom, joueur_choisi->score);

    SDL_Color resultColor = joueur_gagne ? winColor : loseColor;
    SDL_Rect resultRect = {100, y_offset + 50, 0, 0};
    render_text(renderer, font, message, resultColor, &resultRect);

    // Afficher les boutons
    SDL_Rect buttonQuitterRect = {WINDOW_WIDTH / 4, y_offset + 150, BUTTON_WIDTH2, BUTTON_HEIGHT2};
    SDL_Rect buttonRejouerRect = {WINDOW_WIDTH / 4, y_offset + 250, BUTTON_WIDTH2, BUTTON_HEIGHT2};
    draw_button(renderer, font, "Quitter", buttonQuitterRect, buttonColor);
    draw_button(renderer, font, "Rejouer", buttonRejouerRect, buttonColor);

    // Présenter le rendu
    SDL_RenderPresent(renderer);

    // Attendre l'interaction utilisateur

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                // Vérifier si le bouton quitter est cliqué
                if (mouseX >= buttonQuitterRect.x && mouseX <= buttonQuitterRect.x + buttonQuitterRect.w &&
                    mouseY >= buttonQuitterRect.y && mouseY <= buttonQuitterRect.y + buttonQuitterRect.h) {
                    running = 0;
                    *action = 0;
                }
                // Vérifier si le bouton rejouer est cliqué
                if (mouseX >= buttonRejouerRect.x && mouseX <= buttonRejouerRect.x + buttonRejouerRect.w &&
                    mouseY >= buttonRejouerRect.y && mouseY <= buttonRejouerRect.y + buttonRejouerRect.h) {
                    *action = 1;
                    running = 0;
                }
            }
        }
    }
}






// Fonction pour afficher un écran de fin
// void afficher_ecran_fin(SDL_Renderer *renderer, TTF_Font *font) {
//     SDL_Color bgColor = {0, 0, 0, 255};  // Fond noir
//     SDL_Color textColor = {255, 255, 255, 255};  // Texte en blanc
//     SDL_Color buttonColor = {0, 128, 255, 255};  // Couleur des boutons

//     SDL_Rect buttonRect = {WINDOW_WIDTH / 4, 400, BUTTON_WIDTH, BUTTON_HEIGHT};

//     SDL_Surface *textSurface = TTF_RenderText_Blended(font, "Game Over!", textColor);
//     SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
//     SDL_Rect textRect = {WINDOW_WIDTH / 4, 100, textSurface->w, textSurface->h};
//     SDL_RenderClear(renderer);
//     SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
//     SDL_FreeSurface(textSurface);
//     SDL_DestroyTexture(textTexture);

//     draw_button(renderer, font, "Quitter", buttonRect, buttonColor);
    
//     SDL_RenderPresent(renderer);

//     // Attendre que l'utilisateur clique pour quitter
//     int running = 1;
//     while (running) {
//         SDL_Event event;
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) {
//                 running = 0;
//             } else if (event.type == SDL_MOUSEBUTTONDOWN) {
//                 int mouseX = event.button.x;
//                 int mouseY = event.button.y;

//                 // Vérifier si le bouton quitter est cliqué
//                 if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + BUTTON_WIDTH &&
//                     mouseY >= buttonRect.y && mouseY <= buttonRect.y + BUTTON_HEIGHT) {
//                     running = 0;
//                 }
//             }
//         }
//     }
// }



// Fonction principale
int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur d'initialisation SDL : %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur d'initialisation TTF : %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Le jeu des Piments",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Erreur de création de la fenêtre : %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Erreur de création du renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 24);
    if (!font) {
        fprintf(stderr, "Erreur de chargement de la police : %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    int jouerEncore = 1; // Contrôle de la boucle du jeu
    int action = -1;  // Par défaut, aucune action n'est choisie

    while (jouerEncore == 1) { 
        // 1 - Sélectionner un joueur
        choisir_joueur(renderer, font);

        // 2- Commencer le mini-jeu
        if (joueur_choisi != -1) {
            mini_jeu(renderer, font, &piments[joueur_choisi]);
        }

        // 3- Appliquer l'événement bonus/malus pour chaque joueur
        for (int i = 0; i < nombre_piments; i++) {
            evenement_bonus_malus(renderer, font, &piments[i]);
        }

        // 4 - Afficher les résultats partiels
        afficher_resultat_joueur(renderer, font, piments, nombre_piments, &piments[joueur_choisi], &action);

        // 5 - Demander si le joueur veut rejouer

        // Boucle d'attente pour l'action (Quitter ou Rejouer

        // Si action == 1, recommencer le jeu
        if (action == 1) {
            // Réinitialiser le jeu
            restart_game(renderer, font, piments, nombre_piments);
            jouerEncore = 1;
            printf("Rejouer\n");
        } 
        else if (action == 0) {
            // Quitter
            jouerEncore = 0;
            printf("Quitter Action : %d \n", action);
        }

    }

    // Nettoyage de SDL et fermeture du jeu
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;


}