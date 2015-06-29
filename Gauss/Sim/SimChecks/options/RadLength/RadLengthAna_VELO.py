#################################################################################
## Example to run the Radiation lenght scan of the detector                    ##
## In this .py a geometry containing scoring planes is loaded                  ##
## and the radiation lenght tool is activated.                                 ##
## In order for this to work you also need Gauss-Job.py and MaterialEvalGun.py ##
##                                                                             ##
##  @author : L.Pescatore                                                      ##
##  @date   : last modified on 2015-06-29                                      ##
#################################################################################

from Gaudi.Configuration import *
from Gauss.Configuration import *
from Configurables import CondDB, LHCbApp, CondDBAccessSvc
from Configurables import DDDBConf

#DDDBConf(DbRoot = "myDDDB/lhcb.xml")
LHCbApp().DDDBtag   = "dddb-20130929-1"
LHCbApp().CondDBtag = "sim-20130522-1-vc-md100"
CondDB.LocalTags = {"DDDB" : ["radlength-20141010", "radlength-20141003","radlength-20140908"] }

from Configurables import Gauss
from Gauss.Configuration import *
import sys

Gauss().Production = 'PGUN'
Gauss().PhysicsList = {"Em":'NoCuts', "Hadron":'FTFP_BERT', "GeneralPhys":True, "LHCbPhys":True}


Gauss().DetectorGeo = { "Detectors": ['PuVeto', 'Velo', 'Rich1', 'Rich2' ] } 
Gauss().DetectorSim = { "Detectors": ['PuVeto', 'Velo', 'Rich1', 'Rich2' ] } 
Gauss().DetectorMoni = { "Detectors": ['PuVeto', 'Velo', 'Rich1', 'Rich2' ] }
importOptions ("$GAUSSOPTS/RICHesOff.py"  )
Gauss().DeltaRays = False

def scoringGeo():
     from Configurables import GiGaInputStream
     geo = GiGaInputStream('Geo')

     geo.StreamItems      += ["/dd/Structure/LHCb/BeforeMagnetRegion/Scoring_Plane1"]
    
appendPostConfigAction(scoringGeo)

# --- Save ntuple with hadronic cross section information
ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
NTupleSvc().Output = ["FILE2 DATAFILE='Rad_VELO.root' TYP='ROOT' OPT='NEW'"]



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

