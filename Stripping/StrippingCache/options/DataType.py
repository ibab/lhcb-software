# $Id: $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Gaudi.Configuration import *
from Configurables import DaVinci


DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax    = 10000
DaVinci().PrintFreq = 2000
DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# database
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")

DaVinci().EvtMax = 100
