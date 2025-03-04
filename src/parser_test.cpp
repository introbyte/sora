// parser_test.cpp

//- includes 
#include "engine/base.h"
#include "engine/os.h"

#include "engine/base.cpp"
#include "engine/os.cpp"

#include "projects/utils/codegen.h"
#include "projects/utils/codegen.cpp"

//- globals 

global str_t keywords[] = {
    str("int"),
};

//- functions 

function i32 
app_entry_point(i32 argc, char** argv) {
    
    arena_t* arena = arena_create(gigabytes(2));
    
    // open file
    os_handle_t file = os_file_open(str("res/scripts/test.ksl"));
    str_t file_content = os_file_read_all(arena, file);
    os_file_close(file);
    
    // create cg state and set keywords
    cg_state_t* cg_state = cg_init();
    cg_add_keywords(cg_state, keywords, array_count(keywords));
    
    // convert to tokens
    cg_token_list_t token_list = cg_token_list_from_string(cg_state, file_content);
    
    // convert to ast
    cg_node_t* root = cg_ast_from_token_list(cg_state, token_list);
    
    i32 depth = 0;
    for (cg_node_t* node = root, *next = nullptr; node != nullptr; node = next) {
        cg_node_rec_t rec = cg_node_rec_depth_first_pre(node, root);
        next = rec.next;
        char indents[] = "                                 "; 
        
        
        str_t type = cg_string_from_node_type(node->type);
        str_t value;
        if (node->type == cg_node_type_operator) {
            value = cg_string_from_op_type(node->op_type);
        } else if (node->type == cg_node_type_numeric){
            value = str_format(arena, "%.2f", node->value._f32);
        }
        printf("%.*s[%s] : %s\n", depth * 3, indents, type.data, value.data);
        
        if (rec.push_count != 0) {
            printf("%.*s{\n", depth * 3, indents);
        }
        depth += rec.push_count;
        
        for (i32 pop_index = 0; pop_index < rec.pop_count; pop_index++) {
            printf("%.*s}\n", (depth-1-pop_index) * 3, indents);
        }
        depth -= rec.pop_count;
    }
    
    // cleanup
    cg_release(cg_state);
    arena_release(arena);
    
    return 0;
}