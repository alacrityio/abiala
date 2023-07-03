// copyright defined in abiala/LICENSE.txt

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct abiala_context_s abiala_context;
typedef int abiala_bool;

// Create a context. The context holds all memory allocated by functions in this header. Returns null on failure.
abiala_context* abiala_create();

// Destroy a context.
void abiala_destroy(abiala_context* context);

// Get last error. Never returns null. The context owns the returned string.
const char* abiala_get_error(abiala_context* context);

// Get generated binary. The context owns the returned memory. Functions return null on error; use abiala_get_error to
// retrieve error.
int abiala_get_bin_size(abiala_context* context);
const char* abiala_get_bin_data(abiala_context* context);

// Convert generated binary to hex. The context owns the returned string. Returns null on error; use abiala_get_error to
// retrieve error.
const char* abiala_get_bin_hex(abiala_context* context);

// Name conversion. The context owns the returned memory. Functions return null on error; use abiala_get_error to
// retrieve error.
uint64_t abiala_string_to_name(abiala_context* context, const char* str);
const char* abiala_name_to_string(abiala_context* context, uint64_t name);

// Set abi (JSON format). Returns false on error.
abiala_bool abiala_set_abi(abiala_context* context, uint64_t contract, const char* abi);

// Set abi (binary format). Returns false on error.
abiala_bool abiala_set_abi_bin(abiala_context* context, uint64_t contract, const char* data, size_t size);

// Set abi (hex format). Returns false on error.
abiala_bool abiala_set_abi_hex(abiala_context* context, uint64_t contract, const char* hex);

// Get the type name for an action. The context owns the returned memory. Returns null on error; use abiala_get_error
// to retrieve error.
const char* abiala_get_type_for_action(abiala_context* context, uint64_t contract, uint64_t action);

// Get the type name for a table. The context owns the returned memory. Returns null on error; use abiala_get_error
// to retrieve error.
const char* abiala_get_type_for_table(abiala_context* context, uint64_t contract, uint64_t table);

// Get the type name for an action_result. The context owns the returned memory. Returns null on error; use
// abiala_get_error to retrieve error.
const char* abiala_get_type_for_action_result(abiala_context* context, uint64_t contract, uint64_t action_result);

// Convert json to binary. Use abiala_get_bin_* to retrieve result. Returns false on error.
abiala_bool abiala_json_to_bin(abiala_context* context, uint64_t contract, const char* type, const char* json);

// Convert json to binary. Allow json field reordering. Use abiala_get_bin_* to retrieve result. Returns false on error.
abiala_bool abiala_json_to_bin_reorderable(abiala_context* context, uint64_t contract, const char* type,
                                           const char* json);

// Convert binary to json. The context owns the returned string. Returns null on error; use abiala_get_error to retrieve
// error.
const char* abiala_bin_to_json(abiala_context* context, uint64_t contract, const char* type, const char* data,
                               size_t size);

// Convert hex to json. The context owns the returned memory. Returns null on error; use abiala_get_error to retrieve
// error.
const char* abiala_hex_to_json(abiala_context* context, uint64_t contract, const char* type, const char* hex);

// Convert abi json to bin, Use abiala_get_bin_* to retrieve result. Returns false on error.
abiala_bool abiala_abi_json_to_bin(abiala_context* context, const char* json);

// Convert abi bin to json, The context.result_str has the result, Returns null on error; use abiala_get_error to
// retrieve
const char* abiala_abi_bin_to_json(abiala_context* context, const char* abi_bin_data, const size_t abi_bin_data_size);

#ifdef __cplusplus
}
#endif
