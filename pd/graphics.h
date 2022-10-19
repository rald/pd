#ifndef GRAPHICS_H
#define GRAPHICS_H



#include <SDL/SDL.h>



Uint32 Graphics_GetPixel( SDL_Surface *srf, int x, int y );

void Graphics_SetPixel( SDL_Surface *srf, int x, int y, Uint32 c );

void Graphics_DrawLine(SDL_Surface *srf, int x0, int y0, int x1, int y1, Uint32 c);

void Graphics_DrawRect(SDL_Surface *srf,int x,int y,int w,int h,Uint32 c);

void Graphics_FillRect(SDL_Surface *srf,int x,int y,int w,int h,Uint32 c);


extern int sw,sh;


#ifdef GRAPHICS_IMPLEMENTATION



Uint32 Graphics_GetPixel( SDL_Surface *srf, int x, int y ) {
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32 *)srf->pixels;
	//Get the requested pixel
	return pixels[ ( y * srf->w ) + x ];
}



void Graphics_SetPixel( SDL_Surface *srf, int x, int y, Uint32 c ) {
	if(x>=0 && x<sw && y>=0 && y<sh) {
		//Convert the pixels to 32 bit
		Uint32 *pixels = (Uint32 *)srf->pixels;    
		//Set the pixel
		pixels[ ( y * srf->w ) + x ] = c;
	}
}



void Graphics_DrawLine(SDL_Surface *srf, int x0, int y0, int x1, int y1, Uint32 c)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;

	for (;;)
	{
		Graphics_SetPixel(srf, x0, y0, c);
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



void Graphics_DrawRect(SDL_Surface *srf,int x,int y,int w,int h,Uint32 c) {
	for(int i=0;i<w;i++) {
		Graphics_SetPixel(srf,x+i,y,c);
		Graphics_SetPixel(srf,x+i,y+h-1,c);
	}
	for(int j=0;j<h;j++) {
		Graphics_SetPixel(srf,x,y+j,c);
		Graphics_SetPixel(srf,x+w-1,y+j,c);
	}
}

void Graphics_FillRect(SDL_Surface *srf,int x,int y,int w,int h,Uint32 c) {
	for(int j=0;j<h;j++) {
		for(int i=0;i<w;i++) {
			Graphics_SetPixel(srf,x+i,y+j,c);
		}
	}
}

#endif /* GRAPHICS_IMPLEMENTATION */



#endif /* GRAPHICS_H */



