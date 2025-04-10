#ifndef CALLBACK_SYSTEM_H
#define CALLBACK_SYSTEM_H

#include <stddef.h>

#define WASM_EXPORT(name) __attribute__((export_name(#name)))
#define WASM_IMPORT(name) __attribute__((import_module("env"))) __attribute__((import_name(#name)))

// Callback function type
typedef void (*callback_fn)(void*);

// Maximum number of promises that can be tracked
#define MAX_PROMISES 32

// Structure to store callback information
typedef struct {
    int in_use;
    callback_fn callback;
} promise_callback_t;

// Array to store callbacks for each promise ID
static promise_callback_t callbacks[MAX_PROMISES];

// Initialize the callback system
WASM_EXPORT(init_callback_system)
void init_callback_system() {
    for (int i = 0; i < MAX_PROMISES; i++) {
        callbacks[i].in_use = 0;
        callbacks[i].callback = NULL;
    }
}

// Register a callback for a promise
WASM_EXPORT(wasm_promise_callbacks_register)
void wasm_promise_callbacks_register(int promise_id, callback_fn callback) {
    if (promise_id >= 0 && promise_id < MAX_PROMISES) {
        callbacks[promise_id].in_use = 1;
        callbacks[promise_id].callback = callback;
    }
}

// Function to be called by the host when a promise is resolved
WASM_EXPORT(resolve_promise)
void resolve_promise(int promise_id, void* data) {
    if (promise_id >= 0 && promise_id < MAX_PROMISES &&
        callbacks[promise_id].in_use &&
        callbacks[promise_id].callback != NULL) {

        // Call the registered callback with the provided data
        callbacks[promise_id].callback(data);

        // Mark the promise as no longer in use
        callbacks[promise_id].in_use = 0;
        callbacks[promise_id].callback = NULL;
    }
}

#endif // CALLBACK_SYSTEM_H
