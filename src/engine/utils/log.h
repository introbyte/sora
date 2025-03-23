// log.h

#ifndef LOG_H
#define LOG_H

//- defines

// overwrite log macros
#define log_info(s) log_msg(log_level_info, str(s))
#define log_infof(fmt, ...) log_msgf(log_level_info, (fmt), __VA_ARGS__)

#define log_warn(s) log_msg(log_level_warn, str(s))
#define log_warnf(fmt, ...) log_msgf(log_level_warn, (fmt), __VA_ARGS__)

#define log_error(s) log_msg(log_level_error, str(s))
#define log_errorf(fmt, ...) log_msgf(log_level_error, (fmt), __VA_ARGS__)

//- enums

enum log_level {
	log_level_info,
	log_level_warn,
	log_level_error,
};

//- structs

struct log_t {
	log_t* next;
	log_level level;
	str_t string;
};

struct log_state_t {
	arena_t* arena;
	arena_t* scratch;
    
	log_t* log_first;
	log_t* log_last;
};

//- globals

global log_state_t log_state;

//- functions

function void log_init();
function void log_release();
function void log_clear();

function str_t log_level_to_str(log_level level);

function void log_msg(log_level level, str_t string);
function void log_msgf(log_level level, char* fmt, ...);

#endif // LOG_H