#include "callback_system.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// these are used by the test to get/set things in mem

WASM_EXPORT(malloc) void* export_malloc(size_t s) {
  return malloc(s);
}

WASM_EXPORT(free) void export_free(void* p) {
  free(p);
}

// Function to be implemented by host
WASM_IMPORT(http_get)
int http_get(char* url);

// Global variable to store the return value
static char return_value[256];

// Function to set the return value
void set_return_value(const char* value) {
    // In a real implementation, you might need to handle string copying more carefully
    int i = 0;
    while (value[i] && i < 255) {
        return_value[i] = value[i];
        i++;
    }
    return_value[i] = '\0';
}

// Callback function
void mycallback(void* retval) {
  char buffer[255];
  sprintf(buffer, "Ok, I got the data: %lu", strlen(retval));
  set_return_value(buffer);
}

// This will be called by host
WASM_EXPORT(example)
char* example() {
    // Initialize the return value
    set_return_value("");

    // Register the callback for the HTTP request
    wasm_promise_callbacks_register(http_get("https://example.com"), mycallback);

    // The host will need to detect that this is a promise and await its resolution
    return return_value;
}

int main() {
    init_callback_system();
    return 0;
}
