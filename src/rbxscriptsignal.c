#include "rbxscriptsignal.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>

struct _RBXScriptSignal_Internal {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

void RBXScriptConnection_Disconnect(RBXScriptConnection *this)
{
    this->connected = false;
}

RBXScriptSignal *RBXScriptSignal_new()
{
    RBXScriptSignal *signal = malloc(sizeof(RBXScriptSignal));
    signal->private = malloc(sizeof(_RBXScriptSignal_Internal));
    pthread_cond_init(&signal->private->cond, NULL);
    pthread_mutex_init(&signal->private->mutex, NULL);

    signal->connections = NULL;
    signal->connectionCount = 0;

    return signal;
}

RBXScriptConnection *RBXScriptSignal_Connect(RBXScriptSignal *this, void *func)
{
    RBXScriptConnection *connection = malloc(sizeof(RBXScriptConnection));

    connection->connected = false;
    connection->did_once = false;
    connection->do_once = false;
    connection->func = func;
    
    this->connectionCount++;
    this->connections = realloc(this->connections, this->connectionCount * sizeof(RBXScriptConnection *));
    this->connections[this->connectionCount - 1] = connection;

    return connection;
}

RBXScriptConnection *RBXScriptSignal_Once(RBXScriptSignal *this, void *func)
{
    RBXScriptConnection *connection = RBXScriptSignal_Connect(this, func);

    connection->do_once = true;

    return connection;
}

void RBXScriptSignal_Wait(RBXScriptSignal *this)
{
    pthread_mutex_lock(&this->private->mutex);
    pthread_cond_wait(&this->private->cond, &this->private->mutex);
}

void RBXScriptSignal_Fire(RBXScriptSignal *this, void *arg)
{
    //pthread_cond_broadcast(&this->private->cond);

    for (int i = 0; i < this->connectionCount; i++)
    {
        if (this->connections[i]->connected)
        {
            if (this->connections[i]->do_once && this->connections[i]->did_once) continue;
            this->connections[i]->func(arg);
            this->connections[i]->did_once = true;
        }
    }
}
