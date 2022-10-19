#ifndef GRID_H
#define GRID_H



#include "GameState.h"
#include "color.h"
#define CANVAS_IMPLEMENTATION
#include "canvas.h"


typedef struct Grid Grid;

struct Grid {
	Canvas *cvs;

	int x,y;
	int w,h;
	int s;
	Color c;

	int nx,ny;

	int dx,dy;
	bool isDrag;

	bool isGrid;

	bool isDraw;
	int px,py;
};



Grid *Grid_New(Canvas *cvs,int x,int y,int w,int h,int s,Color c);
void Grid_Draw(SDL_Surface *srf,Grid *grid,int f);
void Grid_HandleEvents(Grid *g,Mouse *m,int f);



extern int sw,sh;

extern int drag;

extern GameState gameState; 


#ifdef GRID_IMPLEMENTATION



Grid *Grid_New(Canvas *cvs,int x,int y,int w,int h,int s,Color c) {
	Grid *grid=malloc(sizeof(*grid));
	if(grid) {
		grid->cvs=cvs;
	
		grid->x=x;
		grid->y=y;
		grid->w=w;
		grid->h=h;
		grid->s=s;
		grid->c=c;
		grid->isDrag=false;

		grid->nx=0;
		grid->ny=0;

		grid->isGrid=true;
		
		grid->isDraw=false;
	}
	return grid;
}



void Grid_Draw(SDL_Surface *srf,Grid *g,int f) {

	int x0=max(0,g->x);
	int y0=max(0,g->y);
	int x1=min(g->x+g->w*g->s,sw);
	int y1=min(g->y+g->h*g->s,sh);

	int t;

	if(x0>x1) { t=x0; x0=x1; x1=t; }
	if(y0>y1) { t=y0; y0=y1; y1=t; }

	for(int j=0;j<g->cvs->h;j++) {
		for(int i=0;i<g->cvs->w;i++) {
			int k=g->cvs->p[f*g->cvs->w*g->cvs->h+j*g->cvs->w+i];
			Graphics_FillRect(srf,i*g->s+g->x,j*g->s+g->y,g->s,g->s,SDL_MapRGBA(srf->format,g->cvs->pl->c[k].r,g->cvs->pl->c[k].g,g->cvs->pl->c[k].b,g->cvs->pl->c[k].a));
		}
	}



	if(g->isGrid) {

		for(int i=0;i<=g->w;i++) {
			Graphics_DrawLine(srf,g->x+i*g->s,y0,g->x+i*g->s,y1,SDL_MapRGBA(srf->format,g->c.r,g->c.g,g->c.b,g->c.a));
		}

		for(int j=0;j<=g->h;j++) {
			Graphics_DrawLine(srf,x0,g->y+j*g->s,x1,g->y+j*g->s,SDL_MapRGBA(srf->format,g->c.r,g->c.g,g->c.b,g->c.a));
		}

	}

}



void Grid_HandleEvents(Grid *g,Mouse *m,int f) {



	if(inrect(m->x,m->y,g->x,g->y,g->w*g->s,g->h*g->s)) {
		if(gameState==GAMESTATE_DEFAULT) {
			gameState=GAMESTATE_GRID;
		}
	} else if(gameState==GAMESTATE_GRID) {
		gameState=GAMESTATE_DEFAULT;
	}



	if(gameState==GAMESTATE_GRID) {

		if(!g->isDraw) {
			if(m->isDown && m->b==1 && inrect(m->x,m->y,g->x,g->y,g->w*g->s,g->h*g->s)) {
				g->isDraw=true;
				g->px=(m->x-g->x)/g->s;
				g->py=(m->y-g->y)/g->s;
			}	
		} else {
			if(m->isDown && m->b==1) {
				if(inrect(m->x,m->y,g->x,g->y,g->w*g->s,g->h*g->s)) {
					int x=(m->x-g->x)/g->s;
					int y=(m->y-g->y)/g->s;
					Canvas_DrawLine(g->cvs,f,g->px,g->py,x,y,g->cvs->pl->cc);
					g->px=x; 
					g->py=y;
				}
			}	else {
				g->isDraw=false;
			}	
		}

	}



	if(drag==0) {
		if(m->isDown && m->b==3 && inrect(m->x,m->y,g->x,g->y,g->w*g->s,g->h*g->s)) {
			g->dx=g->x-m->x;
			g->dy=g->y-m->y;
			drag=2;
		}		
	} else if(drag==2) {
		if(m->isDown && m->b==3) {
			g->x=m->x+g->dx;
			g->y=m->y+g->dy;

			if(g->x<-g->w*g->s+g->s) g->x=-g->w*g->s+g->s;
			if(g->y<-g->h*g->s+g->s) g->y=-g->h*g->s+g->s;
			if(g->x>sw-g->s) g->x=sw-g->s;
			if(g->y>sh-g->s) g->y=sh-g->s;
			
		} else {
			drag=0;
		}
	}


	
}



#endif /* GRID_IMPLEMEMTATION */



#endif /* GRID_H  */


