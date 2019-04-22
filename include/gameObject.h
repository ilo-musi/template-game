/*
Ilo Musi Game Template - A starter template for creating new games for ilo musi

Written in 2019 by Wong Cho Ching <https://sadale.net>

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#define GAME_OBJECT_NUM (64U)
#define COIN_WIDTH (8)
#define COIN_HEIGHT (8)

enum GameObjectType{
	NO_OBJECT,
	GAME_OBJECT_COIN,
	//GAME_OBJECT_APPLE,
	//GAME_OBJECT_ORANGE,
};

struct __attribute__((packed)) GameObject{
	enum GameObjectType type;
	int8_t x;
	int8_t y;
	int8_t extra;
};

extern struct GameObject gameObjectArray[GAME_OBJECT_NUM];

extern void gameObjectInit(void);
extern struct GameObject* gameObjectNew(enum GameObjectType objectType, int8_t x, int8_t y);
extern void gameObjectDelete(struct GameObject* gameObject);
