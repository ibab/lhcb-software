try :
    import subprocess
    import optparse
except ImportError:
    from _helpers import subprocess
    from _helpers import optparse


import sys
sys.modules["subprocess"] = subprocess
sys.modules["optparse"] = optparse
import subprocess
import optparse
del sys


import Env, Option
import CMT, Log, Set
import afs, castor
import Storage
import Script
