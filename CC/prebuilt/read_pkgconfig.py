#!/usr/bin/env python3
# Copyright 2023 Huawei Cloud Computing Technology Co., Ltd.
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

import os
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Set


def run_pkgconfig(args: List[str], env: Dict[str, str]) -> str:
    result = subprocess.run(["pkg-config"] + args, env=env, capture_output=True)
    if result.returncode != 0:
        print(result.stderr.decode("utf-8"), file=sys.stderr)
        exit(1)
    return result.stdout.decode("utf-8").strip()


def read_ldflags(pkg: str, args: List[str], env: Dict[str, str]) -> str:
    def libname(filename: str) -> str:
        return filename.split(".")[0]

    local_libs = {
        libname(f)
        for it in os.walk(".") for f in it[2] if f.startswith("lib")
    }

    link_flags = run_pkgconfig(args + ["--libs-only-l", pkg], env).split(" ")

    # deduplicate, keep right-most
    seen: Set[str] = set()
    link_flags = [
        f for f in link_flags[::-1] if f not in seen and not seen.add(f)
    ][::-1]

    def is_local(flag: str) -> bool:
        if not flag.startswith("-l"):
            return False
        lib = libname(flag[3:]) if flag.startswith("-l:") else f"lib{flag[2:]}"
        return lib in local_libs

    return " ".join([f for f in link_flags if not is_local(f)])


def read_pkgconfig() -> None:
    if len(sys.argv) < 3:
        print(f"usage: read_pkgconfig OUT_NAME PC_FILE [PC_ARGS...]")
        exit(1)

    name = sys.argv[1]
    pkg = Path(sys.argv[2]).stem
    args = sys.argv[3:]
    env = dict(os.environ, PKG_CONFIG_PATH="./lib/pkgconfig")

    if name.endswith(".cflags"):
        data = run_pkgconfig(args + ["--cflags-only-other", pkg], env)
    else:
        data = read_ldflags(pkg, args, env)

    with open(f"{name}", 'w') as f:
        f.write(data)


if __name__ == "__main__":
    read_pkgconfig()
