# alloc

A C11 memory allocator implementation for Linux using `brk`/`sbrk`, based on Marwan Burelle's "A Malloc Tutorial" (EPITA/LSE, 2009).

## Differences
- Uses C11 `_Alignof(max_align_t)` / `_Alignas` replacing the hardcoded 32-bit `align4`.
- Uses `_Alignas(max_align_t) char data[]` ensuring payload offsets match platform alignment.
- All internal helper functions and `global_base` use `static` linkage.
- `calloc` includes integer overflow protection.
-  `split_block` splits when there will be atleast `align(1)` byte space left instead of hardcoded 4 bytes.

## Build & Test

```bash
make test   # Build and run unit test suite
make debug  # Build and run with debug symbols (-g3 -O0)
make clean  # Clean build artifacts
```
