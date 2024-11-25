#ifndef _PLAYERS_
#define _PLAYERS_

#include "instance.h"
#include "player.h"

typedef struct Players {
    Instance instance;

    Player *LocalPlayer;
    bool CharacterAutoLoads;
} Players;

Players *Players_new(const char *className, Instance *parent);

void Players_CreateLocalPlayer(Players *this);

void serialize_Players(Players *players, SerializeInstance *inst);

#endif
