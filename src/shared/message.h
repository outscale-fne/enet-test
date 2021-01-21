#ifndef MESSAGE_H
#define MESSAGE_H

#include "player.h"

#define MSG_TYPE_PLAYER_STATE 1
#define MSG_TYPE_PLAYER_INPUTS 2
#define MSG_TYPE_PLAYER_ASK_JOIN 3
#define MSG_TYPE_SERVER_ANSWER_TO_JOIN 4
#define MSG_TYPE_PLAYER_LEFT 5
#define MSG_TYPE_PLAYER_JOINED 6

struct player_state {
    int slot;
    struct player player;
};
struct ask_join {
    struct player player;
};
struct player_joined {
    struct player player;
};
struct join_answer {
    int ok;
    int slot;
};
struct player_left {
    int slot;
};
struct packet{
    int type;
    union{
        struct player_state player_state;
        struct inputs inputs;
        struct ask_join ask_join;
        struct join_answer join_answer;
        struct player_left player_left;
    };
};

struct packet msg_new_ask_join(struct player* player);
struct packet msg_new_player_state(struct player* player, int slot);
struct packet msg_new_join_answer(int ok, int slot);
struct packet msg_new_left(int slot);
struct packet msg_new_player_inputs(struct inputs inputs);

#endif