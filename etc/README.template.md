# C/C++ rules for the `just` build system

A collection of rules for building C/C++ libraries and binaries.

## How to use this Repository

There are two ways to import this repository. You can generate your
`repos.json` from a template (`repos.template.json`) by importing
the `rules-cc` repository with the tool `just-import-git`

~~~sh
$ just-import-git -C repos.template.json --as rules-cc -b master https://github.com/just-buildsystem/rules-cc > repos.json
~~~

Importing this way will also pull in `protoc` and `grpc_cpp_plugin`
as a dependency for generating bindings for proto dependencies, if
this feature is used.

Alternatively, the `rules-cc` repository can be added manually to
your `repos.json`.

~~~jsonc
...
  , "rules-cc":
    { "repository":
      { "type": "git"
      , "branch": "master"
      , "commit": "2ea50063460a3e11dfcbb71651540c0d61fddc1a"
      , "repository": "https://github.com/just-buildsystem/rules-cc"
      , "subdir": "rules"
      }
    }
...
~~~

Importing this way, `protoc` and `grpc_cpp_plugin` will be taken
from the host system. To change the binary names or to bring your
own proto toolchain, add a corresponding `target_root` layer (and
provide the needed binding, if your target files refer to other
repositories containing the toolchain).

## Consume and being consumed by CMake Libraries

For interoperability with CMake projects, see

- [consume CMake libraries](./doc/consume-cmake-libraries.md)
- [being consumed by CMake](./doc/being-consumed-by-cmake.md)

## Rule Documentation

In this documentation, the standard configuration variables
`"AR"`, `"CC"`, `"CXX"`, `"CFLAGS"`, `"CXXFLAGS"`,`"LDFLAGS"`,
`"ADD_CFLAGS"`, `"ADD_CXXFLAGS"`, `"ADD_LDFLAGS"`, `"ENV"`,
`"BUILD_POSITION_INDEPENDENT"` are ommitted.

