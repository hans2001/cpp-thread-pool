# Release Checklist

## Targets
- `tp` library builds cleanly.
- Examples build: `example_hello`, `example_parallel_sum`, `example_stop_token`.
- Tests build: `test_basic`, `test_future`, `test_shutdown`, `test_stop_token`, `test_stress`.
- Benchmarks build: `bench_throughput`, `bench_latency`.

## Compilers
- GCC (>= 11) with `-std=c++20`.
- Clang (>= 14) with `-std=c++20`.

## Platforms
- Linux x86_64.
- macOS (Apple Clang) if targeted.

## Sanity Checks
- Run tests: all pass.
- Run at least one benchmark to confirm no regressions.
- Verify shutdown drains tasks and no deadlocks.

## Packaging
- Tag release version.
- Update `README.md` if public API changed.
- Ensure `TASKS.md` reflects completed items.
