#!/bin/sh
# Copyright 2022 Huawei Cloud Computing Technology Co., Ltd.
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

set -eu

readonly ROOT=$(readlink -f $(dirname $0)/..)

: ${OUTFILE:=README.md}

doc2md() {
  local MAIN="$1"
  local MODULE="$2"
  local RULE="$3"

  local RULE_DOC="$(just-mr --main "$MAIN" describe --rule --json "$MODULE" "$RULE")"
  local DOC="$(echo "$RULE_DOC" | jq -r '.doc')"
  local FIELD_DOC="$(echo "$RULE_DOC" | jq -r '.field_doc')"

  echo "## Rule \`[\"$MODULE\", \"$RULE\"]\`"
  echo
  echo "$DOC" \
    | jq -r '[.[] as $v | if ($v == "") then "\n\n" else $v end] | join(" ")' \
    | sed 's/\("[^"]*"\|\[[^]]*\]\|{[^}]*}\)/`\1`/g'
  echo
  echo "| Field | Description |"
  echo "| ----- | ----------- |"
  echo "$FIELD_DOC" \
    | jq -r 'keys_unsorted[] as $k | "| \"\($k)\" | \(.[$k] | join(" ")) |"' \
    | sed 's/\("[^"]*"\|\[[^]]*\]\|{[^}]*}\)/`\1`/g'
  echo
}

rm -f "$OUTFILE"
( doc2md rules CC library
  doc2md rules CC binary
  doc2md rules CC install-with-deps
  doc2md rules CC/test test
  doc2md rules shell/test script
  doc2md rules proto library
  doc2md rules data staged
  doc2md rules patch file
  doc2md rules CC/auto config
  doc2md rules CC/IDE headers
) >> "$OUTFILE"


