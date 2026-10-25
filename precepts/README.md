# EE209 Precept Code

A cleaned, GitHub-friendly organization of the EE209 precept source-code archive.

The original archive used numeric folders (`2/`, `3/`, ..., `12/`) with a mix of `src/` and `demo/` directories. This repository keeps the precept numbers, groups examples by topic, and preserves every original `.c`, `.h`, and `.s` file byte-for-byte.

## Repository layout

```text
precepts/
├── 02-c-fundamentals/
├── 03-multi-file-programs/
├── 04-arrays-pointers-and-functions/
├── 05-pointers-memory-and-structures/
├── 06-dynamic-arrays/
├── 07-ia32-assembly-basics/
├── 08-ia32-functions-and-addressing/
├── 11-lexing-and-process-control/
└── 12-file-descriptors-and-signals/
```

Only precepts that had code in the supplied archive are present. There were no code files for Precepts 01, 09, or 10 in that archive.

## Contents

| Precept | Topic grouping | Examples included |
|---|---|---|
| 02 | C fundamentals | data types, symbolic constants, a simple circle program, multi-file C |
| 03 | Multi-file programs | header/source separation and a small integer-math module |
| 04 | Arrays, pointers, and functions | arrays, pointer swapping, function pointers, command-line arguments, a simple `cp` example |
| 05 | Pointers, memory, and structures | pointer operators, dynamic-memory/sorting examples, structures |
| 06 | Dynamic arrays | `DynArray` implementation, header, and test program |
| 07 | IA-32 assembly basics | C/flattened-C/assembly comparisons, control transfer, signed and unsigned power examples |
| 08 | IA-32 functions and addressing | function calling, base+displacement addressing, indirect addressing |
| 11 | Lexing and process control | DFA-based lexical analysis, `fork`, `exec`, `wait`, process examples |
| 12 | File descriptors and signals | `dup`, redirection, `fork`+`exec`, signal handling and ignoring signals |

## Notes

- The assembly examples use 32-bit IA-32 / x86 AT&T-style syntax. On a modern 64-bit Linux system, building them may require a 32-bit toolchain and libraries (commonly via `gcc -m32`).
- Several process, file-descriptor, and signal examples assume a Unix/POSIX environment.
- `precepts/11-lexing-and-process-control/lexing/dfa.c` includes `dynarray.h`; the matching `DynArray` implementation is under `precepts/06-dynamic-arrays/dynarray/` in the supplied archive.
- Source comments naming original authors/modifiers were intentionally preserved.

## Original-path mapping

See [`ORIGINAL_PATHS.md`](ORIGINAL_PATHS.md) for a complete mapping from the supplied archive paths to this repository layout.

## Licensing / redistribution

The supplied source files contain multiple original-author and modifier attributions, but the archive did not include an explicit repository-wide open-source license. No license has been added here. Before publishing this repository publicly or relicensing the code, verify that you have permission to redistribute the course materials and preserve the existing attribution comments.
