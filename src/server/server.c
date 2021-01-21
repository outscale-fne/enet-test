// server.c
#include <enet/enet.h>
#include <stdio.h>
#include <string.h>

#include "../shared/player.h"
#include "../shared/message.h"
#include "server.h"
#include "../shared/deltatime.h"

struct server
{
    struct enet
    {
        ENetAddress address;
        ENetHost *host;
    } enet;
    struct status
    {
        int connected_clients;
        int running;
    } status;
    struct inputs inputs[MAX_PLAYERS];
    struct player players[MAX_PLAYERS];
    int (*init)(struct server *server);
    int (*exit)(struct server *server);
    /* Should be called 60 times per second. */
    int (*update)(struct server *server, long double dt);
    /* Callback when a packet is received. */
    int (*on_packet_receive)(struct server *server, struct packet packet, ENetPeer *peer);
    /* Callback when a client is disconnected. */
    int (*on_client_disconnection)(struct server *server, int slot);
    /* Main loop triggering our callbacks. */
    int (*main_loop)(struct server *server);
};

int server_init(struct server *server)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        server->inputs[i].backward = 0;
        server->inputs[i].forward = 0;
        server->inputs[i].left = 0;
        server->inputs[i].right = 0;
        server->inputs[i].tab = 0;
        server->inputs[i].space = 0;
    }

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    atexit(enet_deinitialize);
    server->enet.address.host = ENET_HOST_ANY;
    server->enet.address.port = 1234;

    server->enet.host = enet_host_create(&server->enet.address /* the address to bind the server host to */,
                                         MAX_PEERS /* allow up to MAX_PEERS clients and/or outgoing connections */,
                                         2 /* allow up to 2 channels to be used, 0 and 1 */,
                                         0 /* assume any amount of incoming bandwidth */,
                                         0 /* assume any amount of outgoing bandwidth */);

    if (server->enet.host == NULL)
    {
        fprintf(stderr, "An error occured while trying to create an ENet server host\n");
        exit(EXIT_FAILURE);
    }
    server->status.running = 1;
    server->status.connected_clients = 0;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        server->players[i].empty = 1;
    }
    return 1;
}

int server_update(struct server *server, long double dt)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (server->players[i].empty == 0)
        {
            player_update(&server->players[i], dt);
            //struct packet pkt = msg_new_player_state(server->players[i], i);
            //ENetPacket *packet = enet_packet_create(&pkt, sizeof(pkt), ENET_PACKET_FLAG_RELIABLE);
            //enet_host_broadcast(server->enet.host, 0, packet);
        }
    }
    enet_host_flush(server->enet.host);
    return 1;
}

int server_exit(struct server *server)
{
    delta_time_exit();
    enet_host_destroy(server->enet.host);
    return 1;
}

int server_on_packet_receive(struct server *server, struct packet packet, ENetPeer *peer)
{
    switch (packet.type)
    {
    case MSG_TYPE_PLAYER_ASK_JOIN:
        server->players[peer->incomingPeerID] = packet.ask_join.player;
        printf("Player [%s] joined on slot [%d].\n", packet.ask_join.player.name, peer->incomingPeerID);
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (server->players[i].empty == 0)
            {
                struct packet pkt = msg_new_player_state(&server->players[i], i);
                ENetPacket *packet = enet_packet_create(&pkt, sizeof(pkt), ENET_PACKET_FLAG_RELIABLE);
                enet_host_broadcast(server->enet.host, 0, packet);
            }
        }
        break;
    case MSG_TYPE_PLAYER_INPUTS:
        server->inputs[peer->incomingPeerID] = packet.inputs;
        if (server->players[peer->incomingPeerID].empty == 0)
        {
            player_set_inputs(&server->players[peer->incomingPeerID], server->inputs[peer->incomingPeerID]);
            struct packet pkt = msg_new_player_state(&server->players[peer->incomingPeerID], peer->incomingPeerID);
            ENetPacket *packet = enet_packet_create(&pkt, sizeof(pkt), ENET_PACKET_FLAG_RELIABLE);
            enet_host_broadcast(server->enet.host, 0, packet);
        }
        break;
    default:
        break;
    }
}

int server_on_client_disconnection(struct server *server, int slot)
{
    printf("Player [%s] on slot [%d] left.\n", server->players[slot].name, slot);
    server->players[slot].empty = 1;
    struct packet pkt = msg_new_left(slot);
    ENetPacket *packet = enet_packet_create(&pkt, sizeof(pkt), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(server->enet.host, 0, packet);
    return 1;
}

int server_main_loop(struct server *server)
{
    int eventStatus = 0;
    long double counter = 0.0;
    ENetEvent event;
    delta_time_init();
    while (server->status.running)
    {
        eventStatus = enet_host_service(server->enet.host, &event, 1);

        // If we had some event that interested us
        if (eventStatus > 0)
        {

            switch (event.type)
            {

            case ENET_EVENT_TYPE_CONNECT:
                server->status.connected_clients++;
                if (server->status.connected_clients > MAX_PLAYERS)
                {
                    printf("Gonna disconnect because full.\n");
                    /* Should be changed to tell the client why he has been kicked. */
                    enet_peer_disconnect_now(event.peer, 0);
                }
                {
                    struct packet pkt = msg_new_join_answer(1, event.peer->incomingPeerID);
                    ENetPacket *packet = enet_packet_create(&pkt, sizeof(pkt), ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(event.peer, 0, packet);
                }
                break;

            case ENET_EVENT_TYPE_RECEIVE:;
                {
                    struct packet pkt;
                    memcpy(&pkt, event.packet->data, sizeof(struct packet));
                    server->on_packet_receive(server, pkt, event.peer);
                    // Lets broadcast this message to all
                    //enet_host_broadcast(server, 0, event.packet);
                    //enet_host_flush(server);
                }
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                server->status.connected_clients--;
                server->on_client_disconnection(server, (int)event.peer->incomingPeerID);
                event.peer->data = NULL;
                server->inputs[event.peer->incomingPeerID].backward = 0;
                server->inputs[event.peer->incomingPeerID].forward = 0;
                server->inputs[event.peer->incomingPeerID].left = 0;
                server->inputs[event.peer->incomingPeerID].right = 0;
                server->inputs[event.peer->incomingPeerID].tab = 0;
                server->inputs[event.peer->incomingPeerID].space = 0;
                break;
            }
        }
        delta_time_update();
        counter = counter + delta_time();
        if (counter > 1.0)
        {

            counter = 0;
            //printf("top\n");
        }
        server->update(server, delta_time());
    }

    return 1;
}

int main(int argc, char **argv)
{
    struct server srv = {.init = &server_init,
                         .main_loop = &server_main_loop,
                         .exit = server_exit,
                         .on_packet_receive = server_on_packet_receive,
                         .on_client_disconnection = server_on_client_disconnection,
                         .update = server_update};
    srv.init(&srv);
    srv.main_loop(&srv);
    srv.exit(&srv);
}