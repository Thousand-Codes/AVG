//
//  stafx.h
//  GAME-02
//
//  Created by Kevin Neumann on 14-12-22.
//  Copyright (c) 2014年 Kevin Neumann. All rights reserved.
//

#ifndef MYINC_H
#define MYINC_H
#define MAX 2
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_thread.h>
#include <SDL_timer.h>
#include "GAME_Dia\convert.h"
int FRESH=45;
class scene{

};
class BufferUnit{
public:
	SDL_Surface *SRF;
	bool Available;
	BufferUnit(){Available=false; SRF=NULL;}
};
#endif

