#!/usr/bin/env python3

import json
import os
import sys

with open(sys.argv[1]) as f:
    invocations = json.load(f)

expected = set(sys.argv[2:])
found = set()

for name, cmd in invocations.items():
    print("- %s compiled as %r" % (name, cmd))
    found.add(os.path.basename(name))

if expected != found:
    print()
    print("Found: %r" % (found,))
    print("missing:%r" % (expected - found,))
    print("unexpected: %r" % (found - expected,))
    sys.exit(1)
else:
    print("OK")
