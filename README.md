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
### Rule `["CC", "binary"]`

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

### Rule `["CC", "library"]`

A C++ library

| Field | Description |
| ----- | ----------- |
| `"name"` | The name of the library (without leading `"lib"` or trailing `".a"`). |
| `"stage"` | The logical location of all header and source files, as well as the resulting library file. Individual directory components are joined with `"/"`. |
| `"pure C"` | If non-empty, compile as C sources rather than C++ sources. In particular, CC is used to compile rather than CXX (or their respective defaults). |
| `"defines"` | List of defines set for this target and its consumers. Each list entry will be prepended by `"-D"`. |
| `"private-defines"` | List of defines set for source files local to this target. Each list entry will be prepended by `"-D"`. |
| `"cflags"` | List of compile flags set for this target and its consumers. |
| `"private-cflags"` | List of compile flags set for source files local to this target. |
| `"ldflags"` | Additional linker flags for linking external libraries for this target and its consumers (not built by this tool, typically system libraries). |
| `"private-ldflags"` | Additional linker flags for linking external libraries (not built by this tool, typically system libraries). |
| `"soversion"` | The SOVERSION for shared libraries. Individual version components are joined with `"."`. |
| `"pkg-name"` | Name to use for pkg-config files. If this field is empty, the field `"name"` is used instead. |
| `"srcs"` | The source files of the library. |
| `"hdrs"` | Any public header files of the library. |
| `"private-hdrs"` | Any header files that only need to be present when compiling the source files, but are not needed for any consumer of the library. |
| `"deps"` | Any other libraries this library depends upon. |
| `"private-deps"` | Any other libraries this library depends upon but does not include in its public headers. |
| `"proto"` | Any `["proto", "library"]` this target depends upon directly. The creation of C++ bindings for this proto library as well as of its dependencies will be taken care of (as anonymous targets, so no duplicate work will be carried out, even if the same proto library is used at various places). |
| `"private-proto"` | Any `["proto", "library"]` this target depends upon privately. The creation of C++ bindings for this proto library as well as of its dependencies will be taken care of (as anonymous targets, so no duplicate work will be carried out, even if the same proto library is used at various places). |
| `"shared"` | If non-empty, produce a shared instead of a static library. |

### Rule `["CC/prebuilt", "library"]`

A prebuilt C++ library

| Field | Description |
| ----- | ----------- |
| `"name"` | The name of the library (without leading `"lib"` or trailing file name extension), also used as name for pkg-config files. |
| `"version"` | The library version, used for pkg-config files. Individual version components are joined with `"."`. |
| `"stage"` | The logical location of all header and source files, as well as the resulting library file. Individual directory components are joined with `"/"`. |
| `"defines"` | List of defines set for this target and its consumers. Each list entry will be prepended by `"-D"`. |
| `"cflags"` | List of compile flags set for this target and its consumers. |
| `"ldflags"` | Additional linker flags (typically for linking system libraries). |
| `"lib"` | The actual prebuilt library. If multiple libraries are specified (e.g., one depends on the other), the order matters. Library types cannot be mixed. All of them have to be either static or shared. |
| `"hdrs"` | Any public header files of the library. |
| `"deps"` | Any other libraries this library depends upon. |
| `"pkg-config"` | Pkg-config file for optional infer of public cflags and ldflags. If multiple files are specified (e.g., one depends on the other), the first one is used as entry. Note that if this field is non-empty the tool `"pkg-config"` must be available in `"PATH"`, which is taken from `["CC", "defaults"]` or the `"ENV"` variable. |

### Rule `["CC", "install-with-deps"]`

Install target's artifacts with transitive dependencies. Depending on the target, artifacts and dependencies will be installed to subdirectories `"bin"`, `"include"`, and `"lib"`. For library targets, a pkg-config file is generated and provided in `"share/pkgconfig"`.

| Field | Description |
| ----- | ----------- |
| `"flat-libs"` | Install libraries flat to the `"lib"` subdirectory. Be aware that conflicts may occur if any of the (transitive) libraries happen to have the same base name. |
| `"prefix"` | The prefix used for pkg-config files. The path will be made absolute and individual directory components are joined with `"/"`. If no prefix is specified, the value from the config variable `"PREFIX"` is taken, with the default value being `"/"`. |
| `"targets"` | Targets to install artifacts from. |

### Rule `["CC/test", "test"]`

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

### Rule `["shell/test", "script"]`

Shell test, given by a test script

| Field | Description |
| ----- | ----------- |
| `"keep"` | List of names (relative to the test working directory) of files that the test might generate that should be kept as part of the output. This might be useful for further analysis of the test |
| `"name"` | A name for the test, used in reporting, as well as for staging the test result tree in the runfiles |
| `"deps"` | Any targets that should be staged (with artifacts and runfiles) into the tests working directory |
| `"test"` | The shell script for the test, launched with sh.  An empty directory is created to store any temporary files needed by the test, and it is made available in the environment variable TEST_TMPDIR. The test should not assume write permissions outside the working directory and the TEST_TMPDIR. For convenience, the environment variable TMPDIR is also set to TEST_TMPDIR. |

### Rule `["CC/foreign/cmake", "library"]`

Library produced by CMake configure, build, and install.

| Field | Description |
| ----- | ----------- |
| `"name"` | The name of the library (without leading `"lib"` or trailing file name extension), also used as name for pkg-config files. |
| `"version"` | The library version, used for pkg-config files. Individual version components are joined with `"."`. |
| `"stage"` | The logical location of the public headers and library files. Individual directory components are joined with `"/"`. |
| `"options"` | CMake options for the configuration phase. (e.g., `["-GNinja", "-Ax64"]`) |
| `"defines"` | CMake defines for the configuration phase. (e.g., `["CMAKE_BUILD_TYPE=Release"]`) |
| `"jobs"` | Number of jobs to run simultaneously. If omitted, CMake's default number is used. |
| `"pre_cmds"` | List of commands executed in the project directory before calling CMake. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via $TMPDIR, which is uniquely reserved for this action. |
| `"post_cmds"` | List of commands executed in the install directory after successful installation but before the output files are collected. Useful for renaming files or directories (e.g., in case of SONAME mismatch). Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via $TMPDIR, which is uniquely reserved for this action. |
| `"out_hdrs"` | Paths to produced public header files. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_hdr_dirs"` | Paths to produced public header directories. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_libs"` | Paths to produced library files. The path is considered relative to the library directory, which be set via `"lib_prefix"`. Order matters in the case of one library depending on another. |
| `"cflags"` | List of compile flags set for this target and its consumers. |
| `"ldflags"` | Additional linker flags that are required for the consumer of the produced libraries. |
| `"pkg-config"` | Pkg-config file for optional infer of public cflags and ldflags. If multiple files are specified (e.g., one depends on the other), the first one is used as entry. Note that if this field is non-empty the tool `"pkg-config"` must be available in `"PATH"`, which is taken from `["CC", "defaults"]` or the `"ENV"` variable. |
| `"hdr_prefix"` | Prefix where headers will be installed by CMake. Individual directory components are joined with `"/"`. Defaults to `"include"` if not set. |
| `"lib_prefix"` | Prefix where libraries will be installed by CMake. Individual directory components are joined with `"/"`. Defaults to `"lib"` if not set. |
| `"pc_prefix"` | Prefix where pkg-config files will be installed by CMake. Individual directory components are joined with `"/"`. Defaults to `"lib/pkgconfig"` if not set. |
| `"project"` | The CMake project directory. It should contain a single tree artifact |
| `"deps"` | Public dependency on other CC libraries. |

### Rule `["CC/foreign/cmake", "data"]`

Data produced by CMake configure, build, and install.

| Field | Description |
| ----- | ----------- |
| `"options"` | CMake options for the configuration phase. (e.g., `["-GNinja", "-Ax64"]`) |
| `"defines"` | CMake defines for the configuration phase. (e.g., `["CMAKE_BUILD_TYPE=Release"]`) |
| `"jobs"` | Number of jobs to run simultaneously. If omitted, CMake's default number is used. |
| `"pre_cmds"` | List of commands executed in the project directory before calling CMake. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via $TMPDIR, which is uniquely reserved for this action. |
| `"post_cmds"` | List of commands executed in the install directory after successful installation but before the output files are collected. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via $TMPDIR, which is uniquely reserved for this action. |
| `"out_files"` | Paths to the produced output files. The paths are considered relative to the install directory. Note that `"out_files"` and `"out_dirs"` may not overlap. |
| `"out_dirs"` | Paths to the produced output directories. The paths are considered relative to the install directory. Note that `"out_files"` and `"out_dirs"` may not overlap. |
| `"project"` | The CMake project directory. It should contain a single tree artifact |

### Rule `["CC/foreign/make", "library"]`

Library produced by Make build and install.

| Field | Description |
| ----- | ----------- |
| `"name"` | The name of the library (without leading `"lib"` or trailing file name extension), also used as name for pkg-config files. |
| `"version"` | The library version, used for pkg-config files. Individual version components are joined with `"."`. |
| `"stage"` | The logical location of the public headers and library files. Individual directory components are joined with `"/"`. |
| `"target"` | The Make target to build (default: `["install"]`). |
| `"prefix"` | The prefix used for the Make target. The path will be made absolute and individual directory components are joined with `"/"`. If no prefix is specified, the value from the config variable `"PREFIX"` is taken, with the default value being `"/"`. |
| `"options"` | Make options for the configuration phase. (e.g., `["-f", "Makefile", "ARCH=x86"]`) |
| `"jobs"` | Number of jobs to run simultaneously. If omitted, Make's default number is used. |
| `"pre_cmds"` | List of commands executed in the project directory before calling Make. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via $TMPDIR, which is uniquely reserved for this action. |
| `"post_cmds"` | List of commands executed in the install directory after successful installation but before the output files are collected. Useful for renaming files or directories (e.g., in case of SONAME mismatch). Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via $TMPDIR, which is uniquely reserved for this action. |
| `"out_hdrs"` | Paths to produced public header files. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_hdr_dirs"` | Paths to produced public header directories. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_libs"` | Paths to produced library files. The path is considered relative to the library directory, which be set via `"lib_prefix"`. Order matters in the case of one library depending on another. |
| `"cflags"` | List of compile flags set for this target and its consumers. |
| `"ldflags"` | Additional linker flags that are required for the consumer of the produced libraries. |
| `"pkg-config"` | Pkg-config file for optional infer of public cflags and ldflags. If multiple files are specified (e.g., one depends on the other), the first one is used as entry. Note that if this field is non-empty the tool `"pkg-config"` must be available in `"PATH"`, which is taken from `["CC", "defaults"]` or the `"ENV"` variable. |
| `"hdr_prefix"` | Prefix where headers will be installed by Make. Individual directory components are joined with `"/"`. Defaults to `"include"` if not set. |
| `"lib_prefix"` | Prefix where libraries will be installed by Make. Individual directory components are joined with `"/"`. Defaults to `"lib"` if not set. |
| `"pc_prefix"` | Prefix where pkg-config files will be installed by Make. Individual directory components are joined with `"/"`. Defaults to `"lib/pkgconfig"` if not set. |
| `"var_cc"` | Variable name used to specify the C compiler (default: `"CC"`). |
| `"var_cxx"` | Variable name used to specify the C++ compiler (default: `"CXX"`). |
| `"var_ccflags"` | Variable name used to specify the C flags (default: `"CFLAGS"`). |
| `"var_cxxflags"` | Variable name used to specify the C++ flags (default: `"CXXFLAGS"`). |
| `"var_ldflags"` | Variable name used to specify the link flags (default: `"LDFLAGS"`). |
| `"var_ar"` | Variable name used to specify the archiver (default: `"AR"`). |
| `"var_prefix"` | Variable name used to specify the prefix (default: `"PREFIX"`). |
| `"var_destdir"` | Variable name used to specify the destdir (default: `"DESTDIR"`). |
| `"project"` | The Make project directory. It should contain a single tree artifact |
| `"deps"` | Public dependency on other CC libraries. |

### Rule `["CC/foreign/make", "data"]`

Data produced by Make build and install.

| Field | Description |
| ----- | ----------- |
| `"target"` | The Make target to build (default: `["install"]`). |
| `"prefix"` | The prefix used for the Make target. The path will be made absolute and individual directory components are joined with `"/"`. If no prefix is specified, the value from the config variable `"PREFIX"` is taken, with the default value being `"/"`. |
| `"options"` | Make options for the configuration phase. (e.g., `["-f", "Makefile", "ARCH=x86"]`) |
| `"jobs"` | Number of jobs to run simultaneously. If omitted, Make's default number is used. |
| `"pre_cmds"` | List of commands executed in the project directory before calling Make. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via $TMPDIR, which is uniquely reserved for this action. |
| `"post_cmds"` | List of commands executed in the install directory after successful installation but before the output files are collected. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via $TMPDIR, which is uniquely reserved for this action. |
| `"out_files"` | Paths to the produced output files. The paths are considered relative to the install directory. Note that `"out_files"` and `"out_dirs"` may not overlap. |
| `"out_dirs"` | Paths to the produced output directories. The paths are considered relative to the install directory. Note that `"out_files"` and `"out_dirs"` may not overlap. |
| `"var_cc"` | Variable name used to specify the C compiler (default: `"CC"`). |
| `"var_cxx"` | Variable name used to specify the C++ compiler (default: `"CXX"`). |
| `"var_ccflags"` | Variable name used to specify the C flags (default: `"CFLAGS"`). |
| `"var_cxxflags"` | Variable name used to specify the C++ flags (default: `"CXXFLAGS"`). |
| `"var_ldflags"` | Variable name used to specify the link flags (default: `"LDFLAGS"`). |
| `"var_ar"` | Variable name used to specify the archiver (default: `"AR"`). |
| `"var_prefix"` | Variable name used to specify the prefix (default: `"PREFIX"`). |
| `"var_destdir"` | Variable name used to specify the destdir (default: `"DESTDIR"`). |
| `"project"` | The Make project directory. It should contain a single tree artifact |

### Rule `["proto", "library"]`

A proto library as abstract data structure. 

 Such a library does not produce any artifacts itself, but it can be used as a dependency for various language-specific rules.

| Field | Description |
| ----- | ----------- |
| `"stage"` | The directory to stage the source files to. Directory components are joined by `"/"`. |
| `"name"` | The name of the (abstract) library. |
| `"service"` | If non empty, generate a service library (with access to `"rpc"` definitions) instead of a regular one. |
| `"srcs"` | The proto files for this library |
| `"deps"` | Any other proto library this library depends on |

### Rule `["data", "staged"]`

Stage data to a logical subdirectory.

| Field | Description |
| ----- | ----------- |
| `"stage"` | The logical directory to stage the files to. Individual directory components are joined with `"/"`. |
| `"srcs"` | The files to be staged |
| `"deps"` | Targets of with their runfiles should be added as well. Their staging is not changed. |

### Rule `["patch", "file"]`

Replace a file, logically in place, by a patched version

| Field | Description |
| ----- | ----------- |
| `"patch-part"` | If the patch contains hunks for multiple files, only apply hunks for the specified file path. Individual directory components are joined with `"/"`. Note that the patch must be provided in unified format. |
| `"src"` | The single source file to patch, typically an explicit file reference. |
| `"patch"` | The patch to apply. |

### Rule `["CC/auto", "config"]`

Generate a C/C++ config header 

 Generate a C/C++ configuration header using defines specified via the target configuration. In the usual case, a target using this rule is configured by depending on it from a target that uses the built-in `"configure"` rule.

| Field | Description |
| ----- | ----------- |
| `"name"` | Name of the header file to generate (incl. file name ext). |
| `"stage"` | The location of the header. Path segments are joined with `"/"`. |
| `"guard"` | The include guard. Multiple segments are joined with `"_"`. |
| `"hdrs"` | Additional header files to be available in the include path. Useful for providing additional header files to values given in `"have_{cfile,cxxfile,ctype,cxxtype,csymbol,cxxsymbol}"`. |
| `"deps"` | Additional public header files from targets to be available in the include path. Useful for providing additional header files to values given in `"have_{cfile,cxxfile,ctype,cxxtype,csymbol,cxxsymbol}"`. |

### Rule `["CC/IDE", "headers"]`

Transitive public headers of C++ target

| Field | Description |
| ----- | ----------- |
| `"stage"` | The logical location of the header files. Individual directory components are joined with `"/"`. |
| `"proto"` | The proto source files for creating cc bindings. |
| `"deps"` | The targets to obtain the headers from. |

