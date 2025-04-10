// Map to track active promises
const promiseMap = new Map();
let nextPromiseId = 0;

export function wrapPromise(instanceGetter, asyncFn) {
  return function(...args) {
    const wasmInstance = instanceGetter()
    const promiseId = nextPromiseId++;
    const promise = Promise.resolve()
      .then(() => asyncFn(...args))
      .then(result => {
        wasmInstance.resolve_promise(promiseId, result || 0);
        promiseMap.delete(promiseId);
      })
    promiseMap.set(promiseId, promise);
    return promiseId;
  };
}

// Function to call a WASM function that might return a promise
export async function callPromise(instance, func) {
  const result = func();
  if (promiseMap.size > 0) {
    await Promise.all(promiseMap.values());
  }
  return result;
}
