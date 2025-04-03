#ifndef WASM_PROMISE_CALLBACKS_H
#define WASM_PROMISE_CALLBACKS_H

#ifndef NULL
#define NULL 0
#endif

// Register a callback to be invoked when a promise resolves
void wasm_promise_callbacks_register(int promise_id, void (*callback)());

// Initialize the callback system
void wasm_promise_callbacks_init();

// Invoke a callback by ID
void invoke_callback(int callback_id);

// Maximum number of callbacks to register
#define MAX_CALLBACKS 100

// Callback function type
typedef void (*callback_fn)();

// Array to store callbacks
static callback_fn callbacks[MAX_CALLBACKS];
static int next_callback_id = 1;

// Import the JavaScript function that will store our callback
__attribute__((import_module("env"), import_name("set_callback")))
void set_callback(int promise_id, int callback_id);

// Register a callback to be invoked when a promise resolves
void wasm_promise_callbacks_register(int promise_id, void (*callback)()) {
    int callback_id = next_callback_id++;

    if (callback_id < MAX_CALLBACKS) {
        callbacks[callback_id] = callback;
        set_callback(promise_id, callback_id);
    }
}

// Initialize the callback system
void wasm_promise_callbacks_init() {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        callbacks[i] = NULL;
    }
    next_callback_id = 1;
}

// Exported function to invoke a callback by ID
__attribute__((export_name("invoke_callback")))
void invoke_callback(int callback_id) {
    if (callback_id > 0 && callback_id < MAX_CALLBACKS && callbacks[callback_id] != NULL) {
        callback_fn callback = callbacks[callback_id];
        callbacks[callback_id] = NULL;  // Clear after use
        callback();
    }
}

#endif
