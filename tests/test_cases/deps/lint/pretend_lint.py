#!/usr/bin/env python3

import json
import os
import subprocess
import sys

# log the invocation
with open(os.path.join(os.environ["OUT"], "invocation.json"), "w") as f:
    json.dump(sys.argv[1:], f)

# verify the given command succeeds
result = subprocess.run(sys.argv[2:])

sys.exit(result.returncode)
