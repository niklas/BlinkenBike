#!/usr/bin/env python

# platformio.ini can only run python scripts, so this can be considered a
# wrapper
from subprocess import call
call(["bin/generate_code.rb"])
