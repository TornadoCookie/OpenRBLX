#include "players.h"

Players *Players_new(const char *className, Instance *parent)
{
    Players *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(Players)
    newInst = realloc(newInst, sizeof(Players));

    if (parent) Instance_SetParent(newInst, parent);

    newInst->LocalPlayer = NULL;

    return newInst;
}

static int nextUserId;

void Players_CreateLocalPlayer(Players *this)
{
    this->LocalPlayer = Player_new("Player", this);
    this->LocalPlayer->UserId = --nextUserId;
    this->LocalPlayer->instance.Name = "OpenRBLXPlayer";
}
