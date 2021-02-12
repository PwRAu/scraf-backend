# Scraf

## Build

### Dependencies

#### Linux

scraf-backend can only run and compile on Linux (because of [Pistache](https://github.com/pistacheio/pistache)).

#### Compiler

scraf-backend uses C++20 and requires a modern compiler. It should work on Clang and GCC 10 or newer.

#### Libraries

##### ODB

[ODB](https://www.codesynthesis.com/products/odb/) is an Object-Relational Mapping (ORM) library.
The stable version of ODB packaged by Debian does not support anything newer than C++14, so it is required to use the beta plus the module for PostgreSQL.
It is recommended to follow the official [installation guide](https://www.codesynthesis.com/products/odb/doc/install-build2.xhtml#linux), but if you're lazy you can install it with these commands (spoiler: it is goning to take more than 10 minutes, go drink a tea or something)

```sh
sudo apt install gcc-10-plugin-dev

cd /directory/of/your/choice/where/you/ll/store/build/cache

mkdir build2-build && cd build2-build
curl -sSfO https://download.build2.org/0.13.0/build2-install-0.13.0.sh
sh build2-install-0.13.0.sh --yes --no-check --trust yes ~/build2

cd /directory/of/your/choice/where/you/ll/store/build/cache

mkdir odb-build && cd odb-build
bpkg create --directory odb-gcc-10 cc config.cxx=g++ config.cc.coptions=-O3 config.bin.rpath=/usr/local/lib config.install.root=/usr/local config.install.sudo=sudo --quiet
cd odb-gcc-10
yes | bpkg build odb@https://pkg.cppget.org/1/beta --trust-yes --quiet
bpkg test odb
bpkg install odb --quiet
cd ..
bpkg create --directory gcc-10 cc config.cxx=g++ config.cc.coptions=-O3 config.install.root=/usr/local config.install.sudo=sudo --quiet
cd gcc-10
bpkg add https://pkg.cppget.org/1/beta --trust-yes --quiet
bpkg fetch --trust-yes --quiet
yes | bpkg build libodb --trust-yes --quiet
yes | bpkg build libodb-pgsql --trust-yes --quiet
bpkg install --all --recursive --quiet
```

##### GoogleTest

We use [GoogleTest](https://github.com/google/googletest) for unit testing. We chose not to include the library as a submodule of scraf-backend because it is already packaged by most Linux distributions. We use the latest version provided by Debian Testing, but probably anything newer than 1.9.x will work. On Debian you can install it with `apt install libgtest-dev`

##### JSON

For parsing and serializing JSON we use [simdjson](https://github.com/simdjson/simdjson) and [nlohmann json](https://github.com/nlohmann/json) (also known as JSON for Modern C++ or the cool JSON library). On a Debian system, you can install them with `apt install libsimdjson-dev nlohmann-json3-dev`.
