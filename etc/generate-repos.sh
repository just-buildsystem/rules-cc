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

: ${BRANCH:=v1.6.4-alpha+20260102}
: ${OUTFILE:=${ROOT}/etc/repos.json}

just-import-git --as just -C ${ROOT}/etc/repos.template.json \
        -b ${BRANCH} https://github.com/just-buildsystem/justbuild \
        | ${ROOT}/etc/json-format.py > ${OUTFILE}
