#include "player.h"
#include "transform.h"
#include <string.h>
#include "../shared/deltatime.h"

struct player player_new_empty()
{
    struct player p;
    p.empty = 1;
    return p;
}

#ifndef SERVER

void triangle_draw(struct triangle *t, SDL_Renderer *renderer, struct vector offset)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, (int)t->_p[0].x + offset.x, (int)t->_p[0].y + offset.y, (int)t->_p[1].x + offset.x, (int)t->_p[1].y + offset.y);
    SDL_RenderDrawLine(renderer, (int)t->_p[0].x + offset.x, (int)t->_p[0].y + offset.y, (int)t->_p[2].x + offset.x, (int)t->_p[2].y + offset.y);
    SDL_RenderDrawLine(renderer, (int)t->_p[1].x + offset.x, (int)t->_p[1].y + offset.y, (int)t->_p[2].x + offset.x, (int)t->_p[2].y + offset.y);
}

void player_draw(struct player *player, SDL_Renderer *renderer, struct vector offset)
{
    for (long unsigned int i = 0; i < sizeof(player->triangles) / sizeof(struct triangle); i++)
    {
        triangle_draw(&player->triangles[i], renderer, offset);
    }
}
#endif
struct player player_new(struct Transform t, char *name)
{
    struct player p = {.name = {0}};
    strcpy(p.name, name);
    p.transform = t;
    p.triangles[0] = triangle_new(vector_new(0, 0), vector_new(15 * 3, 5 * 3), vector_new(15 * 3, -5 * 3));
    p.triangles[1] = triangle_new(vector_new(50 * 3, 0), vector_new(15 * 3, 5 * 3), vector_new(15 * 3, -5 * 3));
    p.triangles[2] = triangle_new(vector_new(0, 0), vector_new(-15 * 3, 5 * 3), vector_new(-15 * 3, -5 * 3));
    p.triangles[3] = triangle_new(vector_new(0, 0), vector_new(15, 10 * 3), vector_new(-5 * 3, 25 * 3));
    p.triangles[4] = triangle_new(vector_new(0, 0), vector_new(15, -10 * 3), vector_new(-5 * 3, -25 * 3));
    p.triangles[5] = triangle_new(vector_new(85, 0), vector_new(20 * 3, 0), vector_new(20 * 3, 10));
    p.triangles[6] = triangle_new(vector_new(85, 0), vector_new(20 * 3, 0), vector_new(20 * 3, -10));
    p.triangles[7] = triangle_new(vector_new(0, 0), vector_new(-20 * 5, -25 * 3), vector_new(-20 * 3, -25 * 3));
    p.triangles[8] = triangle_new(vector_new(0, 0), vector_new(-20 * 5, 25 * 3), vector_new(-20 * 3, 25 * 3));
    p.empty = 0;
    return p;
}
void player_update(struct player *player, double dt)
{
    /* First update the position according to velocity and angular velocity. */
    //printf("-->%f\n", dt);
    transform_update(&player->transform, dt);
    /* Update triangle position. */
    for (long unsigned int i = 0; i < sizeof(player->triangles) / sizeof(struct triangle); i++)
    {

        struct triangle *t = &player->triangles[i];
        //printf("triangle %f %f    %f %f    %f %f\n", t->_p[0].x, t->_p[0].y, t->_p[1].x, t->_p[1].y, t->_p[2].x, t->_p[2].y);
        triangle_update(t, &player->transform);
    }
}
void player_set_inputs(struct player *player, struct inputs inputs)
{
    struct vector v = vector_zero();
    if (inputs.forward)
    {
        v.y += 1;
    }
    if (inputs.backward)
    {
        v.y -= 1;
    }
    if (inputs.right)
    {
        v.x -= 1;
    }
    if (inputs.left)
    {
        v.x += 1;
    }
    player->transform.angular_velocity = v.x * 100;
    if (v.y > 0)
    {
        v.y = v.y * 5;
    }
    player->transform.velocity = vector_mul(vector_set_arg(vector_new(0, 1), player->transform.rotation), (v.y + 1.5) * 100);
}