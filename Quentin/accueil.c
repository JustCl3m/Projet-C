#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

int balance = 0;  // Variable pour stocker le solde d'argent

// Fonction pour afficher du texte
void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int x, int y) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color); // Rendu du texte
    if (!surface) {
        printf("Erreur lors du rendu du texte : %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Erreur lors de la création de la texture de texte : %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest_rect = {x, y, surface->w, surface->h}; // Position et dimensions du texte
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Fonction pour afficher les options d'un sous-menu
void render_submenu(SDL_Renderer *renderer, TTF_Font *font, int selected_option, const char *options[], int num_options, int y_start, SDL_Texture *background_texture) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    // Dessiner l'image de fond (si spécifiée)
    if (background_texture) {
        SDL_Rect bg_rect = {0, 0, 800, 600}; // L'image couvre toute la fenêtre
        SDL_RenderCopy(renderer, background_texture, NULL, &bg_rect);
    }

    // Afficher les options
    for (int i = 0; i < num_options; i++) {
        SDL_Color color = (i == selected_option) ? yellow : white; // Option sélectionnée en jaune
        render_text(renderer, font, options[i], color, 350, y_start + i * 100);
    }
}

void handle_balance_update(int option) {
    if (option == 0) {  // Ajouter de l'argent
        balance += 5;
    } else if (option == 1) {  // Retirer de l'argent
        if (balance >= 5) {
            balance -= 5;
        } else {
            printf("Solde insuffisant pour retirer de l'argent\n");
        }
    }
}

void render_balance(SDL_Renderer *renderer, TTF_Font *font) {
    char balance_text[50];
    sprintf(balance_text, "Solde: %d", balance);
    SDL_Color white = {255, 255, 255, 255};
    render_text(renderer, font, balance_text, white, 350, 50);
}

int main(int argc, char *argv[]) {
    // Initialisation des bibliothèques SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { // Ne pas initialiser SDL_AUDIO
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("Erreur SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur SDL_image: %s\n", IMG_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Accueil", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur création renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 48);
    if (!font) {
        printf("Erreur chargement police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Charger l'image du menu principal
    SDL_Surface *menu_surface = IMG_Load("Plan_de_travail_1.png");
    if (!menu_surface) {
        printf("Erreur chargement image: %s\n", IMG_GetError());
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture *menu_texture = SDL_CreateTextureFromSurface(renderer, menu_surface);
    SDL_FreeSurface(menu_surface);

    // Charger l'image de fond pour le sous-menu Solde
    SDL_Surface *banque_surface = IMG_Load("banque.png");
    if (!banque_surface) {
        printf("Erreur chargement de l'image banque.png: %s\n", IMG_GetError());
        TTF_CloseFont(font);
        SDL_DestroyTexture(menu_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Texture *banque_texture = SDL_CreateTextureFromSurface(renderer, banque_surface);
    SDL_FreeSurface(banque_surface);

    // Couleur du texte
    SDL_Color yellow = {255, 255, 0, 255};

    int running = 1, on_menu = 0, selected_option = 0, on_submenu = -1;
    int submenu_selected = 0;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                if (on_menu) { // Navigation dans le menu principal ou sous-menus
                    if (event.key.keysym.sym == SDLK_UP) {
                        submenu_selected = (submenu_selected - 1 + 3) % 3;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        submenu_selected = (submenu_selected + 1) % 3;
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
<<<<<<< HEAD
                        if (submenu_selected == 0) { // Solde
                            on_submenu = 0;
                        } else if (submenu_selected == 1) { // Quitter
                            running = 0; // Quitter le jeu
                        } else if (submenu_selected == 2) { // Jeux
                            on_submenu = 1;
                        }
                    }
                } else if (on_submenu == 0) { // Sous-menu Solde
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        if (submenu_selected == 0) { // Ajouter de l'argent
                            balance += 5;  // Ajouter 5 au solde
                        } else if (submenu_selected == 1) { // Retirer de l'argent
=======
                        if (submenu_selected == 0) {
                            on_submenu = 0; // Sous-menu Solde
                        } else if (submenu_selected == 2) {
                            running = 0; // Quitter
                        } else if (submenu_selected == 1) {
                            on_submenu = 1; // Sous-menu Jeux
                        }
                    }
                } else if (on_submenu == 0) { // Sous-menu Solde
                    const char *solde_options[] = {"Ajouter de l'argent", "Retirer de l'argent", "Retour"}; // Sous-menu Solde
                    if (event.key.keysym.sym == SDLK_UP) {
                        submenu_selected = (submenu_selected - 1 + 3) % 3;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        submenu_selected = (submenu_selected + 1) % 3;
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
                        if (submenu_selected == 0) {
                            balance += 5; // Ajouter de l'argent
                        } else if (submenu_selected == 1) {
>>>>>>> 8f3cba6 (grrggrgs)
                            if (balance >= 5) {
                                balance -= 5; // Retirer 5 du solde
                            } else {
                                printf("Solde insuffisant\n");
                            }
                        } else if (submenu_selected == 2) { // Retour au menu
                            on_submenu = -1; // Retour au menu principal
                        }
                    }
                } else if (on_submenu == 1) { // Sous-menu Jeux
<<<<<<< HEAD
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        if (submenu_selected == 2) { // Retour au menu principal
                            on_submenu = -1; // Retour au menu principal
=======
                    const char *jeu_options[] = {"Roulette", "Surf", "Retour"}; // Sous-menu Jeux
                    if (event.key.keysym.sym == SDLK_UP) {
                        submenu_selected = (submenu_selected - 1 + 3) % 3;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        submenu_selected = (submenu_selected + 1) % 3;
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
                        if (submenu_selected == 0) {
                            system("./roulette"); // Lancer Roulette
                        } else if (submenu_selected == 1) {
                            system("./surf"); // Lancer Surf
                        } else if (submenu_selected == 2) {
                            on_submenu = -1; // Retour
>>>>>>> 8f3cba6 (grrggrgs)
                        }
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;

                // Vérification des clics dans le menu principal (Play Now)
                if (!on_menu) {
                    SDL_Rect play_rect = {300, 400, 200, 80};
                    if (x >= play_rect.x && x <= play_rect.x + play_rect.w && y >= play_rect.y && y <= play_rect.y + play_rect.h) {
                        on_menu = 1;
                    }
                }

                // Vérification des clics dans le sous-menu Solde
                if (on_submenu == 0) {
                    // Ajouter de l'argent
                    SDL_Rect add_rect = {300, 200, 200, 80};
                    if (x >= add_rect.x && x <= add_rect.x + add_rect.w && y >= add_rect.y && y <= add_rect.y + add_rect.h) {
                        balance += 5; // Ajouter 5 à l'argent
                    }

                    // Retirer de l'argent
                    SDL_Rect remove_rect = {300, 300, 200, 80};
                    if (x >= remove_rect.x && x <= remove_rect.x + remove_rect.w && y >= remove_rect.y && y <= remove_rect.y + remove_rect.h) {
                        if (balance >= 5) {
                            balance -= 5; // Retirer 5 de l'argent
                        }
                    }

                    // Retour
                    SDL_Rect back_rect = {300, 400, 200, 80};
                    if (x >= back_rect.x && x <= back_rect.x + back_rect.w && y >= back_rect.y && y <= back_rect.y + back_rect.h) {
                        on_submenu = -1; // Retour au menu principal
                    }
                }
            }
        }

        SDL_RenderClear(renderer);

        if (on_menu) {
            if (on_submenu == -1) {
                const char *menu_options[] = {"Solde", "Jeux", "Quitter"}; // Quitter en bas
                render_submenu(renderer, font, submenu_selected, menu_options, 3, 200, NULL); // Menu principal
            } else if (on_submenu == 0) {
<<<<<<< HEAD
                const char *solde_options[] = {"Ajouter de l'argent", "Retirer de l'argent", "Retour"};
                render_submenu(renderer, font, submenu_selected, solde_options, 3, 200);
                render_balance(renderer, font); // Afficher le solde actuel
=======
                const char *solde_options[] = {"Ajouter de l'argent", "Retirer de l'argent", "Retour"}; // Sous-menu Solde
                render_submenu(renderer, font, submenu_selected, solde_options, 3, 200, banque_texture);
>>>>>>> 8f3cba6 (grrggrgs)
            } else if (on_submenu == 1) {
                const char *jeu_options[] = {"Roulette", "Surf", "Retour"}; // Sous-menu Jeux
                render_submenu(renderer, font, submenu_selected, jeu_options, 3, 200, NULL);
            }
        } else {
            SDL_RenderCopy(renderer, menu_texture, NULL, NULL);
            render_text(renderer, font, "Appuyez sur Entrée", yellow, 200, 300);
        }

        render_balance(renderer, font);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menu_texture);
    SDL_DestroyTexture(banque_texture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
