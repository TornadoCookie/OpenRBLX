#include "players.h"

Players *Players_new(const char *className, Instance *parent)
{
    Players *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(Players);
    newInst = realloc(newInst, sizeof(Players));

    if (parent) Instance_SetParent(newInst, parent);

    newInst->LocalPlayer = NULL;

    newInst->instance.Name = "Players";

    return newInst;
}

static int nextUserId;

void Players_CreateLocalPlayer(Players *this)
{
    this->LocalPlayer = Player_new("Player", this);
    this->LocalPlayer->UserId = --nextUserId; // start at -1, then -2, -3, etc.
    this->LocalPlayer->instance.Name = "OpenRBLXPlayer";
}

void serialize_Players(Players *players, SerializeInstance *inst)
{
    serialize_Instance(players, inst);

    serialize_atomic(bool, players, CharacterAutoLoads);
    serialize_atomic(Ref, players, LocalPlayer);
}
