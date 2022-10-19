#ifndef CANVAS_H
#define CANVAS_H



#define PALETTE_IMPLEMENTATION
#include "palette.h"



typedef struct Canvas Canvas;

struct Canvas {
	int w,h,f,t;
	int *p;
	Palette *pl;
};



Canvas *Canvas_New(int w,int h,int f,int t,Palette *pl);

bool Canvas_SetPixel(Canvas *cvs,int f,int x,int y,int c);

bool Canvas_GetPixel(Canvas *cvs,int f,int x,int y,int *c);

void Canvas_DrawLine(Canvas *cvs, int f, int x0, int y0, int x1, int y1, int c);



#ifdef CANVAS_IMPLEMENTATION



Canvas *Canvas_New(int w,int h,int f,int t,Palette *pl) {
	Canvas *cvs=malloc(sizeof(*cvs));
	if(cvs) {
		cvs->w=w;
		cvs->h=h;
		cvs->f=f;
		cvs->t=t;
		cvs->pl=pl;
		cvs->p=malloc(sizeof(*cvs->p)*(w*h*f));
		for(int i=0;i<w*h*f;i++) cvs->p[i]=12;
	}
	return cvs;
}



bool Canvas_SetPixel(Canvas *cvs,int f,int x,int y,int c) {
	if(x>=0 && x<cvs->w && y>=0 && y<cvs->h && f>=0 && f<cvs->f) {
		cvs->p[f*cvs->w*cvs->h+y*cvs->w+x]=c;
		return true;
	} else {
		return false;
	}
}



bool Canvas_GetPixel(Canvas *cvs,int f,int x,int y,int *c) {
	if(x>=0 && x<cvs->w && y>=0 && y<cvs->h && f>=0 && f<cvs->f) {
		*c=cvs->p[f*cvs->w*cvs->h+y*cvs->w+x];
		return true;
	} else {
		return false;
	}
}



void Canvas_DrawLine(Canvas *cvs, int f, int x0, int y0, int x1, int y1, int c)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;

	for (;;)
	{
		Canvas_SetPixel(cvs, f, x0, y0, c);
		if (x0 == x1 && y0 == y1)
			break;
		e2 = err;
		if (e2 > -dx)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy)
		{
			err += dx;
			y0 += sy;
		}
	}
}



void Canvas_Draw(SDL_Renderer *r,Canvas *cvs,int f,int x,int y,int s) {
	for(int j=0;j<cvs->h;j++) {
		for(int i=0;i<cvs->w;i++) {
			int k=cvs->p[f*cvs->w*cvs->h+j*cvs->w+i];
			SDL_SetRenderDrawColor(r,cvs->pl->c[k].r,cvs->pl->c[k].g,cvs->pl->c[k].b,cvs->pl->c[k].a);

			SDL_RenderFillRect(r,&(SDL_Rect){i*s+x,j*s+x,s,s});
		}
	}
}



#endif /* CANVAS_IMPLEMENTATION */



#endif /* CANVAS_H */


