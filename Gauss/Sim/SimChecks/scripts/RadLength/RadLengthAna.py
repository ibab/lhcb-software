#################################################################################
## Example to run the Radiation length scans of the detector                   ##
## In this .py a geometry containing scoring planes is loaded                  ##
## and the radiation lenght tool is activated.                                 ##
## You can run this simply by "gaudirun.py RadLengthAna.py"                    ##
##                                                                             ##
## N.B.: Since there is no space between VELO and Rich1 to scan the velo you   ##
## need a special run where all LHCb after the VELO is switched off.           ##
## This can be done by rutting "gaudirun.py RadLengthAna_VELO.py               ##
##                                                                             ##
##  @author : L.Pescatore                                                      ##
##  @date   : last modified on 2014-02-14                                      ##
#################################################################################

from Gaudi.Configuration import *
from Gauss.Configuration import *
from Configurables import Gauss
import sys



#Set Gauss Job
#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts=100
LHCbApp().EvtMax = nEvts

Gauss().OutputType = 'NONE'
Gauss().Histograms = 'NONE'






# Define database
from Configurables import DDDBConf
from Configurables import CondDB, LHCbApp, CondDBAccessSvc

base_tag = "dddb-20130503"
cond_base_tag = "HEAD"

LHCbApp(DDDBtag = base_tag, CondDBtag = cond_base_tag)
#DDDBConf(DbRoot = "myDDDB/lhcb.xml")
DDDBConf(DbRoot = "/afs/cern.ch/work/p/pluca/public/GaussTools/RadLenghtTool/myDDDB/lhcb.xml")




#Define geometry and activate RadLength tool

Gauss().Production = 'PGUN'
Gauss().PhysicsList = {"Em":'NoCuts', "Hadron":'FTFP_BERT', "GeneralPhys":True, "LHCbPhys":True}


doVELO = False
if(len(sys.argv) > 1) :
     if(sys.argv[1] == "VELO") :
          doVELO = True


#Switch On/Off normal LHCb detector
Gauss().DetectorGeo = { "Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }
Gauss().DetectorSim = { "Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }
Gauss().DetectorMoni = { "Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }

Gauss().DeltaRays = False


#Switch On/Off scoring planes
def scoringGeo():
     from Configurables import GiGaInputStream
     geo = GiGaInputStream('Geo')
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane2"]
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane3"]
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane4"]
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane5"]
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane6"]
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane7"]
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane8"]
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane9"]
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane10"]
     geo.StreamItems      += ["/dd/Structure/ScoringDet/Scoring_Plane11"]


appendPostConfigAction(scoringGeo)

# --- Save ntuple with hadronic cross section information
ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
NTupleSvc().Output = ["FILE2 DATAFILE='Rad.root' TYP='ROOT' OPT='NEW'"]


# --- activate RadLenghtColl tool
def addMyTool():
    from Configurables import GiGa, GiGaStepActionSequence
    giga = GiGa()
    giga.addTool( GiGaStepActionSequence("StepSeq") , name = "StepSeq" )
    giga.StepSeq.Members.append( "RadLengthColl" )

appendPostConfigAction(addMyTool)



# --- Configure the tool
from Configurables import GiGa, GiGaStepActionSequence, RadLengthColl
giga = GiGa()
giga.addTool( GiGaStepActionSequence("StepSeq") , name = "StepSeq" )
giga.StepSeq.addTool( RadLengthColl )
#giga.StepSeq.RadLengthColl.OutputLevel = DEBUG




#Define particle Gun for now 200 GeV muon (to be changed with neutrino)
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *
from Configurables import MomentumRange

# Set up ParticleGun
ParticleGun = ParticleGun("ParticleGun")

# Control of the ParticleGun
# Event type is set as described in LHCb-2005-034 : G=5! (Def = 0)
ParticleGun.EventType = 53210205

# Pick the type of particle gun generator
from Configurables import MaterialEval
ParticleGun.addTool(MaterialEval, name="MaterialEval")
ParticleGun.ParticleGunTool = "MaterialEval"

from Configurables import FlatNParticles
ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
ParticleGun.NumberOfParticlesTool = "FlatNParticles"
ParticleGun.FlatNParticles.MinNParticles = 1
ParticleGun.FlatNParticles.MaxNParticles = 1
ParticleGun.MaterialEval.PdgCode = 13

ParticleGun.MaterialEval.ModP = 200 * GeV
ParticleGun.MaterialEval.EtaPhi = True
ParticleGun.MaterialEval.ZPlane = 19.89*m
ParticleGun.MaterialEval.UseGrid = False
