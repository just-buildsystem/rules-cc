# Test Rules

This is a test rule that supports building and installing multiple targets with
a given set of rules. For each target, it can be specified whether building it
should fail or succeed. After processing all targets, additional assertions
(list of shell commands) can be run.

## Setup

The test rules expect to find the following two bindings:
 - `[["@", "test-rules", "", "tree"]]`, which contains a single tree artifact
   with the rules to test.
 - `[["@", "test-just", "", ""]]`, which contains a single executable artifact
   that is the JustBuild binary to use for the tests.

## Rule `["test_rules", "test_case"]`

Define a test case for rule tests.

| Field | Description |
| ----- | ----------- |
| `"name"` | Name of the test (multiple entries are joined). |
| `"targets"` | Target names to build and install. Each target name is prefixed by `"+"` or `"-"`, indicating if the build should fail or not. Targets that build successfully will be installed to a directory named identical to the target name (without the prefix). |
| `"asserts"` | List of commands to execute after all targets were processed. To access artifacts from installed targets, use the corresponding target name as prefix dir (e.g., target `"+foo"` installs to `"./foo/"`). |
| `"data"` | The directory that contains the project with the targets to test. |
