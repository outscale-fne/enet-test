#include <string.h>
#include "message.h"

struct packet msg_new_ask_join(struct player * player)
{
    struct packet p = {.type = MSG_TYPE_PLAYER_ASK_JOIN};
    p.ask_join.player = *player;
    return p;
}
struct packet msg_new_player_state(struct player * player, int slot)
{
    struct packet p = {.type = MSG_TYPE_PLAYER_STATE, .player_state.slot = slot, .player_state.player = *player};
    return p;
}
struct packet msg_new_join_answer(int ok, int slot)
{
    struct packet p = {.type = MSG_TYPE_SERVER_ANSWER_TO_JOIN, .join_answer.ok = ok, .join_answer.slot = slot};
    return p;
}
struct packet msg_new_left(int slot)
{
    struct packet p = {.type = MSG_TYPE_PLAYER_LEFT, .player_left.slot = slot};
    return p;
}
struct packet msg_new_player_inputs(struct inputs inputs)
{
    struct packet p = {.type = MSG_TYPE_PLAYER_INPUTS, .inputs = inputs};
    return p;
}