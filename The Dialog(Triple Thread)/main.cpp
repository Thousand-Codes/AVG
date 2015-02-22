//
//  main.cpp
//  GAME-02
//
//  Created by Kevin Neumann on 6/17/14.
//  Copyright (c) 2014 Kevin Neumann. All rights reserved.
//


#include "MyInc.h"

#ifndef SDL_VIDEO
#define SDL_VIDEO 1
#endif

#ifndef SDL_TTF
#define SDL_TTF 2
#endif

#ifndef SDL_ALL
#define SDL_ALL 3
#endif

using std::cout;
using std::string;

bool quit=false,M_end=false,P_wait=true,red=true;
SDL_Event e;
SDL_Window *window;
SDL_Surface *icon;
SDL_Renderer *renderer;
SDL_mutex *BufferLock,*winmtx;
SDL_cond *FillBuf,*SetBuf,*redraw;

TTF_Font *font;
Mix_Music *BGM;
int StackPointer=0;
BufferUnit SRFBuff[200];

int say(const char *name,string *str,int x,int y,SDL_Surface *DiaStyle,SDL_Surface *Background);
void cls();
bool init(int index);
int DrawScr(void* p);
int SRFMk(void *p);
void Effect_Fade(SDL_Surface *bksrf);
void CreateNameBox(const char *n,SDL_Surface *total);
void CreateBackground(SDL_Surface *back,SDL_Surface *Diastl,SDL_Surface *total);
void CreateNPC(char NPC[],SDL_Surface *total,int x,int y);
void Thread_Wait(bool *flag);
void UpdateMouth(SDL_Surface* mouth,int i);
void SetRec(SDL_Rect* rect,int x=0,int y=0,int w=0,int h=0);
int main(int argc, char * argv[])
{
    
    
    SDL_Thread *Draw;
	SDL_Thread *compute;
    if(!init(SDL_ALL))
        cout<<"init failed.";
	BufferLock=SDL_CreateMutex();
	winmtx=SDL_CreateMutex();
	FillBuf=SDL_CreateCond();
	SetBuf=SDL_CreateCond();
	redraw=SDL_CreateCond();
	//SDL_CondSignal(FillBuf);
	compute=SDL_CreateThread(SRFMk,"SurfacePro",NULL);
    window=SDL_CreateWindow("Steins;Gate", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED , 1024, 600, SDL_WINDOW_SHOWN);
    icon=IMG_Load("F:/VS2010PJ/GAME_Dia/Release/lib/i.png");
    SDL_SetWindowIcon(window, icon);
    BGM=Mix_LoadMUS("F:/VS2010PJ/GAME_Dia/Release/lib/BGM/bgm05.ogg");
    Mix_PlayMusic(BGM,-1);
    //SDL_ShowCursor(0);
	
    Draw=SDL_CreateThread(DrawScr, "DrawScreen" , NULL);
    while(!quit)
    {	
        while(SDL_PollEvent(&e))
            {
                if (e.type==SDL_QUIT){
                
                    M_end=true;
                    SDL_WaitThread(Draw, NULL);
                    Mix_HaltMusic();
                    Mix_FreeMusic(BGM);
                    BGM=NULL;
                    IMG_Quit();
                    TTF_Quit();
                    Mix_Quit();
                    SDL_FreeSurface(icon);
                    SDL_Quit();
                    return 0;
                    
				}
                if (e.type==SDL_KEYDOWN)
                    switch (e.key.keysym.sym)
                    {
                        case 0x0d:
                            StackPointer++;
                            break;
					}
				if(e.type==SDL_WINDOWEVENT)
					switch (e.window.event)
					{
						/*case SDL_WINDOWEVENT_SHOWN:
							Mix_ResumeMusic();
							red=true;
							SDL_CondSignal(redraw);
							break;*/
						case SDL_WINDOWEVENT_RESTORED:
							Mix_ResumeMusic();
							FRESH=45;
							red=true;
							SDL_CondSignal(redraw);
							break;
						/*case SDL_WINDOWEVENT_HIDDEN:
							Mix_PauseMusic();
							red=false;
							break;*/
						case SDL_WINDOWEVENT_MINIMIZED:
							Mix_PauseMusic();
							FRESH=100;
							red=false;
							break;
						case SDL_WINDOWEVENT_FOCUS_GAINED:
							if(Mix_PausedMusic())
								Mix_ResumeMusic();
							FRESH=45;
							red=true;
							SDL_CondSignal(redraw);
							break;
						case SDL_WINDOWEVENT_FOCUS_LOST:
							Mix_PauseMusic();
							FRESH=100;
							red=false;
							break;
					}
                SDL_Delay(20);
            }
        SDL_Delay(20);
    }
}
    


int say(const char *name,string *str,int x,int y,SDL_Surface *DiaStyle,SDL_Surface *Background)
{
    SDL_Color color_text={255,255,255,254};
    SDL_Color color_shade={0,0,0,50};
	SDL_Surface *TotalBack=SDL_CreateRGBSurface(0, 1024, 600, 32,0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    SDL_Surface *character=NULL;
    SDL_Surface *Shade=NULL;
    SDL_Surface *mouth=NULL;
    SDL_Rect pos_render;
    SDL_Rect rec_shade;
    SDL_Rect rec;
    SDL_Rect temp;
    int pre1=0;
    int pre2=0;
    int dx=0;
    int wide=0;
    int al=0;
    static int frame=0;
    unsigned long int len=str->length()/2;
    char sen[510];
	P_wait=true;
    Uint16 t[255];
	Uint16 ch;
    //mouth=IMG_Load("F:/VS2010PJ/GAME_Dia/Release/lib/cg/crs_bsb06trm.png");
    SDL_RenderClear(renderer);
	memset(t,0,sizeof t);
    strcpy(sen, str->c_str());
	convert(sen,t);
    CreateBackground(Background, DiaStyle, TotalBack);
    rec.x=x;
    rec.y=y;
    rec_shade.x=x+2;
    rec_shade.y=y+2;
    pos_render.x=0;
    pos_render.y=0;
    
    CreateNameBox(name,TotalBack);
    
    for (int i=0; i<len-2; i++) {
        SDL_RenderClear(renderer);
        if(!red)
		{
			SDL_LockMutex(winmtx);
			SDL_CondWait(redraw,winmtx);
			SDL_UnlockMutex(winmtx);
		}	
        for (int j=i; j<i+3; j++) {
            if (M_end){
                SDL_FreeSurface(character);
                SDL_FreeSurface(Shade);
                SDL_RenderPresent(renderer);
                SDL_FreeSurface(TotalBack);
                SDL_FreeSurface(mouth);
                return NULL;
            }
			ch=t[j];
            //character=TTF_RenderUTF8_Solid(font,&t,color_text);
            //Shade=TTF_RenderUTF8_Solid(font,&ch[1],color_shade);
			character=TTF_RenderUNICODE_Solid(font,&ch,color_text);
			Shade=TTF_RenderUNICODE_Solid(font,&ch,color_shade);
            SDL_SetSurfaceBlendMode(character, SDL_BLENDMODE_BLEND);
            SDL_SetSurfaceBlendMode(Shade, SDL_BLENDMODE_BLEND);
            
            
            if (i>=len-3)
                al=252;
            else
                al=(3-j+i)*84;
            
            SDL_SetSurfaceAlphaMod(character,al);
            SDL_SetSurfaceAlphaMod(Shade,al);
            
            if (character->w+rec.x>884) {
                
                dx=884-rec.x;
                pre2=pre1;
                
                
                rec.y+=character->h;
                rec.x=x;
                rec_shade.y=(rec.y+2);
                rec_shade.x=(rec.x+2);
            }
        
           if ((rec.x==x)&&(ch==0x0020)) {
                rec.x-=11;
                rec_shade.x=(rec.x+2);
            }
            
            
            if (rec.x<x) {
                
                temp.w=pre2;
                temp.h=character->h;
                temp.x=884-dx-pre2+2;
                temp.y-=character->h;
                temp.y+=2;
                
                SDL_BlitSurface(Shade,NULL,TotalBack,&temp);
                
                temp.x-=2;
                temp.y-=2;
                
                SDL_BlitSurface(character,NULL,TotalBack,&temp);
            }else{
                SDL_BlitSurface(Shade,NULL,TotalBack,&rec_shade);
                SDL_BlitSurface(character,NULL,TotalBack,&rec);
                
            }
            
            if (j>=i+1) {
                wide+=character->w;
            }
        
            pre1=character->w;
            temp.y=rec.y;
            rec.x+=(character->w);
            rec_shade.x=(rec.x+2);
            
            
            SDL_FreeSurface(character);
            SDL_FreeSurface(Shade);
        }
        
        
        rec.x-=wide;
        rec_shade.x=(rec.x+2);
        wide=0;
        
        //mTexture=SDL_CreateTextureFromSurface(renderer,TotalBack);
		//SDL_RenderCopy(renderer, mTexture,NULL,&pos_render);
		SetRec(&pos_render,0,0,Background->w,Background->h);
		//SDL_CondSignal(SetBuf);
		//cout<<"SRF\n";
		SDL_LockMutex(BufferLock);
		//SDL_CondWait(FillBuf,BufferLock);
		SRFBuff[frame].SRF=SDL_CreateRGBSurface(0, 1024, 600, 32,0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
		SDL_BlitSurface(TotalBack,NULL,SRFBuff[frame].SRF,&pos_render);
		SRFBuff[frame].Available=true;
		frame=(frame+1)%200;
		SDL_UnlockMutex(BufferLock);
		SDL_CondSignal(SetBuf);
		

		/*if (i==len-3)
            UpdateMouth(mouth, 0);
        else
            UpdateMouth(mouth, i);
			*/
       
        //SDL_DestroyTexture(mTexture);
       // SDL_Delay(53);
        //SDL_RenderPresent(renderer);
    }
	P_wait=false;
	SDL_FreeSurface(TotalBack);
    return 1;
};


void cls()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255,0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
};

bool init(int index)
{
    bool success=true;
    switch (index)
    {
        case 1:
            if(SDL_Init(SDL_INIT_EVERYTHING)<0||(!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)))
                    {
                        cout<<"Error1.";
                        success=false;
                    }
            break;
        case 2:
            if(TTF_Init()==-1||(!(IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG)&(IMG_INIT_JPG|IMG_INIT_PNG))))
                    {
                        cout<<"Error2.";
                        success=false;
                    }
            break;
        case 3:
            if((SDL_Init(SDL_INIT_EVERYTHING)<0)||(TTF_Init()==-1)||(!(IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG)&(IMG_INIT_JPG|IMG_INIT_PNG)))||(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)<0))
                    {
                        cout<<"Error3.";
                        success=false;
                    }
            break;
        default:
            cout<<"wrong index.";
            success=false;
            break;
    }
    return success;
};


int DrawScr(void* p)
{
	SDL_Texture* All=NULL;
	SDL_Rect rect;
	Uint32 OldTime,CurTime;
	int top;
	SetRec(&rect,0,0,1024,600);
	renderer=SDL_CreateRenderer(window,-1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
	while(!M_end)
	{
		
		OldTime=SDL_GetTicks();
		while(OldTime+FRESH>SDL_GetTicks())
		{
			SDL_Delay(8);
			//SDL_CondSignal(FillBuf);
			if (M_end)
				return 0;
		}
		/*if(!red)
		{
			SDL_LockMutex(winmtx);
			SDL_CondWait(redraw,winmtx);
			SDL_UnlockMutex(winmtx);
		}*/
		if(SRFBuff[top].SRF!=NULL)
			All=SDL_CreateTextureFromSurface(renderer,SRFBuff[top].SRF);
		if (M_end)
            break;
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer,All,NULL,&rect);
		SDL_RenderPresent(renderer);
		//cout<<"DRA\n";
		//SDL_CondSignal(FillBuf);

		if(SRFBuff[top+1].Available)
		{
			SDL_LockMutex(BufferLock);
			if(P_wait)
				SDL_CondWait(SetBuf,BufferLock);
			SRFBuff[top].Available=false;
			SDL_UnlockMutex(BufferLock);
			top=(top+1)%200;
		}//else
			//SDL_CondWait(SetBuf,BufferLock);
		//SDL_CondSignal(FillBuf);
	}
    TTF_CloseFont(font);
    return 0;
};

int SRFMk(void *p)
{
	SDL_Surface *stl;
    SDL_Surface *background;
	SDL_Texture *mTexture;
	int i=0;

    string str[]={"看好这可是最新的版本","期待这么久总算是到来了"};
    font=TTF_OpenFont("F:/VS2010PJ/GAME_Dia/Release/lib/font/font_CN.ttf", 30);
    //TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    stl=IMG_Load("F:/VS2010PJ/GAME_Dia/Release/lib/others/box00.png");
    background=IMG_Load("F:/VS2010PJ/GAME_Dia/Release/lib/bg/bg01a.jpg");
	
	while(!M_end)
	{
		if(StackPointer==i&&i<MAX)
		{
			say("ABC", &str[i],145,450,stl,background);
			SDL_CondSignal(SetBuf);
			i++;
		}
		SDL_Delay(10);
	}		
	return 0;
}
void Effect_Fade(SDL_Surface *bksrf)
{
    SDL_Texture *b;
    SDL_Rect rec;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    
    b=SDL_CreateTextureFromSurface(renderer, bksrf);
    SDL_SetTextureBlendMode(b, SDL_BLENDMODE_BLEND);
    
    rec.x=0;
    rec.y=0;
    rec.w=bksrf->w;
    rec.h=bksrf->h;
    
    for (Uint8 i=255; i>0; i--) {
        
        SDL_RenderClear(renderer);
        
        SDL_SetTextureAlphaMod(b,i);
        SDL_RenderCopy(renderer, b, NULL,&rec);
        SDL_RenderPresent(renderer);
        SDL_Delay(5);
    }
};


void CreateNameBox(const char *n,SDL_Surface *total)
{
    SDL_Surface *namebox[2];
    SDL_Surface *nm;
    SDL_Color cl={255,255,255,254};
    SDL_Rect rect;
    rect.x=269;
    rect.y=570;
    
    namebox[0]=IMG_Load("F:/VS2010PJ/GAME_Dia/Release/lib/others/name01.png");
    namebox[1]=IMG_Load("F:/VS2010PJ/GAME_Dia/Release/lib/others/name02.png");
    nm=TTF_RenderUTF8_Solid(font, n, cl);
    SDL_BlitSurface(namebox[0],NULL,total,&rect);
    rect.x=561;
    rect.y=570;
    SDL_BlitSurface(namebox[1],NULL,total,&rect);
    rect.x=463+((100-(nm->w))/2);
    rect.y=rect.y-nm->h+17;
    SDL_BlitSurface(nm,NULL,total,&rect);
    SDL_FreeSurface(namebox[0]);
    SDL_FreeSurface(namebox[1]);
    SDL_FreeSurface(nm);
};

void CreateBackground(SDL_Surface *back,SDL_Surface *Diastl,SDL_Surface *total)
{
    SDL_Rect rect_back;
    char NPCDir[]="F:/VS2010PJ/GAME_Dia/Release/lib/cg/crs_bsb06a.png";
    rect_back.x=0;
    rect_back.y=0;
    rect_back.w=1024;
    rect_back.h=600;
    
    SDL_BlitScaled(back,NULL,total,&rect_back);
    
    rect_back.x=0;
    rect_back.y=407;
    rect_back.w=1024;
    rect_back.h=193;
    CreateNPC(NPCDir, total,320,5);
    SDL_BlitScaled(Diastl,NULL,total,&rect_back);
};

void CreateNPC(char NPC[],SDL_Surface *total,int x,int y)
{
    SDL_Rect rect;
    SDL_Surface *NPCSur;
    NPCSur=IMG_Load("F:/VS2010PJ/GAME_Dia/Release/lib/cg/crs_bsb06a.png");
    rect.x=x;
    rect.y=y;
    rect.w=NPCSur->w;
    rect.h=NPCSur->h;
    SDL_BlitSurface(NPCSur,NULL,total,&rect);
    SDL_FreeSurface(NPCSur);
};

void Thread_Wait(bool *flag)
{
        while (*flag) {
            if (M_end)
                break;
            SDL_Delay(10);
            SDL_Delay(20);
            SDL_Delay(30);
            SDL_Delay(40);
        }
    *flag=true;
};

void UpdateMouth(SDL_Surface* mouth,int i)
{
    static bool f=true;
    SDL_Texture* Tex_mouth=NULL;
    static SDL_Rect mouth_rect[5];
    static SDL_Rect blit_rect[5];
    if (f) {
        SetRec(&mouth_rect[0]);
        SetRec(&mouth_rect[1],0,0,20,17);
        SetRec(&mouth_rect[2],0,17,20,17);
        SetRec(&mouth_rect[4]);
        SetRec(&mouth_rect[3],0,0,20,17);
        SetRec(&blit_rect[0]);
        SetRec(&blit_rect[1],494,150,20,17);
        SetRec(&blit_rect[2],494,150,20,17);
        SetRec(&blit_rect[4]);
        SetRec(&blit_rect[3],494,150,20,17);
        f=false;
    }
    Tex_mouth=SDL_CreateTextureFromSurface(renderer, mouth);
	//cout<<SDL_SetRenderTarget(renderer,texture);
    SDL_RenderCopy(renderer, Tex_mouth, &mouth_rect[i%6], &blit_rect[i%6]);
    SDL_DestroyTexture(Tex_mouth);
}
void SetRec(SDL_Rect* rect,int x,int y,int w,int h)
{
    rect->x=x;
    rect->y=y;
    rect->w=w;
    rect->h=h;
}