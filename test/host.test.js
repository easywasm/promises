import { test } from "node:test";
import { readFile } from 'node:fs/promises'
import { wrapPromise, callPromise } from '../callback_system.js'

import MemoryView from '@easywasm/memory'
import WasiPreview1 from '@easywasm/wasi'

// this is sort of boilerplate
// gives you a WASI instance with nice mem utils (dataview, string, etc)
async function getWasm(wasmBytes, env = {}) {
  const wasi_snapshot_preview1 = new WasiPreview1()
  const mod = (await WebAssembly.instantiate(wasmBytes, { env, wasi_snapshot_preview1 })).instance.exports
  wasi_snapshot_preview1.start(mod)
  const view = new MemoryView(mod.memory, mod.malloc)
  return { ...mod, view }
}

const bytes = await readFile('test/test.wasm')

// this test should wait for response from example() in wasm, which will call http_request in host and wait for it to resolve
test("wasm can call host function", async ({ assert }) => {
  let calls = 0

  const env = {
    http_get: wrapPromise(() => w, async (urlPtr) => {
      assert.equal(w.view.getString(urlPtr), 'https://example.com')
      calls++
      return w.view.setString('FAKE HTTP RESPONSE')
    })
  }

  const w = await getWasm(bytes, env);

  // this waits for the wasm to fire it's callback (which will call http_request, and return)
  const r = await callPromise(w, w.example)
  assert.equal(w.view.getString(r), 'Ok, I got the data: 18')

  assert.equal(calls, 1)
});
