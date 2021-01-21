#include <enet/enet.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#include "../shared/player.h"
#include "../shared/message.h"
#include "../shared/shared.h"
#include "client.h"
#include "../shared/deltatime.h"

struct client
{
    char name[15];
    char serverip[20];
    struct inputs inputs;
    struct enet
    {

        ENetAddress address;
        ENetHost *host;
        ENetPeer *peer;
    } enet;
    struct sdl
    {
        SDL_Window *window;
        SDL_Renderer *renderer;
        TTF_Font *font;
    } sdl;
    struct status
    {
        int status;
        int running;
    } status;
    int local_player_slot;
    struct player players[MAX_PLAYERS];
    int (*init)(struct client *client);
    int (*draw_pending)(struct client *client);
    int (*exit)(struct client *client);
    /* Should be called 60 times per second. */
    int (*update)(struct client *client, long double dt);
    int (*draw)(struct client *client);
    int (*draw_scoreboard)(struct client *client);
    /* Callback when a packet is received. */
    int (*on_packet_receive)(struct client *client, struct packet packet);
    /* Callback when a client is disconnected. */
    int (*on_client_disconnection)(struct client *client, int slot);
    /* Main loop triggering our callbacks. */
    int (*main_loop)(struct client *client);
};
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
int client_draw_scoreboard(struct client *client)
{
    SDL_Rect rect1;
    SDL_Texture *texture1;
    char buffer[500] = {0};
    int cursor = 0;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        char b1[20];
        memset(b1, '-', 20);
        int last = sprintf(b1, "ID: %d", i);
        b1[last] = '-';
        b1[19] = 0;
        cursor += sprintf(buffer + cursor, "%s NAME:%s\n", b1, (client->players[i].empty) ? "EMPTY" : client->players[i].name);
    }
    get_text_and_rect(client->sdl.renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, buffer, client->sdl.font, &texture1, &rect1);
    rect1.x -= rect1.w / 2;
    rect1.y -= rect1.h / 2;
    SDL_RenderCopy(client->sdl.renderer, texture1, NULL, &rect1);
    SDL_DestroyTexture(texture1);
    return 1;
}
int client_draw(struct client *client)
{

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (client->players[i].empty == 0)
        {
            player_update(&client->players[i], delta_time());
            if (client->local_player_slot >= 0)
            {
                player_draw(&client->players[i], client->sdl.renderer, vector_sub(vector_new(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), client->players[client->local_player_slot].transform.position));
            }
            else
            {
                player_draw(&client->players[i], client->sdl.renderer, vector_zero());
            }
        }
    }

    return 1;
}
int client_draw_pending(struct client *client)
{

    struct player p = player_new(transform_new(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, (double)clock() / 10000), client->name);
    player_update(&p, -1);
    player_draw(&p, client->sdl.renderer, vector_zero());

    return 1;
}
int client_on_packet_receive(struct client *client, struct packet packet)
{
    switch (packet.type)
    {
    case MSG_TYPE_SERVER_ANSWER_TO_JOIN:
        if (packet.join_answer.ok == 1)
        {
            client->local_player_slot = packet.join_answer.slot;
            client->status.status = STATUS_CONNECTED;
            struct packet pkt;
            pkt = msg_new_player_inputs(client->inputs);
            ENetPacket *packet = enet_packet_create(&pkt, sizeof(pkt), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(client->enet.peer, 0, packet);
        }
        else
        {
            enet_peer_disconnect_now(client->enet.peer, 0);
        }
        break;
    case MSG_TYPE_PLAYER_STATE:

        client->players[packet.player_state.slot] = packet.player_state.player;
        break;
    case MSG_TYPE_PLAYER_LEFT:
        client->players[packet.player_left.slot].empty = 1;
        break;

    default:
        break;
    }
    return 1;
}
int client_init(struct client *client)
{
    client->inputs.backward = 0;
    client->inputs.forward = 0;
    client->inputs.left = 0;
    client->inputs.right = 0;
    client->inputs.tab = 0;
    client->inputs.space = 0;
    client->status.running = 1;
    client->local_player_slot = -1;
    client->status.status = STATUS_PENDING;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        client->players[i].empty = 1;
    }
    // a. Initialize enet
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    // b. Create a host using enet_host_create
    client->enet.host = enet_host_create(NULL /* create a client host */,
                                         1 /* only allow 1 outgoing connection */,
                                         2 /* allow up 2 channels to be used, 0 and 1 */,
                                         0 /* assume any amount of incoming bandwidth */,
                                         0 /* assume any amount of outgoing bandwidth */);

    if (client->enet.host == NULL)
    {
        fprintf(stderr, "An error occured while trying to create an ENet server host\n");
        exit(EXIT_FAILURE);
    }

    enet_address_set_host(&client->enet.address, client->serverip);
    client->enet.address.port = 1234;

    // c. Connect and user service
    client->enet.peer = enet_host_connect(client->enet.host, &client->enet.address, 2, 0);

    if (client->enet.peer == NULL)
    {
        fprintf(stderr, "No available peers for initializing an ENet connection");
        exit(EXIT_FAILURE);
    }

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    else
    {
        //Create window

        if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &client->sdl.window, &client->sdl.renderer) != 0)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return -1;
        }
        if (TTF_Init() < 0)
        {
            printf("SDL TTF could not initialize!\n");
            return -1;
        }
        client->sdl.font = TTF_OpenFont("WhiteRabbit-47pD.ttf", 24);
        printf("client->sdl.font%d\n", client->sdl.font == NULL);
        SDL_SetWindowTitle(client->sdl.window, "Spaceship Warfare v0.1 by Fran6nd");
    }
    return 1;
}

int client_main_loop(struct client *client)
{
    delta_time_init();
    long double counter = 0;
    while (client->status.running)
    {
        int eventStatus;
        ENetEvent event;
        SDL_SetRenderDrawColor(client->sdl.renderer, 0, 0, 0, 24);
        SDL_RenderClear(client->sdl.renderer);
        eventStatus = enet_host_service(client->enet.host, &event, 1);
        //printf("yooo\n");
        // If we had some event that interested us
        if (eventStatus > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {
                printf("(Client) We are now connected to %x.\n",
                       event.peer->address.host);
                struct packet pkt;
                struct player p = player_new(transform_new(0, 0, 0), client->name);

                pkt = msg_new_ask_join(&p);
                ENetPacket *packet = enet_packet_create(&pkt, sizeof(pkt), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(client->enet.peer, 0, packet);
            }
            break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                struct packet pkt;
                memcpy(&pkt, event.packet->data, sizeof(struct packet));
                client->on_packet_receive(client, pkt);
                enet_packet_destroy(event.packet);
            }
            break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("(Client) %s disconnected.\n", (char *)event.peer->data);
                client->status.running = 0;
                client->status.status = STATUS_DISCONNECTED;
                // Reset client's information
                event.peer->data = NULL;
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
            }
        }
        SDL_Event sdl_event;
        int input_changed = 0;
        if (SDL_PollEvent(&sdl_event) == 1)
        {
            switch (sdl_event.type)
            {
            case SDL_WINDOWEVENT:
                /* code */
                break;
            case SDL_KEYDOWN:;
                if (sdl_event.key.repeat == 0)
                {
                    switch (sdl_event.key.keysym.sym)
                    {
                    case SDLK_z:
                        input_changed = 1;
                        client->inputs.forward = 1;
                        break;
                    case SDLK_s:
                        input_changed = 1;
                        client->inputs.backward = 1;
                        break;
                    case SDLK_q:
                        input_changed = 1;
                        client->inputs.left = 1;
                        input_changed = 1;
                        break;

                    case SDLK_d:
                        client->inputs.right = 1;
                        input_changed = 1;
                        break;
                    case SDLK_SPACE:
                        client->inputs.space = 1;
                        input_changed = 1;
                        break;

                    case SDLK_TAB:
                        client->inputs.tab = 1;
                        input_changed = 1;
                        break;
                    default:
                        break;
                    }
                }
                break;

            case SDL_KEYUP:
                if (sdl_event.key.repeat == 0)
                {
                    switch (sdl_event.key.keysym.sym)
                    {
                    case SDLK_z:
                        input_changed = 1;
                        client->inputs.forward = 0;
                        break;
                    case SDLK_s:
                        client->inputs.backward = 0;
                        input_changed = 1;
                        break;
                    case SDLK_q:
                        client->inputs.left = 0;
                        input_changed = 1;
                        break;
                    case SDLK_d:
                        client->inputs.right = 0;
                        input_changed = 1;
                        break;
                    case SDLK_SPACE:
                        client->inputs.space = 0;
                        input_changed = 1;
                        break;
                    case SDLK_TAB:
                        client->inputs.tab = 0;
                        input_changed = 1;
                        break;
                    default:
                        break;
                    }
                }
                break;

            case SDL_QUIT:
                client->status.running = 0;
                break;
            default:
                break;
            }
            if (input_changed)
            {
                struct packet pkt;
                pkt = msg_new_player_inputs(client->inputs);
                ENetPacket *packet = enet_packet_create(&pkt, sizeof(pkt), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(client->enet.peer, 0, packet);
            }
        }
        delta_time_update();
        counter += delta_time();
        if (counter > 1.0)
        {
            /*every 1 seconds!*/
            counter = 0;
        }
        switch (client->status.status)
        {
        case STATUS_PENDING:
            client->draw_pending(client);
            break;
        case STATUS_CONNECTED:
            client->draw(client);
            break;
        default:
            break;
        }
        if (client->inputs.tab)
        {
            client->draw_scoreboard(client);
        }
        SDL_RenderPresent(client->sdl.renderer);
    }
    return 1;
}
int client_exit(struct client *client)
{
    enet_peer_disconnect_now(client->enet.peer, 0);
    if (client->sdl.renderer)
    {
        SDL_DestroyRenderer(client->sdl.renderer);
    }
    if (client->sdl.window)
    {
        SDL_DestroyWindow(client->sdl.window);
    }
    delta_time_exit();
    SDL_Quit();
    TTF_Quit();
    return 1;
}

int main(int argc, char **argv)
{
    struct client client = {.init = client_init,
                            .main_loop = client_main_loop,
                            .draw_pending = client_draw_pending,
                            .on_packet_receive = client_on_packet_receive,
                            .draw = client_draw,
                            .exit = client_exit,
                            .draw_scoreboard = client_draw_scoreboard};
    if (argc == 2)
    {
        strcpy(client.name, argv[1]);
    }
    else
    {
        strcpy(client.name, "NewPlayer");
    }
    if (argc == 3)
    {
        strcpy(client.serverip, argv[2]);
    }
    else
    {
        strcpy(client.serverip, "localhost");
    }
    client.init(&client);
    client.main_loop(&client);
    client.exit(&client);
}