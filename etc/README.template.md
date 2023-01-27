# C/C++ rules for the `just` build system

A collection of rules for building C/C++ libraries and binaries.

## How to use this Repository

Either generate your `repos.json` from a template (`repos.template.json`) by
importing the `rules-cc` repository with the tool `just-import-git`

~~~sh
$ just-import-git -C repos.template.json --as rules-cc -b master https://github.com/just-buildsystem/rules-cc > repos.json
~~~

or manually add the `rules-cc` repository to your `repos.json`.

~~~json
...
, "rules-cc":
    { "repository":
      { "type": "git"
      , "branch": "master"
      , "commit": "7a90f68e5207d541dea937aaff5b6c4c499b8968"
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
