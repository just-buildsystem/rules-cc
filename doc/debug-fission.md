Debug fission support in C/C++ rules
====================================

Motivation
----------

Building with debug symbols is needed for debugging purposes, however it
results in artifacts that are many times larger than their release versions.
In general, debug builds are also slower than release builds due to various
reasons, the main ones being the disabling of certain code optimizations (in
order to allow debuggers to properly work) and debug-only checks and
diagnostics. Furthermore, sections of debug symbols from common dependencies can
be replicated many times between different artifacts, but also inside single
artifacts.

In the cases where one does not produce separate release and debug versions,
it is usual to just generate the debug artifacts, then strip the debug symbols
from them to obtain a pseudo-release version. Even in this case, being able to
reduce the time and space requirements for producing the debug artifacts in the
first place is of value.

Moreover, distributions usually provide debug information in different packages
and for that purpose apply themselves debug fission or stripping techniques.
Projects that provide separated debug information have thus an advantage in
getting accepted by reducing the work involved in packaging them for distros.

This method can be applied when one already splits builds into their
constituting compile and link steps, which is the case with most modern build
tools, including *justbuild*. Debug fission proposes that the compilation step
of a debug build produce, instead of one object file, two artifacts: a `.dwo`
DWARF file containing all the debug symbols of the compilation unit, and the
(now smaller) `.o` object file containing now only references to the debug
symbols from the `.dwo` file. These object files can be linked as usual to
produce the final build artifact, and the `.dwo` files can be either retained
as-is, or packed into a corresponding `.dwp` file.

By splitting the debug symbols of each compilation unit into separate
artifacts, these can be cached and reused as needed, removing any previous
debug symbols duplication across build artifacts. This in general also has a
beneficial impact on the build times. Moreover, the stripped artifacts remain
quasi-identical (e.g., executables differ only in their internal Build ID).

Usage
-----

Our approach targets specifically Linux ELF files and `gdb(1)`. This is,
however, more than enough to cover the most used UNIX-like platforms and most
general purpose debugging tools, which rely on `gdb(1)`.

To enable a debug fission build, simply provide as `"DEBUG"` configuration
variable a map providing the necessary mandatory and optional fields, as
described in the documentation of the variable in the rules.

For example, a configuration of
``` json
{ "DEBUG": 
  { "USE_DEBUG_FISSION": true
  , "FISSION_CONFIG": 
    { "USE_SPLIT_DWARF": true
    , "DWARF_VERSION": 4
    }
  }
}
```
instructs that the debug fission rules should be used and that the compile flags
`-gsplit-dwarf` and `-gdwarf-4` should be added additionally to the `-g` flag
(which is set for any debug build with no other flags otherwise set).

**IMPORTANT:** Currently only the `-gslit-dwarf` compile flag can trigger the
generation of `.dwo` files and therefore the entry `"USE_SPLIT_DWARF": true`
should always be provided if debug fission rules are used (i.e.,
`"USE_DEBUG_FISSION": true`).

Additional notes
----------------

Below we provide more context, possible caveats, and known limitations to the
currently supported compile and/or link flags, which can be enabled via various
subfields of the `"FISSION_CONFIG"` debug configuration entry.

### DWARF format versions

Each toolchain supporting generation of debug symbols using the DWARF format
comes with a default in terms of which version of this format is used (in our
rules providable explicitly via the `"DWARF_VERSION"` subfield). Basically all
reasonably modern toolchains (GCC >=4.8.1, Clang >=7.0.0 at least) and debugging
tools (GDB >= 7.0) use DWARFv4 by default, with the more recent versions having
already switched to using the newer, upward compatible
[DWARFv5](https://dwarfstd.org/dwarf5std.html) format. However, the degree of
implementation and default support of the various compilers and tools differs,
so we recommend to always use version 4. 

### Support for `--gdb-index` link flag

This flag (in our rules enabled via the `"USE_GDB_INDEX"` flag) enables, in
linkers that support it, an optimization which bundles certain debug symbols
sections together into a single `.gdb_index` section, reducing the size of the
final artifact (quite significantly for large artifacts) and drastically
improving the debugger start time, but at the cost of a slower linking step.

As mentioned, this flag is not available to all linkers, so below is a
non-exhaustive listing to guide you:

- Known supported linkers: `lld` (LLVM >=7), `gold`* (binutils >=2.24), `mold` (>=2.3)

  *As per the [release notes](https://lwn.net/Articles/1007541/), the main
  `binutils` 2.44 (2025-02-02) release tarball does **NOT** contain the `gold`
  linker anymore, but can still be acquired moving forward (for an unknown, but
  still presumably reasonable long time) in a separate amended release tarball
  for all even-numbered releases of `binutils` >=2.44. Importantly, this also
  affects the distribution of the DWARF packing tool (`dwp`) with `binutils`,
  which will also be available moving forwards only together with the `gold`
  linker.

- Known unsupported linkers: `ld` (GNU)

### Separate types debug sections and artifact size reduction

The `-fdebug-types-section` compile flag (in our rules enabled via the
`"USE_DEBUG_TYPES_SECTION"` flag) enables, for toolchains supporting at least
DWARFv4 (i.e., any reasonably modern toolchain), an optimization that produces
separate debug symbols sections for certain large data types, thus providing the
linker the opportunity to deduplicate more debug information, resulting in
smaller artifacts. 

More performant approaches to reduce the size of the debug information exist,
but are not as straight-forward to implement as enabling a flag. For example,
Fedora opted instead to use the `dwz` compression tool, another known approach,
and make it its default for handling debug RPMs already since version 18
(2013-01-15). This can be a possible future avenue for further expansion of our
rules' debug fission feature set.

References
----------

- The introduction of [debug fission](https://gcc.gnu.org/wiki/DebugFission)
  support in GCC 4.7.
- A comprehensive and modern look into
  [separating debug symbols from executables](https://www.tweag.io/blog/2023-11-23-debug-fission).
