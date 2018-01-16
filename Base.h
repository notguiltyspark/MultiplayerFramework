#ifndef BASE_H
#define BASE_H

#define STD_PORT 12345
#define MAX_PLAYER_NAME_LENGTH 20

enum class STATE_OF_GAME {
    START,
    ACTIVE,
    FINISH,
};

struct GameConnection{
    int id;
    int player_1;
    int player_2;

    STATE_OF_GAME state;
};

#endif