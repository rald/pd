#include <SDL/SDL.h>



#include <stdbool.h>



#include "GameState.h"
#include "color.h"
#include "sweetie.h"
#define PALETTE_IMPLEMENTATION
#include "palette.h"
#define MOUSE_IMPLEMENTATION
#include "mouse.h"



#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_BPP 32



int sw=SCREEN_WIDTH,sh=SCREEN_HEIGHT;



GameState gameState=GAMESTATE_DEFAULT;

bool quit=false;

SDL_Surface *screen=NULL;
SDL_Event event;

bool draw=false;

int drag=0;

int px,py;



int main() {



	SDL_Init(SDL_INIT_VIDEO);



	screen=SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE);



	Mouse *mouse=Mouse_New();

	Palette *palette=Palette_New(0,0,16,12,sweetie_num_colors,sweetie);



	while(!quit) {

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				quit=true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse->isDown=true;
				mouse->x=event.button.x;
				mouse->y=event.button.y;
				mouse->b=event.button.button;
				break;				
			case SDL_MOUSEMOTION:
				mouse->x=event.button.x;
				mouse->y=event.button.y;
				break;				
			case SDL_MOUSEBUTTONUP:
				mouse->isDown=false;
				mouse->x=event.button.x;
				mouse->y=event.button.y;
				mouse->b=event.button.button;
				break;				
			}
		}



		SDL_FillRect(screen,&(SDL_Rect){0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1},SDL_MapRGBA(screen->format,0x00,0x00,0x00,0xFF));



		if(!draw) {
			if(mouse->isDown && mouse->b==1) {				draw=true;
				px=mouse->x;
				py=mouse->y;
			}
		} else {
			if(mouse->isDown && mouse->b==1) {
				Graphics_DrawLine(screen,px,py,mouse->x,mouse->y,SDL_MapRGBA(screen->format,0xFF,0xFF,0xFF,0xFF));
				px=mouse->x;
				py=mouse->y;
			} else {
				draw=false;
			}
		}



		Palette_Draw(screen,palette);

		Palette_HandleEvents(palette,mouse);

		
		Graphics_DrawRect(screen,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_MapRGBA(screen->format,0xFF,0xFF,0xFF,0xFF));



		SDL_Flip(screen);

		SDL_Delay(1000/60);
		
	}



	SDL_Quit();

	return 0;
} 


