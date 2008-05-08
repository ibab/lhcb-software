#!/usr/bin/env python
from LbUtils import VersionsDB as vdb

import sys

vdb.load(sys.argv[1])
open(sys.argv[2],"w").write(vdb.generateHTML())
