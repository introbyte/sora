// camera.cpp

#ifndef CAMERA_CPP
#define CAMERA_CPP

// implementation

function camera_t*
camera_create(arena_t* arena, os_handle_t window, gfx_handle_t renderer, f32 fov, f32 z_near, f32 z_far) {
    
	camera_t* camera = (camera_t*)arena_alloc(arena, sizeof(camera_t));
    
	// fill struct
	camera->window = window;
	camera->renderer = renderer;
	camera->mode = camera_mode_free;
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
    camera->mouse_is_down = false;
	//os_set_cursor(os_cursor_null);
    
	return camera;
}

function void
camera_release(camera_t* camera) {
    
}

function void
camera_update(camera_t* camera, uvec2_t size) {
    
	// get delta time
	uvec2_t window_size = os_window_get_size(camera->window);
	f32 et = os_window_get_elapsed_time(camera->window);
	f32 dt = os_window_get_delta_time(camera->window);
    
	// get input
	f32 forward_input = 0.0f;
	f32 right_input = 0.0f;
	f32 up_input = 0.0f;
	f32 roll_input = 0.0f;
	f32 pitch_input = 0.0f;
	f32 yaw_input = 0.0f;
	f32 speed = 2.5f;
    
    // only get input if window is focused
    if (os_window_is_active(camera->window)) {
        forward_input = (f32)(os_key_is_down(os_key_W) - os_key_is_down(os_key_S));
        right_input = (f32)(os_key_is_down(os_key_D) - os_key_is_down(os_key_A));
        up_input = (f32)(os_key_is_down(os_key_space) - os_key_is_down(os_key_ctrl));
        roll_input = (f32)(os_key_is_down(os_key_E) - os_key_is_down(os_key_Q));
        speed = os_key_is_down(os_key_shift) ? 10.0f : 2.5f;
    }
    
    // mouse input
    if (os_mouse_press(camera->window, os_mouse_button_right)) {
        camera->mouse_start =  os_window_get_cursor_pos(camera->window);
        camera->mouse_is_down = true;
        os_set_cursor(os_cursor_null);
    }
    
    if (os_mouse_release(camera->window, os_mouse_button_right)) {
        camera->mouse_is_down = false;
        os_set_cursor(os_cursor_pointer);
    }
    
    if (camera->mouse_is_down) {
        vec2_t mouse_pos = os_window_get_cursor_pos(camera->window);
        vec2_t delta = vec2_sub(camera->mouse_start, mouse_pos);
        os_window_set_cursor_pos(camera->window, camera->mouse_start);
        yaw_input = delta.x;
        pitch_input = delta.y;
    }
    
	// mouse delta
    /*if (os_window_is_active(camera->window)) {
        vec2_t mouse_pos = os_window_get_cursor_pos(camera->window);
        vec2_t delta = vec2_sub(vec2(window_size.x *0.5f, window_size.y * 0.5f), mouse_pos);
        if (camera->fps_lock) {
            os_window_set_cursor_pos(camera->window, vec2(window_size.x * 0.5f, window_size.y * 0.5f));
            yaw_input = delta.x;
            pitch_input = -delta.y;
        }
        
    }*/
    
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
    
	const f32 sensitivity = 1.0f;
	f32 zoom_adjustment = (camera->fov / 160.0f);
	quat_t pitch = quat_from_axis_angle({ 1.0f, 0.0f, 0.0f }, zoom_adjustment * sensitivity * pitch_input * dt);
	quat_t yaw = quat_from_axis_angle({ 0.0f, 1.0f, 0.0f }, zoom_adjustment * sensitivity * yaw_input * dt);
	quat_t roll = quat_from_axis_angle({ 0.0f, 0.0f, 1.0f }, 2.5f * sensitivity * roll_input * dt);
    
	// orientation
	if (camera->mode & camera_mode_disable_roll) {
		camera->target_orientation = quat_mul(pitch, camera->target_orientation);
		camera->target_orientation = quat_mul(camera->target_orientation, yaw);
	} else {
		camera->target_orientation = quat_mul(pitch, camera->target_orientation);
		camera->target_orientation = quat_mul(yaw, camera->target_orientation);
		camera->target_orientation = quat_mul(roll, camera->target_orientation);
	}
    
	// smooth orientation
	camera->orientation = quat_slerp(camera->orientation, camera->target_orientation, 30.0f * dt);
    
	// translate
	vec3_t translation = vec3_mul(vec3_normalize(vec3(right_input, up_input, forward_input)), dt * speed);
	vec3_t rotated_translation = vec3_rotate(translation, camera->orientation);
	camera->target_position = vec3_add(camera->target_position, rotated_translation);
	camera->position = vec3_lerp(camera->position, camera->target_position, 60.0f * dt);
    
	// update fov
	f32 scroll_delta = os_mouse_scroll(camera->window);
	camera->target_fov -= scroll_delta * 1.5f;
	camera->target_fov = clamp(camera->target_fov, 1.0f, 160.0f);
	camera->fov = lerp(camera->fov, camera->target_fov, 30.0f * dt);
    
	// update constants
	camera->constants.view = mat4_mul(mat4_from_quat(camera->orientation), mat4_translate(vec3_negate(camera->position)));
	camera->constants.inv_view = mat4_inverse(camera->constants.view);
	camera->constants.projection = mat4_perspective(camera->fov, (f32)size.x / (f32)size.y, camera->z_near, camera->z_far);
	camera->constants.inv_projection = mat4_inverse(camera->constants.projection);
	camera->constants.view_projection = mat4_mul(camera->constants.projection, camera->constants.view);
	camera->constants.camera_pos = camera->position;
	camera->constants.window_size = { (f32)window_size.x, (f32)window_size.y };
	camera->constants.time = vec2(et, dt);
}




#endif // CAMERA_CPP