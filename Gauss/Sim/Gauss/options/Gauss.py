# Minimal file for running Gauss from python prompt
#
# Syntax is:
#   gaudirun.py $GAUSSOPTS/Gauss.py OtherSteerig.{opts,py}
#
# To specify an EventType you can give it as argument to follow
#   gaudirun.py $GAUSSOPTS/Gauss.py $DECFILESROOT/options/30000000.opts
#
# To set the random seeds and number of events do the following
#   gaudirun.py $GAUSSOPTS/Gauss.py $DECFILESROOT/options/30000000.opts \
#               $GAUSSOPTS/Gauss-Job.py
#
from Gauss.Configuration import *

theApp = Gauss()
