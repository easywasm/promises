// Map of pending callback IDs by promise ID
const promiseCallbacks = new Map();
let nextPromiseId = 1;
let wasmExports = null;

// Wraps a JavaScript function that returns a Promise into a function that returns a promise ID for tracking
export function wrapPromise(promiseFn) {
  return (...args) => {
    const promiseId = nextPromiseId++;

    // Call the function to get the promise
    promiseFn(...args)
      .then(() => {
        if (promiseCallbacks.has(promiseId)) {
          const callbackId = promiseCallbacks.get(promiseId);
          wasmExports.invoke_callback(callbackId);
          promiseCallbacks.delete(promiseId);
        }
      })
      .catch((err) => {
        console.error(`Promise rejected:`, err);
      });

    return promiseId;
  };
}

// Stores a callback ID to be triggered when a promise resolves
export function setCallback(promiseId, callbackId) {
  promiseCallbacks.set(promiseId, callbackId);
}

// Initialize with WebAssembly exports object
export function initWasmPromiseCallbacks(exports) {
  wasmExports = exports;
}
