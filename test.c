#include "wasm_promise_callbacks.h"

__attribute__((import_module("env"), import_name("example")))
int example();

// this is just to tell host we got the callback
__attribute__((import_module("env"), import_name("notify_finish")))
void notify_finish();

void finished() {
  notify_finish();
}

__attribute__((export_name("test")))
void test() {
  wasm_promise_callbacks_init();
  wasm_promise_callbacks_register(example(), &finished);
}
