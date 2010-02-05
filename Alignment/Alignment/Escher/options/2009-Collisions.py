from GaudiKernel.ProcessJobOptions import importOptions
#importOptions("$APPCONFIGOPTS/Brunel/earlyData.py")

from Configurables import (Escher, LHCbApp, ApplicationMgr, NTupleSvc, ATrackSelector )
from Gaudi.Configuration import FileCatalog, EventSelector


#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

Escher().DataType = "2009"
Escher().Simulation = False
Escher().SpecialData += ["fieldOff", "veloOpen", "earlyData"]
#Escher().InputType = "MDF"
Escher().InputType = "DST"
Escher().Detectors = ["OT"]
Escher().EvtMax = 100
Escher().PrintFreq = 1000
Escher().AlignmentLevel = "layers"
Escher().Millepede = True
Escher().Kalman = False
Escher().Incident = "GlobalMPedeFit"
Escher().DatasetName = "63596_1"

ATrackSelector().UseWeights = True
ATrackSelector().UniformCutOff = 5
ATrackSelector().MinEnergyCut = 0

#-- Use latest 2009 database tags for real data
#LHCbApp().DDDBtag   = "head-20090330"
#LHCbApp().CondDBtag = "head-20090402"

LHCbApp().DDDBtag   = "head-20100119"
LHCbApp().CondDBtag = "head-20091112"


# add 2009 collision data files
EventSelector().Input = [
#    "DATAFILE='file:/lhcb/data/2009/DST/00005714/0000/00005714_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='file:/lhcb/data/2009/DST/00005738/0000/00005738_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
    ]

