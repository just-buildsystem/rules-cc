# Copyright 2024 Huawei Cloud Computing Technology Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from runner import (
    compute_value,
    get_tokens,
    handle01,
    replace_value,
    undefine,
)

from hypothesis import given
from hypothesis.strategies import text
import re


def test_get_tokens():
    magic_string = "cmakedefine"
    match, is_01 = get_tokens(
        f"# \t \t {magic_string} \t  TOKEN_KEY  \t\t  TOKEN_VALUE \t\n", magic_string
    )
    assert match
    assert not is_01
    groups = match.groups()
    assert len(groups) == 6
    assert groups[1] == magic_string
    assert groups[3] == "TOKEN_KEY"
    assert groups[5] == "TOKEN_VALUE \t"

    match, _ = get_tokens("#cmakedefine TOKEN_KEY TOKEN_VALUE", magic_string)
    assert match
    groups = match.groups()
    assert groups[1] == magic_string
    assert groups[3] == "TOKEN_KEY"
    assert groups[5] == "TOKEN_VALUE"

    match, _ = get_tokens("#cmakedefine TOKEN_KEY @TOKEN_VALUE@", magic_string)
    assert match
    groups = match.groups()
    assert len(groups) == 6
    assert groups[1] == magic_string
    assert groups[3] == "TOKEN_KEY"
    assert groups[5] == "@TOKEN_VALUE@"

    match, _ = get_tokens("#cmakedefine TOKEN_KEY ${TOKEN_VALUE}", magic_string)
    assert match
    groups = match.groups()
    assert len(groups) == 6
    assert groups[1] == magic_string
    assert groups[3] == "TOKEN_KEY"
    assert groups[5] == "${TOKEN_VALUE}"

    match, _ = get_tokens("#cmakedefine TOKEN_KEY", magic_string)
    assert match
    groups = match.groups()
    assert len(groups) == 6
    assert groups[1] == magic_string
    assert groups[3] == "TOKEN_KEY"
    assert groups[5] == ""

    match, _ = get_tokens("#differnt_magic_string TOKEN_KEY", magic_string)
    assert not match

    match, is_01 = get_tokens(f"#{magic_string}01 TOKEN_KEY", magic_string)
    assert match
    assert is_01

    match, _ = get_tokens(f'#cmakedefine FOO "@FOO@"', "cmakedefine")
    groups = match.groups()
    assert groups[5] == '"@FOO@"'


def test_handle_01():
    line = "#cmakedefine01 FOO"
    tokens, _ = get_tokens(line, "cmakedefine")
    assert handle01(line, tokens, True) == "#define FOO 1"
    assert handle01(line, tokens, False) == "#define FOO 0"


def test_undefine():
    lines = [
        "#cmakedefine FOO",
        "#cmakedefine FOO foo",
        "#cmakedefine FOO ${FOO}",
        "#cmakedefine FOO @FOO@",
    ]
    for line in lines:
        tokens, _ = get_tokens(line, "cmakedefine")
        assert undefine(tokens) == "/* #undef FOO */"


def test_replace_value():
    line = "#cmakedefine FOO whatever@VVV@${FFF}"
    tokens, _ = get_tokens(line, "cmakedefine")
    groups = tokens.groups()
    assert len(groups) == 6
    assert groups[1] == "cmakedefine"
    assert groups[3] == "FOO"
    assert groups[5] == "whatever@VVV@${FFF}"
    assert replace_value(tokens, "FOO", "VALUE") == "#define FOO VALUE"


def test_compute_value():
    token = "@KEY1@foo${KEY2}@KEY3@@KEY4@"
    key_value_dict = {
        "KEY1": "value1",
        "KEY2": "value2",
        "KEY3": "value3",
        "KEY4": "value4",
        "KEY5": "0",
    }

    assert compute_value(token, True, key_value_dict) == "value1foo${KEY2}value3value4"
    assert compute_value(token, False, key_value_dict) == "value1foovalue2value3value4"
    assert compute_value("@KEY5@", True, key_value_dict) == "0"


@given(text())
def test_substitution_with_given_value(s):
    kv = {"FOO": s}
    assert compute_value("@FOO@", True, kv) == s
    assert compute_value("${FOO}", False, kv) == s
    assert compute_value("${FOO}", True, kv) == "${FOO}"
