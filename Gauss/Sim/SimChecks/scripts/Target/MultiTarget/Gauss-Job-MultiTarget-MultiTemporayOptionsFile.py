


##############################################################################
# File for running Gauss with MC09 configuration and beam conditions as in
# production (5 TeV beams, nu=1, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-MC09.py <someInputJobConfiguration>.py
##############################################################################

from Gaudi.Configuration import *
from Gauss.Configuration import *


importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam4000GeV-md100-2012-nu2.5.py")

##############################################################################
# Database tags must be set and are defined in a separate file
##############################################################################
from Configurables import LHCbApp
from Configurables import DDDBConf


#DDDBConf(DbRoot = "$SQLDDDBROOT/db/myDDDB-Upgrade-TargetGeom-January2014/TargetsDet.xml")

target = 'Target_1mmAl'


from Configurables import CondDB
CondDB().Upgrade = True
LHCbApp().DDDBtag   = "dddb-20140120"
LHCbApp().CondDBtag = "sim-20131108-vc-md100"
LHCbApp().Simulation = True


Gauss.DetectorGeo = {"Detectors": [ ] }
Gauss.DetectorSim = {"Detectors": [ ] }
Gauss.DetectorMoni ={"Detectors": [ ] }

Gauss.DataType = "Upgrade"
Gauss.PhysicsList = {"Em":'NoCuts', "Hadron":'QGSP_BERT', "GeneralPhys":True, "LHCbPhys":True}



def targetGeo():
    from Configurables import GiGaInputStream
    geo = GiGaInputStream('Geo')
#    geo.StreamItems      = ["/dd/Structure/TargetDet/"+target] #Adds only the target you are currently looking at
    geo.StreamItems      = ["/dd/Structure/TargetDet"]          #Adds all targets at once
    
    from Configurables import SimulationSvc
    SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHesOff.xml" 

appendPostConfigAction(targetGeo)

# --- Switch off delta rays
Gauss.DeltaRays = False

# --- activate GaussTargetMultiplicity tool
def addMyTool():
    from Configurables import GiGa, GiGaTrackActionSequence
    giga = GiGa()
    giga.addTool( GiGaTrackActionSequence("TrackSeq") , name = "TrackSeq" )
    giga.TrackSeq.Members.append( "GaussTargetMultiplicity" )

appendPostConfigAction(addMyTool)

# --- Configure the tool
from Configurables import GiGa, GiGaTrackActionSequence, GaussTargetMultiplicity
giga = GiGa()
giga.addTool( GiGaTrackActionSequence("TrackSeq") , name = "TrackSeq" )
giga.TrackSeq.addTool( GaussTargetMultiplicity )
giga.TrackSeq.GaussTargetMultiplicity.InteractionVolumeName = [ "/dd/Structure/TargetDet/"+target+"#pv" + target.replace('Target','Targ') ]
giga.TrackSeq.GaussTargetMultiplicity.InteractionMaterialName = []
giga.TrackSeq.GaussTargetMultiplicity.InteractionVolumeString = [target]
#giga.TrackSeq.GaussTargetMultiplicity.OutputLevel = DEBUG


#
# Options specific for a given job
# ie. setting of random number seed and name of output files
#
from Gauss.Configuration import *
from Configurables import CondDB, LHCbApp, DDDBConf, CondDBAccessSvc
from Configurables import Gauss

Gauss().Production = 'PGUN'

from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts=10
LHCbApp().EvtMax = nEvts
Gauss().Production='PGUN'

Gauss().OutputType = 'NONE'
#Gauss().Histograms = 'NONE'
#--Set name of output files for given job (uncomment the lines)
#  Note that if you do not set it Gauss will make a name based on event type,
#  number of events and the date
#idFile = 'Test43r3'
#HistogramPersistencySvc().OutputFile = idFile+'-histos.root'
#
#OutputStream("GaussTape").Output = "DATAFILE='PFN:%s.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"%idFile
# --- Save ntuple with hadronic cross section information
ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
NTupleSvc().Output = ["FILE1 DATAFILE='Multi_Piminus_inAl.root' TYP='ROOT' OPT='NEW'"]

importOptions ("TargetMaterialGunMultiTargetLocalTemporary.py")


