#ifndef UTILS_H
#define UTILS_H
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
                       TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);
void draw_circle(SDL_Renderer * renderer, int x, int y, int radius);
#endif