#!/usr/bin/env python
'''
Test loading of module * at top level.
'''

import sys
sys.path.append('../python')

try :
    from PhysSelPython.Wrappers import *
    sys.stdout.write('\ntest_import_wrappers_star:    PASS\n\n')
except :
    sys.stderr.write('\ntest_import_wrappers_star:    FAIL\n\n')
