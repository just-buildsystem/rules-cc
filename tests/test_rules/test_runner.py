#!/usr/bin/env python3
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

import json
import os
import subprocess
import time

time_start: float = time.time()
time_stop: float = 0
result: str = "UNKNOWN"
stderr: str = ""
stdout: str = ""


def dump_results() -> None:
    with open("result", "w") as f:
        f.write("%s\n" % (result, ))
    with open("time-start", "w") as f:
        f.write("%d\n" % (time_start, ))
    with open("time-stop", "w") as f:
        f.write("%d\n" % (time_stop, ))
    with open("stdout", "w") as f:
        f.write("%s\n" % (stdout, ))
    with open("stderr", "w") as f:
        f.write("%s\n" % (stderr, ))


dump_results()

with open('config.json') as f:
    config = json.load(f)

os.makedirs("./outs")

# install targets
failed_targets = 0
stdout += "Test targets:\n"
for t in config.get('targets', []):
    target = t[1:]
    should_fail = t[0] == "-"
    ret = subprocess.run([
        "./bin/just", "install", "--local-build-root", "./build_root", "-C",
        "repos.json", "-o", "/".join(["./outs", target]),
        "-c", "conf_vars.json", target
    ],
                         capture_output=True)
    success = ret.returncode != 0 if should_fail else ret.returncode == 0
    failed_targets += 0 if success else 1
    stdout += f"  [{'PASS' if success else 'FAIL'}] {target}\n"
    stderr += "".join([
        f"stdout/stderr of test target '{target}':\n",
        ret.stdout.decode("utf-8"),
        ret.stderr.decode("utf-8"), "\n"
    ])
stdout += f"  {failed_targets} targets failed\n"

# run asserts
failed_asserts = 0
stdout += "Test asserts:\n"
for cmd in config.get('asserts', []):
    ret = subprocess.run(cmd, cwd="./outs", shell=True, capture_output=True)
    success = ret.returncode == 0
    failed_asserts += 0 if success else 1
    stdout += f"  [{'PASS' if success else 'FAIL'}] {cmd}\n"
    stderr += "".join([
        f"stdout/stderr of test assert '{cmd}':\n",
        ret.stdout.decode("utf-8"),
        ret.stderr.decode("utf-8"), "\n"
    ])
stdout += f"  {failed_asserts} asserts failed\n"

retval = min(failed_targets + failed_asserts, 125)
result = "PASS" if retval == 0 else "FAIL"

time_stop = time.time()
dump_results()
exit(retval)
