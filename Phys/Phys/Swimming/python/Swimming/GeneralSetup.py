#some more general options which come before
#the gaudi.initialize step

# THIS IS OBSOLETE, IT IS ONLY BEING KEPT IN FOR COMPATIBILITY
# WITH THE PRE-CONFIGURABLE ERA SWIMMING SCRIPT

from LHCbKernel.Configuration import *
from Configurables import DataOnDemandSvc,CondDB
from Configurables import DstConf, CaloDstUnPackConf
from Configurables import Gaudi__IODataManager as IODataManager
CaloDstUnPackConf ( Enable = True )
DstConf           ( EnableUnpack = ["Reconstruction","Stripping"] ) 
IODataManager().UseGFAL=False
importOptions("$STDOPTS/DecodeRawEvent.py")
appConf = ApplicationMgr()
appConf.HistogramPersistency = 'ROOT'
appConf.ExtSvc += ['DataOnDemandSvc']
EventSelector().PrintFreq = -1
EventSelector().OutputLevel = 6 
