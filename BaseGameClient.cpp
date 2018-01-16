
#include "BaseGameClient.h"
/*
 *  1. Why us setState instead of simple _state = ...  ??
 *
 *
 *
 *
 * */


BaseGameClient::BaseGameClient() : _state(CLIENT_STATE::CREATED) {
    char opponent_name[MAX_PLAYER_NAME_LENGTH] = "Opponent";
    this->setOpponentName(opponent_name);

    char player_name[MAX_PLAYER_NAME_LENGTH]   = "Player";
    this->setPlayerName(player_name);
}

BaseGameClient::~BaseGameClient() {
    // if (_state != CLIENT_STATE::DESTROYED)???? from listen for example
    this->setState(CLIENT_STATE::DESTROYED);
    close(_sock_fd);
}

bool BaseGameClient::createConnection(char *server_IP, int server_PORT) {
    _mutex_data = PTHREAD_MUTEX_INITIALIZER;

    if (this->getState() != CLIENT_STATE::CREATED)
        return false;

    _sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock_fd < 0){
        std::perror("can't open socket");
        return false;
    }

    _addr.sin_family      = AF_INET;
    _addr.sin_port        = htons(server_PORT);
    _addr.sin_addr.s_addr = inet_addr(server_IP);

    int cnt = connect(_sock_fd, (struct sockaddr *) &_addr, sizeof(_addr));

    if (cnt < 0) {
        perror("can't connect");
        return false;
    }

    this->setState(CLIENT_STATE::CONNECTED);
    return true;
}

bool BaseGameClient::handshake(char name[MAX_PLAYER_NAME_LENGTH]){
    if (this->getState() != CLIENT_STATE::CONNECTED)
        return false;

    // ---- agreeing on START ------

    TYPE_OF_MSG msg_t;

    // loop starts, but who will send mm???

    do {
        msg_t = recvTypeMessage(_sock_fd);
    } while (msg_t != TYPE_OF_MSG::START);

    sendTypeMessage(_sock_fd, TYPE_OF_MSG::START);

    // ---- exchanging NAMES -----

    msg_t = recvTypeMessage(_sock_fd);
    if (msg_t != TYPE_OF_MSG::SEND_NAME)
        return false;

    /* ........
       ........ Where to take non-deafult names from??
       ........
    */

    send(_sock_fd, _player_name, MAX_PLAYER_NAME_LENGTH * sizeof(char), 0);

    msg_t = recvTypeMessage(_sock_fd);
    if (msg_t != TYPE_OF_MSG::RECV_NAME)
        return false;

    recv(_sock_fd, _opponent_name, MAX_PLAYER_NAME_LENGTH * sizeof(char), 0);
    // or change _opponent_name to the proxy opponent_name + this->setOppName...;


    // ------ HANDSHAKE complete -------

    this->setState(CLIENT_STATE::ACTIVE);
    return true;

}


bool BaseGameClient::launchServesServer() {
    if (this->getState() != CLIENT_STATE::ACTIVE)
        return false;

    pthread_t       thread_listen;
    pthread_attr_t  thread_listen_attr;

    pthread_attr_init(&thread_listen_attr);

    pthread_create(&thread_listen, &thread_listen_attr, this->wrapperListenToServer, (void*) this);
    return true;
}

void* BaseGameClient::wrapperListenToServer(void* client_ptr) {
    BaseGameClient* pClient = (BaseGameClient*) client_ptr;
    pClient->listenToServer();

    return NULL;
}

void BaseGameClient::listenToServer() {
    TYPE_OF_MSG msg_t;

    do{
        msg_t = recvTypeMessage(_sock_fd);
        this->recvServerData(msg_t);
    } while (msg_t != TYPE_OF_MSG::FINISH && this->getState() == CLIENT_STATE::ACTIVE);

    if (this->getState() != CLIENT_STATE::ACTIVE){
        sendTypeMessage(_sock_fd, TYPE_OF_MSG::FINISH);
        msg_t = recvTypeMessage(_sock_fd);
    }

    if (msg_t == TYPE_OF_MSG::FINISH){
        this->setState(CLIENT_STATE::DISCONNECTED);
    }

    this->setState(CLIENT_STATE::DESTROYED);  // really needed? what about destructor?
}

void BaseGameClient::sendData(Data& data) {
    sendTypeMessage(_sock_fd, TYPE_OF_MSG::DATA);
    send(_sock_fd, &data, sizeof(Data), 0);
}

void BaseGameClient::setDisconnected(){
    this->setState(CLIENT_STATE::DISCONNECTED);
}

CLIENT_STATE BaseGameClient::getState() {
    return _state;
}

void BaseGameClient::setState(CLIENT_STATE state) {
    // _state = state;   -   better?
    this->setState(state);
}

char* BaseGameClient::getPlayerName() {
    return _player_name;
}

char* BaseGameClient::getOpponentName() {
    return _opponent_name;
}

void BaseGameClient::setPlayerName(char* name) {
    strcpy(this->_player_name, name);
}

void BaseGameClient::setOpponentName(char* name) {
    strcpy(this->_opponent_name, name);
}

void BaseGameClient::mutexDataLock(){
    pthread_mutex_lock(&_mutex_data);
}

void BaseGameClient::mutexDataUnlock() {
    pthread_mutex_unlock(&_mutex_data);
}