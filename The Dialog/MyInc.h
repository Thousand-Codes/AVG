//
//  stafx.h
//  GAME-02
//
//  Created by Kevin Neumann on 14-12-22.
//  Copyright (c) 2014年 Kevin Neumann. All rights reserved.
//

#ifndef GAME_02_stafx_h
#define GAME_02_stafx_h
#include <iostream>
#include <string>
#include "SDL2/SDL.h"
#include "SDL2_ttf/SDL_ttf.h"
#include "SDL2_image/SDL_image.h"
#include "SDL2_mixer/SDL_mixer.h"
#include "SDL2/SDL_thread.h"

class FArgs{
public:
    void* data[10];
    void SetEnv(bool *com,SDL_Window* win);
    void Permit_to_go();
};

#endif

