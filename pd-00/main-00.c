#include <SDL2/SDL.h>

#include <stdbool.h>
#include <math.h>

#define GAME_TITLE "Pixel Dancer"

#define DEFAULT_FILE "default.cvs"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

int gap = 4;

int sw = SCREEN_WIDTH;
int sh = SCREEN_HEIGHT;

int us = 32;

int bs = 8;
int bw = 32, bh = 32, bf = 16;
int bx, by;

int bc = 0;
int bn = 0;

SDL_Color pl[] = {
	{0x1a, 0x1c, 0x2c, 0xff},
	{0x5d, 0x27, 0x5d, 0xff},
	{0xb1, 0x3e, 0x53, 0xff},
	{0xef, 0x7d, 0x57, 0xff},
	{0xff, 0xcd, 0x75, 0xff},
	{0xa7, 0xf0, 0x70, 0xff},
	{0x38, 0xb7, 0x64, 0xff},
	{0x25, 0x71, 0x79, 0xff},
	{0x29, 0x36, 0x6f, 0xff},
	{0x3b, 0x5d, 0xc9, 0xff},
	{0x41, 0xa6, 0xf6, 0xff},
	{0x73, 0xef, 0xf7, 0xff},
	{0xf4, 0xf4, 0xf4, 0xff},
	{0x94, 0xb0, 0xc2, 0xff},
	{0x56, 0x6c, 0x86, 0xff},
	{0x33, 0x3c, 0x57, 0xff},
};

int pn = 16;
int pc = 12;
int ps = 32;

int *bp = NULL;

int *bb = NULL;

float fingerX = 0, fingerY = 0;
bool fingerDown = false;

int targetX = 0, targetY = 0;
int currX = 0, currY = 0;

int px = 0, py = 0;

bool draw = false;
bool grid = false;
bool play = false;

bool quit = false;

int hold = -1;

char *hex = "0123456789ABCDEF";

typedef enum GameState
{
	GAMESTATE_PEN = 0,
	GAMESTATE_TARGET,
	GAMESTATE_FLOODFILL,
	GAMESTATE_DROPPER,
	GAMESTATE_LINE,
	GAMESTATE_OVAL,
	GAMESTATE_FILLOVAL,
	GAMESTATE_RECT,
	GAMESTATE_FILLRECT,
	GAMESTATE_MAX
} GameState;

GameState gameState = GAMESTATE_PEN;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Event event;

bool inRect(int x, int y, int rx, int ry, int rw, int rh)
{
	return x >= rx && x < rx + rw && y >= ry && y < ry + rh;
}

void setPixel(int *b, int f, int x, int y, int c)
{
	if (x >= 0 && x < bw && y >= 0 && y < bh)
	{
		b[f * bw * bh + y * bw + x] = c;
	}
}

void drawLine(int *b, int f, int x0, int y0, int x1, int y1, int c)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;

	for (;;)
	{
		setPixel(b, f, x0, y0, c);
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

void drawOval(int *b, int a, int x0, int y0, int x1, int y1, int c, bool f)
{
	int xb, yb, xc, yc;
	int qa, qb;
	int dx, dy;
	int qx, qy;
	long qt;

	yb = yc = (y0 + y1) / 2;
	qb = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	qy = qb;
	dy = qb / 2;
	if (qb % 2 != 0)
		yc++;

	xb = xc = (x0 + x1) / 2;
	qa = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	qx = qa % 2;
	dx = 0;
	qt = (long)qa * qa + (long)qb * qb - 2L * qa * qa * qb;
	if (qx != 0)
	{
		xc++;
		qt += 3L * qb * qb;
	}

	while (qy >= 0 && qx <= qa)
	{
		if (!f)
		{
			setPixel(b, a, xb - dx, yb - dy, c);
			if (dx != 0 || xb != xc)
			{
				setPixel(b, a, xc + dx, yb - dy, c);
				if (dy != 0 || yb != yc)
					setPixel(b, a, xc + dx, yc + dy, c);
			}
			if (dy != 0 || yb != yc)
				setPixel(b, a, xb - dx, yc + dy, c);
		}

		if (qt + 2L * qb * qb * qx + 3L * qb * qb <= 0L ||
			qt + 2L * qa * qa * qy - (long)qa * qa <= 0L)
		{
			qt += 8L * qb * qb + 4L * qb * qb * qx;
			dx++;
			qx += 2;
		}
		else if (qt - 2L * qa * qa * qy + 3L * qa * qa > 0L)
		{
			if (f)
			{
				drawLine(b, a, xb - dx, yc + dy, xc + dx, yc + dy, c);
				if (dy != 0 || yb != yc)
					drawLine(b, a, xb - dx, yb - dy, xc + dx, yb - dy, c);
			}
			qt += 8L * qa * qa - 4L * qa * qa * qy;
			dy--;
			qy -= 2;
		}
		else
		{
			if (f)
			{
				drawLine(b, a, xb - dx, yc + dy, xc + dx, yc + dy, c);
				if (dy != 0 || yb != yc)
					drawLine(b, a, xb - dx, yb - dy, xc + dx, yb - dy, c);
			}
			qt += 8L * qb * qb + 4L * qb * qb * qx + 8L * qa * qa - 4L * qa * qa * qy;
			dx++;
			qx += 2;
			dy--;
			qy -= 2;
		}
	}
}

void drawRect(int *b, int f, int x0, int y0, int x1, int y1, int c)
{
	int t;

	if (x0 > x1)
	{
		t = x0;
		x0 = x1;
		x1 = t;
	}
	if (y0 > y1)
	{
		t = y0;
		y0 = y1;
		y1 = t;
	}

	int w = x1 - x0;
	int h = y1 - y0;

	for (int i = 0; i <= w; i++)
	{
		setPixel(b, f, x0 + i, y0, c);
		setPixel(b, f, x0 + i, y0 + h, c);
	}
	for (int j = 0; j <= h; j++)
	{
		setPixel(b, f, x0, y0 + j, c);
		setPixel(b, f, x0 + w, y0 + j, c);
	}
}

void fillRect(int *b, int f, int x0, int y0, int x1, int y1, int c)
{
	int t;

	if (x0 > x1)
	{
		t = x0;
		x0 = x1;
		x1 = t;
	}
	if (y0 > y1)
	{
		t = y0;
		y0 = y1;
		y1 = t;
	}

	int w = x1 - x0;
	int h = y1 - y0;

	for (int j = 0; j <= h; j++)
	{
		for (int i = 0; i <= w; i++)
		{
			setPixel(b, f, i + x0, j + y0, c);
		}
	}
}

void floodFill(int *b, int f, int x, int y, int pp, int nn)
{
	if (x < 0 || x >= bw || y < 0 || y >= bh)
		return;

	int cc = b[f * bw * bh + y * bw + x];

	if (cc == pp)
	{
		setPixel(b, f, x, y, nn);

		floodFill(b, f, x, y - 1, pp, nn);
		floodFill(b, f, x, y + 1, pp, nn);
		floodFill(b, f, x - 1, y, pp, nn);
		floodFill(b, f, x + 1, y, pp, nn);
	}
}

void drawGrid(SDL_Renderer *r, int *b, int x, int y, int w, int h, int f, int s, int g)
{
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			int k = b[f * w * h + j * w + i];
			SDL_SetRenderDrawColor(r, pl[k].r, pl[k].g, pl[k].b, pl[k].a);

			SDL_RenderFillRect(r, &(SDL_Rect){x + i * s, y + j * s, s, s});
		}
	}

	SDL_SetRenderDrawColor(r, 0xFF, 0xFF, 0xFF, 0xFF);

	if (g)
	{
		for (int i = 0; i <= w; i++)
		{
			SDL_RenderDrawLine(r, x + i * s, y, x + i * s, y + h * s);
		}

		for (int j = 0; j <= h; j++)
		{
			SDL_RenderDrawLine(r, x, y + j * s, x + w * s, y + j * s);
		}
	}
	else
	{
		SDL_RenderDrawRect(r, &(SDL_Rect){x, y, w * s, h * s});
	}

	/*

	int numDevices = SDL_GetNumTouchDevices();

	for (int i = 0; i < numDevices; i++)
	{
		SDL_TouchID touchId = SDL_GetTouchDevice(i);
		int numTouchFingers = SDL_GetNumTouchFingers(touchId);

		for (int j = 0; j < numTouchFingers; j++)
		{
			SDL_Finger *finger = SDL_GetTouchFinger(touchId, j);

			int x0 = finger->x * sw;
			int y0 = finger->y * sh ;

			if (inRect(x0, y0, x, y, w * s, h * s))
			{
				int x1 = (x0-x)/s;
				int y1 = (y0-y)/s;

				bp[y1 * w + x1] = 1;
			}
		}
	}
	
*/
}

void drawPalette(SDL_Renderer *r, SDL_Color *p, int np, int *pc, int x, int y, int s)
{
	for (int k = 0; k < np; k++)
	{
		int i = k % 8;
		int j = k / 8;

		SDL_SetRenderDrawColor(r, p[k].r, p[k].g, p[k].b, p[k].a);

		SDL_RenderFillRect(r, &(SDL_Rect){x + i * s, y + j * s, s, s});

		if (k == *pc)
		{
			for (int l = 0; l < 5; l++)
			{
				SDL_SetRenderDrawColor(r, p[12].r, p[12].g, p[12].b, p[12].a);

				SDL_RenderDrawRect(r, &(SDL_Rect){x + i * s + l, y + j * s + l, s - l * 2, s - l * 2});
			}

			for (int l = 5; l < 10; l++)
			{
				SDL_SetRenderDrawColor(r, p[0].r, p[0].g, p[0].b, p[0].a);

				SDL_RenderDrawRect(r, &(SDL_Rect){x + i * s + l, y + j * s + l, s - l * 2, s - l * 2});
			}
		}
	}

	if (fingerDown && inRect(fingerX, fingerY, x, y, 8 * s, 2 * s))
	{
		int x0 = (fingerX - x) / s;
		int y0 = (fingerY - y) / s;

		*pc = y0 * 8 + x0;
	}
}

bool drawButton(SDL_Renderer *r, int n, int x, int y, int w, int h, int c)
{
	bool res = false;

	if (hold == n)
	{
		SDL_SetRenderDrawColor(r, pl[c].r, pl[c].g, pl[c].b, pl[c].a);

		SDL_RenderFillRect(r, &(SDL_Rect){x + 10, y + 10, w - 20, h - 20});

		SDL_SetRenderDrawColor(r, pl[12].r, pl[12].g, pl[12].b, pl[12].a);

		SDL_RenderDrawRect(r, &(SDL_Rect){x + 10, y + 10, w - 20, h - 20});
	}
	else
	{
		SDL_SetRenderDrawColor(r, pl[c].r, pl[c].g, pl[c].b, pl[c].a);

		SDL_RenderFillRect(r, &(SDL_Rect){x, y, w, h});

		SDL_SetRenderDrawColor(r, pl[12].r, pl[12].g, pl[12].b, pl[12].a);

		SDL_RenderDrawRect(r, &(SDL_Rect){x, y, w, h});
	}

	if (hold != n)
	{
		if (fingerDown)
		{
			if (inRect(fingerX, fingerY, x, y, w, h))
			{
				hold = n;
			}
		}
	}
	else
	{
		if (!fingerDown)
		{
			res = true;
			hold = -1;
		}
	}

	return res;
}

void Surface_setPixel(SDL_Surface *s, int x, int y, Uint32 c)
{
	Uint32 *pixels = (Uint32 *)s->pixels;

	pixels[y * s->w + x] = c;
}

void Surface_fillRect(SDL_Surface *s, int x, int y, int w, int h, Uint32 c)
{
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			Surface_setPixel(s, x, y, c);
		}
	}
}

void save(const char *n, int *b, int w, int h, int f)
{
	FILE *fout = fopen(n, "w");

	fprintf(fout, "%d %d %d\n\n", w, h, f);

	for (int k = 0; k < f; k++)
	{
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				int l = b[k * w * h + j * w + i];

				fprintf(fout, "%c", hex[l]);
			}
			fprintf(fout, "\n");
		}
		fprintf(fout, "\n");
	}

	fclose(fout);
}

int hexval(char c)
{
	int r = 0;
	for (int i = 0; hex[i]; i++)
	{
		if (c == hex[i])
		{
			r = i;
			break;
		}
	}
	return r;
}

void load(const char *n, int **b, int *w, int *h, int *f)
{
	FILE *fin = fopen(n, "r");

	fscanf(fin, "%d %d %d\n\n", w, h, f);

	(*b) = calloc((*f) * (*w) * (*h), sizeof(**b));

	for (int k = 0; k < *f; k++)
	{
		for (int j = 0; j < *h; j++)
		{
			for (int i = 0; i < *w; i++)
			{
				char l;

				fscanf(fin, "%c", &l);

				int m = hexval(l);

				if (m != 0 && bn < k)
				{
					bn = k;
				}

				(*b)[k * *w * *h + j * *w + i] = m;
			}
			fscanf(fin, "\n");
		}
		fscanf(fin, "\n");
	}

	fclose(fin);
}

SDL_Color contrastColor(SDL_Color c)
{
	double luma = ((0.299 * c.r) + (0.587 * c.g) + (0.114 * c.b)) / 255;

	return luma > 0.5 ? (SDL_Color){0x00, 0x00, 0x00, 0xFF} : (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF};
}

void drawTarget(SDL_Renderer *r, int x, int y)
{
	int tx = x * bs + bx;
	int ty = y * bs + by;

	SDL_Color cc = pl[bp[y * bw + x]];

	SDL_Color tc = contrastColor(cc);

	SDL_SetRenderDrawColor(renderer, tc.r, tc.g, tc.b, tc.a);

	SDL_RenderDrawLine(renderer, tx + 1, ty, tx + bs + 1, ty + bs);
	SDL_RenderDrawLine(renderer, tx, ty, tx + bs, ty + bs);

	SDL_RenderDrawLine(renderer, tx, ty + 1, tx + bs, ty + bs + 1);

	SDL_RenderDrawLine(renderer, tx + bs - 1, ty, tx - 1, ty + bs);
	SDL_RenderDrawLine(renderer, tx + bs, ty, tx, ty + bs);

	SDL_RenderDrawLine(renderer, tx + bs, ty + 1, tx, ty + bs + 1);
}

void Rubberband_setPixel(SDL_Renderer *r, int x, int y, int s)
{
	if (x >= 0 && x < bw && y >= 0 && y < bh)
	{
		SDL_Color k = contrastColor(pl[bp[y * bw + x]]);

		SDL_SetRenderDrawColor(r, k.r, k.g, k.b, k.a);
		SDL_RenderDrawRect(r, &(SDL_Rect){x * s + bx, y * s + by, s, s});
	}
}

void Rubberband_drawLine(SDL_Renderer *r, int x0, int y0, int x1, int y1, int s)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;

	for (;;)
	{
		Rubberband_setPixel(r, x0, y0, s);
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

void Rubberband_drawRect(SDL_Renderer *r, int x0, int y0, int x1, int y1, int s)
{
	int t;

	if (x0 > x1)
	{
		t = x0;
		x0 = x1;
		x1 = t;
	}

	if (y0 > y1)
	{
		t = y0;
		y0 = y1;
		y1 = t;
	}

	int w = x1 - x0;
	int h = y1 - y0;

	for (int i = 0; i <= w; i++)
	{
		Rubberband_setPixel(r, x0 + i, y0, s);
		Rubberband_setPixel(r, x0 + i, y0 + h, s);
	}
	for (int j = 0; j <= h; j++)
	{
		Rubberband_setPixel(r, x0, y0 + j, s);
		Rubberband_setPixel(r, x0 + w, y0 + j, s);
	}
}

void Rubberband_fillRect(SDL_Renderer *r, int x0, int y0, int x1, int y1, int s)
{
	int t;

	if (x0 > x1)
	{
		t = x0;
		x0 = x1;
		x1 = t;
	}

	if (y0 > y1)
	{
		t = y0;
		y0 = y1;
		y1 = t;
	}

	int w = x1 - x0;
	int h = y1 - y0;

	for (int j = 0; j <= h; j++)
	{
		for (int i = 0; i <= w; i++)
		{
			Rubberband_setPixel(r, x0 + i, y0 + j, s);
		}
	}
}

void Rubberband_drawOval(SDL_Renderer *r, int x0, int y0, int x1, int y1, int s, bool f)
{
	int xb, yb, xc, yc;
	int qa, qb;
	int dx, dy;
	int qx, qy;
	long qt;

	yb = yc = (y0 + y1) / 2;
	qb = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	qy = qb;
	dy = qb / 2;
	if (qb % 2 != 0)
		yc++;

	xb = xc = (x0 + x1) / 2;
	qa = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	qx = qa % 2;
	dx = 0;
	qt = (long)qa * qa + (long)qb * qb - 2L * qa * qa * qb;
	if (qx != 0)
	{
		xc++;
		qt += 3L * qb * qb;
	}

	while (qy >= 0 && qx <= qa)
	{
		if (!f)
		{
			Rubberband_setPixel(r, xb - dx, yb - dy, s);
			if (dx != 0 || xb != xc)
			{
				Rubberband_setPixel(r, xc + dx, yb - dy, s);
				if (dy != 0 || yb != yc)
					Rubberband_setPixel(r, xc + dx, yc + dy, s);
			}
			if (dy != 0 || yb != yc)
				Rubberband_setPixel(r, xb - dx, yc + dy, s);
		}

		if (qt + 2L * qb * qb * qx + 3L * qb * qb <= 0L ||
			qt + 2L * qa * qa * qy - (long)qa * qa <= 0L)
		{
			qt += 8L * qb * qb + 4L * qb * qb * qx;
			dx++;
			qx += 2;
		}
		else if (qt - 2L * qa * qa * qy + 3L * qa * qa > 0L)
		{
			if (f)
			{
				Rubberband_drawLine(r, xb - dx, yc + dy, xc + dx, yc + dy, s);
				if (dy != 0 || yb != yc)
					Rubberband_drawLine(r, xb - dx, yb - dy, xc + dx, yb - dy, s);
			}
			qt += 8L * qa * qa - 4L * qa * qa * qy;
			dy--;
			qy -= 2;
		}
		else
		{
			if (f)
			{
				Rubberband_drawLine(r, xb - dx, yc + dy, xc + dx, yc + dy, s);
				if (dy != 0 || yb != yc)
					Rubberband_drawLine(r, xb - dx, yb - dy, xc + dx, yb - dy, s);
			}
			qt += 8L * qb * qb + 4L * qb * qb * qx + 8L * qa * qa - 4L * qa * qa * qy;
			dx++;
			qx += 2;
			dy--;
			qy -= 2;
		}
	}
}

int main(void)
{
	//SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait");

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sw, sh, SDL_WINDOW_SHOWN);

	SDL_GetWindowSize(window, &sw, &sh);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, sw, sh);

	//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	bp = calloc(bw * bh * bf, sizeof(*bp));

	bb = calloc(bw * bh, sizeof(*bb));

	while (!quit)
	{
		SDL_PumpEvents();

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				fingerDown = true;
				fingerX = event.button.x;
				fingerY = event.button.y;
				break;
			case SDL_MOUSEMOTION:
				fingerX = event.button.x;
				fingerY = event.button.y;
				break;
			case SDL_MOUSEBUTTONUP:
				fingerDown = false;
				fingerX = event.button.x;
				fingerY = event.button.y;
				break;
			}
		}

		SDL_SetRenderTarget(renderer, texture);

		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x10);
		SDL_RenderFillRect(renderer, &(SDL_Rect){0, 0, sw, sh});

		bx = (sw - bw * bs) / 2;

		by = (sh - bh * bs) / 2;

		drawGrid(renderer, bp, bx, by, bw, bh, bc, bs, grid);

		drawPalette(renderer, pl, pn, &pc, (sw - 8 * ps) / 2, sh - ps * 2 - gap, ps);

		if (drawButton(renderer, 1, gap, gap, us, us, 0))
		{
			bc--;
			if (bc < 0)
				bc = 0;
		};

		if (drawButton(renderer, 2, sw - us - gap, gap, us, us, 0))
		{
			bc++;
			if (bc >= bf)
				bc = bf - 1;
		}

		if (drawButton(renderer, 3, (sw - us) / 2, gap, us, us, 0))
		{
			play = !play;
		};

		if (drawButton(renderer, 4, gap, gap + us/4 + us, us, us, 0))
		{
			for (int i = 0; i < bw * bh; i++)
				bb[i] = bp[bc * bw * bh + i];
		};

		if (drawButton(renderer, 5, sw - us - gap, gap + us/4 + us, us, us, 0))
		{
			if (bn < bc)
			{
				bn = bc;
			}

			for (int i = 0; i < bw * bh; i++)
				bp[bc * bw * bh + i] = bb[i];
		};

		if (drawButton(renderer, 6, (sw - us) / 2, gap + us/4 + us, us, us, 0))
		{
			for (int i = 0; i < bw * bh; i++)
			{
				if (bc == bn)
				{
					bn--;
				}

				bb[i] = bp[bc * bw * bh + i];
				bp[bc * bw * bh + i] = 0;
			}
		};

		if (drawButton(renderer, 7, gap, (gap + us/4 + us) * 2, us, us, 0))
		{
			for (int i = 0; i <= bn; i++)
			{
				save(DEFAULT_FILE, bp, bw, bh, bf);
			}
		}

		if (drawButton(renderer, 8, sw - us - gap, (gap + us/4 + us) * 2, us, us, 0))
		{
			free(bp);
			bp = NULL;
			bc = 0;
			bn = 0;

			load(DEFAULT_FILE, &bp, &bw, &bh, &bf);
		}

		if (drawButton(renderer, 9, (sw - us) / 4, (gap + us/4 + us) * 2, us, us, 0))
		{
			gameState = GAMESTATE_FLOODFILL;
		}

		if (drawButton(renderer, 10, (sw - us) / 4 * 3, (gap + us/4 + us) * 2, us, us, 0))
		{
			gameState = GAMESTATE_DROPPER;
		}

		if (drawButton(renderer, 11, (sw - us) / 2, (gap + us/4 + us) * 2, us, us, 0))
		{
			gameState = GAMESTATE_TARGET;
		}

		if (drawButton(renderer, 12, gap, (gap + us/4 + us) * 3, us, us, 0))
		{
			gameState = GAMESTATE_LINE;
		}

		if (drawButton(renderer, 13, (sw - us) / 4, (gap + us/4 + us) * 3, us, us, 0))
		{
			gameState = GAMESTATE_RECT;
		}

		if (drawButton(renderer, 14, (sw - us) / 4 * 2, (gap + us/4 + us) * 3, us, us, 0))
		{
			gameState = GAMESTATE_FILLRECT;
		}

		if (drawButton(renderer, 15, (sw - us) / 4 * 3, (gap + us/4 + us) * 3, us, us, 0))
		{
			gameState = GAMESTATE_OVAL;
		}

		if (drawButton(renderer, 16, sw - us - gap, (gap + us/4 + us) * 3, us, us, 0))
		{
			gameState = GAMESTATE_FILLOVAL;
		}

		if (drawButton(renderer, 17, (sw - us) / 2, (gap + us/4 + us) * 4, us, us, 0))
		{
			gameState = GAMESTATE_PEN;
		}

		switch (gameState)
		{
		case GAMESTATE_PEN:
			if (fingerDown)
			{
				if (inRect(fingerX, fingerY, bx, by, bw * bs, bh * bs))
				{
					int x0 = (fingerX - bx) / bs;
					int y0 = (fingerY - by) / bs;

					if (draw)
					{
						if (bn < bc)
						{
							bn = bc;
						}

						drawLine(bp, bc, px, py, x0, y0, pc);
					}
					else
					{
						draw = true;
					}

					px = x0;
					py = y0;
				}
			}
			else
			{
				draw = false;
			}
			break;
		case GAMESTATE_TARGET:
			if(fingerDown) {
				if (inRect(fingerX, fingerY, bx, by, bw * bs, bh * bs))
				{
					targetX = (fingerX - bx) / bs;
					targetY = (fingerY - by) / bs;
				}
			}
			
			break;
		case GAMESTATE_FLOODFILL:
		
			if(fingerDown) {
				int x0 = (fingerX - bx) / bs;
				int y0 = (fingerY - by) / bs;

				int cc = bp[y0 * bw + x0];

				if (cc != pc)
				{
					floodFill(bp, bc, x0, y0, cc, pc);
				}
			}
		
		break;
		case GAMESTATE_DROPPER:
			if(fingerDown) {
				if (inRect(fingerX, fingerY, bx, by, bw * bs, bh * bs))
				{
					int x0 = (fingerX - bx) / bs;
					int y0 = (fingerY - by) / bs;

					pc = bp[y0 * bw + x0];
				}
			}
			break;
		case GAMESTATE_LINE:

			if (hold != -2)
			{
				if (fingerDown)
				{
					if (inRect(fingerX, fingerY, bx, by, bw * bs, bh * bs))
					{
						hold = -2;
					}
				}
			}
			else
			{
				if (fingerDown)
				{
					currX = (fingerX - bx) / bs;
					currY = (fingerY - by) / bs;
					if (currX < 0)
						currX = 0;
					if (currY < 0)
						currY = 0;
					if (currX >= bw)
						currX = bw - 1;
					if (currY >= bh)
						currY = bh - 1;

					Rubberband_drawLine(renderer, targetX, targetY, currX, currY, bs);
				}
				else
				{
					drawLine(bp, bc, targetX, targetY, currX, currY, pc);

					targetX = currX;
					targetY = currY;

					hold = -1;
				}
			}

			break;
		case GAMESTATE_OVAL:

			if (hold != -3)
			{
				if (fingerDown)
				{
					if (inRect(fingerX, fingerY, bx, by, bw * bs, bh * bs))
					{
						hold = -3;
					}
				}
			}
			else
			{
				if (fingerDown)
				{
					currX = (fingerX - bx) / bs;
					currY = (fingerY - by) / bs;
					if (currX < 0)
						currX = 0;
					if (currY < 0)
						currY = 0;
					if (currX >= bw)
						currX = bw - 1;
					if (currY >= bh)
						currY = bh - 1;

					Rubberband_drawOval(renderer, targetX, targetY, currX, currY, bs, 0);
				}
				else
				{
					drawOval(bp, bc, targetX, targetY, currX, currY, pc, 0);

					hold = -1;
				}
			}

			break;
		case GAMESTATE_FILLOVAL:

			if (hold != -3)
			{
				if (fingerDown)
				{
					if (inRect(fingerX, fingerY, bx, by, bw * bs, bh * bs))
					{
						hold = -3;
					}
				}
			}
			else
			{
				if (fingerDown)
				{
					currX = (fingerX - bx) / bs;
					currY = (fingerY - by) / bs;
					if (currX < 0)
						currX = 0;
					if (currY < 0)
						currY = 0;
					if (currX >= bw)
						currX = bw - 1;
					if (currY >= bh)
						currY = bh - 1;

					Rubberband_drawOval(renderer, targetX, targetY, currX, currY, bs, 1);
				}
				else
				{
					drawOval(bp, bc, targetX, targetY, currX, currY, pc, 1);

					hold = -1;
				}
			}

			break;
		case GAMESTATE_RECT:

			if (hold != -3)
			{
				if (fingerDown)
				{
					if (inRect(fingerX, fingerY, bx, by, bw * bs, bh * bs))
					{
						hold = -3;
					}
				}
			}
			else
			{
				if (fingerDown)
				{
					currX = (fingerX - bx) / bs;
					currY = (fingerY - by) / bs;
					if (currX < 0)
						currX = 0;
					if (currY < 0)
						currY = 0;
					if (currX >= bw)
						currX = bw - 1;
					if (currY >= bh)
						currY = bh - 1;

					Rubberband_drawRect(renderer, targetX, targetY, currX, currY, bs);
				}
				else
				{
					drawRect(bp, bc, targetX, targetY, currX, currY, pc);

					hold = -1;
				}
			}

			break;
		case GAMESTATE_FILLRECT:

			if (hold != -3)
			{
				if (fingerDown)
				{
					if (inRect(fingerX, fingerY, bx, by, bw * bs, bh * bs))
					{
						hold = -3;
					}
				}
			}
			else
			{
				if (fingerDown)
				{
					currX = (fingerX - bx) / bs;
					currY = (fingerY - by) / bs;
					if (currX < 0)
						currX = 0;
					if (currY < 0)
						currY = 0;
					if (currX >= bw)
						currX = bw - 1;
					if (currY >= bh)
						currY = bh - 1;

					Rubberband_fillRect(renderer, targetX, targetY, currX, currY, bs);
				}
				else
				{
					fillRect(bp, bc, targetX, targetY, currX, currY, pc);

					hold = -1;
				}
			}

			break;
			default:
			break;
		}

		drawTarget(renderer, targetX, targetY);

		SDL_SetRenderTarget(renderer, NULL);

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		if (play)
		{
			bc = (bc + 1) % (bn + 1);
			SDL_Delay(1000 / 6);
		}

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
