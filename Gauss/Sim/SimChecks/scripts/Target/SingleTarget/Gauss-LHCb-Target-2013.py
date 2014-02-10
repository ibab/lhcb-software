##############################################################################
# Option file to run hadronic cross section checks.
# You may change the terget type: Al, Si, Be available and 1, 5, 10 mm
# For moreusage informations: https://twiki.cern.ch/twiki/bin/view/LHCb/TargetStudy
# 
# Last modified: Luca Pescatore, 8/02/2014
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
target = 'Target_10mmAl'


from Configurables import CondDB
DDDBConf().DbRoot="conddb:/TargetsDet.xml"
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

# --- activate Multiplicity tool
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



