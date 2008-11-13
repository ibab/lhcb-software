#!/usr/bin/env sh

source `which SetupProject.sh` LCGCMT lcgnightlies

python `which startNightlies.py` "$@"

