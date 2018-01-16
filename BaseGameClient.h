
#ifndef BASEGAMECLIENT_H
#define BASEGAMECLIENT_H

#include "Base.h"
#include "MsgIO.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

enum class CLIENT_STATE{
    CREATED,
    CONNECTED,
    ACTIVE,
    DISCONNECTED,
    DESTROYED
};

class BaseGameClient {
public:

    BaseGameClient();
    ~BaseGameClient();

    bool createConnection(char* server_IP, int server_PORT = STD_PORT);
    bool handshake(char name[MAX_PLAYER_NAME_LENGTH]);

    bool  launchServesServer();

    CLIENT_STATE getState();
    void  setDisconnected();

    char* getPlayerName();
    char* getOpponentName();

    void  sendData(Data& data);

protected:

    // bool initConnection(char* server_IP, int server_PORT);  - implementation function
    static void* wrapperListenToServer(void* client_ptr);
           void  listenToServer();

    virtual void recvServerData(TYPE_OF_MSG msg_t) = 0;

    void setState(CLIENT_STATE state);
    void setPlayerName(char name[MAX_PLAYER_NAME_LENGTH]);
    void setOpponentName(char name[MAX_PLAYER_NAME_LENGTH]);

    void mutexDataLock();
    void mutexDataUnlock();

protected:
    int                 _sock_fd;
    struct sockaddr_in  _addr;
    CLIENT_STATE        _state;
    pthread_mutex_t     _mutex_data;

    char  _player_name  [MAX_PLAYER_NAME_LENGTH];
    char  _opponent_name[MAX_PLAYER_NAME_LENGTH];

};


#endif