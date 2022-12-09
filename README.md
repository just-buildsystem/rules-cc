## Rule `["CC", "library"]`

A C++ library

| Field | Description |
| ----- | ----------- |
| `"name"` | The name of the library (without leading `"lib"` or trailing `".a"`). |
| `"stage"` | The logical location of all header and source files, as well as the resulting library file. Individual directory components are joined with `"/"`. |
| `"pure C"` | If non-empty, compile as C sources rathter than C++ sources. In particular, CC is used to compile rather than CXX (or their respective defaults). |
| `"defines"` | List of defines set for this target and its consumers. Each list entry will be prepended by `"-D"`. |
| `"private-defines"` | List of defines set for source files local to this target. Each list entry will be prepended by `"-D"`. |
| `"cflags"` | List of compile flags set for this target and its consumers. |
| `"private-cflags"` | List of compile flags set for source files local to this target. |
| `"private-ldflags"` | Additional linker flags for linking external libraries (not built by this tool, typically system libraries). |
| `"srcs"` | The source files of the library. |
| `"hdrs"` | Any public header files of the library. |
| `"private-hdrs"` | Any header files that only need to be present when compiling the source files, but are not needed for any consumer of the library. |
| `"deps"` | Any other libraries this library depends upon. |
| `"private-deps"` | Any other libraries this library depends upon but does not include in its public headers. |
| `"proto"` | Any `["proto", "library"]` this target depends upon directly. The creation of C++ bindings for this proto library as well as of its dependencies will be taken care of (as anonymous targets, so no duplicate work will be carried out, even if the same proto library is used at various places). |

## Rule `["CC", "binary"]`

A binary written in C++

| Field | Description |
| ----- | ----------- |
| `"name"` | The name of the binary |
| `"stage"` | The logical location of all header and source files, as well as the resulting binary file. Individual directory components are joined with `"/"`. |
| `"pure C"` | If non-empty, compile as C sources rathter than C++ sources. In particular, CC is used to compile rather than CXX |
| `"private-defines"` | List of defines set for source files local to this target. Each list entry will be prepended by `"-D"`. |
| `"private-cflags"` | List of compile flags set for source files local to this target. |
| `"private-ldflags"` | Additional linker flags for linking external libraries. |
| `"srcs"` | The source files of the library. |
| `"private-hdrs"` | Any header files that need to be present when compiling the source files. |
| `"private-deps"` | Any other libraries this binary depends upon. |
| `"private-proto"` | Any `["proto", "library"]` this target depends upon directly. The creation of C++ bindings for this proto library as well as of is dependencies will be taken care of (as anonymous targets, so no duplicate work will be carried out, even if the same proto library is used at various places). |

## Rule `["CC/test", "test"]`

A test written in C++

| Field | Description |
| ----- | ----------- |
| `"name"` | The name of the test  Used to name the test binary as well as for staging the test result |
| `"args"` | Command line arguments for the test binary |
| `"stage"` | The logical location of all header and source files. Individual directory components are joined with `"/"`. |
| `"private-defines"` | List of defines set for source files local to this target. Each list entry will be prepended by `"-D"`. |
| `"private-cflags"` | List of compile flags set for source files local to this target. |
| `"private-ldflags"` | Additional linker flags for linking external libraries (not built by this tool, typically system libraries). |
| `"srcs"` | The sources of the test binary |
| `"private-hdrs"` | Any additional header files that need to be present when compiling the test binary. |
| `"data"` | Any files the test binary needs access to when running |

## Rule `["shell/test", "script"]`

Shell test, given by a test script

| Field | Description |
| ----- | ----------- |
| `"keep"` | List of names (relative to the test working directory) of files that the test might generate that should be kept as part of the output. This might be useful for further analysis of the test |
| `"name"` | A name for the test, used in reporting, as well as for staging the test result tree in the runfiles |
| `"deps"` | Any targets that should be staged (with artifacts and runfiles) into the tests working directory |
| `"test"` | The shell script for the test, launched with sh.  An empty directory is created to store any temporary files needed by the test, and it is made available in the environment variable TEST_TMPDIR. The test should not assume write permissions outside the working directory and the TEST_TMPDIR. For convenience, the environment variable TMPDIR is also set to TEST_TMPDIR. |

## Rule `["proto", "library"]`

A proto library as abtract data structure. 

 Such a libray does not produce any artifacts itself, but it can be used as a dependency for various language-specific rules.

| Field | Description |
| ----- | ----------- |
| `"stage"` | The directory to stage the source files to. Directory components are joined by `"/"`. |
| `"name"` | The name of the (abstract) library. |
| `"service"` | If non empty, generate a service library (with acces sto `"rpc"` definitions) instead of a regular one. |
| `"srcs"` | The proto files for this library |
| `"deps"` | Any other proto library this library depends on |

## Rule `["data", "staged"]`

Stage data to a logical subdirectory.

| Field | Description |
| ----- | ----------- |
| `"stage"` | The logical directory to stage the files to. Individual directory components are joined with `"/"`. |
| `"srcs"` | The files to be staged |
| `"deps"` | Targets of with their runfiles should be added as well. Their staging is not changed. |

## Rule `["patch", "file"]`

Replace a file, logically in place, by a patched version

| Field | Description |
| ----- | ----------- |
| `"patch-part"` | If the patch contains hunks for multiple files, only apply hunks for the specified file path. Individual directory components are joined with `"/"`. Note that the patch must be provided in unified format. |
| `"src"` | The single source file to patch, typically an explict file reference. |
| `"patch"` | The patch to apply. |

## Rule `["CC/auto", "config"]`

Generate a C/C++ config header 

 Generate a C/C++ configuration header using defines specified via the target configuration. In the usual case, a target using this rule is configured by depending on it from a target that uses the built-in `"configure"` rule.

| Field | Description |
| ----- | ----------- |
| `"name"` | Name of the header file to generate (incl. file name ext). |
| `"stage"` | The location of the header. Path segments are joined with `"/"`. |
| `"guard"` | The include guard. Multiple segments are joined with `"_"`. |
| `"hdrs"` | Additional header files to be available in the include path. Useful for providing additional header files to values given in `"have_{cfile,cxxfile,ctype,cxxtype,csymbol,cxxsymbol}"`. |
| `"deps"` | Additional public header files from targets to be available in the include path. Useful for providing additional header files to values given in `"have_{cfile,cxxfile,ctype,cxxtype,csymbol,cxxsymbol}"`. |

## Rule `["CC/IDE", "headers"]`

Transitive public headers of C++ target

| Field | Description |
| ----- | ----------- |
| `"stage"` | The logical location of the header files. Individual directory components are joined with `"/"`. |
| `"proto"` | The proto source files for creating cc bindings. |
| `"deps"` | The targets to obtain the headers from. |

