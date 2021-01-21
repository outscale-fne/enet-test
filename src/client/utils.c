#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../shared/vector.h"
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
                       TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect)
{
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {0, 255, 0, 0};

    surface = TTF_RenderText_Blended_Wrapped(font, text, textColor, 500);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}
void draw_circle(SDL_Renderer *renderer, int x, int y, int radius)
{
    for (int x1 = -radius; x1 <= radius; x1++)
    {
        for (int y1 = -radius; y1 <= radius; y1++)
        {
            if(x1 == -radius || y1 == -radius || x1 == radius || y1 == radius){
                
                struct vector v = vector_new(x1, y1);
                v = vector_normalize(v);
                v = vector_mul(v, radius);
                SDL_RenderDrawPoint(renderer, (int) v.x + x,(int) v.y + y);
            }
        }
    }
}