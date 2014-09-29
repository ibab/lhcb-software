#################################################################################
## Example to run the Radiation lenght scan of the detector                    ##
## In this .py a geometry containing scoring planes is loaded                  ##
## and the radiation lenght tool is activated.                                 ##
## In order for this to work you also need Gauss-Job.py and MaterialEvalGun.py ##
##                                                                             ##
##  @author : L.Pescatore                                                      ##
##  @date   : last modified on 2013-06-09                                      ##
#################################################################################

from Gaudi.Configuration import *
from Gauss.Configuration import *
from Configurables import CondDB, LHCbApp, CondDBAccessSvc

# Define the new tags
#base_tag = "dddb-20130503"
#cond_base_tag = "HEAD"

# Set the default tags
#LHCbApp(DDDBtag = base_tag, CondDBtag = cond_base_tag)



from Configurables import DDDBConf

#DDDBConf(DbRoot = "myDDDB/lhcb.xml")
#DDDBConf(DbRoot = "$SQLDDDBROOT/db/myDDDB-RadLength-InRegion-CurrentLHCb-dddb-20130929-1-sim-20130522-1-Data2012/lhcb.xml")
LHCbApp().DDDBtag   = "dddb-20130929-1"
LHCbApp().CondDBtag = "sim-20130522-1-vc-md100"
CondDB.LocalTags = {"DDDB" : ["radlength-20140908-1","radlength-20140908"]  }
from Configurables import CondDB
conddb = CondDB()
conddb.addLayer(dbFile = "$PWD/db_test/DDDB_Addon.db", dbName = "DDDB")




from Configurables import Gauss
from Gauss.Configuration import *
import sys

Gauss().Production = 'PGUN'
Gauss().PhysicsList = {"Em":'NoCuts', "Hadron":'FTFP_BERT', "GeneralPhys":True, "LHCbPhys":True}


Gauss().DetectorGeo = { "Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }
Gauss().DetectorSim = { "Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }
Gauss().DetectorMoni = { "Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }

Gauss().DeltaRays = False

def scoringGeo():
     from Configurables import GiGaInputStream
     geo = GiGaInputStream('Geo')

     geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/Scoring_Plane2"]
     geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/Scoring_Plane3"]
     geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/T/Scoring_Plane4"]
     geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/T/Scoring_Plane5"]
     geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/T/Scoring_Plane6"]
     geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/Scoring_Plane7"]
     geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane8"]
     geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane9"]
     geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane10"]
     geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane11"]


appendPostConfigAction(scoringGeo)

# --- Save ntuple with hadronic cross section information
ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
NTupleSvc().Output = ["FILE2 DATAFILE='Rad.root' TYP='ROOT' OPT='NEW'"]



# --- activate RadLenghtColl tool
def addMyTool():
    from Configurables import GiGa, GiGaStepActionSequence
    giga = GiGa()
    giga.StepSeq.Members.append( "RadLengthColl" )

appendPostConfigAction(addMyTool)

def trackNeutrinos():
    from Configurables import GiGa, GiGaRunActionSequence, TrCutsRunAction
    giga = GiGa()
    giga.addTool( GiGaRunActionSequence("RunSeq") , name="RunSeq" )
    giga.RunSeq.addTool( TrCutsRunAction("TrCuts") , name = "TrCuts" )
    giga.RunSeq.TrCuts.DoNotTrackParticles = []
	
appendPostConfigAction(trackNeutrinos)

# --- Configure the tool
#from Configurables import GiGa, GiGaStepActionSequence, RadLengthColl
#giga = GiGa()
#giga.addTool( GiGaStepActionSequence("StepSeq") , name = "StepSeq" )
#giga.StepSeq.addTool( RadLengthColl )
#giga.StepSeq.RadLengthColl.OutputLevel = DEBUG

