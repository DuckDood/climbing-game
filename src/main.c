#include <SDL3/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

struct appstate {
	SDL_Window *window;
	SDL_Renderer *renderer;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
	*appstate = calloc(1, sizeof(struct appstate));
	if(*appstate == NULL) {
		fprintf(stderr, "Failed to allocate memory for app\n");
		goto exit;
		

	}
	struct appstate *state = *appstate;
	
	if(!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to init SDL3: %s\n", SDL_GetError());
	}

	if(!SDL_CreateWindowAndRenderer("climbing", 800, 600, 0, &state->window, &state->renderer)) {
		goto exit;
	}


	return SDL_APP_CONTINUE;


exit:

	fflush(stderr);
	return SDL_APP_FAILURE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
	if(appstate == NULL) goto exit_now;
	struct appstate *state = appstate;

	SDL_DestroyRenderer(state->renderer);
	SDL_DestroyWindow(state->window);


	free(appstate);

exit_now:
	SDL_Quit();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
	switch(event->type) {
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;
			break;
	}


	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
	struct appstate *state = appstate;

	SDL_RenderClear(state->renderer);

	SDL_RenderPresent(state->renderer);

	return SDL_APP_CONTINUE;
}
