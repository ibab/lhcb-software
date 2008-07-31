# Minimal file for running Gauss from python prompt
# Syntax is:
#   gaudirun.py $GAUSSOPTS/lhcb-200802.py
#
from Gaudi.Configuration import *

#--Allow to use ns (ie. double) in options of EventClockSvc
from Configurables import EventClockSvc, FakeEventTime

ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")

#--Geometry dependent options
importOptions('$GAUSSOPTS/lhcb-2008-450GeV-BOff.opts')
