#!/usr/bin/env python3

import json
import os
import sys

status = 0
invocations = {}

for lint in sorted(os.listdir()):
    if os.path.isdir(lint):
        with open(os.path.join(lint, "result")) as f:
            result = f.read().strip()
        if result != "PASS":
            status = 1
            with open(os.path.join(lint, "stdout")) as f:
                print(f.read())
            with open(os.path.join(lint, "stderr")) as f:
                print(f.read())
        with open(os.path.join(lint, "out/invocation.json")) as f:
            invocation = json.load(f)
        invocations[invocation[0]] = invocation[1:]

with open(os.path.join(os.environ["OUT"], "invocations.json"), "w") as f:
    json.dump(invocations, f)

sys.exit(status)
