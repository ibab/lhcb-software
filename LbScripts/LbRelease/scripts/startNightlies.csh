#!/usr/bin/env csh

source `which SetupProject.csh` LCGCMT lcgnightlies

python `which startNightlies.py` ${*:q}

