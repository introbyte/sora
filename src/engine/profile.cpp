// profile.cpp

#ifndef PROFILE_CPP
#define PROFILE_CPP

//- implementation  

function void
pf_init() {
    
    pf_state.arena = arena_create(megabytes(128));
    
    pf_state.entry_first = nullptr;
    pf_state.entry_last = nullptr;
    pf_state.entry_free = nullptr;
    
}

function void
pf_release() {
    arena_release(pf_state.arena);
}

function void
pf_begin(cstr name) {
    
    // hash
    u64 hash = str_hash(0, str(name));
    
    pf_entry_t* entry = nullptr;
    
    // try to find existing entry
    for (pf_entry_t* e = pf_state.entry_first; e != nullptr; e = e->next) {
        if (e->hash == hash) {
            entry = e;
            break;
        }
    }
    
    // if we didn't find one, create one
    if (entry == nullptr) {
        entry = pf_state.entry_free;
        if (entry != nullptr) {
            stack_pop(pf_state.entry_free);
        } else {
            entry = (pf_entry_t*)arena_alloc(pf_state.arena, sizeof(pf_entry_t));
        }
        memset(entry, 0, sizeof(pf_entry_t));
        dll_push_back(pf_state.entry_first, pf_state.entry_last, entry);
        entry->hash = hash;
        entry->name = name;
    }
    
    entry->start_time = prof_get_timestamp();
    
}

function void
pf_end(cstr name) {
    
    // hash
    u64 hash = str_hash(0, str(name));
    
    pf_entry_t* entry = nullptr;
    
    // try to find existing entry
    for (pf_entry_t* e = pf_state.entry_first; e != nullptr; e = e->next) {
        if (e->hash == hash) {
            entry = e;
            break;
        }
    }
    
    if (entry != nullptr) {
        entry->end_time = prof_get_timestamp();
        entry->elapsed_time = entry->end_time - entry->start_time;
    }
}

#endif // PROFILE_CPP