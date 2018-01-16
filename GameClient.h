
#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "BaseGameClient.h"
#include <stdio.h>
#include <memory>

class GameClient : public BaseGameClient{

    virtual void recvServerData(TYPE_OF_MSG msg_t) {
        if (msg_t == TYPE_OF_MSG::DATA){

            /*
             * @IMPORTANT: objects are passed to the send/recv etc need to be dynamically
             * allocated or else segmentation error because of automatic variable's life-time
             */

            Data* data_ptr = new Data();

            mutexDataLock();
                // recv or recv_all ?
                recv(_sock_fd, data_ptr, sizeof(Data), 0);
                printf("Received: %s", data_ptr->data_msg);

            mutexDataUnlock();

            delete data_ptr;
        }

    }

};

#endif