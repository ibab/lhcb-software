from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/Upgrade-WithTruth.py")

from Configurables import Brunel
Brunel().EvtMax = 10
from Configurables import TrackSys,RecSysConf,RecMoniConf,DstConf
##Brunel().MainSequence = [""]
### Set Cheated Pattern reco algorithm for VeloPix
## Should be  TrackSys().TrackPatRecAlgorithms = ["VeloPix","Forward","TsaSeed","Match","Downstream","VeloPixTT"]
##TrackSys().TrackPatRecAlgorithms = ["VeloPix","Forward","TsaSeed","Match","Downstream","VeloPixTT"]
TrackSys().TrackPatRecAlgorithms = ["VeloPix"]
### remove MUON because of the call to TrackMasterFitter in MuonIDChi2Tool
### or something like that + remove "VELO" but nothing velo specific...
RecSysConf().RecoSequence = ["Decoding", "VELOPIX","TT","IT","OT","Tr","Vertex","RICH","CALO","PROTO"]
### remove VELO and Tr from the moni line (need to code something for VELOPIX) as well as Muon (because of muonID)
RecMoniConf().MoniSequence=["CALO","OT","ST","PROTO","TT","IT"]
### remove Pat and tr as well as Mun 
Brunel().MCCheckSequence = ["Pat","TT","IT","OT","CALO","PROTO"]
### skip the MCLink sequence, for some reason can't unpack????
Brunel().MCLinksSequence = []
### skip the DST writing,not needed for the moment and anyway don't have some requiered container (trackbest for ex)
Brunel().OutputType = "VELOPIXDST"

from Configurables import LHCbApp
LHCbApp().DDDBtag   = "velopix-mul-20091116"
LHCbApp().CondDBtag = "sim-20091112-vc-md100"


from Configurables import NTupleSvc , EventSelector                 
NTupleSvc ( Output = [ "FILE1 DATAFILE='VeloPixTTTest.root' TYPE='ROOT' OPT='NEW'"] )
EventSelector().FirstEvent = 1

from Gaudi.Configuration import * 
IODataManager().AgeLimit = 1
from Gaudi.Configuration import * 

EventSelector().Input   = [
"   DATAFILE='file:/data/shared/cocov/Data/00005898_00000005_2.digi' TYP='POOL_ROOTTREE' OPT='READ'"]
###importOptions("$BRUNELOPTS/VeloPixDIGI.py")
###FileCatalog().Catalogs = [ 'xmlcatalog_file:$BRUNELOPTS/VeloPixSIM.xml' ]
