#ifndef __OVERSTACKED_H_INCLUDED_F43A938B4202DE9C__
#define __OVERSTACKED_H_INCLUDED_F43A938B4202DE9C__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum overstacked_value_type {
    OVERSTACKED_TNIL = 0,
    OVERSTACKED_TNUMERIC = 2,
    OVERSTACKED_TBOOLEAN = 3,
    OVERSTACKED_TWORD = 128,
    OVERSTACKED_TWORD_ASSIGN = 129,
    OVERSTACKED_TWORD_DECLARE = 130,
    OVERSTACKED_TNATIVE_HANDLE = 132,
    OVERSTACKED_TARRAY = 388,
    OVERSTACKED_TPLACEHOLDER = 389
};

struct overstacked_value {
    enum overstacked_value_type type;
    union {
        bool boolean_value;
        int64_t numeric_value;
        struct overstacked_string *str;
        struct overstacked_array *arr;
        struct overstacked_native_handle *handle;
    };
};

enum overstacked_exec_result_type {
    OVERSTACKED_ER_ERROR = 0,
    OVERSTACKED_ER_SUCCESS = 1,
    OVERSTACKED_ER_RETURN = 2,
    OVERSTACKED_ER_BREAK = 3,
    OVERSTACKED_ER_CONTINUE = 4,
    OVERSTACKED_ER_CUSTOM = 5
};

struct overstacked_exec_result {
    enum overstacked_exec_result_type *type;
    const char32_t *error;
    overstacked_value val;
};

struct overstacked_api_table {
    size_t api_version;
    void *ctx;
    void *lib_ctx;
    bool (*eval_stack_pop_back)(void *ctx, struct overstacked_value *val);
    bool (*eval_stack_assert_depth)(size_t, void *ctx);
    void (*eval_stack_push_back)(struct overstacked_value *val, void *ctx);
    void (*sym_table_create_scope)(void *ctx);
    void (*sym_table_leave_scope)(void *ctx);
    void (*call_interpreter)(struct overstacked_array *arr, bool new_scope,
                             void *ctx, struct overstacked_exec_result *buf);
    struct overstacked_value *(*get_array_data)(struct overstacked_array *arr,
                                                void *ctx);
    size_t (*get_array_size)(struct overstacked_array *arr, void *ctx);
    const char32_t *(*get_string_data)(struct overstacked_string *str,
                                       void *ctx);
    size_t (*get_string_size)(struct overstacked_string *str, void *ctx);
    struct overstacked_string *(*make_string)(const char32_t *str, void *ctx);
    struct overstacked_array *(*make_array)(struct overstacked_value *val,
                                            size_t size, void *ctx);
};

#ifdef __cplusplus
}
#endif

#endif
