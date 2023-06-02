Being consumed by CMake
=======================

To support libraries built with JustBuild to be consumed by CMake, the
rule `["CC", "install-with-deps"]` can be used. This rule will install
the library's artifacts, its public headers, and a pkg-config file,
which can be used by CMake to properly consume the library.

In this example, we show how to build and install the `ssl` library with
JustBuild, so it can be picked up and used by CMake afterwards.

Example: Build and install the `ssl` Library with JustBuild
-----------------------------------------------------------

First make sure that `just`, `just-mr`, and `just-import-git` are
available in your `PATH`. Then, define a new workspace by creating the
`ROOT` marker.

``` sh
$ touch ROOT
```

The `ssl` library that we want to use is a defined dependency of the
JustBuild repository `just/ssl`. To define a repository for that
library, create the following `repos.template.json` file.

``` {.jsonc srcname="repos.template.json"}
{ "main": "ssl"
, "repositories":
  { "ssl":
    { "repository": {"type": "file", "path": "."}
    , "bindings": {"rules": "rules-cc", "just-ssl": "just/ssl"}
    }
  }
}
```

and import the `rules-cc` and JustBuild repository, including its
dependency `just/ssl`, via:

``` sh
$ cat repos.template.json \
  | just-import-git -C - --as just -b master https://github.com/just-buildsystem/justbuild \
  | just-import-git -C - --as rules-cc -b master https://github.com/just-buildsystem/rules-cc \
  > repos.json
$
```

Next, the following `TARGETS` file needs to be provided.

``` {.jsonc srcname="TARGETS"}
{ "ssl-lib":
  { "type": ["@", "rules", "CC", "library"]
  , "name": ["ssl"]
  , "deps": [["@", "just-ssl", "", "ssl"]]
  }
, "ssl-pkg":
  {"type": ["@", "rules", "CC", "install-with-deps"], "targets": ["ssl-lib"]}
}
```

The library target `ssl-lib` specifies the name `"ssl"` (also used as
package name) and a dependency on the `ssl` library from `just-ssl`.
Note that although this library target is merely a "wrapper"
(header-only library without headers), it could equally well be a
full-blown library target. Furthermore, the package target `ssl-pkg`
installs the `ssl-lib` target including its dependencies and generates a
pkg-config file that can be later used by CMake.

Finally, the `ssl-pkg` target can be built and installed to a specific
`PREFIX` (note that `OS` and `ARCH` must be set as well).

``` sh
$ export PREFIX=/tmp/just_ssl
$ just-mr install -D'{"OS":"linux", "ARCH":"x64_64", "PREFIX":"'$PREFIX'"}' -o $PREFIX ssl-pkg
INFO: Requested target is [["@","ssl","","ssl-pkg"],{"ARCH":"x64_64","OS":"linux","PREFIX":"/tmp/just_ssl"}]
INFO: Analysed target [["@","ssl","","ssl-pkg"],{"ARCH":"x64_64","OS":"linux","PREFIX":"/tmp/just_ssl"}]
INFO: Export targets found: 0 cached, 2 uncached, 0 not eligible for caching
INFO: Discovered 269 actions, 3 trees, 1 blobs
INFO: Building [["@","ssl","","ssl-pkg"],{"ARCH":"x64_64","OS":"linux","PREFIX":"/tmp/just_ssl"}].
PROG: [ 79%] 0 cached, 215 run, 8 processing (["@","just/ssl","","crypto-lib"]#105, ...).
INFO: Processed 269 actions, 0 cache hits.
INFO: Artifacts can be found in:
        /tmp/just_ssl/include/openssl/aead.h [6d78db27d24746b39eacc20374697476e080c9f6:21054:f]
        ...
        /tmp/just_ssl/lib/libcrypto.a [df64da43f0168a717d09d609bf0c631d29c86b61:2336092:f]
        /tmp/just_ssl/lib/libssl.a [77d2c2bfbe3ef3608895c854f1d1f6e1c200efd0:852620:f]
        /tmp/just_ssl/share/pkgconfig/ssl.pc [9b69c758430f5b5fb6ff7a9b1f1ffc89471509af:406:f]
$
```

Example: Consume the installed `ssl` Library with CMake
-------------------------------------------------------

As an example, a minimal `main.c` file is created that depends on the
`ssl` library.

``` {.c srcname="main.c"}
#include <openssl/evp.h>

int main(int argc, char** argv) {
  OpenSSL_add_all_algorithms();
  return 0;
}
```

In the `CMakeLists.txt` file, the `pkg_check_modules()` macro can be
used to determine the compile and linker flags needed to consume our
`ssl` library with CMake.

``` {.cmake srcname="CMakeLists.txt"}
cmake_minimum_required(VERSION 3.1)

project(test_ssl)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SSL REQUIRED ssl)

add_executable(main main.c)
target_include_directories(main PRIVATE ${SSL_INCLUDE_DIRS})
target_compile_options(main PRIVATE ${SSL_CFLAGS_OTHER})
target_link_directories(main PRIVATE ${SSL_LIBRARY_DIRS})
target_link_libraries(main PRIVATE ${SSL_LDFLAGS_OTHER} ${SSL_LIBRARIES})
```

Note that `${SSL_LDFLAGS_OTHER}` can be omitted if the `ssl-pkg` target
was defined with a non-empty value for the field `"flat-libs"` (see rule
documentation of `["CC", "install-with-deps"]`).

Finally, when running CMake, make sure to set the `CMAKE_PREFIX_PATH` to
the previously used install prefix in order to successfully find the
installed `ssl` library and its pkg-config file.

``` sh
$ cmake -DCMAKE_PREFIX_PATH=/tmp/just_ssl -S . -B /tmp/test_ssl
-- The C compiler identification is GNU 10.2.1
-- The CXX compiler identification is GNU 10.2.1
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found PkgConfig: /usr/bin/pkg-config (found version "0.29.2") 
-- Checking for module 'ssl'
--   Found ssl, version unknown
-- Configuring done
-- Generating done
-- Build files have been written to: /tmp/test_ssl
$ cmake --build /tmp/test_ssl
[ 50%] Building CXX object CMakeFiles/main.dir/main.c.o
[100%] Linking CXX executable main
[100%] Built target main
$
```

Note that if the package is moved to a different location, the `prefix`
variable within the pkg-config file `share/pkgconfig/ssl.pc` must be
updated as well. Alternatively, CMake can be instructed to automatically
guess the correct prefix by setting the variable `PKG_CONFIG_ARGN` to
`"--define-prefix"` (either on the command line or in the
`CMakeLists.txt`). However, this is a fairly recent CMake feature and
requires at least CMake version 3.22.
