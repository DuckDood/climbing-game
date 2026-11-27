#include <SDL3/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <physics.h>

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

struct appstate {
	SDL_Window *window;
	SDL_Renderer *renderer;

	/*float robot_pos_x;
	float robot_pos_y;

	float robot_prev_x;
	float robot_prev_y;*/

	ball_2d robot;

	ball_2d arm_end;
	vec2 arm_lock_pos;

	float cam_x;
	float cam_y;

	float arm_len;

	//float arm_angle;
	//float arm_angle_vel;

	//float arm_end_x;
	//float arm_end_y;
	
	SDL_Texture *robot_sprite;

	SDL_Texture *armopen_sprite;
	SDL_Texture *armclose_sprite;

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

	state->robot_sprite = SDL_CreateTextureFromSurface(state->renderer, SDL_LoadPNG("resources/robot.png"));
	SDL_SetTextureScaleMode(state->robot_sprite, SDL_SCALEMODE_PIXELART);
	
	state->armopen_sprite = SDL_CreateTextureFromSurface(state->renderer, SDL_LoadPNG("resources/armopen.png"));
	SDL_SetTextureScaleMode(state->armopen_sprite, SDL_SCALEMODE_PIXELART);

	state->armclose_sprite = SDL_CreateTextureFromSurface(state->renderer, SDL_LoadPNG("resources/armclose.png"));
	SDL_SetTextureScaleMode(state->armclose_sprite, SDL_SCALEMODE_PIXELART);

	SDL_SetRenderVSync(state->renderer, 1);

	state->width = 800;
	state->height = 600;

	state->robot.mass = 1;
	state->robot.position = (vec2){0};
	state->robot.velocity = (vec2){0};
	state->robot.radius = 0.2;

	state->arm_end.mass = 0.00001;
	state->arm_end.position = (vec2){0, 1};
	state->arm_end.velocity = (vec2){0};
	state->arm_end.radius = 0.1;

	state->arm_len = 0.5;

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
	struct appstate *state = appstate;
	switch(event->type) {
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			state->arm_lock_pos = state->arm_end.position;
			break;
	}


	return SDL_APP_CONTINUE;
}

#define TO_SCREENSPACE(x, y, z, w) ((((x - y)*w) * 0.5 + 0.5) * z)

SDL_AppResult SDL_AppIterate(void *appstate) {
	struct appstate *state = appstate;

	float mouse_x;
	float mouse_y;

	SDL_MouseButtonFlags mouse_flags = SDL_GetMouseState(&mouse_x, &mouse_y);
	int mouse_down = SDL_BUTTON_LMASK & mouse_flags;

	const _Bool *kbd_state = SDL_GetKeyboardState(NULL);

	state->robot.velocity.y += 0.05;
	state->arm_end.velocity.y += 0.05;

	if(mouse_down) {
		v2_fmult(state->arm_end.velocity, 0);
	}
	begin_ball_update_2d(&state->robot, 1./60);
	begin_ball_update_2d(&state->arm_end, 1./60);



	SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
	SDL_RenderClear(state->renderer);

	float aspect_ratio = (float)state->height / state->width;

	float screenspace_robot_center_x = TO_SCREENSPACE(state->robot.position.x, state->cam_x, state->width, aspect_ratio);
	float screenspace_robot_center_y = TO_SCREENSPACE(state->robot.position.y, state->cam_y, state->height, 1);

	float wished_arm_angle = atan2(mouse_y - screenspace_robot_center_y, mouse_x - screenspace_robot_center_x);
	float real_arm_angle = atan2(state->arm_end.position.y - state->robot.position.y, state->arm_end.position.x - state->robot.position.x);

	float arm_angle_diff = fabs(wished_arm_angle - real_arm_angle);
	//arm_angle_diff *= arm_angle_diff;
	arm_angle_diff = 1;
	printf("arm_angle_diff: %f\n", arm_angle_diff);

	if(mouse_down) {
		state->arm_end.mass = 100;
		state->arm_end.position = state->arm_lock_pos;
	} else {
		state->arm_end.position.x += cos(wished_arm_angle) * 0.1 * arm_angle_diff;
		state->arm_end.position.y += sin(wished_arm_angle) * 0.1 * arm_angle_diff;
		state->arm_end.mass = 0.00001;
	}

	//float arm_angle_diff = state->arm_angle - wished_arm_angle;
	//https://stackoverflow.com/questions/11498169/dealing-with-angle-wrap-in-c-code

	/*arm_angle_diff = fmod(arm_angle_diff + M_PI, 2*M_PI);
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


	state->arm_angle += state->arm_angle_vel;*/



	/*if(mouse_down) {
		state->robot_pos_x = -(cos(state->arm_angle) * 0.5) + state->arm_end_x;
		state->robot_pos_y = -(sin(state->arm_angle) * 0.5) + state->arm_end_y;
		
	} else {
		state->arm_end_x = cos(state->arm_angle) * 0.5 + state->robot_pos_x;
		state->arm_end_y = sin(state->arm_angle) * 0.5 + state->robot_pos_y;
	}*/


	//float screenspace_arm_end_pos_x = TO_SCREENSPACE(state->arm_end_x * aspect_ratio, state->cam_x, state->width);
	//float screenspace_arm_end_pos_y = TO_SCREENSPACE(state->arm_end_y, state->cam_y, state->height);
	

	SDL_Vertex height_vertices[6];

	height_vertices[0].position.x = TO_SCREENSPACE(0, state->cam_x, state->width, aspect_ratio);
	height_vertices[0].position.y = TO_SCREENSPACE(0, state->cam_y, state->height, 1);

	height_vertices[0].color.a = 1;
	height_vertices[0].color.r = 1;
	height_vertices[0].color.g = 1;
	height_vertices[0].color.b = 1;

	height_vertices[1].position.x = TO_SCREENSPACE(0, state->cam_x, state->width, aspect_ratio);
	height_vertices[1].position.y = TO_SCREENSPACE(-20, state->cam_y, state->height, 1);

	height_vertices[1].color.a = 1;
	height_vertices[1].color.r = 1;
	height_vertices[1].color.g = 0;
	height_vertices[1].color.b = 0;

	height_vertices[2].position.x = TO_SCREENSPACE(1, state->cam_x, state->width, aspect_ratio);
	height_vertices[2].position.y = TO_SCREENSPACE(-20, state->cam_y, state->height, 1);

	height_vertices[2].color.a = 1;
	height_vertices[2].color.r = 1;
	height_vertices[2].color.g = 0;
	height_vertices[2].color.b = 0;

	SDL_RenderGeometry(state->renderer, NULL, height_vertices, 3, NULL, 0);

	float floor_y = TO_SCREENSPACE(0.9, state->cam_y, state->height, 1);

	//state->robot_velocity_y += 0.001;

	//state->cam_x = (state->cam_x + state->robot_pos_x) * 0.5;
	if(mouse_down) {
		state->cam_y = state->cam_y * 0.95 + state->arm_end.position.y * 0.05;
		state->cam_x = state->cam_x * 0.95 + state->arm_end.position.x * 0.05;
	} else { 
		state->cam_y = state->cam_y * 0.9 + state->robot.position.y * 0.1;
		state->cam_x = state->cam_x * 0.99 + state->robot.position.x * 0.01;
	}


	float robot_width = state->robot.radius/2;
	float robot_height = state->robot.radius/2;

	/*if(state->robot.position.y > 0.9 - robot_height) {
		state->robot.position.y = 0.9 - robot_height;
		float x_vel = state->robot_pos_x - state->robot_prev_x;
		state->robot_pos_x += x_vel * -0.1;
	}*/
	vec2 output_ptr[2];


	solve_constraint_2d(boundary_constraint_2d, (vec2*[]){&state->robot.position}, output_ptr, (float[]){1}, 1, &(boundary_args_2d){(vec2){0, 1}, (vec2){0, -1}, state->robot.radius}, 1/60., 0, INEQ_GREATER);

	solve_constraint_2d(boundary_constraint_2d, (vec2*[]){&state->arm_end.position}, output_ptr, (float[]){1}, 1, &(boundary_args_2d){(vec2){0, 1}, (vec2){0, -1}, state->arm_end.radius}, 1/60., 0, INEQ_GREATER);

	if(kbd_state[SDL_SCANCODE_SPACE]) {
		state->arm_len = state->arm_len * 0.6 + 0.3 * 0.4;
	} else {
		state->arm_len = state->arm_len * 0.8 + 0.5 * 0.2;
	}

	if(kbd_state[SDL_SCANCODE_A]) {
		state->robot.position.x -= 0.001;
	}
	if(kbd_state[SDL_SCANCODE_D]) {
		state->robot.position.x += 0.001;
	}


	solve_constraint_2d(distance_constraint_2d, (vec2*[]){&state->robot.position, &state->arm_end.position}, output_ptr, (float[]){1/state->robot.mass, 1/state->arm_end.mass}, 2, (float[]){state->arm_len}, 1/60., 0, EQUALITY);


	screenspace_robot_center_x = TO_SCREENSPACE(state->robot.position.x, state->cam_x, state->width, aspect_ratio);
	screenspace_robot_center_y = TO_SCREENSPACE(state->robot.position.y, state->cam_y, state->height, 1);

	float screenspace_arm_center_x = TO_SCREENSPACE(state->arm_end.position.x, state->cam_x, state->width, aspect_ratio);
	float screenspace_arm_center_y = TO_SCREENSPACE(state->arm_end.position.y, state->cam_y, state->height, 1);

	//float robot_width_scsp = TO_SCREENSPACE(robot_width * aspect_ratio, 0, state->width);
	//float robot_height_scsp = TO_SCREENSPACE(robot_height, 0, state->width);
	float robot_width_scsp = robot_width * state->width * aspect_ratio;
	float robot_height_scsp = robot_height * state->height;

	SDL_SetRenderDrawColor(state->renderer, 255, 0, 0, 255);

	SDL_FRect arm_rect = {screenspace_robot_center_x, screenspace_robot_center_y - (36./180) * state->height / 4, (96. / 180) * state->width * aspect_ratio / 2, (36. / 180) * state->height / 2};
	SDL_FPoint arm_center = {0, (36. / 180) * state->height / 4};
	if(mouse_down) {
		SDL_RenderTextureRotated(state->renderer, state->armclose_sprite, NULL, &arm_rect, real_arm_angle / M_PI * 180, &arm_center, SDL_FLIP_NONE);
	} else {
		SDL_RenderTextureRotated(state->renderer, state->armopen_sprite, NULL, &arm_rect, real_arm_angle / M_PI * 180, &arm_center, SDL_FLIP_NONE);
	}

	SDL_FRect robot_rect = {screenspace_robot_center_x - robot_width_scsp / 2, screenspace_robot_center_y - robot_height_scsp / 2, robot_width_scsp, robot_height_scsp};

	SDL_RenderTexture(state->renderer, state->robot_sprite, NULL, &robot_rect);
	//SDL_RenderRect(state->renderer, &robot_rect);
	//SDL_BlitSurface(st->robot_sprite, , );

	SDL_RenderLine(state->renderer, screenspace_robot_center_x, screenspace_robot_center_y, screenspace_arm_center_x, screenspace_arm_center_y);

	SDL_RenderLine(state->renderer, 0, floor_y, 800, floor_y);

	SDL_RenderPresent(state->renderer);

	end_ball_update_2d(&state->robot, 1/60.);
	end_ball_update_2d(&state->arm_end, 1/60.);

	return SDL_APP_CONTINUE;
}
