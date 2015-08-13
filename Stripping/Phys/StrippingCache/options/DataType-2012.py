# $Id: DataType.py 184170 2015-02-22 11:45:31Z ibelyaev $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Configurables       import DaVinci


DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax    = 10000
DaVinci().PrintFreq = 2000
DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# database
DaVinci().DDDBtag   = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"

# input file
from Gaudi.Configuration import importOptions 
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")

DaVinci().EvtMax = 100
