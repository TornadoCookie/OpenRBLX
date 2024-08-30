#ifndef _PLAYERS_
#define _PLAYERS_

#include "instance.h"
#include "player.h"

typedef struct Players {
    Instance instance;

    Player *LocalPlayer;
} Players;

Players *Players_new(const char *className, Instance *parent);

void Players_CreateLocalPlayer(Players *this);

#endif
