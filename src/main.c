#include <SDL3/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

struct appstate {
	SDL_Window *window;
	SDL_Renderer *renderer;

	float robot_pos_x;
	float robot_pos_y;

	float rebot_velocity_x;
	float rebot_velocity_y;

	float cam_x;
	float cam_y;


	int width;
	int height;
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

	state->width = 800;
	state->height = 600;


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

	float mouse_x;
	float mouse_y;

	SDL_GetMouseState(&mouse_x, &mouse_y);

	SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
	SDL_RenderClear(state->renderer);

	float screenspace_robot_center_x = ((state->robot_pos_x - state->cam_x) * 0.5 + 0.5) * state->width;
	float screenspace_robot_center_y = ((state->robot_pos_y - state->cam_y) * 0.5 + 0.5) * state->height;


	float arm_angle = atan2(mouse_y - screenspace_robot_center_y, mouse_x - screenspace_robot_center_x);

	float arm_end_pos_x = cos(arm_angle) * 100 + screenspace_robot_center_x;
	float arm_end_pos_y = sin(arm_angle) * 100 + screenspace_robot_center_y;

	SDL_SetRenderDrawColor(state->renderer, 255, 0, 0, 255);
	SDL_FRect robot_rect = {screenspace_robot_center_x - 15, screenspace_robot_center_y - 15, 30, 30};

	SDL_RenderRect(state->renderer, &robot_rect);

	SDL_RenderLine(state->renderer, screenspace_robot_center_x, screenspace_robot_center_y, arm_end_pos_x, arm_end_pos_y);

	SDL_RenderPresent(state->renderer);

	return SDL_APP_CONTINUE;
}
