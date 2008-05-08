# Minimal file for running Gauss from python prompt
# Syntax is:
#   gaudirun.py $GAUSSOPTS/lhcb-200801.py
#
from Gaudi.Configuration import *

#--Allow to use ns (ie. double) in options of EventClockSvc
from Configurables import EventClockSvc, FakeEventTime

ec = EventClockSvc()
ec.EventTimeDecoder = FakeEventTime().name()
ec.FakeEventTime = FakeEventTime()

#--Geometry dependent options
importOptions('$GAUSSOPTS/lhcb-200801.opts')

#--Application name and version
ApplicationMgr( AppName    = 'Gauss',
                AppVersion = 'v31r0' )

