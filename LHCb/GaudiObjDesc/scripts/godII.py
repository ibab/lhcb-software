#!/usr/bin/env python
import os
import sys
# Ensure we can find the GaudiObjDesc module even if the PYTHONPATH does not
# have it. (Assume the standard relative locations of the modules)
sys.path.append(os.path.join(os.path.dirname(os.path.dirname(__file__)), 'python'))

from GaudiObjDesc.godII import godII

g = godII(sys.argv)
g.doit()
