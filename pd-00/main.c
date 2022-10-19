#include <SDL2/SDL.h>



#include <stdio.h>
#include <stdbool.h>
#include <math.h>



#include "GameState.h"
#include "sweetie.h"



#define PALETTE_IMPLEMENTATION
#include "palette.h"

#define UTIL_IMPLEMENTATION
#include "util.h"

#define MOUSE_IMPLEMENTATION
#include "mouse.h"

#define CANVAS_IMPLEMENTATION
#include "canvas.h"

#define GRID_IMPLEMENTATION
#include "grid.h"



#define GAME_TITLE "Pixel Dancer"

#define DEFAULT_FILE "default.cvs"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240



int sw=SCREEN_WIDTH,sh=SCREEN_HEIGHT;

char *hex = "0123456789ABCDEF";



GameState gameState = GAMESTATE_DEFAULT;



SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Event event;

int drag=0;

bool quit=false;

int mouseX,mouseY,mouseButton;
bool mouseDown=false;

int f=0;



SDL_Color contrastColor(SDL_Color c) {
	double luma = ((0.299 * c.r) + (0.587 * c.g) + (0.114 * c.b)) / 255;

	return luma > 0.5 ? (SDL_Color){0x00, 0x00, 0x00, 0xFF} : (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF};
}



int main(void) {



	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sw, sh, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);

	SDL_GetWindowSize(window, &sw, &sh);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, sw, sh);

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);



	Mouse *mouse=Mouse_New(); 

	Palette *palette=Palette_New(0,0,16,12,16,sweetie);


	Canvas *canvas=Canvas_New(16,16,16,-1,palette);


	Grid *grid=Grid_New(canvas,0,0,16,16,8,(SDL_Color){0x00,0x00,0x00,0xFF});



	while (!quit) {

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse->isDown = true;
				mouse->x = event.button.x;
				mouse->y = event.button.y;
				mouse->b = event.button.button;
				break;
			case SDL_MOUSEMOTION:
				mouse->x = event.button.x;
				mouse->y = event.button.y;
				break;
			case SDL_MOUSEBUTTONUP:
				mouse->isDown = false;
				mouse->x = event.button.x;
				mouse->y = event.button.y;
				mouse->b = event.button.button;
				break;
			}
		}



		SDL_SetRenderTarget(renderer, texture);



		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(renderer, &(SDL_Rect){0, 0, sw, sh});



		Grid_Draw(renderer,grid,f);
		Palette_Draw(renderer,palette);

		Canvas_Draw(renderer,canvas,0,0,0,2);

		Palette_HandleEvents(palette,mouse);
		Grid_HandleEvents(grid,mouse,f);


		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDrawRect(renderer, &(SDL_Rect){0, 0, sw, sh});



		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);



		SDL_Delay(1000 / 60);
	}



	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);

	window = NULL;
	renderer = NULL;
	texture = NULL;

	SDL_Quit();



	return 0;
}


