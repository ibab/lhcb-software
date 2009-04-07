# Minimal file for running Gauss from python prompt
#
# Syntax is:
#   gaudirun.py $GAUSSOPTS/Gauss-2008.py OtherSteerig.{opts,py}
#
# To specify an EventType you can give it as argument to follow
#   gaudirun.py $GAUSSOPTS/Gauss-2008.py $DECFILESROOT/options/30000000.opts
#
# Or for a slightly fancier version where the name of the file is automatically
# set look at the example EvtType.py and use it as
#   gaudirun.py $GAUSSOPTS/Gauss-2008.py $GAUSSOPTS/EvtType.py
#
from Gauss.Configuration import *

#--Geometry dependent options, use information from SIMCOND for positions
LHCbApp().DDDBtag = "head-20090112"
LHCbApp().CondDBtag = "sim-20090112"

importOptions('$GAUSSOPTS/Gauss.py')
