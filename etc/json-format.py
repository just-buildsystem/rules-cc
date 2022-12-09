#!/usr/bin/env python3

# Copyright (C) 2021-2022 Klaus Aehlig. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import json
import sys

def is_simple(entry):
  if isinstance(entry, list):
    return len(entry) == 0
  if isinstance(entry, dict):
    return len(entry) == 0
  return True

def is_short(entry, indent):
  return len(json.dumps(entry)) + indent < 80

def hdumps(entry, *, _current_indent=0):
  if is_short(entry, _current_indent):
    return json.dumps(entry)
  if isinstance(entry, list) and entry:
    result = "[ " + hdumps(entry[0], _current_indent=_current_indent+2)
    for x in entry[1:]:
      result += "\n" + " " * _current_indent + ", "
      result += hdumps(x, _current_indent=_current_indent+2)
    result += "\n" + " " * _current_indent + "]"
    return result
  if isinstance(entry, dict) and entry:
    result = "{ "
    is_first = True
    for k in entry.keys():
      if not is_first:
        result += "\n" + " " * _current_indent + ", "
      result += json.dumps(k) + ":"
      if is_simple(entry[k]):
        result += " " + json.dumps(entry[k])
      elif is_short(entry[k], _current_indent + len(json.dumps(k)) + 4):
        result += " " + json.dumps(entry[k])
      else:
        result += "\n" + " " * _current_indent + "  "
        result += hdumps(entry[k], _current_indent=_current_indent+2)
      is_first = False
    result += "\n" + " " * _current_indent + "}"
    return result
  return json.dumps(entry)

if __name__ == "__main__":
  data = json.load(sys.stdin)
  print(hdumps(data))
