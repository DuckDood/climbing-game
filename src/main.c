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

	float robot_prev_x;
	float robot_prev_y;

	float cam_x;
	float cam_y;

	float arm_angle;
	float arm_angle_vel;

	float arm_end_x;
	float arm_end_y;


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

	SDL_SetRenderVSync(state->renderer, 1);

	state->width = 800;
	state->height = 600;

	state->robot_pos_x = 0.00000001;
	state->robot_prev_x = 0;

	state->robot_pos_y = 0;
	state->robot_prev_y = 0;

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

#define TO_SCREENSPACE(x, y, z) (((x - y) * 0.5 + 0.5) * z)

SDL_AppResult SDL_AppIterate(void *appstate) {
	struct appstate *state = appstate;

	float mouse_x;
	float mouse_y;

	SDL_MouseButtonFlags mouse_flags = SDL_GetMouseState(&mouse_x, &mouse_y);
	int mouse_down = SDL_BUTTON_LMASK & mouse_flags;

	printf("velocity: %f\n", state->robot_pos_x - state->robot_prev_x);
	float new_robot_pos_x = state->robot_pos_x * 2 - state->robot_prev_x;
	float new_robot_pos_y = state->robot_pos_y * 2 - state->robot_prev_y + 0.001;

	state->robot_prev_x = state->robot_pos_x;
	state->robot_prev_y = state->robot_pos_y;

	state->robot_pos_x = new_robot_pos_x;
	state->robot_pos_y = new_robot_pos_y;




	SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
	SDL_RenderClear(state->renderer);

	float aspect_ratio = (float)state->height / state->width;

	float screenspace_robot_center_x = TO_SCREENSPACE(state->robot_pos_x * aspect_ratio, state->cam_x, state->width);
	float screenspace_robot_center_y = TO_SCREENSPACE(state->robot_pos_y, state->cam_y, state->height);

	float wished_arm_angle = atan2(mouse_y - screenspace_robot_center_y, mouse_x - screenspace_robot_center_x);

	float arm_angle_diff = state->arm_angle - wished_arm_angle;
	//https://stackoverflow.com/questions/11498169/dealing-with-angle-wrap-in-c-code

	arm_angle_diff = fmod(arm_angle_diff + M_PI, 2*M_PI);
	if(arm_angle_diff < 0) {
		arm_angle_diff += 2*M_PI;
	}
	arm_angle_diff -= M_PI;

	if(arm_angle_diff < 0) {
		state->arm_angle_vel += 0.1 * fabs(arm_angle_diff);
	}
	if(arm_angle_diff > 0) {
		state->arm_angle_vel -= 0.1 * fabs(arm_angle_diff);
	}

	state->arm_angle_vel *= 0.75;


	state->arm_angle += state->arm_angle_vel;



	if(mouse_down) {
		state->robot_pos_x = -(cos(state->arm_angle) * 0.5) + state->arm_end_x;
		state->robot_pos_y = -(sin(state->arm_angle) * 0.5) + state->arm_end_y;
		
	} else {
		state->arm_end_x = cos(state->arm_angle) * 0.5 + state->robot_pos_x;
		state->arm_end_y = sin(state->arm_angle) * 0.5 + state->robot_pos_y;
	}


	float screenspace_arm_end_pos_x = TO_SCREENSPACE(state->arm_end_x * aspect_ratio, state->cam_x, state->width);
	float screenspace_arm_end_pos_y = TO_SCREENSPACE(state->arm_end_y, state->cam_y, state->height);

	float floor_y = TO_SCREENSPACE(0.9, state->cam_y, state->height);

	//state->robot_velocity_y += 0.001;

	//state->cam_x = (state->cam_x + state->robot_pos_x) * 0.5;
	state->cam_y = state->cam_y * 0.95 + state->robot_pos_y * 0.05;


	float robot_width = 0.1;
	float robot_height = 0.1;

	if(state->robot_pos_y > 0.9 - robot_height) {
		state->robot_pos_y = 0.9 - robot_height;
		float x_vel = state->robot_pos_x - state->robot_prev_x;
		state->robot_pos_x += x_vel * -0.1;
	}

	screenspace_robot_center_x = TO_SCREENSPACE(state->robot_pos_x * aspect_ratio, state->cam_x, state->width);
	screenspace_robot_center_y = TO_SCREENSPACE(state->robot_pos_y, state->cam_y, state->height);

	//float robot_width_scsp = TO_SCREENSPACE(robot_width * aspect_ratio, 0, state->width);
	//float robot_height_scsp = TO_SCREENSPACE(robot_height, 0, state->width);
	float robot_width_scsp = robot_width * aspect_ratio * state->width;
	float robot_height_scsp = robot_height * state->height;

	SDL_SetRenderDrawColor(state->renderer, 255, 0, 0, 255);
	SDL_FRect robot_rect = {screenspace_robot_center_x - robot_width_scsp / 2, screenspace_robot_center_y - robot_height_scsp / 2, robot_width_scsp, robot_height_scsp};

	SDL_RenderRect(state->renderer, &robot_rect);

	SDL_RenderLine(state->renderer, screenspace_robot_center_x, screenspace_robot_center_y, screenspace_arm_end_pos_x, screenspace_arm_end_pos_y);

	SDL_RenderLine(state->renderer, 0, floor_y, 800, floor_y);

	SDL_RenderPresent(state->renderer);

	return SDL_APP_CONTINUE;
}
