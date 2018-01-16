
#ifndef MSGIO_H
#define MSGIO_H

#include "Base.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdio>

// typedef?

typedef struct{
    char data_msg[10]{0};
} Data;

enum class TYPE_OF_MSG{
    START,
    FINISH,
    SEND_NAME,
    RECV_NAME,
    DATA
};

inline int send_all(int sock_fd, void* buf, int len, int flags){
    int total = 0;
    int n     = 0;

    while (total < len){
        n = send(sock_fd, (char*)buf + total, len - total, flags);

        if (n == -1){
            perror("ERR_SEND");
            break;
        }

        total += n;
    }

    if (n == -1)
        return -1;
    else
        return total;
}

inline int recv_all(int sock_fd, void* buf, int len, int flags){

    int total = 0;
    int n     = 0;

    while (total < len){
        n = recv(sock_fd, (char*)buf + total, len - total, flags);

        if (n == -1){
            perror("ERR_RECV");
            break;
        }

        total += n;
    }

    if (n == -1)
        return -1;
    else
        return total;
}

// --------- IO type of message ----------

inline void sendTypeMessage(int sock_fd, TYPE_OF_MSG msg_t){
    send(sock_fd, &msg_t, sizeof(msg_t), 0);
}

inline TYPE_OF_MSG recvTypeMessage(int sock_fd){
    TYPE_OF_MSG msg_t;
    recv(sock_fd, &msg_t, sizeof(msg_t), 0);
    return msg_t;
}

// ---------- start game ------------

inline void sendStartGame(GameConnection* game_connection){
    auto msg_t = TYPE_OF_MSG::START;

    sendTypeMessage(game_connection->player_1, msg_t);
    sendTypeMessage(game_connection->player_2, msg_t);
}

inline bool recvStartGame(GameConnection* game_connection){

    // strange function, depending on where it is invoked

    auto msg_t = recvTypeMessage(game_connection->player_1);

    if (msg_t != TYPE_OF_MSG::START)
        return false;

    return recvTypeMessage(game_connection->player_2) == TYPE_OF_MSG::START;
}

// -------- finish game --------

inline void sendFinishGame(GameConnection* game_connection){
    auto msg_t = TYPE_OF_MSG::FINISH;

    sendTypeMessage(game_connection->player_1, msg_t);
    sendTypeMessage(game_connection->player_2, msg_t);
}

inline bool recvFinishGame(GameConnection* game_connection){
    auto msg_t = recvTypeMessage(game_connection->player_1);

    if (msg_t != TYPE_OF_MSG::FINISH)
        return false;

    return recvTypeMessage(game_connection->player_2) == TYPE_OF_MSG::FINISH;
}

// -------- name of players --------

inline void sendPlayersName(GameConnection* game_connection,
                     char player1_name[MAX_PLAYER_NAME_LENGTH],
                     char player2_name[MAX_PLAYER_NAME_LENGTH]
                    ){
    sendTypeMessage(game_connection->player_1, TYPE_OF_MSG::SEND_NAME);
    send(game_connection->player_1, player2_name, MAX_PLAYER_NAME_LENGTH * sizeof(char), 0);

    sendTypeMessage(game_connection->player_2, TYPE_OF_MSG::SEND_NAME);
    send(game_connection->player_2, player1_name, MAX_PLAYER_NAME_LENGTH * sizeof(char), 0);

}

inline bool recvPlayersName(GameConnection* game_connection,
                     char player1_name[MAX_PLAYER_NAME_LENGTH],
                     char player2_name[MAX_PLAYER_NAME_LENGTH]
                    ) {
    sendTypeMessage(game_connection->player_1, TYPE_OF_MSG::RECV_NAME);
    int n1 = recv(game_connection->player_1, player2_name, MAX_PLAYER_NAME_LENGTH * sizeof(char), 0);

    if (n1 == -1)
        return false;

    sendTypeMessage(game_connection->player_2, TYPE_OF_MSG::RECV_NAME);
    int n2 = recv(game_connection->player_2, player1_name, MAX_PLAYER_NAME_LENGTH * sizeof(char), 0);

    if (n2 == -1)
        return false;

    return true;

}

#endif