This will let you call js promise-functions from C wasm, with callbacks instead of asyncify/etc.

### host

I currently only have JS (browser/node/etc) host made:

```js
import { wrapPromise, setCallback } from 'wasm_promise_callbacks'

const example = () => new Promise(resolve => setTimeout(resolve, 1000))

const imports = {
  env: {
    example: wrapPromise(example),
    set_callback: setCallback
  }
}

const wasmBytes = '...'
const mod = await WebAssembly.compile(wasmBytes)
const instance = await WebAssembly.instantiate(mod, imports)

instance.exports.test()
```

### wasm


#### C

```c
#include "wasm_promise_callbacks.h"

// this is exposed function that normally retuns a promise
__attribute__((import_module("env"), import_name("example")))
int example();

void finished() {
  // this will be called when it's done
}

__attribute__((export_name("test")))
void test() {
  // Initialize the callback system
  wasm_promise_callbacks_init();

  // Call the async function and register callback
  wasm_promise_callbacks_register(example(), &finished);
}
```
