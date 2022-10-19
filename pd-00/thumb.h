#ifndef THUMB_H
#define THUMB_H



typedef struct Thumb Thumb;

struct Thumb {
	int x,y;
	int s;
	Canvas *cvs;
};



#ifdef THUMB_IMPLEMENTATION



Thumb *Thumb_New(Canvas *cvs,int x,int y,int s) {

}



#endif /* THUMB_IMPLEMENTATION */



#endif /* THUMB_H */
