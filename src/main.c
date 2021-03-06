/*
Ilo Musi Game Template - A starter template for creating new games for ilo musi

Written in 2019 by Wong Cho Ching <https://sadale.net>

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ilomusi.h"
#include "gameObject.h"
#define FRAME_RATE_PER_SECOND 10
#define FRAME_DELAY_MS (1000/FRAME_RATE_PER_SECOND)
#define INITIAL_MOVEMENT_POINT 100
#define MOVEMENT_POINT_TEXT_X 0
#define MOVEMENT_POINT_TEXT_Y GRAPHIC_PIXEL_HEIGHT-8
#define MOVEMENT_POINT_REQUIRED_PER_STEP 2

//Sound effect for switching between static and animated graphic
const struct SynthData coinSound = {
	.durationRemaining = 15,
	.initialFreq = 160,
	.finalFreq = 170,
	.freqSweepPeriod = 5,
	.initialDutyCycle = 1,
	.finalDutyCycle = 1,
	.dutySweepPeriod = 0,
	.controlFlags = SYNTH_FREQ_SWEEP_BIPOLAR,
};

//Background music. Generated using 
const uint8_t bgm[] = {
	//Sets the instrument on the first note.
	0, SYNTH_COMMAND_MODE_FULL_ONCE_RETURN_TO_DF,
	//Instrument setup note
	20, 129, 129, //duration, initfreq, finalfreq,
		0, //freqSweepPeriod,
		0x12, //initialDutyCycle(upper 4bits)|finalDutyCycle(lower 4bits)
		5, 0xE4, //dutySweepPeriod, controlFlags (found in the synth test game)
	//After that, copy the content generated by iloMusiMidiFileToCommandConverter.py
	//20,129, //comment out the first line and integrate it into Instrument setup note
	20,140,
	20,148,
	20,140,
	40,153,
	40,255,
	40,129,
	40,140,
	40,142,
	40,255,
	40,134,
	40,126,
	20,116,
	20,116,
	20,126,
	20,126,
	40,129,
	40,134,
	40,140,
	40,255,
	20,129,
	20,140,
	20,148,
	20,255,
	40,161,
	40,255,
	40,153,
	40,142,
	40,134,
	40,255,
	40,126,
	20,255,
	20,129,
	40,126,
	40,116,
	40,134,
	40,126,
	20,116,
	20,116,
	20,121,
	20,126,
	20,121,
	20,129,
	20,140,
	20,129,
	20,121,
	20,129,
	20,140,
	20,129,
	20,121,
	20,134,
	20,142,
	20,255,
	20,121,
	20,134,
	20,142,
	20,255,
	20,126,
	20,118,
	20,140,
	20,255,
	20,126,
	20,118,
	20,140,
	20,255,
	40,121,
	40,108,
	20,129,
	20,129,
	20,118,
	20,118,
	20,121,
	20,129,
	20,153,
	20,140,
	20,121,
	20,129,
	20,153,
	20,140,
	10,110,
	10,102,
	10,110,
	10,121,
	10,134,
	10,121,
	10,142,
	10,153,
	40,161,
	40,153,
	20,140,
	20,150,
	20,150,
	20,255,
	40,172,
	40,158,
	20,153,
	20,140,
	20,121,
	20,140,
	20,129,
	20,121,
	20,140,
	20,121,
	20,153,
	20,153,
	20,140,
	20,140,
	80,129,
	20,142,
	20,142,
	20,134,
	20,134,
	80,121,
	20,150,
	20,150,
	20,140,
	20,140,
	20,150,
	20,150,
	20,140,
	20,140,
	20,153,
	20,153,
	20,140,
	20,140,
	20,153,
	20,153,
	20,140,
	20,140,
	20,153,
	20,153,
	20,140,
	20,140,
	80,129,
	20,142,
	20,142,
	20,134,
	20,134,
	80,121,
	20,142,
	20,142,
	20,134,
	20,134,
	20,142,
	20,142,
	20,134,
	20,134,
	20,108,
	20,108,
	20,118,
	20,118,
	20,126,
	20,126,
	20,134,
	20-FRAME_DELAY_MS/2/10,134, //For the last note, makes it a little bit
							//shorter to compensate the delay of looping
							//It makes the bgm sounds more seamless
	//Finally, stop the music
	0, SYNTH_COMMAND_MODE_ONCE
};

#define STORAGE_MAGIC_NUMBER 0x55 //Used for telling if the storage resource has been initialized

//Resource number mapping
#define GRAPHIC_RESOURCE_COIN 1
#define GRAPHIC_RESOURCE_STATIC_COIN 2
//Example for additional resources:
//#define GRAPHIC_RESOURCE_APPLE 2
//#define GRAPHIC_RESOURCE_ORANGE 3
#define STORAGE_RESOURCE_GAME_STATE 255

#define GRAPHIC_RESOURCE_COIN_BUFFER_SIZE 10 //The size of assets/coinX.img
#define GRAPHIC_RESOURCE_STATIC_COIN_BUFFER_SIZE 10 //The size of assets/coin0.img
//#define GRAPHIC_RESOURCE_APPLE_BUFFER_SIZE 5
//#define GRAPHIC_RESOURCE_ORANGE_BUFFER_SIZE 10

uint8_t graphicBufferCoin[GRAPHIC_RESOURCE_COIN_BUFFER_SIZE*8]; //Animated
uint8_t graphicBufferStaticCoin[GRAPHIC_RESOURCE_STATIC_COIN_BUFFER_SIZE]; //Non-animated
//uint8_t graphicBufferApple[GRAPHIC_RESOURCE_APPLE];
//uint8_t graphicBufferOrange[GRAPHIC_RESOURCE_ORANGE];

struct GraphicImage graphicImageCoin[8]; //Animated
struct GraphicImage graphicImageStaticCoin; //Non-animated
//struct GraphicImage graphicImageApple;
//struct GraphicImage graphicImageOrange;

//For game logic
struct GameObject* gameObjectCoin;
uint32_t movementPoints; //Movement requires movement point.
uint32_t frameCounter;

//For game save/load
struct __attribute__ ((__packed__)) GameState{
	uint8_t magicNumber; //If it isn't STORAGE_MAGIC_NUMBER, it means that whatever stored in the resource is invalid
	uint8_t x;
	uint8_t y;
	uint8_t animated;
	uint32_t movementPoints;
};

uint8_t lcdDmaBuffer[GRAPHIC_WIDTH][GRAPHIC_HEIGHT];

void loadGraphic(void){
	//Load image data from SD card to buffer
	storageRead(GRAPHIC_RESOURCE_COIN, 0, graphicBufferCoin, sizeof(graphicBufferCoin));
	storageRead(GRAPHIC_RESOURCE_STATIC_COIN, 0, graphicBufferStaticCoin, sizeof(graphicBufferStaticCoin));
	//storageRead(GRAPHIC_RESOURCE_APPLE, 0, graphicBufferCoin, sizeof(graphicBufferCoin));
	//storageRead(GRAPHIC_RESOURCE_ORANGE, 0, graphicBufferStaticCoin, sizeof(graphicBufferStaticCoin));

	//Load image data from SD card to buffer
	for(size_t i=0; i<8; i++)
		graphicLoadImage(&graphicBufferCoin[GRAPHIC_RESOURCE_COIN_BUFFER_SIZE*i], &graphicImageCoin[i]);
	graphicLoadImage(&graphicBufferStaticCoin, &graphicImageStaticCoin);
	//graphicLoadImage(graphicImageApple, graphicBufferCoin);
	//graphicLoadImage(graphicImageOrange, graphicBufferCoin);
}

void gameInit(void){
	srand(systemGetTick());
	gameObjectInit();

	//Load the game state
	struct GameState gameState;
	int len = storageRead(STORAGE_RESOURCE_GAME_STATE, 0, &gameState, sizeof(gameState));

	//If loading game state had failed, populate the game state with default values
	if(len!=sizeof(gameState) || gameState.magicNumber != STORAGE_MAGIC_NUMBER){
		gameState.x = GRAPHIC_WIDTH/2-COIN_WIDTH/2;
		gameState.y = GRAPHIC_HEIGHT*8/2-COIN_HEIGHT/2;
		gameState.animated = 1; //animated = 1. non-animated = 0.
		gameState.movementPoints = INITIAL_MOVEMENT_POINT;
	}

	//Construct the coin game object
	gameObjectCoin = gameObjectNew(GAME_OBJECT_COIN,
								gameState.x, gameState.y);
	gameObjectCoin->extra = gameState.animated;
	movementPoints = gameState.movementPoints;

	//Initialize animation frame counter
	frameCounter = 0;
}

void saveGameState(void){
	//Store the values into the game state struct
	struct GameState gameState;
	gameState.magicNumber = STORAGE_MAGIC_NUMBER;
	gameState.x = gameObjectCoin->x;
	gameState.y = gameObjectCoin->y;
	gameState.animated = gameObjectCoin->extra;
	gameState.movementPoints = movementPoints;
	//Write the game state struct into the ROM resource
	storageWrite(STORAGE_RESOURCE_GAME_STATE, 0, &gameState, sizeof(gameState));
}

void handleKeyInput(void){
	uint8_t justChanged = keysGetJustChangedState();
	uint8_t state = keysGetPressedState();
	//Example: ((state&KEYS_LEFT)!=0) evaluates to true if the LEFT key is held.
	//Example: ((justChanged&KEYS_LEFT)!=0 && (state&KEYS_LEFT)==0) evaluates to true if the LEFT key is just released.

	//Handle left/right movement
	if((state&KEYS_LEFT)!=0){
		if(movementPoints > MOVEMENT_POINT_REQUIRED_PER_STEP){
			gameObjectCoin->x--;
			movementPoints -= MOVEMENT_POINT_REQUIRED_PER_STEP;
		}
	}else if((state&KEYS_RIGHT)!=0){
		if(movementPoints > MOVEMENT_POINT_REQUIRED_PER_STEP){
			gameObjectCoin->x++;
			movementPoints -= MOVEMENT_POINT_REQUIRED_PER_STEP;
		}
	}

	//Handle up/down movement
	if((state&KEYS_UP)!=0){
		if(movementPoints > MOVEMENT_POINT_REQUIRED_PER_STEP){
			gameObjectCoin->y--;
			movementPoints -= MOVEMENT_POINT_REQUIRED_PER_STEP;
		}
	}else if((state&KEYS_DOWN)!=0){
		if(movementPoints > MOVEMENT_POINT_REQUIRED_PER_STEP){
			gameObjectCoin->y++;
			movementPoints -= MOVEMENT_POINT_REQUIRED_PER_STEP;
		}
	}

	//Handle toggling static and animation mode
	if((justChanged&KEYS_1)!=0 && (state&KEYS_1)!=0){
		gameObjectCoin->extra = 1; //Enable animation
		synthPlayOne(true, &coinSound); //Plays coin sound
		saveGameState(); //Saves the game
	}
	if((justChanged&KEYS_2)!=0 && (state&KEYS_2)!=0){
		gameObjectCoin->extra = 0; //Disable animation
		synthPlayOne(true, &coinSound); //Plays coin sound
		saveGameState(); //Saves the game
	}
}

void processGameLogic(void){
	//The logic of the game goes here (e.g. spawning items, collision detection and handling, etc.)

	//Prevent the coin getting out of the bound of the screen
	if(gameObjectCoin->x<0)
		gameObjectCoin->x = 0;
	else if(gameObjectCoin->x>GRAPHIC_WIDTH-COIN_WIDTH)
		gameObjectCoin->x = GRAPHIC_WIDTH-COIN_WIDTH;

	if(gameObjectCoin->y<0)
		gameObjectCoin->y = 0;
	else if(gameObjectCoin->y>GRAPHIC_HEIGHT*8-COIN_HEIGHT)
		gameObjectCoin->y = GRAPHIC_HEIGHT*8-COIN_HEIGHT;
	
	//Replenishes movement point every frame
	movementPoints++;
	//Count the current frame for animating the coin
	frameCounter++;
}

void renderGameObjects(void){
	//Render game objects
	for(size_t i=0; i<GAME_OBJECT_NUM; i++){
		switch(gameObjectArray[i].type){
			case GAME_OBJECT_COIN:
				if(gameObjectArray[i].extra){ //Animation enabled
					graphicDrawImage(&graphicImageCoin[frameCounter%8], gameObjectArray[i].x, gameObjectArray[i].y, GRAPHIC_MODE_FOREGROUND_OR);
				}else{ //Animation disabled
					graphicDrawImage(&graphicImageStaticCoin, gameObjectArray[i].x, gameObjectArray[i].y, GRAPHIC_MODE_FOREGROUND_OR);
				}
			break;
			//case GAME_OBJECT_APPLE:
			//...
			//break;
			//case GAME_OBJECT_ORANGE:
			//...
			//break;
			default:
			break;
		}
	}
	
	//Render movement point text
	char buf[32];
	sprintf(buf, "MP: %lu", movementPoints);
	graphicDrawText(buf, 0, MOVEMENT_POINT_TEXT_X, MOVEMENT_POINT_TEXT_Y, GRAPHIC_PIXEL_WIDTH, 8, GRAPHIC_MODE_FOREGROUND_AND_NOT|GRAPHIC_MODE_BACKGROUND_OR);
}

int main(void){
	//Game initialization
	loadGraphic();

	//Game Objects Initialization
	gameInit();

	//Initialize display
	graphicSetDrawBuffer(lcdDmaBuffer);

	unsigned int previousTick = systemGetTick();
	while(true){
		//wait for completion of the previous rendering
		while(!graphicIsDisplayReady()){} //Do not start drawing until the rendering is completed
		graphicClearDisplay(false);

		//Loop BGM. Play again it if it has stopped
		if((synthGetStatus()&0x01) == 0){
			synthPlayCommand(false, bgm);
		}

		//Perform these functions every frame
		handleKeyInput();
		processGameLogic();
		renderGameObjects();
		graphicDisplay(lcdDmaBuffer);


		//Frame limiting
		unsigned int timeElapsed = (systemGetTick()-previousTick);
		if(timeElapsed<FRAME_DELAY_MS)
			systemSleep(FRAME_DELAY_MS-timeElapsed, false);
		previousTick = systemGetTick();
	}
	return 0;
}
