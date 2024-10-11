// camera.h

#ifndef CAMERA_H
#define CAMERA_H

// enum

typedef u32 camera_mode;
enum {
	camera_mode_disable_roll = (1 << 0),
	camera_mode_disable_move_in_world_plane = (1 << 1),
	camera_mode_clamp_pitch = (1 << 2),
	camera_mode_clamp_yaw = (1 << 3),
	camera_mode_clamp_roll = (1 << 4),

	camera_mode_free = 0,
	camera_mode_first_person = camera_mode_disable_roll | camera_mode_disable_move_in_world_plane | camera_mode_clamp_pitch,
	camera_mode_orbit = camera_mode_clamp_roll | camera_mode_clamp_pitch

};

// struct

struct camera_constants_t {
	mat4_t view_projection;
	mat4_t view;
	mat4_t projection;
	mat4_t inv_view;
	mat4_t inv_projection;
	vec2_t window_size;
	vec2_t time;
};

struct camera_t {

	// context
	os_window_t* window;
	gfx_renderer_t* renderer;

	camera_mode mode;
	camera_constants_t constants;

	// position
	vec3_t target_position;
	vec3_t position;

	// orientation
	quat_t target_orientation;
	quat_t orientation;

	f32 target_fov;

	f32 fov;
	f32 z_near;
	f32 z_far;
	f32 min_pitch;
	f32 max_pitch;
	f32 min_yaw;
	f32 max_yaw;
	f32 min_roll;
	f32 max_roll;

	b8 fps_lock;
	vec2_t last_mouse_pos;
};

// functions

function camera_t* camera_create(arena_t*, gfx_renderer_t*, f32, f32, f32);
function void camera_release(camera_t*);
function void camera_update(camera_t*);

// implementation

function camera_t*
camera_create(arena_t* arena, gfx_renderer_t* renderer, f32 fov, f32 z_near, f32 z_far) {

	camera_t* camera = (camera_t*)arena_alloc(arena, sizeof(camera_t));

	// fill struct
	camera->window = renderer->window;
	camera->renderer = renderer;
	camera->mode = camera_mode_first_person;
	camera->target_fov = fov;
	camera->fov = fov;
	camera->z_near = z_near;
	camera->z_far = z_far;
	camera->target_position = { 0.0f, 1.0f, 0.0f };
	camera->position = { 0.0f, 1.0f, 0.0f };
	camera->target_orientation = { 0.0f, -1.0f, 0.0f, 0.0f };
	camera->orientation = { 0.0f, -1.0f, 0.0f, 0.0f };
	camera->min_pitch = -f32_pi / 2.0f;
	camera->max_pitch = f32_pi / 2.0f;
	camera->min_yaw = 0.0f;
	camera->max_yaw = 0.0f;
	camera->min_roll = 0.0f;
	camera->max_roll = 0.0f;
	camera->fps_lock = true;
	camera->last_mouse_pos = vec2(0.0f, 0.0f);

	return camera;
}

function void
camera_release(camera_t* camera) {

}

function void
camera_update(camera_t* camera) {

	// get delta time
	f32 dt = camera->window->delta_time;
	f32 fast_rate = 1.0f - powf(2.0f, -50.0f * dt);

	// toggle fps lock
	if (os_key_press(camera->window, os_key_tab)) {
		camera->fps_lock = !camera->fps_lock;
	}

	// get input
	f32 forward_input = 0.0f;
	f32 right_input = 0.0f;
	f32 up_input = 0.0f;
	f32 roll_input = 0.0f;
	f32 pitch_input = 0.0f;
	f32 yaw_input = 0.0f;
	f32 speed = 1.2f;

	forward_input = (f32)(os_key_is_down(os_key_S) - os_key_is_down(os_key_W));
	right_input = (f32)(os_key_is_down(os_key_D) - os_key_is_down(os_key_A));
	up_input = (f32)(os_key_is_down(os_key_space) - os_key_is_down(os_key_ctrl));
	roll_input = (f32)(os_key_is_down(os_key_Q) - os_key_is_down(os_key_E));
	speed = os_key_is_down(os_key_shift) ? 5.0f : 1.2f;

	// mouse delta
	vec2_t mouse_pos = os_get_cursor_pos(camera->window);
	vec2_t delta = vec2_sub(camera->last_mouse_pos, mouse_pos);
	if (camera->fps_lock) {
		os_set_cursor_pos(camera->window, vec2(camera->window->resolution.x * 0.5f, camera->window->resolution.y * 0.5f));
		pitch_input = delta.y;
		yaw_input = delta.x;
	}

	camera->last_mouse_pos = os_get_cursor_pos(camera->window);

	// clamp input
	vec3_t euler_angle = quat_to_euler_angle(camera->target_orientation);

	if (camera->mode & camera_mode_clamp_pitch) {
		pitch_input = max(degrees(camera->min_pitch - euler_angle.x), pitch_input);
		pitch_input = min(degrees(camera->max_pitch - euler_angle.x), pitch_input);
	}

	if (camera->mode & camera_mode_clamp_yaw) {
		yaw_input = max(degrees(camera->min_yaw - euler_angle.y), yaw_input);
		yaw_input = min(degrees(camera->max_yaw - euler_angle.y), yaw_input);
	}

	if (camera->mode & camera_mode_clamp_roll) {
		roll_input = max(degrees(camera->min_roll - euler_angle.z), roll_input);
		roll_input = min(degrees(camera->max_roll - euler_angle.z), roll_input);
	}

	const f32 sensitivity = 0.6f;
	f32 zoom_adjustment = (camera->fov / 130.0f);
	quat_t pitch = quat_axis_angle({ 1.0f, 0.0f, 0.0f }, zoom_adjustment * sensitivity * pitch_input * dt);
	quat_t yaw = quat_axis_angle({ 0.0f, 1.0f, 0.0f }, zoom_adjustment * sensitivity * yaw_input * dt);
	quat_t roll = quat_axis_angle({ 0.0f, 0.0f, 1.0f }, 2.5f * sensitivity * roll_input * dt);

	// orientation
	if (camera->mode & camera_mode_disable_roll) {
		camera->target_orientation = quat_mul(camera->target_orientation, pitch);
		camera->target_orientation = quat_mul(yaw, camera->target_orientation);
	} else {
		camera->target_orientation = quat_mul(camera->target_orientation, pitch);
		camera->target_orientation = quat_mul(camera->target_orientation, yaw);
		camera->target_orientation = quat_mul(camera->target_orientation, roll);
	}

	// smooth orientation
	camera->orientation = quat_slerp(camera->orientation, camera->target_orientation, 30.0f * dt);
	camera->orientation = quat_normalize(camera->orientation);

	// translate
	vec3_t translation = vec3_mul(vec3_normalize(vec3(right_input, up_input, forward_input)), dt * speed);
	vec3_t rotated_translation = vec3_rotate(translation, camera->orientation);
	camera->target_position = vec3_add(camera->target_position, rotated_translation);
	camera->position = vec3_lerp(camera->position, camera->target_position, 30.0f * dt);

	// update fov
	f32 scroll_delta = os_mouse_scroll(camera->window);
	camera->target_fov -= scroll_delta * 1.5f;
	camera->target_fov = clamp(camera->target_fov, 1.0f, 130.0f);
	camera->fov = lerp(camera->fov, camera->target_fov, 30.0f * dt);

	// update constants
	camera->constants.view = mat4_mul(mat4_from_quat(camera->orientation), mat4_translate(vec3_negate(camera->position)));
	camera->constants.inv_view = mat4_inverse(camera->constants.view);
	camera->constants.projection = mat4_perspective(camera->fov, (f32)camera->window->resolution.x / (f32)camera->window->resolution.y, camera->z_near, camera->z_far);
	camera->constants.inv_projection = mat4_inverse(camera->constants.projection);
	camera->constants.view_projection = mat4_mul(camera->constants.projection, camera->constants.view);
	camera->constants.window_size = { (f32)camera->window->resolution.x, (f32)camera->window->resolution.y };
	camera->constants.time = vec2(camera->window->elasped_time, camera->window->delta_time);
}

#endif // CAMERA_H