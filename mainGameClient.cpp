

#include "GameClient.h"
//#include "MsgIO.h"

#include <stdio.h>
#include <stdlib.h>

int main(){

    char server_IP[16] = "127.0.0.1";
    char player_name[MAX_PLAYER_NAME_LENGTH] = "Player";

    printf("Enter your name: ");
    scanf("%s", player_name);


    GameClient client;

    if (client.createConnection(server_IP)){
        perror("can't establish connection\n");
        return 0;
    } else {
        printf("connection established\n");
    }

    if (!client.handshake(player_name)){
        perror("NE POZHAL RUKU, PES\n");
        return 0;
    }

    printf(" PLAYER NAME: %s\nOPPONENT NAME: %s\n",
           client.getPlayerName(),
           client.getOpponentName()
    );

    client.launchServesServer();

    int n       = 0;
    int count   = 0;
    int command = 0;

    Data data;

    while (client.getState() == CLIENT_STATE::ACTIVE){

        printf("Enter message: \n");
        scanf("%s", data.data_msg);
        client.sendData(data);
    }

    printf("EXITING\n");
    return 0;

}