#ifndef UTIL_H
#define UTIL_H



#define min(x,y) (x)<(y)?(x):(y)
#define max(x,y) (x)>(y)?(x):(y)



bool inrect(int x, int y, int rx, int ry, int rw, int rh);



#ifdef UTIL_IMPLEMENTATION 



bool inrect(int x, int y, int rx, int ry, int rw, int rh)
{
	return x >= rx && x < rx + rw && y >= ry && y < ry + rh;
}



#endif /* UTIL_IMPLEMENTATION */



#endif /* UTIL_H */


