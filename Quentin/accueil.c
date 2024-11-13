#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Structure pour représenter une option de menu
typedef struct {
    const char *label;
} MenuOption;

// Fonction pour afficher le texte dans le menu
void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect text_rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Fonction principale pour l'accueil et le menu
int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("Erreur TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Page d'Accueil", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Charger la police
    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 24); // Utiliser la police Roboto-Regular
    if (!font) {
        printf("Erreur chargement de la police: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Variables pour gérer le solde
    float solde = 100.0; // Solde initial
    char solde_text[50];  // Texte pour afficher le solde
    snprintf(solde_text, sizeof(solde_text), "Solde: %.2f EUR", solde);

    // Options du menu principal
    MenuOption menu[] = {
        {"Bienvenue sur le casino des piments"},
        {"Accéder aux jeux"},
        {"Solde"},
        {"Quitter"}
    };

    // Options des jeux
    MenuOption games_menu[] = {
        {"Jeu 1: Course de Poneys"},
        {"Jeu 2: Machine à Sous"},
        {"Jeu 3: Roulette"},
        {"Retour"}
    };

    int menu_length = sizeof(menu) / sizeof(MenuOption);
    int games_menu_length = sizeof(games_menu) / sizeof(MenuOption);
    int selected_option = 0;
    int selected_game_option = 0;
    int in_balance_page = 0; // Flag pour savoir si on est dans la page solde
    int in_game_menu = 0;  // Flag pour savoir si on est dans le menu des jeux

    // Couleurs
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color green = {0, 255, 0, 255};  // Couleur verte claire

    int running = 1;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                if (in_balance_page) {  // Si on est sur la page de gestion du solde
                    switch (event.key.keysym.sym) {
                        case SDLK_a:  // Appuyer sur "A" pour ajouter de l'argent
                            solde += 10.0;  // Ajouter 10 euros
                            snprintf(solde_text, sizeof(solde_text), "Solde: %.2f EUR", solde);
                            break;
                        case SDLK_r:  // Appuyer sur "R" pour retirer de l'argent
                            solde -= 10.0;  // Retirer 10 euros
                            if (solde < 0) solde = 0; // Empêcher d'avoir un solde négatif
                            snprintf(solde_text, sizeof(solde_text), "Solde: %.2f EUR", solde);
                            break;
                        case SDLK_ESCAPE:  // Quitter la page solde et revenir à la page d'accueil
                            in_balance_page = 0;
                            break;
                    }
                } else if (in_game_menu) {  // Si on est sur le menu des jeux
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            selected_game_option = (selected_game_option - 1 + games_menu_length) % games_menu_length;
                            break;
                        case SDLK_DOWN:
                            selected_game_option = (selected_game_option + 1) % games_menu_length;
                            break;
                        case SDLK_RETURN:
                            if (selected_game_option == games_menu_length - 1) { // Retour
                                in_game_menu = 0; // Retour à la page d'accueil
                            } else {
                                // Lancer le jeu correspondant
                                if (selected_game_option == 0) { // Course de Poneys
                                    printf("Lancement du jeu: Course de Poneys\n");
                                    system("./course_poneys"); // Lancer le programme course_poneys dans le sous-dossier (si nécessaire)
                                } else if (selected_game_option == 1) { // Machine à Sous
                                    printf("Lancement du jeu: Machine à Sous\n");
                                    system("./machine_a_sous"); // Lancer le programme machine_a_sous dans le sous-dossier (si nécessaire)
                                } else if (selected_game_option == 2) { // Roulette
                                    printf("Lancement du jeu: Roulette\n");
                                    system("./roulette/roulette"); // Lancer le programme roulette dans le sous-dossier "roulette"
                                }
                            }
                            break;
                    }
                } else {  // Si on est dans le menu d'accueil
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            selected_option = (selected_option - 1 + menu_length) % menu_length;
                            break;
                        case SDLK_DOWN:
                            selected_option = (selected_option + 1) % menu_length;
                            break;
                        case SDLK_RETURN:
                            if (selected_option == menu_length - 1) { // Quitter
                                running = 0;
                            } else if (selected_option == 1) { // Accéder aux jeux
                                in_game_menu = 1; // Aller au menu des jeux
                            } else if (selected_option == 2) { // Solde
                                in_balance_page = 1; // Aller à la page solde
                            }
                            break;
                    }
                }
            }
        }

        // Rendu du menu
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fond noir
        SDL_RenderClear(renderer);

        if (in_balance_page) {  // Page Solde
            render_text(renderer, font, "Page de gestion du solde", 250, 100, green);
            render_text(renderer, font, solde_text, 250, 150, yellow);
            render_text(renderer, font, "Appuyez sur A pour ajouter", 250, 200, yellow);
            render_text(renderer, font, "Appuyez sur R pour retirer", 250, 250, yellow);
            render_text(renderer, font, "Appuyez sur Echap pour revenir", 250, 300, yellow);
        } else if (in_game_menu) {  // Menu des jeux
            render_text(renderer, font, "Choisissez un jeu:", 250, 100, green);

            for (int i = 0; i < games_menu_length; i++) {
                int y_position = 150 + i * 50;
                SDL_Color color = (i == selected_game_option) ? yellow : green;
                render_text(renderer, font, games_menu[i].label, 300, y_position, color);
            }
        } else {  // Page d'accueil
            for (int i = 0; i < menu_length; i++) {
                int y_position = 200 + i * 50;
                SDL_Color color = (i == selected_option) ? yellow : green;
                render_text(renderer, font, menu[i].label, 300, y_position, color);
            }
        }

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
