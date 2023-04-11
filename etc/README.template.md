# C/C++ rules for the `just` build system

A collection of rules for building C/C++ libraries and binaries.

## How to use this Repository

Either generate your `repos.json` from a template (`repos.template.json`) by
importing the `rules-cc` repository with the tool `just-import-git`

~~~sh
$ just-import-git -C repos.template.json --as rules-cc -b master https://github.com/just-buildsystem/rules-cc > repos.json
~~~

or manually add the `rules-cc` repository to your `repos.json` (also binding
`protobuf` and `grpc` appropriately if proto libraries are to be used).

~~~json
...
  , "rules-cc":
    { "repository":
      { "type": "git"
      , "branch": "master"
      , "commit": "c3b895900468f34d6808c56b736ddd88933b81ec"
      , "repository": "https://github.com/just-buildsystem/rules-cc"
      , "subdir": "rules"
      }
    }
...
~~~

## Consume and being consumed by CMake Libraries

For interoperability with CMake projects, see

- [consume CMake libraries](./doc/consume-cmake-libraries.org)
- [being consumed by CMake](./doc/being-consumed-by-cmake.org)

## Rule Documentation

In this documentation, the standard configuration variables
`"AR"`, `"CC"`, `"CXX"`, `"CFLAGS"`, `"CXXFLAGS"`,`"LDFLAGS"`,
`"ADD_CFLAGS"`, `"ADD_CXXFLAGS"`, `"ADD_LDFLAGS"`, `"ENV"`,
`"BUILD_POSITION_INDEPENDENT"` are ommitted.

