This will let you call js promise-functions from C wasm, with callbacks instead of asyncify/etc.

Essentially, you pick a host implementation, and a wasm-implementation for your language, and they will allow callbacks, and it's able to respond.

### host

#### JS (node/browser/etc)

```js
import { wrapPromise, callPromise } from "wasm_promise_callbacks";

const imports = {
  env: {
    // here is a promise-returning function that the wasm will get the output of
    example_host_func: wrapPromise((messageP) => new Promise((resolve) => setTimeout(() => resolve('nice!'), 1000)))
  },
};

const wasmBytes = "...";
const mod = await WebAssembly.compile(wasmBytes);
const instance = await WebAssembly.instantiate(mod, imports);

// wait for getting the response back
const response = await callPromise(instance.exports, instance.exports.test);
```

### wasm

#### C

For a C example, that can perform http-requests and process it, see:

- [test.c](test/test.c)
- [host.test.js](test/host.test.js)

I am faking http, but you can see the result is the output of `mycallback`
