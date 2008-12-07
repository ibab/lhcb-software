# Minimal file for running Gauss from python prompt
#
# Syntax is:
#   gaudirun.py $GAUSSOPTS/lhcb-2008.py OtherSteerig.{opts,py}
#
# To specify an EventType you can give it as argument to follow
#   gaudirun.py $GAUSSOPTS/lhcb-2008.py $DECFILESROOT/options/30000000.opts
#
# Or for a slightly fancier version where the name of the file is automatically
# set look at the example EvtType.py and use it as
#   gaudirun.py $GAUSSOPTS/lhcb-2008.py $GAUSSOPTS/EvtType.py
#
from Gauss.Configuration import *

#--Allow to use ns (ie. double) in options of EventClockSvc
ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")
ec.EventTimeDecoder.StartTime = 0
ec.EventTimeDecoder.TimeStep  = 25*SystemOfUnits.ns

#--Generic Gauss options
importOptions('$GAUSSOPTS/Gauss.opts')

#--Geometry dependent options, use information from SIMCOND for positions
LHCbApp( DataType = "2008", Simulation = True )

giGaGeo = GiGaGeo()
giGaGeo.UseAlignment      = True
giGaGeo.AlignAllDetectors = True
importOptions('$GAUSSOPTS/SimVeloGeometry.py')  # -- To misalign VELO

