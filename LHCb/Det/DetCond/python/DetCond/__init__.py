# Hack to prevent spurious warnings in GenConfUser.
# See bug #96673, https://savannah.cern.ch/bugs/index.php?96673
#import os,sys
#__path__ = filter(os.path.exists, [os.path.join(d, __name__) for d in sys.path if d])
