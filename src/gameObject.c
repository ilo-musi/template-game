/*
Ilo Musi Game Template - A starter template for creating new games for ilo musi

Written in 2019 by Wong Cho Ching <https://sadale.net>

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "gameObject.h"

struct GameObject gameObjectArray[GAME_OBJECT_NUM];

void gameObjectInit(void){
	for(size_t i=0; i<GAME_OBJECT_NUM; i++)
		gameObjectArray[i].type = NO_OBJECT;
}

struct GameObject* gameObjectNew(enum GameObjectType objectType, int8_t x, int8_t y){
	for(size_t i=0; i<GAME_OBJECT_NUM; i++){
		if(gameObjectArray[i].type == NO_OBJECT){
			gameObjectArray[i].type = objectType;
			gameObjectArray[i].x = x;
			gameObjectArray[i].y = y;
			return &gameObjectArray[i];
		}
	}
	return NULL;
}

void gameObjectDelete(struct GameObject* gameObject){
	gameObject->type = NO_OBJECT;
}
