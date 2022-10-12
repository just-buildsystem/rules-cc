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


import os
import time

RESULTS = {}

time_start = time.time()
time_stop = 0

for attempt in os.listdir("."):
    if os.path.isdir(attempt):
        with open(os.path.join(attempt, "result")) as f:
            result = f.read().strip()
        RESULTS[result] = RESULTS.get(result, []) + [int(attempt)]
        try:
            with open(os.path.join(attempt, "time-start")) as f:
                time_start = min(time_start, float(f.read().strip()))
        except:
            pass
        try:
            with open(os.path.join(attempt, "time-stop")) as f:
                time_stop = max(time_start, float(f.read().strip()))
        except:
            pass

result = "UNKNOWN"
if set(RESULTS.keys()) <= set(["PASS", "FAIL"]):
    if not RESULTS.get("FAIL"):
        result = "PASS"
    elif not RESULTS.get("PASS"):
        result = "FAIL"
    else:
        result = "FLAKY"
with open("result", "w") as f:
    f.write("%s\n" % (result,))

with open("time-start", "w") as f:
    f.write("%d\n" % (time_start,))
with open("time-stop", "w") as f:
    f.write("%d\n" % (time_stop,))

with open("stdout", "w") as f:
    f.write("Summary: %s\n\n" % (result,))
    f.write("PASS: %s\n" % (sorted(RESULTS.get("PASS", [])),))
    f.write("FAIL: %s\n" % (sorted(RESULTS.get("FAIL", [])),))
    RESULTS.pop("PASS", None)
    RESULTS.pop("FAIL", None)
    if RESULTS:
        f.write("\nother results: %r\n" % (RESULTS,))

with open("stderr", "w") as f:
    pass
