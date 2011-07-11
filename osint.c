
#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "osint.h"
#include "vecx.h"

#define EMU_TIMER 30 /* the emulators heart beats at 20 milliseconds */

static SDL_Surface *screen = NULL;

static long screenx;
static long screeny;
static long scl_factor;
static long offx;
static long offy;

void osint_render(void){
	SDL_FillRect(screen, NULL, 0);

	int v;
	for(v = 0; v < vector_draw_cnt; v++){
		Uint8 c = vectors_draw[v].color * 256 / VECTREX_COLORS;
		aalineRGBA(screen,
				offy + vectors_draw[v].y0 / scl_factor,
				screenx - (offx + vectors_draw[v].x0 / scl_factor),
				offy + vectors_draw[v].y1 / scl_factor,
				screenx - (offx + vectors_draw[v].x1 / scl_factor),
				c, c, c, 0xff);
	}
	SDL_Flip(screen);
}

static char *romfilename = "rom.dat";
static char *cartfilename = NULL;

static void init(){
	FILE *f;
	if(!(f = fopen(romfilename, "rb"))){
		perror(romfilename);
		exit(EXIT_FAILURE);
	}
	if(fread(rom, 1, sizeof (rom), f) != sizeof (rom)){
		printf("Invalid rom length\n");
		exit(EXIT_FAILURE);
	}
	fclose(f);

	memset(cart, 0, sizeof (cart));
	if(cartfilename){
		FILE *f;
		if(!(f = fopen(cartfilename, "rb"))){
			perror(cartfilename);
			exit(EXIT_FAILURE);
		}
		fread(cart, 1, sizeof (cart), f);
		fclose(f);
	}
}

void resize(int width, int height){
	long sclx, scly;

	// screen = SDL_SetVideoMode(screenx, screeny, 0, SDL_SWSURFACE | SDL_RESIZABLE);
	screen = SDL_SetVideoMode(0, 0, 0, 0);
	screenx = screen->h;
	screeny = screen->w;

	sclx = ALG_MAX_X / screen->h;
	scly = ALG_MAX_Y / screen->w;

	scl_factor = sclx > scly ? sclx : scly;

	offx = (screenx - ALG_MAX_X / scl_factor) / 2;
	offy = (screeny - ALG_MAX_Y / scl_factor) / 2;
}

static void readevents(){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				exit(0);
				break;
			case SDL_VIDEORESIZE:
				resize(e.resize.w, e.resize.h);
				break;
			/* custom keydown code to handle iCade keyboard interface */
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym){

					/* button 1 - h/r */
					case SDLK_h: snd_regs[14] &= ~0x01; break;
					case SDLK_r: snd_regs[14] |= 0x01; break;
					
					/* button 2 - j/n */
					case SDLK_j: snd_regs[14] &= ~0x02; break;
					case SDLK_n: snd_regs[14] |= 0x02; break;

					/* button 3 - k/p */
					case SDLK_k: snd_regs[14] &= ~0x04; break;
					case SDLK_p: snd_regs[14] |= 0x04; break;

					/* button 4 - l/v */
					case SDLK_l: snd_regs[14] &= ~0x08; break;
					case SDLK_v: snd_regs[14] |= 0x08; break;

					/* left - a/q */
					case SDLK_a:	alg_jch0 = 0x00; break;
					case SDLK_q:	alg_jch0 = 0x80; break;
					
					/* right - d/c */
					case SDLK_d: alg_jch0 = 0xff; break;
					case SDLK_c: alg_jch0 = 0x80; break;

					/* up - w/e */
					case SDLK_w: alg_jch1 = 0xff; break;
					case SDLK_e: alg_jch1 = 0x80; break;

					/* down - x/z */
					case SDLK_x: alg_jch1 = 0x00; break;
					case SDLK_z: alg_jch1 = 0x80; break;

					default:
						break;
				}
				break;
			default:
				break;
		}
	}
}

void osint_emuloop(){
	Uint32 next_time = SDL_GetTicks() + EMU_TIMER;
	vecx_reset();
	for(;;){
		vecx_emu((VECTREX_MHZ / 1000) * EMU_TIMER, 0);
		readevents();

		{
			Uint32 now = SDL_GetTicks();
			if(now < next_time)
				SDL_Delay(next_time - now);
			else
				next_time = now;
			next_time += EMU_TIMER;
		}
	}
}

int main(int argc, char *argv[]){
	SDL_Init(SDL_INIT_VIDEO);

	// use native touchpad resolution
	resize(1024, 768);

	if(argc > 1)
		cartfilename = argv[1];

	init();

	osint_emuloop();

	return 0;
}

