#ifndef _PLAYER_
#define _PLAYER_

#include "instance.h"
#include "model.h"

typedef struct Player {
    Instance instance;

    Model_Instance *Character;
    int64_t UserId;
} Player;

Player *Player_new(const char *className, Instance *parent);

void Player_LoadCharacter(Player *this);
void Player_Move(Player *this, Vector3 walkDirection, bool relativeToCamera);

void Player_RunScripts(Player *this); // Run all client scripts

#endif
