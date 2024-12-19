#!/usr/bin/env python3
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

#
# Include scanner
#   Parse file paths from command's space-separated stdout, select those that
#   start with "include/", and copy them to the user-defined output directory.
#
# Usage:
#   include_scan OUT_DIR ARGV...
#
# Example:
#   ./include_scan.py out gcc -isystem include foo.c -E -M
#   -> Generates output directory "out/include" from gcc's output

import subprocess
import sys
import os
import shutil


def include_scan(out_dir: str, cmd: list[str]):
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    if not proc.stdout:
        proc.poll()
        exit(proc.returncode)

    includes: set[str] = set()
    for line in proc.stdout:
        items = line.decode('utf-8').replace('\n', ' ')
        paths = {os.path.normpath(i) for i in items.split(' ')}
        includes |= {p for p in paths if p.startswith('include/')}

    for path in includes:
        out_path = os.path.join(out_dir, path)
        try:
            shutil.copyfile(path, out_path, follow_symlinks=False)
        except:
            try:
                os.makedirs(os.path.dirname(out_path), exist_ok=True)
                shutil.copyfile(path, out_path, follow_symlinks=False)
            except Exception as e:
                print(e, file=sys.stderr)
                exit(1)

    proc.poll()
    if proc.returncode != 0:
        exit(proc.returncode)


if __name__ == '__main__':
    include_scan(out_dir=sys.argv[1], cmd=sys.argv[2:])
