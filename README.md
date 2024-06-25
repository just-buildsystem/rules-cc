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

### Rule `["CC", "defaults"]`

A rule to provide defaults. All CC targets take their defaults for CC, CXX, flags, etc from the target `["CC", "defaults"]`. This is probably the only sensible use of this rule. As targets form a different root, the defaults can be provided without changing this directory.

| Field | Description |
| ----- | ----------- |
| `"CC"` | The C compiler to use |
| `"CXX"` | The C++ compiler to use |
| `"CFLAGS"` | Flags for C compilation. Specifying this field overwrites values from `"base"`. |
| `"CXXFLAGS"` | Flags for C++ compilation. Specifying this field overwrites values from `"base"`. |
| `"LDFLAGS"` | Linker flags for linking the final CC library. Specifying this field overwrites values from `"base"`. |
| `"ADD_COMPILE_FLAGS"` | Additional compilation flags for C and C++. Specifying this field extends values from `"base"` for both, `"CFLAGS"` and `"CXXFLAGS"`. |
| `"ADD_CFLAGS"` | Additional compilation flags specific for C. Specifying this field extends values from `"base"`. |
| `"ADD_CXXFLAGS"` | Additional compilation flags specific for C++. Specifying this field extends values from `"base"`. |
| `"ADD_LDFLAGS"` | Additional linker flags for linking the final CC library. Specifying this field extends values from `"base"`. |
| `"AR"` | The archiver tool to use |
| `"PATH"` | Path for looking up the compilers. Individual paths are joined with `":"`. Specifying this field extends values from `"base"`. |
| `"SYSTEM_TOOLS"` | List of tools (`"CC"`, `"CXX"`, or `"AR"`) that should be taken from the system instead of from `"toolchain"` (if specified). |
| `"base"` | Other targets (using the same rule) to inherit values from. |
| `"toolchain"` | Optional toolchain directory. A collection of artifacts that provide the tools CC, CXX, and AR (if needed). Note that only artifacts of the specified targets are considered (no runfiles etc.). Specifying this field extends artifacts from `"base"`. If the toolchain supports cross-compilation, it should perform a dispatch on the configuration variable `"BUILD_ARCH"` to determine for which architecture to generate code for. |
| `"deps"` | Optional CC libraries any CC library and CC binary implicitly depend on. Those are typically `"libstdc++"` or `"libc++"` for C++ targets. Specifying this field extends dependencies from `"base"`. |

| Config variable | Description |
| --------------- | ----------- |
| `"ARCH"` | The unqualified architecture. Is taken as a default for `"HOST_ARCH"` and `"TARGET_ARCH"` if not set. |
| `"HOST_ARCH"` | The architecture on which the build actions are carried out. |
| `"TARGET_ARCH"` | The architecture for which to build. |
| `"DEBUG"` | Compute the debug-stage, needed for local debugging. |

### Rule `["CC/proto", "defaults"]`

A rule to provide protoc/GRPC defaults. Used to implement `["CC/proto", "defaults"]` for CC proto libraries and `["CC/proto", "service defaults"]` for CC proto service libraries (GRPC).

| Field | Description |
| ----- | ----------- |
| `"PROTOC"` | The proto compiler. If `"toolchain"` is empty, this field's value is considered the proto compiler name that is looked up in `"PATH"`. If `"toolchain"` is non-empty, this field's value is assumed to be the relative path to the proto compiler in `"toolchain"`. Specifying this field overwrites values from `"base"`. |
| `"PROTOCFLAGS"` | Protobuf compiler flags. Specifying this field overwrites values from `"base"`. |
| `"ADD_PROTOCFLAGS"` | Additional protobuf compiler flags. Specifying this field extends values from `"base"`. |
| `"LDFLAGS"` | Linker flags for linking the final CC library. Specifying this field overwrites values from `"base"`. |
| `"ADD_LDFLAGS"` | Additional linker flags for linking the final CC library. Specifying this field extends values from `"base"`. |
| `"GRPC_PLUGIN"` | The GRPC plugin for the proto compiler. If `"toolchain"` is empty, this field's value is considered to be the absolute system path to the plugin. If `"toolchain"` is non-empty, this field's value is assumed to be the relative path to the plugin in `"toolchain"`. Specifying this field overwrites values from `"base"`. |
| `"PATH"` | Path for looking up the proto compiler. Individual paths are joined with `":"`. Specifying this field extends values from `"base"`. |
| `"base"` | Other targets (using the same rule) to inherit values from. If multiple targets are specified, for values that are overwritten (see documentation of other fields) the last specified value wins. |
| `"toolchain"` | Optional toolchain directory. A collection of artifacts that provide the protobuf compiler and the GRPC plugin (if needed). Note that only artifacts of the specified targets are considered (no runfiles etc.). Specifying this field extends artifacts from `"base"`. |
| `"deps"` | Optional CC libraries the resulting CC proto libraries implicitly depend on. Those are typically `"libprotobuf"` for CC proto libraries and `"libgrpc++"` for CC proto service libraries. Specifying this field extends dependencies from `"base"`. |

| Config variable | Description |
| --------------- | ----------- |
| `"ARCH"` | The unqualified architecture. Is taken as a default for `"HOST_ARCH"` and `"TARGET_ARCH"` if not set. |
| `"HOST_ARCH"` | The architecture on which the build actions are carried out. |
| `"DEBUG"` | Compute the debug-stage, needed for local debugging. |

### Rule `["CC/foreign", "defaults"]`

A rule to provide defaults for foreign rules. All foreign rules take their defaults for MAKE, CMAKE, etc from the target `["CC/foreign", "defaults"]`.

| Field | Description |
| ----- | ----------- |
| `"MAKE"` | The make binary to use |
| `"CMAKE"` | The cmake binary to use |
| `"PATH"` | Path for looking up the tools. Individual paths are joined with with `":"`. Specifying this field extends values from `"base"`. |
| `"SYSTEM_TOOLS"` | List of tools (`"MAKE"`, `"CMAKE"`) that should be taken from the system instead of from `"toolchain"` (if specified). |
| `"base"` | Other targets (using the same rule) to inherit values from. |
| `"toolchain"` | Optional toolchain directory. A collection of artifacts that provide the tools MAKE, CMAKE. Note that only artifacts of the specified targets are considered (no runfiles etc.). Specifying this field extends artifacts from `"base"`. |

### Rule `["patch", "defaults"]`

A rule to provide defaults. All targets take their defaults for PATCH from the target `["patch", "defaults"]`. This is probably the only sensible use of this rule. As targets form a different root, the defaults can be provided without changing this directory.

| Field | Description |
| ----- | ----------- |
| `"PATCH"` | The patch binary to use |
| `"PATH"` | Path for looking up the compilers. Individual paths are joined with `":"`. Specifying this field extends values from `"base"`. |
| `"SYSTEM_TOOLS"` | List of tools (`"PATCH"`) that should be taken from the system instead of from `"toolchain"` (if specified). |
| `"base"` | Other targets (using the same rule) to inherit values from. |
| `"toolchain"` | Optional toolchain directory. A collection of artifacts that provide the tool PATCH. Note that only artifacts of the specified targets are considered (no runfiles etc.). Specifying this field extends artifacts from `"base"`. |

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

| Config variable | Description |
| --------------- | ----------- |
| `"DEBUG"` | Compute the debug-stage, needed for local debugging. |

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
| `"shared"` | If non-empty, produce a shared instead of a static library. Setting this option is mutually exclusive to the `"object_only"` option. |
| `"object_only"` | If non-empty, produce an object library, resulting in object files added to the linker line of all depending targets. Setting this option is mutually exclusive to the `"shared"` option. |

| Config variable | Description |
| --------------- | ----------- |
| `"DEBUG"` | Compute the debug-stage, needed for local debugging. |

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

### Rule `["CC/pkgconfig", "system_library"]`

A system library via pkg-config

| Field | Description |
| ----- | ----------- |
| `"name"` | The pkg-config name of the library. |
| `"args"` | Additional pkg-config arguments (e.g., `"--define-prefix"` or `"--static"`), appended to the config variable `"PKG_CONFIG_ARGS"`. |
| `"stage"` | The stage of the internally created flag files. |

| Config variable | Description |
| --------------- | ----------- |
| `"PKG_CONFIG_ARGS"` | Additional pkg-config arguments (e.g., `"--define-prefix"` or `"--static"`). |

### Rule `["CC", "install-with-deps"]`

Install target's artifacts with transitive dependencies. Depending on the target, artifacts and dependencies will be installed to subdirectories `"bin"`, `"include"`, and `"lib"`. For library targets, a pkg-config file is generated and provided in `"lib/pkgconfig"`. In debug mode, depending on the target, additional artifacts needed for local debugging are gathered and installed, depending on target, to `"work"` and `"include"`.

| Field | Description |
| ----- | ----------- |
| `"flat-libs"` | Install libraries flat to the `"lib"` subdirectory. Be aware that conflicts may occur if any of the (transitive) libraries happen to have the same base name. |
| `"prefix"` | The prefix used for pkg-config files. The path will be made absolute and individual directory components are joined with `"/"`. If no prefix is specified, the value from the config variable `"PREFIX"` is taken, with the default value being `"/"`. |
| `"hdrs-only"` | Only collect headers from deps (without subdirectory). |
| `"skip-debug-stage"` | If in debug mode, skip installing additional artifacts gathered if no local debugging is needed, but debug targets are nonetheless desired. |
| `"targets"` | Targets to install artifacts from. |

| Config variable | Description |
| --------------- | ----------- |
| `"PREFIX"` | The absolute path that is used as prefix inside generated pkg-config files. The default value for this variable is `"/"`. This variable is ignored if the field `"prefix"` is set. |

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
| `"srcs"` | The sources of the test binary  The resulting test binary is run in an environment where it can assume that the environment variable TEST_TMPDIR points to a directory that may be used exclusively by this test. For convenience, the environment variable TMPDIR is also set to TEST_TMPDIR.  This running of the test is carried out by the implicit dependency on the target `"runner"`. By setting this target in the target layer of this rules repository (instead of letting it default to the respective file), the C/C++ test environment can be modified globally. |
| `"private-hdrs"` | Any additional header files that need to be present when compiling the test binary. |
| `"data"` | Any files the test binary needs access to when running |

| Config variable | Description |
| --------------- | ----------- |
| `"TARGET_ARCH"` | The architecture to build the test for.  Will only be honored, if that architecture is available in the ARCH_DISPATCH map. Otherwise, the test will be built for and run on the host architecture. |
| `"TEST_ENV"` | The environment for executing the test runner. |
| `"TIMEOUT_SCALE"` | Factor on how to scale the timeout for this test. Defaults to 1.0. |
| `"CC_TEST_LAUNCHER"` | List of strings representing the launcher that is prepend to the command line for running the test binary. |
| `"RUNS_PER_TEST"` | The number of times the test should be run in order to detect flakyness. If set, no test action will be taken from cache.  Test runs are summarized by the `["shell/test", "summarizer"]` that is also used by shell tests. |
| `"ARCH_DISPATCH"` | Map of architectures to execution properties that ensure execution on that architecture. Only the actual test binary will be run with the specified execution properties (i.e., on the target architecture); all building will be done on the host architecture. |

### Rule `["shell", "defaults"]`

A rule to provide defaults for the usage of the shell 

 All targets using invocations of the shell use the target `["shel", "defaults"]` to determine which shell to use and how to invoke it. The definition of this default target is probably the only meaningful use of this rule.

| Field | Description |
| ----- | ----------- |
| `"sh"` | The name of the sh binary; if the the field `"toolchain"` is not empty, the value is interpreted as relative to the toolchain directory. |
| `"PATH"` | Paths for looking up system tools. Specifying this field extends values from `"base"`. |
| `"bin dirs"` | Directories of the toolchain that contain additional binaries. Shell-specific rules will add those into PATH. |
| `"base"` | Other targets (using the same rule) to inherit values from. |
| `"toolchain"` | Optional toolchain directory. A collection of artifacts that form the toolchain, in particular the shell itself, where not taken from the ambient host environment. Values provided from base are extended. This field is built for the host. |

### Rule `["shell", "cmds"]`

Execute comands using the shell 

 This rule behaves similar to the built-in `"generic"` rule, however with the difference that the shell toolchain is honored.

| Field | Description |
| ----- | ----------- |
| `"cmds"` | The command to be executed. Individual entries are joined by newline characters; the whole script is then prefixed by commands necessary to set up the work environment using the shell tool chain. |
| `"outs"` | The expected file outputs |
| `"out_dirs"` | The expected output directories |
| `"deps"` | Any  inputs to the argument. Both, artifacts and rufiles of the dependecies are staged into the (effective) working directory of the action. Conflicts are resolved by giving artifacts priority to runfiles, and within each of those priority to ones brought by the latest dependency. |

### Rule `["shell/test", "script"]`

Shell test, given by a test script

| Field | Description |
| ----- | ----------- |
| `"keep"` | List of names (relative to the test working directory) of files that the test might generate that should be kept as part of the output. This might be useful for further analysis of the test |
| `"name"` | A name for the test, used in reporting, as well as for staging the test result tree in the runfiles |
| `"deps"` | Any targets that should be staged (with artifacts and runfiles) into the tests working directory |
| `"test"` | The shell script for the test, launched with sh.  An empty directory is created to store any temporary files needed by the test, and it is made available in the environment variable TEST_TMPDIR. The test should not assume write permissions outside the working directory and the TEST_TMPDIR. For convenience, the environment variable TMPDIR is also set to TEST_TMPDIR.  If the configuration variable RUNS_PER_TEST is set, the environment variable TEST_RUN_NUMBER will also be set to the number of the attempt, counting from 0.  This running of the test is carried out by the implicit dependency on the target `"runner"`. By setting this target in the target layer of this rues repository (instead of letting it default to the respective file), the shell test environment can be modified globally. |

| Config variable | Description |
| --------------- | ----------- |
| `"RUNS_PER_TEST"` | The number of times the test should be run in order to detect flakyness. If set, no test action will be taken from cache.  The individual test runs will be summarized by the implict dependency on the target `"summarizer"`. By setting this target in the target in the target layer of this rues repository (instead of letting it default to the respective file) the layout of the summary can be changed globally. |
| `"TEST_ENV"` | Additional environment for executing the test runner. |
| `"TIMEOUT_SCALE"` | Factor on how to scale the timeout for this test. Defaults to 1.0. |
| `"TARGET_ARCH"` | The architecture to build the test for.  Will only be honored, if that architecture is available in the ARCH_DISPATCH map. Otherwise, the test will be built for and run on the host architecture. |
| `"ARCH_DISPATCH"` | Map of architectures to execution properties that ensure execution on that architecture. Only the actual test binary will be run with the specified execution properties (i.e., on the target architecture); all building will be done on the host architecture. |

### Rule `["CC/foreign/cmake", "library"]`

Library produced by CMake configure, build, and install. 

 All variables accessible to commands and options are: `"TMPDIR"`, `"LOCALBASE"`, `"CC"`, `"CXX"`, `"CFLAGS"`, `"CXXFLAGS"`, `"LDFLAGS"`, and `"AR"`. `"LOCALBASE"` contains the path to the installed artifacts from `"deps"`.

| Field | Description |
| ----- | ----------- |
| `"subdir"` | The subdirectory that contains the entry CMakeLists.txt. Individual directory components are joined with `"/"`. |
| `"name"` | The name of the library (without leading `"lib"` or trailing file name extension), also used as name for pkg-config files. |
| `"version"` | The library version, used for pkg-config files. Individual version components are joined with `"."`. |
| `"stage"` | The logical location of the public headers and library files. Individual directory components are joined with `"/"`. |
| `"options"` | CMake options for the configuration phase (e.g., `["-GNinja", "-Ax64"]`). Variables can be accessed via `"$(<varname>)"`, e.g., `"$(TMPDIR)"` for variable `"$TMPDIR"`. |
| `"defines"` | CMake defines for the configuration phase (e.g., `["CMAKE_BUILD_TYPE=Release"]`). Variables can be accessed via `"$(<varname>)"`, e.g., `"$(TMPDIR)"` for variable `"$TMPDIR"`. |
| `"jobs"` | Number of jobs to run simultaneously. If omitted, CMake's default number is used. |
| `"pre_cmds"` | List of commands executed in the project directory before calling CMake. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via `"$TMPDIR"` which is uniquely reserved for this action. |
| `"post_cmds"` | List of commands executed in the install directory after successful installation but before the output files are collected. Useful for renaming files or directories (e.g., in case of SONAME mismatch). Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via `"$TMPDIR"`, which is uniquely reserved for this action. The CMake source and build directory can be accessed via `"$CMAKE_SOURCE_DIR"` and `"$CMAKE_BINARY_DIR"`, respectively. |
| `"out_hdrs"` | Paths to produced public header files. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_hdr_dirs"` | Paths to produced public header directories. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_libs"` | Paths to produced library files. The path is considered relative to the library directory, which be set via `"lib_prefix"`. Order matters in the case of one library depending on another. |
| `"cflags"` | List of compile flags set for this target and its consumers. |
| `"ldflags"` | Additional linker flags that are required for the consumer of the produced libraries. |
| `"pkg-config"` | Pkg-config file for optional infer of public cflags and ldflags. If multiple files are specified (e.g., one depends on the other), the first one is used as entry. Note that if this field is non-empty the tool `"pkg-config"` must be available in `"PATH"`, which is taken from `["CC", "defaults"]` or the `"ENV"` variable. |
| `"hdr_prefix"` | Prefix where headers will be installed by CMake. Individual directory components are joined with `"/"`. Defaults to `"include"` if not set. |
| `"lib_prefix"` | Prefix where libraries will be installed by CMake. Individual directory components are joined with `"/"`. Defaults to `"lib"` if not set. |
| `"pc_prefix"` | Prefix where pkg-config files will be installed by CMake. Individual directory components are joined with `"/"`. Defaults to `"lib/pkgconfig"` if not set. |
| `"resolve_symlinks"` | Resolve all symlinks in the install directory. This operation is performed immediately after the install, before `"post_cmds"` are run. |
| `"project"` | The CMake project directory. It should contain a single tree artifact |
| `"deps"` | Public dependency on other CC libraries. |

| Config variable | Description |
| --------------- | ----------- |
| `"TIMEOUT_SCALE"` | The scaling of the timeout for the invocation of the foreign build. Defaults to 10. |

### Rule `["CC/foreign/cmake", "data"]`

Data produced by CMake configure, build, and install. 

 All variables accessible to commands and options are: `"TMPDIR"`, `"LOCALBASE"`, `"CC"`, `"CXX"`, `"CFLAGS"`, `"CXXFLAGS"`, `"LDFLAGS"`, and `"AR"`. `"LOCALBASE"` contains the path to the installed artifacts from `"deps"`.

| Field | Description |
| ----- | ----------- |
| `"subdir"` | The subdirectory that contains the entry CMakeLists.txt. Individual directory components are joined with `"/"`. |
| `"options"` | CMake options for the configuration phase. (e.g., `["-GNinja", "-Ax64"]`) |
| `"defines"` | CMake defines for the configuration phase. (e.g., `["CMAKE_BUILD_TYPE=Release"]`) |
| `"targets"` | The CMake targets to build in the specified order (default: `["install"]`). |
| `"jobs"` | Number of jobs to run simultaneously. If omitted, CMake's default number is used. |
| `"pre_cmds"` | List of commands executed in the project directory before calling CMake. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via `"$TMPDIR"` which is uniquely reserved for this action. |
| `"post_cmds"` | List of commands executed in the install directory after successful installation but before the output files are collected. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via `"$TMPDIR"`, which is uniquely reserved for this action. The CMake source and build directory can be accessed via `"$CMAKE_SOURCE_DIR"` and `"$CMAKE_BINARY_DIR"`, respectively. |
| `"out_files"` | Paths to the produced output files. The paths are considered relative to the install directory. Note that `"out_files"` and `"out_dirs"` may not overlap. |
| `"out_dirs"` | Paths to the produced output directories. The paths are considered relative to the install directory. Note that `"out_files"` and `"out_dirs"` may not overlap. |
| `"resolve_symlinks"` | Resolve all symlinks in the install directory. This operation is performed immediately after the install, before `"post_cmds"` are run. |
| `"project"` | The CMake project directory. It should contain a single tree artifact |

| Config variable | Description |
| --------------- | ----------- |
| `"TIMEOUT_SCALE"` | The scaling of the timeout for the invocation of the foreign build. Defaults to 10. |

### Rule `["CC/foreign/make", "library"]`

Library produced by Configure and Make build and install. 

 All variables accessible to commands and options are: `"TMPDIR"`, `"LOCALBASE"`, `"CC"`, `"CXX"`, `"CFLAGS"`, `"CXXFLAGS"`, `"LDFLAGS"`, `"AR"`, and `"PREFIX"`. `"LOCALBASE"` contains the path to the installed artifacts from `"deps"`.

| Field | Description |
| ----- | ----------- |
| `"subdir"` | The subdirectory that contains the configure and Makefile. Individual directory components are joined with `"/"`. |
| `"name"` | The name of the library (without leading `"lib"` or trailing file name extension), also used as name for pkg-config files. |
| `"version"` | The library version, used for pkg-config files. Individual version components are joined with `"."`. |
| `"stage"` | The logical location of the public headers and library files. Individual directory components are joined with `"/"`. |
| `"configure"` | Run ./configure if non-empty. |
| `"configure_options"` | The configure options (the `"--prefix"` option is automatically set. |
| `"targets"` | The Make targets to build in the specified order (default: `["install"]`). |
| `"prefix"` | The prefix used for the Make target. The path will be made absolute and individual directory components are joined with `"/"`. If no prefix is specified, the value from the config variable `"PREFIX"` is taken, with the default value being `"/"`. |
| `"options"` | Make options for the build phase. (e.g., `["-f", "Makefile", "ARCH=x86"]`) |
| `"jobs"` | Number of jobs to run simultaneously. If omitted, Make's default number is used. |
| `"pre_cmds"` | List of commands executed in the project directory before calling Configure or Make. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via `"$TMPDIR"`, which is uniquely reserved for this action. |
| `"post_cmds"` | List of commands executed in the install directory after successful installation but before the output files are collected. Useful for renaming files or directories (e.g., in case of SONAME mismatch). Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via `"$TMPDIR"`, which is uniquely reserved for this action. |
| `"out_hdrs"` | Paths to produced public header files. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_hdr_dirs"` | Paths to produced public header directories. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_libs"` | Paths to produced library files. The path is considered relative to the library directory, which be set via `"lib_prefix"`. Order matters in the case of one library depending on another. |
| `"cflags"` | List of compile flags set for this target and its consumers. |
| `"ldflags"` | Additional linker flags that are required for the consumer of the produced libraries. |
| `"pkg-config"` | Pkg-config file for optional infer of public cflags and ldflags. If multiple files are specified (e.g., one depends on the other), the first one is used as entry. Note that if this field is non-empty the tool `"pkg-config"` must be available in `"PATH"`, which is taken from `["CC", "defaults"]` or the `"ENV"` variable. |
| `"hdr_prefix"` | Prefix where headers will be installed by Make. Individual directory components are joined with `"/"`. Defaults to `"include"` if not set. |
| `"lib_prefix"` | Prefix where libraries will be installed by Make. Individual directory components are joined with `"/"`. Defaults to `"lib"` if not set. |
| `"pc_prefix"` | Prefix where pkg-config files will be installed by Make. Individual directory components are joined with `"/"`. Defaults to `"lib/pkgconfig"` if not set. |
| `"resolve_symlinks"` | Resolve all symlinks in the install directory. This operation is performed immediately after the install, before `"post_cmds"` are run. |
| `"project"` | The Make project directory. It should contain a single tree artifact |
| `"deps"` | Public dependency on other CC libraries. |

| Config variable | Description |
| --------------- | ----------- |
| `"PREFIX"` | The absolute path that is used as prefix inside generated pkg-config files. The default value for this variable is `"/"`. This variable is ignored if the field `"prefix"` is set. |
| `"TIMEOUT_SCALE"` | The scaling of the timeout for the invocation of the foreign build. Defaults to 10. |

### Rule `["CC/foreign/make", "data"]`

Data produced by Configure and Make build and install. 

 All variables accessible to commands and options are: `"TMPDIR"`, `"LOCALBASE"`, `"CC"`, `"CXX"`, `"CFLAGS"`, `"CXXFLAGS"`, `"LDFLAGS"`, `"AR"`, and `"PREFIX"`. `"LOCALBASE"` contains the path to the installed artifacts from `"deps"`.

| Field | Description |
| ----- | ----------- |
| `"subdir"` | The subdirectory that contains the configure and Makefile. Individual directory components are joined with `"/"`. |
| `"configure"` | Run ./configure if non-empty. |
| `"configure_options"` | The configure options (the `"--prefix"` option is automatically set. Variables can be accessed via `"$(<varname>)"`, e.g., `"$(TMPDIR)"` for variable `"$TMPDIR"`. |
| `"targets"` | The Make targets to build in the specified order (default: `["install"]`). |
| `"prefix"` | The prefix used for the Make target. The path will be made absolute and individual directory components are joined with `"/"`. If no prefix is specified, the value from the config variable `"PREFIX"` is taken, with the default value being `"/"`. |
| `"options"` | Make options for the configuration phase (e.g., `["-f", "Makefile", "ARCH=x86", "LD=$(CC)"]`). Variables can be accessed via `"$(<varname>)"`, e.g., `"$(TMPDIR)"` for variable `"$TMPDIR"`. |
| `"jobs"` | Number of jobs to run simultaneously. If omitted, Make's default number is used. |
| `"pre_cmds"` | List of commands executed in the project directory before calling Configure or Make. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via `"$TMPDIR"`, which is uniquely reserved for this action. |
| `"post_cmds"` | List of commands executed in the install directory after successful installation but before the output files are collected. Useful for renaming files or directories. Note that data between `"pre_cmds"` and `"post_cmds"` can be exchanged via `"$TMPDIR"`, which is uniquely reserved for this action. |
| `"out_files"` | Paths to the produced output files. The paths are considered relative to the install directory. Note that `"out_files"` and `"out_dirs"` may not overlap. |
| `"out_dirs"` | Paths to the produced output directories. The paths are considered relative to the install directory. Note that `"out_files"` and `"out_dirs"` may not overlap. |
| `"resolve_symlinks"` | Resolve all symlinks in the install directory. This operation is performed immediately after the install, before `"post_cmds"` are run. |
| `"project"` | The Make project directory. It should contain a single tree artifact |

| Config variable | Description |
| --------------- | ----------- |
| `"PREFIX"` | The absolute path that is used as prefix inside generated pkg-config files. The default value for this variable is `"/"`. This variable is ignored if the field `"prefix"` is set. |
| `"TIMEOUT_SCALE"` | The scaling of the timeout for the invocation of the foreign build. Defaults to 10. |

### Rule `["CC/foreign/shell", "library"]`

Library produced by generic shell commands with toolchain support. 

 All variables accessible to commands and options are: `"TMPDIR"`, `"LOCALBASE"`, `"WORKDIR"`, `"DESTDIR"`, `"CC"`, `"CXX"`, `"CFLAGS"`, `"CXXFLAGS"`, `"LDFLAGS"`, and `"AR"`. `"LOCALBASE"` contains the path to the staged artifacts from `"localbase"` and the installed artifacts from `"deps"`. Furthermore, the variable `"ACTION_DIR"` points to the current action directory, if needed for achieving reproducibility.

| Field | Description |
| ----- | ----------- |
| `"name"` | The name of the library (without leading `"lib"` or trailing file name extension), also used as name for pkg-config files. |
| `"version"` | The library version, used for pkg-config files. Individual version components are joined with `"."`. |
| `"stage"` | The logical location of the public headers and library files. Individual directory components are joined with `"/"`. |
| `"cmds"` | List of commands to execute by `"sh"`. Multiple commands will be joined with the newline character. |
| `"out_hdrs"` | Paths to produced public header files. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_hdr_dirs"` | Paths to produced public header directories. The path is considered relative to the include directory, which be set via `"hdr_prefix"`. Note that `"out_hdrs"` and `"out_hdr_dirs"` may not overlap. |
| `"out_libs"` | Paths to produced library files. The path is considered relative to the library directory, which be set via `"lib_prefix"`. Order matters in the case of one library depending on another. |
| `"cflags"` | List of compile flags set for this target and its consumers. |
| `"ldflags"` | Additional linker flags that are required for the consumer of the produced libraries. |
| `"pkg-config"` | Pkg-config file for optional infer of public cflags and ldflags. If multiple files are specified (e.g., one depends on the other), the first one is used as entry. Note that if this field is non-empty the tool `"pkg-config"` must be available in `"PATH"`, which is taken from `["CC", "defaults"]` or the `"ENV"` variable. |
| `"hdr_prefix"` | Prefix where headers will be installed by Make. Individual directory components are joined with `"/"`. Defaults to `"include"` if not set. |
| `"lib_prefix"` | Prefix where libraries will be installed by Make. Individual directory components are joined with `"/"`. Defaults to `"lib"` if not set. |
| `"pc_prefix"` | Prefix where pkg-config files will be installed by Make. Individual directory components are joined with `"/"`. Defaults to `"lib/pkgconfig"` if not set. |
| `"resolve_symlinks"` | Resolve all symlinks in the install directory. This operation is performed immediately after `"cmds"` were executed. |
| `"project"` | The project directory. It should contain a single tree artifact. It's path can be accessed via the `"WORKDIR"` variable. |
| `"localbase"` | Artifacts to stage to `"LOCALBASE"`. |
| `"deps"` | Public dependency on other CC libraries. |

| Config variable | Description |
| --------------- | ----------- |
| `"PREFIX"` | The absolute path that is used as prefix inside generated pkg-config files. The default value for this variable is `"/"`. This variable is ignored if the field `"prefix"` is set. |
| `"TIMEOUT_SCALE"` | The scaling of the timeout for the invocation of the foreign build. Defaults to 10. |

### Rule `["CC/foreign/shell", "data"]`

Data produced by generic shell commands with toolchain support. 

 All variables accessible to commands and options are: `"TMPDIR"`, `"LOCALBASE"`, `"WORKDIR"`, `"DESTDIR"`, `"CC"`, `"CXX"`, `"CFLAGS"`, `"CXXFLAGS"`, `"LDFLAGS"`, and `"AR"`. `"LOCALBASE"` contains the path to the staged artifacts from `"localbase"` and the installed artifacts from `"deps"`. Furthermore, the variable `"ACTION_DIR"` points to the current action directory, if needed for achieving reproducibility.

| Field | Description |
| ----- | ----------- |
| `"cmds"` | List of commands to execute by `"sh"`. Multiple commands will be joined with the newline character. |
| `"outs"` | Paths to the produced output files in `"DESTDIR"`. |
| `"out_dirs"` | Paths to the produced output directories in `"DESTDIR"`. |
| `"resolve_symlinks"` | Resolve all symlinks in the install directory. This operation is performed immediately after `"cmds"` were executed. |
| `"project"` | The project directory. It should contain a single tree artifact. It's path can be accessed via the `"WORKDIR"` variable. |
| `"localbase"` | Artifacts to stage to `"LOCALBASE"`. |
| `"deps"` | CC targets to install to `"LOCALBASE"`. |

| Config variable | Description |
| --------------- | ----------- |
| `"PREFIX"` | The absolute path that is used as prefix inside generated pkg-config files. The default value for this variable is `"/"`. This variable is ignored if the field `"prefix"` is set. |
| `"TIMEOUT_SCALE"` | The scaling of the timeout for the invocation of the foreign build. Defaults to 10. |

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
| `"drop"` | List of paths to drop from (each of) the `"srcs"` targets before joining and staging. In this way, GLOB constructs can be used is `"srcs"`. |
| `"srcs"` | The files to be staged |
| `"deps"` | Targets of with their runfiles should be added as well. Their staging is not changed. |

### Rule `["patch", "file"]`

Replace a file, logically in place, by a patched version

| Field | Description |
| ----- | ----------- |
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

| Config variable | Description |
| --------------- | ----------- |
| `"defines"` | Set a define to a specific value unless its value is `"null"`. Must contain a list of pairs. The first element of each pair is the define name and the second argument is the value to set. Strings must be properly escaped. Defines generated from this field are added last, so that they can refer to defines from other `"defines*"`,  `"have_*"`, and `"size_*"` values. |
| `"defines1"` | Set a define to `"1"` unless its value is untrue. Must contain a list of pairs. The first element of each pair is the define name and the second argument is the value. |
| `"defines01"` | Set a define to `"0"` or `"1"` depending on its value being true. Must contain a list of pairs. The first element of each pair is the define name and the second argument is the value. |
| `"have_cfile"` | Set a define to `"1"` if the specified C header is in the include path. Must contain a list of pairs. The first element of each pair is the define name and the second argument is the C header file name. |
| `"have_cxxfile"` | Set a define to `"1"` if the specified C++ header is in the include path. Must contain a list of pairs. The first element of each pair is the define name and the second argument is the C++ header file name. |
| `"have_ctype"` | Set a define to `"1"` if the specified C type is defined. Must contain a list of pairs. The first element of each pair is the define name and the second argument is name of the C type. If the specified C type is not a built-in type, additionally the headers `"sys/types.h"`, `"stdint.h"`, and `"stddef.h"` are checked as well. |
| `"have_cxxtype"` | Set a define to `"1"` if the specified C++ type is defined. Must contain a list of pairs. The first element of each pair is the define name and the second argument is name of the C++ type. If the specified C++ type is not a built-in type, additionally the headers `"sys/types.h"`, `"stdint.h"`, and `"stddef.h"` are checked as well. |
| `"have_csymbol"` | Set a define to `"1"` if the specified C symbol is defined by one of the specified headers in the include path. Must contain a list of pairs. The first element of each pair is the define name and the second argument is another pair. This pair's first value is the C symbol to search for and the second value is a list with the header file names to consider for searching. If the header file defines the symbol as a macro it is considered available and assumed to work. |
| `"have_cxxsymbol"` | Set a define to `"1"` if the specified C++ symbol is defined by one of the specified headers in the include path. Must contain a list of pairs. The first element of each pair is the define name and the second argument is another pair. This pair's first value is the C++ symbol to search for and the second value is a list with the header file names to consider for searching. If the header file defines the symbol as a macro it is considered available and assumed to work. |
| `"size_ctype"` | Set a define to size of the specified C type. Must contain a list of pairs. The first element of each pair is the define name and the second argument is another pair. This pair's first value is the C type to check for and the second value is a list with possible sizes as numbers. If none of the specified sizes matches, the action fails. |
| `"size_cxxtype"` | Set a define to size of the specified C++ type. Must contain a list of pairs. The first element of each pair is the define name and the second argument is another pair. This pair's first value is the C++ type to check for and the second value is a list with possible sizes as numbers. If none of the specified sizes matches, the action fails. |

### Rule `["CC/auto", "config_file"]`

Generate a C/C++ config header from a given template 

 Generate a C/C++ configuration header using defines specified via the target configuration. In the usual case, a target using this rule is configured by depending on it from a target that uses the built-in `"configure"` rule. 

 The actual generation of the header file from the template is done by the implicit dependency on the `"runner"` target which can be changed globally by setting this target in the target layer of this repository.

| Field | Description |
| ----- | ----------- |
| `"magic_string"` | The magic string (e.g., `"cmakedefine"`) which identifies in which line we have to `"#define"` or `"#undef"` variables according to what is defined in the config field `"defines"`. |
| `"@only"` | If set, only replace @VAR@ and not $`{VAR}` |
| `"output"` | Name of the header file to generate (incl. file name ext). Components are joined with /. |
| `"input"` | The input configuration file, used as template. |

| Config variable | Description |
| --------------- | ----------- |
| `"defines"` | Set a define to a specific value unless its value is `"null"`. Must contain a list of pairs. The first element of each pair is the define name and the second argument is the value to set. Strings must be properly escaped. Defines generated from this field are added last, so that they can refer to defines from other `"defines*"` values. |

