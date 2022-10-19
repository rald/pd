#ifndef PALETTE_H
#define PALETTE_H



#include "GameState.h"



#define UTIL_IMPLEMENTATION
#include "util.h"

#define MOUSE_IMPLEMENTATION
#include "mouse.h"



typedef struct Palette Palette;

struct Palette {
	int x,y;
	int w,h;
	int s;

	int nx,ny;
	
	int cc;

	int nc;
	SDL_Color *c;

	int dx,dy;
	bool isDrag;
};



Palette *Palette_New(int x,int y,int s,int cc,int nc,SDL_Color *c);
void Palette_Draw(SDL_Renderer *r,Palette *p);void Palette_HandleEvents(Palette *p,Mouse *m);


extern int sw,sh;

extern int drag;

extern GameState gameState; 

 

#ifdef PALETTE_IMPLEMENTATION



Palette *Palette_New(int x,int y,int s,int cc,int nc,SDL_Color *c) {
	Palette *palette=malloc(sizeof(*palette));
	if(palette) {

		palette->x=x;
		palette->y=y;

		palette->nc=nc;
		palette->c=c;

		palette->s=s;

		palette->w=s*8;
		palette->h=s*2;

		palette->isDrag=false;

		palette->nx=0;
		palette->ny=0;

		palette->cc=cc;
	}
	return palette;
}


void Palette_Draw(SDL_Renderer *r,Palette *p) {
	SDL_Rect rect;		
	rect=(SDL_Rect){p->x,p->y,p->w,p->h};

	SDL_SetRenderDrawColor(r,0x00,0x00,0x00,0xFF);
	SDL_RenderFillRect(r,&rect);

	for(int k=0;k<p->nc;k++) {
		int i=k%8;
		int j=k/8;

		SDL_SetRenderDrawColor(r,p->c[k].r,p->c[k].g,p->c[k].b,p->c[k].a);

		if(k==p->cc) {
			rect=(SDL_Rect){p->x+i*p->s,p->y+j*p->s,p->s,p->s};
		} else {
			rect=(SDL_Rect){p->x+i*p->s+2,p->y+j*p->s+2,p->s-4,p->s-4};
		}

		SDL_RenderFillRect(r,&rect);
	}
}



void Palette_HandleEvents(Palette *p,Mouse *m) {



	if(inrect(m->x,m->y,p->x,p->y,p->w,p->h)) {
			gameState=GAMESTATE_PALETTE;	
	} else if(gameState==GAMESTATE_PALETTE) {
		gameState=GAMESTATE_DEFAULT;		
	}	



	if(gameState==GAMESTATE_PALETTE) {
		if(m->isDown && m->b==1 && inrect(m->x,m->y,p->x,p->y,p->w,p->h)) {
			int x=(m->x-p->x)/p->s;
			int y=(m->y-p->y)/p->s;
			p->cc=y*8+x;
		}		
	}



	if(drag==0) {
		if(m->isDown && m->b==3 && inrect(m->x,m->y,p->x,p->y,p->w,p->h)) {
			p->dx=p->x-m->x;
			p->dy=p->y-m->y;
			drag=1;
		}		
	} else if(drag==1) {
		if(m->isDown && m->b==3) {
			p->x=m->x+p->dx;
			p->y=m->y+p->dy;

			if(p->x<-p->w+p->s) p->x=-p->w+p->s;
			if(p->y<-p->h+p->s) p->y=-p->h+p->s;
			if(p->x>sw-p->s) p->x=sw-p->s;
			if(p->y>sh-p->s) p->y=sh-p->s;
		} else {
			drag=0;
		}
	}



}


#endif /* PALETTE_IMPLEMENTATION */



#endif /* PALETTE_H */


