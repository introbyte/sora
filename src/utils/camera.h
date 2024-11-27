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
	vec3_t camera_pos;
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

#endif // CAMERA_H