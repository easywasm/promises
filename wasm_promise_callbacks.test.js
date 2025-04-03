import { test } from "node:test";
import path from "node:path";
import { readFile } from "node:fs/promises";

import {
  wrapPromise,
  setCallback,
  initWasmPromiseCallbacks,
} from "./wasm_promise_callbacks.js";

const example = () => new Promise((resolve) => setTimeout(resolve, 1000));

let finishedTest = false;

const imports = {
  // wasi_snapshot_preview1,
  env: {
    example: wrapPromise(example),
    set_callback: setCallback,

    // this is example-function called by the wasm to inform host it worked
    notify_finish: () => {
      finishedTest = true;
    },
  },
};

const wasmBytes = await readFile(
  path.resolve(import.meta.dirname, "test.wasm"),
);
const mod = await WebAssembly.compile(wasmBytes);
const instance = await WebAssembly.instantiate(mod, imports);

// Initialize the promise callbacks with the instance's exports
initWasmPromiseCallbacks(instance.exports);

test("trigger callback for async function", async ({ assert }) => {
  instance.exports.test();

  // this just waits long enough for it to have been called
  await example();

  assert.equal(finishedTest, true);
});
