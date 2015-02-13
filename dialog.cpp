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

bool quit=false,M_end=false;
SDL_Event e;
Mix_Music *BGM;


SDL_Surface* say(const char *name,string *str,int x,int y,TTF_Font *font,SDL_Renderer *renderer,SDL_Surface *DiaStyle,SDL_Surface *Background);
void cls(SDL_Renderer *ren);
bool init(int index);
int DrawDia(void* p);
void Effect_Fade(SDL_Renderer *renderer2,SDL_Surface *bksrf);
void CreateNameBox(const char *n,SDL_Surface *total,TTF_Font *f);
void CreateBackground(SDL_Surface *back,SDL_Surface *Diastl,SDL_Surface *total);
void CreateNPC(char NPC[],SDL_Surface *total,int x,int y);
void Thread_Wait(bool *flag);
void UpdateMouth(SDL_Renderer* renderer,SDL_Surface* mouth,int i);
void SetRec(SDL_Rect* rect,int x=0,int y=0,int w=0,int h=0);
void SetRec2(SDL_Rect* rect,int x,int y);
int main(int argc, const char * argv[])
{
    
    SDL_Window *window;
    SDL_Surface *icon;
    FArgs FA;
    bool Wait_or_not=true;
    SDL_Thread *tID;
    
    
    if(!init(SDL_ALL))
        cout<<"init failed.";
    
    window=SDL_CreateWindow("Steins;Gate", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED , 1024, 600, SDL_WINDOW_SHOWN);
    icon=IMG_Load("./lib/icon.png");
    SDL_SetWindowIcon(window, icon);
    BGM=Mix_LoadMUS("./lib/BGM/bgm05.ogg");
    
    FA.SetEnv(&Wait_or_not,window);
    Mix_PlayMusic(BGM,-1);
    
    tID=SDL_CreateThread(DrawDia, "DrawAll" , &FA);
    
    while(!quit)
    {
        while(SDL_PollEvent(&e))
            {
                if (e.type==SDL_QUIT){
                
                    M_end=true;
                    SDL_WaitThread(tID, NULL);
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
                if (e.type==SDL_KEYDOWN) {
                    switch (e.key.keysym.sym)
                    {
                        case 0x0d:
                            FA.Permit_to_go();
                            break;
                    }
                }
                SDL_Delay(50);
            }
        
            SDL_Delay(50);
        }
        SDL_Delay(50);
    }
    


SDL_Surface* say(const char *name,string *str,int x,int y,TTF_Font *font,SDL_Renderer *renderer,SDL_Surface *DiaStyle,SDL_Surface *Background)
{

    SDL_Texture *mTexture=NULL;
    SDL_Color color_text={255,255,255,254};
    SDL_Color color_shade={0,0,0,50};
    SDL_Surface *character=NULL;
    SDL_Surface *TotalBack=NULL;
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
    
    unsigned long int len=str->length();
    char sen[255];
    char ch[3]={' ',' ','\0'};
    //Uint16 t[]={0x4f60,0x597d,0};

    mouth=IMG_Load("./lib/cg/crs_bsb06trm.png");
    SDL_RenderClear(renderer);
    strcpy(sen, str->c_str());
    TotalBack=SDL_CreateRGBSurface(0, 1024, 600, 32,0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    
    
    CreateBackground(Background, DiaStyle, TotalBack);
    SetRec2(&rec,x,y);
    SetRec2(&rec_shade,x+2,y+2);
    SetRec2(&pos_render,0,0);
    CreateNameBox(name,TotalBack,font);
    for (int i=0; i<len-2; i++) {
            SDL_RenderClear(renderer);
        for (int j=i; j<i+3; j++) {
            if (M_end){
                SDL_FreeSurface(character);
                SDL_FreeSurface(Shade);
                SDL_DestroyTexture(mTexture);
                SDL_RenderPresent(renderer);
                SDL_FreeSurface(TotalBack);
                SDL_FreeSurface(mouth);
                return NULL;
            }
            ch[1]=sen[j];
            character=TTF_RenderUTF8_Solid(font,&ch[1],color_text);
            Shade=TTF_RenderUTF8_Solid(font,&ch[1],color_shade);
            SDL_SetSurfaceBlendMode(character, SDL_BLENDMODE_BLEND);
            SDL_SetSurfaceBlendMode(Shade, SDL_BLENDMODE_BLEND);
            if (i==len-3)
                al=252;
            else
                al=(3-j+i)*84;
            SDL_SetSurfaceAlphaMod(character,al);
            SDL_SetSurfaceAlphaMod(Shade,al);
            if (character->w+rec.x>884) {
                dx=884-rec.x;
                pre2=pre1;
                SetRec2(&rec,x,rec.y+character->h);
                SetRec2(&rec_shade,rec.x+2,rec.y+2);
            }
        
           if ((rec.x==x)&&(ch[1]==' ')) {
                rec.x-=11;
                rec_shade.x=(rec.x+2);
            }
            
            
            if (rec.x<x) {
                SetRec(&temp,884-dx-pre2+2,temp.y-character->h+2,pre2,character->h);
                SDL_BlitSurface(Shade,NULL,TotalBack,&temp);
                SetRec2(&temp, temp.x-2, temp.y-2);
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
        
        mTexture=SDL_CreateTextureFromSurface(renderer,TotalBack);
        
        pos_render.w=TotalBack->w;
        pos_render.h=TotalBack->h;
        
        SDL_RenderCopy(renderer, mTexture, NULL,&pos_render);
        if (i==len-3||ch[1]=='.'||ch[1]==','||ch[1]=='!'||ch[1]=='?')
        {
            UpdateMouth(renderer,mouth, 0);
            SDL_DestroyTexture(mTexture);
            SDL_Delay(80);
            SDL_RenderPresent(renderer);
            SDL_Delay(150);
        }else{
            UpdateMouth(renderer, mouth, i);
            SDL_DestroyTexture(mTexture);
            SDL_Delay(80);
            SDL_RenderPresent(renderer);
        }
        
    }
    
    return TotalBack;
    SDL_FreeSurface(TotalBack);
};


void cls(SDL_Renderer *ren)
{
    SDL_SetRenderDrawColor(ren, 255, 255, 255,0);
    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);
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


int DrawDia(void* p)
{
    
    FArgs *m=static_cast<FArgs*>(p);
    SDL_Surface *stl;
    SDL_Surface *background;
    string str[2]={"This is a Test.jbasdihasd.asdasd.asdasdasfasc................acxcwedfsaxcac.","Linsten,the game will begin and we don't have time to waste!"};
    TTF_Font *font_EN;
    SDL_Renderer *renderer;
    SDL_Window *win2=static_cast<SDL_Window*>(m->data[1]);
    bool *P_wait=static_cast<bool*>(m->data[0]);

    font_EN=TTF_OpenFont("./lib/font/raleway.ttf", 25);
    TTF_SetFontStyle(font_EN, TTF_STYLE_BOLD);
    stl=IMG_Load("./lib/others/box00.png");
    background=IMG_Load("./lib/bg/bg01a.jpg");
    renderer=SDL_CreateRenderer(win2, 1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
    
    for (short i=0; i<=1; i++) {
    say("ABC", &str[i],145,450, font_EN, renderer, stl, background);
        Thread_Wait(P_wait);
        if (M_end)
            break;
    }
    TTF_CloseFont(font_EN);
    SDL_FreeSurface(stl);
    SDL_FreeSurface(background);
    return 0;
};

void Effect_Fade(SDL_Renderer *renderer2,SDL_Surface *bksrf)
{
    SDL_Texture *b;
    SDL_Rect rec;
    SDL_SetRenderDrawColor(renderer2, 0, 0, 0, 0);
    
    b=SDL_CreateTextureFromSurface(renderer2, bksrf);
    SDL_SetTextureBlendMode(b, SDL_BLENDMODE_BLEND);
    
    rec.x=0;
    rec.y=0;
    rec.w=bksrf->w;
    rec.h=bksrf->h;
    
    for (Uint8 i=255; i>0; i--) {
        
        SDL_RenderClear(renderer2);
        
        SDL_SetTextureAlphaMod(b,i);
        SDL_RenderCopy(renderer2, b, NULL,&rec);
        SDL_RenderPresent(renderer2);
        SDL_Delay(5);
    }
};


void CreateNameBox(const char *n,SDL_Surface *total,TTF_Font *f)
{
    SDL_Surface *namebox[2];
    SDL_Surface *nm;
    SDL_Color cl={255,255,255,254};
    SDL_Rect rect;
    rect.x=269;
    rect.y=588;
    
    namebox[0]=IMG_Load("./lib/others/name01.png");
    namebox[1]=IMG_Load("./lib/others/name02.png");
    nm=TTF_RenderUTF8_Solid(f, n, cl);
    SDL_BlitSurface(namebox[0],NULL,total,&rect);
    rect.x=561;
    rect.y=588;
    SDL_BlitSurface(namebox[1],NULL,total,&rect);
    rect.x=463+((100-(nm->w))/2);
    rect.y=600-nm->h+5;
    SDL_BlitSurface(nm,NULL,total,&rect);
    SDL_FreeSurface(namebox[0]);
    SDL_FreeSurface(namebox[1]);
    SDL_FreeSurface(nm);
};

void CreateBackground(SDL_Surface *back,SDL_Surface *Diastl,SDL_Surface *total)
{
    SDL_Rect rect_back;
    char NPCDir[]="./lib/cg/crs_bsb06a.png";
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
    NPCSur=IMG_Load(NPC);
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

void FArgs::SetEnv(bool *com,SDL_Window* win){
    data[0]=com;
    data[1]=win;
}
void FArgs::Permit_to_go()
{
    *static_cast<bool*>(data[0])=false;
}

void UpdateMouth(SDL_Renderer* renderer,SDL_Surface* mouth,int i)
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
    if (i==0)
        SDL_RenderCopy(renderer, Tex_mouth, &mouth_rect[0], &blit_rect[0]);
    else
        SDL_RenderCopy(renderer, Tex_mouth, &mouth_rect[i%4+1], &blit_rect[i%4+1]);
    SDL_DestroyTexture(Tex_mouth);
}
void SetRec(SDL_Rect* rect,int x,int y,int w,int h)
{
    rect->x=x;
    rect->y=y;
    rect->w=w;
    rect->h=h;
}
void SetRec2(SDL_Rect* rect,int x,int y)
{
    rect->x=x;
    rect->y=y;
}
