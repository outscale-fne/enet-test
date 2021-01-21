#ifndef CLIENT_H
#define CLIENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <enet/enet.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "utils.h"

#include "../shared/player.h"
#include "../shared/message.h"
#include "../shared/shared.h"
#include "../shared/deltatime.h"

const int SCREEN_WIDTH = 840;
const int SCREEN_HEIGHT = 640;

#define STATUS_PENDING 1
#define STATUS_CONNECTED 2
#define STATUS_DISCONNECTED 3

#endif