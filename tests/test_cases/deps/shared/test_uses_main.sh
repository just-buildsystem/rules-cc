#!/bin/sh

set -e

./main_uses_bar | grep main
./main_uses_bar | grep bar
./main_uses_bar | grep foo
