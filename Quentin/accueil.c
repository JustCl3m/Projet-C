#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

int balance = 0;  // Variable pour stocker le solde d'argent

// Fonction pour afficher du texte
void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int x, int y) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
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

    SDL_Rect dest_rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Fonction pour afficher les options d'un sous-menu
void render_submenu(SDL_Renderer *renderer, TTF_Font *font, int selected_option, const char *options[], int num_options, int y_start, SDL_Texture *background_texture) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    // Dessiner l'image de fond (ajustée à 1920x1080)
    if (background_texture) {
        SDL_Rect bg_rect = {0, 0, 1920, 1080};
        SDL_RenderCopy(renderer, background_texture, NULL, &bg_rect);
    }

    // Afficher les options
    for (int i = 0; i < num_options; i++) {
        SDL_Color color = (i == selected_option) ? yellow : white;
        render_text(renderer, font, options[i], color, 800, y_start + i * 100);  // Centré horizontalement
    }
}

// Fonction pour afficher le solde
void render_balance(SDL_Renderer *renderer, TTF_Font *font) {
    char balance_text[50];
    sprintf(balance_text, "Solde: %d", balance);
    SDL_Color white = {255, 255, 255, 255};
    render_text(renderer, font, balance_text, white, 850, 50);  // Centré horizontalement
}

int main(int argc, char *argv[]) {
    // Initialisation des bibliothèques SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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

    // Création de la fenêtre avec résolution 1920x1080
    SDL_Window *window = SDL_CreateWindow("Accueil", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN);
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

    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 24);  // Taille augmentée pour meilleure lisibilité
    if (!font) {
        printf("Erreur chargement police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Charger les images (assurez-vous qu'elles soient en 1920x1080)
    SDL_Surface *menu_surface = IMG_Load("Plan_de_travail_1.png");
    if (!menu_surface) {
        printf("Erreur chargement image Plan_de_travail_1.png: %s\n", IMG_GetError());
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

    SDL_Surface *submenu_surface = IMG_Load("sousmenu.png");
    if (!submenu_surface) {
        printf("Erreur chargement image sousmenu.png: %s\n", IMG_GetError());
        TTF_CloseFont(font);
        SDL_DestroyTexture(menu_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Texture *submenu_texture = SDL_CreateTextureFromSurface(renderer, submenu_surface);
    SDL_FreeSurface(submenu_surface);

    SDL_Surface *solde_surface = IMG_Load("solde.png");
    if (!solde_surface) {
        printf("Erreur chargement image solde.png: %s\n", IMG_GetError());
        TTF_CloseFont(font);
        SDL_DestroyTexture(menu_texture);
        SDL_DestroyTexture(submenu_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Texture *solde_texture = SDL_CreateTextureFromSurface(renderer, solde_surface);
    SDL_FreeSurface(solde_surface);

    // Boucle principale
    int running = 1, on_menu = 0, on_submenu = -1, submenu_selected = 0;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                if (!on_menu) {
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        on_menu = 1;
                    }
                } else if (on_menu && on_submenu == -1) {
                    if (event.key.keysym.sym == SDLK_UP) {
                        submenu_selected = (submenu_selected - 1 + 3) % 3;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        submenu_selected = (submenu_selected + 1) % 3;
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
                        if (submenu_selected == 0) {
                            on_submenu = 0;
                        } else if (submenu_selected == 2) {
                            running = 0;
                        }
                    }
                } else if (on_submenu == 0) {
                    const char *solde_options[] = {"Ajouter de l'argent", "Retirer de l'argent", "Retour"};
                    if (event.key.keysym.sym == SDLK_UP) {
                        submenu_selected = (submenu_selected - 1 + 3) % 3;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        submenu_selected = (submenu_selected + 1) % 3;
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
                        if (submenu_selected == 0) {
                            balance += 5;
                        } else if (submenu_selected == 1 && balance >= 5) {
                            balance -= 5;
                        } else if (submenu_selected == 2) {
                            on_submenu = -1;
                        }
                    }
                }
            }
        }

        SDL_RenderClear(renderer);

        if (on_menu) {
            if (on_submenu == -1) {
                const char *menu_options[] = {"Solde", "Jeux", "Quitter"};
                render_submenu(renderer, font, submenu_selected, menu_options, 3, 400, submenu_texture);
            } else if (on_submenu == 0) {
                const char *solde_options[] = {"Ajouter de l'argent", "Retirer de l'argent", "Retour"};
                render_submenu(renderer, font, submenu_selected, solde_options, 3, 400, solde_texture);
                render_balance(renderer, font);
            }
        } else {
            SDL_Rect bg_rect = {0, 0, 1672, 941};
            SDL_RenderCopy(renderer, menu_texture, NULL, &bg_rect);
            render_text(renderer, font, "Commencer", (SDL_Color){255, 255, 255, 255}, 740, 695);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menu_texture);
    SDL_DestroyTexture(submenu_texture);
    SDL_DestroyTexture(solde_texture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
