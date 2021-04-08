# scraf-backend

[![Language grade: C++](https://img.shields.io/lgtm/grade/cpp/g/PwRAu/scraf-backend.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/PwRAu/scraf-backend/context:cpp)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/7a105015c95b49ae94ebfa41739a6c1c)](https://app.codacy.com/gh/PwRAu/scraf-backend?utm_source=github.com&utm_medium=referral&utm_content=PwRAu/scraf-backend&utm_campaign=Badge_Grade_Settings)
[![codecov](https://codecov.io/gh/PwRAu/scraf-backend/branch/main/graph/badge.svg?token=Q9PWI8IRXH)](https://codecov.io/gh/PwRAu/scraf-backend)

scraf-backend is the backend of Scraf. It is developed as a REST API, and handles all the interactions between the frontend and the persistent storage (database) as well as the integration with the various online school registers.

This project follows the [Canonical Project Structure](https://wg21.link/P1204R0) for C++ projects: no separation of `src/` and `include/`, unit tests in the same directory as the main [source directory](scraf-backend) and integration tests in the [`tests/`](tests) directory.

## Build

scraf-backend uses the Meson build system.

### Dependencies

#### Linux

scraf-backend can only run and compile on Linux (because of [Pistache](subprojects/pistache)).

#### Compiler

scraf-backend uses C++20 and requires a modern compiler. It should work on Clang and GCC 10 or newer.

If using Clang, you must link the program using LLD (probably a Meson bug related to static libraries)

#### Libraries

##### ODB

[ODB](https://www.codesynthesis.com/products/odb/) is an Object-Relational Mapping (ORM) library.
The stable version of ODB packaged by Debian does not support anything newer than C++14, so it is required to use the beta plus the module for PostgreSQL.
It is recommended to follow the official [installation guide](https://www.codesynthesis.com/products/odb/doc/install-build2.xhtml#linux), but if you're lazy you can install it with these commands (spoiler: it is goning to take more than 10 minutes, go drink a tea or something)

```sh
sudo apt install gcc-10-plugin-dev

cd /directory/of/your/choice/where/you/ll/store/build/cache

mkdir build2-build && cd build2-build
curl --silent --remote-name https://download.build2.org/0.13.0/build2-install-0.13.0.sh
sh build2-install-0.13.0.sh --yes --trust yes --no-check

cd /directory/of/your/choice/where/you/ll/store/build/cache

mkdir odb-build && cd odb-build
bpkg --quiet create --directory odb-gcc-10 cc config.cxx=g++ config.c.std=gnu17 config.cxx.std=gnu++17 config.cc.coptions=-O3 config.bin.rpath=/usr/local/lib config.install.root=/usr/local config.install.sudo=sudo
cd odb-gcc-10
bpkg --quiet --trust-yes build --yes odb@https://pkg.cppget.org/1/beta
bpkg test odb
bpkg --quiet install odb
cd ..
bpkg --quiet create --directory gcc-10 cc config.cxx=g++ config.c.std=gnu17 config.cxx.std=gnu++17 config.cc.coptions=-O3 config.install.root=/usr/local config.install.sudo=sudo
cd gcc-10
bpkg --quiet add https://pkg.cppget.org/1/beta
bpkg --quiet --trust-yes fetch
bpkg --quiet build --yes libodb
bpkg --quiet build --yes libodb-pgsql
bpkg --quiet install --all --recursive
```

##### GoogleTest

We use [GoogleTest](https://github.com/google/googletest) for unit testing. We chose not to include the library as a submodule of scraf-backend because it is already packaged by most Linux distributions. We use the latest version provided by Debian Testing, but probably anything newer than 1.9.x will work. On Debian you can install it with `apt install libgtest-dev`

##### JSON

For parsing and serializing JSON we use [simdjson](https://github.com/simdjson/simdjson) and [nlohmann json](https://github.com/nlohmann/json) (also known as JSON for Modern C++). On a Debian system, you can install them with `apt install libsimdjson-dev nlohmann-json3-dev`.

### Building

After you installed all the dependencies, you can build Scraf with Meson.

First, clone the repository

```sh
git clone https://github.com/PwRAu/scraf-backend.git --recurse-submodules
```

Then configure the build directory

```sh
cd scraf-backend
meson setup build --buildtype=release
```

And finally compile the executable

```sh
meson compile -C build
```
