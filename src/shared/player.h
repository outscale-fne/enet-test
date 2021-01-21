#ifndef PLAYER_H
#define PLAYER_H

#ifndef SERVER
#include <SDL2/SDL.h>
#endif

#include "triangle.h"
#include "transform.h"

struct inputs{
    char space, forward, backward, left, right, tab;
};

struct player
{
    int empty, slot;
    char name[15];
    struct Transform transform;
    struct triangle triangles[9];
};

#ifndef SERVER
void player_draw(struct player *player, SDL_Renderer *r, struct vector offset);
#endif
struct player player_new(struct Transform t, char *);
struct player player_new_empty();
void player_update(struct player *player, double dt);
void player_set_inputs(struct player * player, struct inputs inputs);

#endif