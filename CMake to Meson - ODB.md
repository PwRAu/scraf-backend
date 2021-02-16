# Passare da CMake a Meson

`odb --database pgsql --generate-query student.hpp`

Come output ottengo student-odb.hxx, student-odb.cxx, e student-odb.ixx. Il .hxx va #incluso nei file sorgente di Scraf, mentre il .cxx va compilato e linkato.

Altre opzioni utili sono:

- `--std c++17`
- `--output-dir dir`
- `--odb-file-suffix _odb`
- `--hxx-suffix .hpp`
- `--ixx-suffix .ipp`
- `--cxx-suffix .cpp`
- `-x c++20`

Con OdbCmake faccio `odb_compile(Scraf_SOURCES FILES student.hpp DB pgsql GENERATE_QUERY)` e in Scraf_SOURCES ottengo una serie di file da aggiungere ad `add_executable()`, che in Meson sarebbero quelli che passo a `sources:` in `executable()`
