###import GaudiPython
from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/Upgrade-WithTruth.py")

from Configurables import Brunel
Brunel().EvtMax = 50
from Configurables import TrackSys,RecSysConf,RecMoniConf,DstConf
TrackSys().TrackPatRecAlgorithms = ["VP","Forward","TsaSeed","Match","Downstream","VPTT"]
### remove MUON because of the call to TrackMasterFitter in MuonIDChi2Tool
### or something like that + remove "VELO" but nothing velo specific...
RecSysConf().RecoSequence = ["Decoding", "VP","TT","IT","OT","Tr","Vertex","RICH","CALO","PROTO","MUON"]
### remove VELO and Tr from the moni line (need to code something for VP) as well as Muon (because of muonID)
RecMoniConf().MoniSequence=["CALO","OT","ST","PROTO","TT","IT"]
### remove Pat and tr as well as Mun 
Brunel().MCCheckSequence = ["Pat","TT","IT","OT","CALO","PROTO"]
### skip the MCLink sequence, for some reason can't unpack????
Brunel().MCLinksSequence = []
### skip the DST writing,not needed for the moment and anyway don't have some requiered container (trackbest for ex)
Brunel().OutputType = "DST"

from Configurables import LHCbApp
LHCbApp().DDDBtag   = "vp-mul-20100909"
LHCbApp().CondDBtag = "sim-20100510-vc-md100"


from Configurables import NTupleSvc , EventSelector                 
NTupleSvc ( Output = [ "FILE1 DATAFILE='VPTuple.root' TYPE='ROOT' OPT='NEW'"] )
EventSelector().FirstEvent = 1

from Gaudi.Configuration import * 
IODataManager().AgeLimit = 1
from Gaudi.Configuration import * 

EventSelector().Input   = [
"   DATAFILE='file:/data4/cocov/VP/Boole-50ev-20100922.digi' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs = [ 'xmlcatalog_file:/data4/cocov/VP/Boole_Gauss-50ev-20100922.xml' ]
