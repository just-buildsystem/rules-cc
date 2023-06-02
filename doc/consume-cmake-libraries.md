Consume CMake Libraries
=======================

To support libraries built with CMake to be consumed by JustBuild, the
rule `["CC/foreign/cmake", "library"]` can be used. This rule will run
CMake and collect the library's artifacts, public headers, and
pkg-config files (if available), to produce a proper JustBuild library
target.

In this example, we show how to consume the `gtest` library built with
CMake using JustBuild.

Example: Build the `gtest` Library with CMake
---------------------------------------------

First make sure that `just`, `just-mr`, and `just-import-git` are
available in your `PATH`. Then, define a new workspace by creating a
`ROOT` marker.

``` sh
$ touch ROOT
```

To define a repository for the
[gtest](https://github.com/google/googletest/tree/v1.13.0) library,
create the following `repos.template.json` file.

``` {.jsonc srcname="repos.template.json"}
{ "main": "tests"
, "repositories":
  { "imports": {"repository": {"type": "file", "path": "imports"}}
  , "gtest":
    { "repository":
      { "type": "git"
      , "branch": "v1.13.0"
      , "commit": "b796f7d44681514f58a683a3a71ff17c94edb0c1"
      , "repository": "https://github.com/google/googletest"
      }
    , "target_root": "imports"
    , "target_file_name": "gtest.TARGETS"
    , "bindings": {"rules": "rules-cc"}
    }
  , "tests":
    { "repository": {"type": "file", "path": "."}
    , "bindings": {"rules": "rules-cc", "gtest": "gtest"}
    }
  }
}
```

Now the missing `rules-cc` repository can be imported via:

``` sh
$ just-import-git -C repos.template.json --as rules-cc -b master https://github.com/just-buildsystem/rules-cc > repos.json
$
```

Create the file `imports/gtest.TARGETS` with the following content.

``` {.jsonc srcname="imports/gtest.TARGETS"}
{ "gtest_main":
  { "type": ["@", "rules", "CC/foreign/cmake", "library"]
  , "name": ["gtest_main"]
  , "version": ["1", "13", "0"]
  , "project": [["TREE", null, "."]]
  , "defines": ["BUILD_SHARED_LIBS=ON"]
  , "out_hdr_dirs": ["gtest"]
  , "out_libs": ["libgtest_main.so.1.13.0", "libgtest.so.1.13.0"]
  , "pkg-config": ["gtest_main.pc", "gtest.pc"]
  }
}
```

The library `gtest_main` uses the rule
`["CC/foreign/cmake", "library"]`. It sets `defines` to build shared
libraries and collects the public header directory `gtest`, as well as
the two libraries `libgtest_main.so.1.13.0` and `libgtest.so.1.13.0` (in
this particular link order). Furthermore, to automatically infer public
compile and link flags (e.g., a link dependency to the system's
`pthread` library), the pkg-config files `gtest_main.pc` and `gtest.pc`
are read, with the former (entry point) depending on the latter.

Now, create the actual `test` target, which consumes the `gtest_main`
library, by creating the following `TARGETS` file.

``` {.jsonc srcname="TARGETS"}
{ "test":
  { "type": ["@", "rules", "CC/test", "test"]
  , "name": ["test"]
  , "srcs": ["test.cpp"]
  , "private-deps": [["@", "gtest", "", "gtest_main"]]
  }
}
```

The last file missing yet is the actual test source file `test.cpp`.

``` {.cpp srcname="test.cpp"}
#include <gtest/gtest.h>

TEST(CastTest, double) {
  EXPECT_EQ (42.0, double(42));
}
```

Finally, build the `test` target to run the test.

``` sh
$ just-mr build test -Pstdout
INFO: Requested target is [["@","tests","","test"],{}]
INFO: Analysed target [["@","tests","","test"],{}]
INFO: Export targets found: 0 cached, 0 uncached, 0 not eligible for caching
INFO: Target tainted ["test"].
INFO: Discovered 7 actions, 2 trees, 3 blobs
INFO: Building [["@","tests","","test"],{}].
INFO: Processed 7 actions, 0 cache hits.
INFO: Artifacts built, logical paths are:
        result [7ef22e9a431ad0272713b71fdc8794016c8ef12f:5:f]
        stderr [8b137891791fe96927ad78e64b0aad7bded08bdc:1:f]
        stdout [fc46e9e95a8a393e3c94875d66b0de956305f6a6:728:f]
        time-start [93887fee067665917f67e2cb757dd30bf22b23a0:11:f]
        time-stop [93887fee067665917f67e2cb757dd30bf22b23a0:11:f]
      (1 runfiles omitted.)
Running main() from src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from CastTest
[ RUN      ] CastTest.double
[       OK ] CastTest.double (0 ms)
[----------] 1 test from CastTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.

INFO: Target tainted ["test"].
$
```
