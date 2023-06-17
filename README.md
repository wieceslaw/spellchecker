## Requirements

- Cmake, Makefile
- Clang (C++17)

## Build

```bash
cmake -DCMAKE_BUILD_TYPE=<type> -B ./.build # <type> = Debug | Sanitized | Release
make -C .build
```

## Clean

```bash
make -C .build clean
```

## Run

```bash
.build/01-spellchecker file [options]

Options:
-i <double>
  To specify minimum index of tracked words

-w <int>
  To specify maximum number of tracked words 
```
