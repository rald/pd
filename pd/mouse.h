#ifndef MOUSE_H
#define MOUSE_H



typedef struct Mouse Mouse; 

struct Mouse {
	int x,y;
	int b;
	bool isDown;
};



#ifdef MOUSE_IMPLEMENTATION



Mouse *Mouse_New() {
	Mouse *mouse=malloc(sizeof(*mouse));
	if(mouse) {
		mouse->x=0;
		mouse->y=0;
		mouse->b=0;
		mouse->isDown=false;	 
	}
	return mouse;
}



#endif /* MOUSE_IMPLEMENRATION */



#endif /* MOUSE_H */


