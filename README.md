# mpt
A port of the [signed distance function path tracer](https://github.com/seanpringle/spt) from Go to C11.

## Differences:

* Faster than the Go implementation on a single machine.
* Slower than Go on a cluster because it can't do RPC at all!
* Uses Lua for [defining a scene](https://github.com/seanpringle/mpt/blob/master/test.lua)

![demo.png](https://raw.githubusercontent.com/wiki/seanpringle/mpt/demo.png)
