// log.cpp

#ifndef LOG_CPP
#define LOG_CPP

// implementation

function void 
log_init() {

	log_state.arena = arena_create(megabytes(64));
	log_state.scratch = arena_create(megabytes(64));

}

function void 
log_release() {

	arena_release(log_state.arena);
	arena_release(log_state.scratch);

}

function void
log_print() {

	for (log_t* log = log_state.log_first; log != 0; log = log->next) {
		printf("%.*s\n", log->string.size, log->string.data);
	}

}

function void
log_msg(log_level level, str_t string) {
	log_t* log = (log_t*)arena_alloc(log_state.arena, sizeof(log_t));
	log->level = level;
	str_t level_string = log_level_to_str(level);
	log->string = str_format(log_state.arena, "[%.*s] %.*s", level_string.size, level_string.data, string.size, string.data);
	queue_push(log_state.log_first, log_state.log_last, log);
}

function void
log_msgf(log_level level, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	str_t formatted_string = str_formatv(log_state.scratch, fmt, args);
	log_msg(level, formatted_string);
	va_end(args);
	arena_clear(log_state.scratch);
}

function str_t 
log_level_to_str(log_level level) {
	str_t label;

	switch (level) {
		default:
		case log_level_info: label = str("info"); break;
		case log_level_warn: label = str("warn"); break;
		case log_level_error: label = str("error"); break;
	}

	return label;
}


#endif // LOG_CPP