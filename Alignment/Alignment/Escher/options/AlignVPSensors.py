##############################################################################
# File for running VP alignment on simulated Data
#
# Syntax is:
#
#   gaudirun.py $ESCHEROPTS/AlignVeloSensors.py $ESCHEROPTS/2008-MC-Files.py
#   gaudirun.py $ESCHEROPTS/AlignVeloSensors.py $ESCHEROPTS/DC06-Files.py
#
#   $ESCHEROPTS/gaudiiter.py -n 3 -e 1000 $ESCHEROPTS/AlignVeloSensors.py $ESCHEROPTS/2008-MC-Files.py
#
##############################################################################

from Configurables import ( Escher, TrackSys, RecSysConf, LHCbApp, DstConf, RecMoniConf )

from GaudiKernel.ProcessJobOptions import importOptions
importOptions('$STDOPTS/PreloadUnits.opts')

# Load the velo conditions
from Configurables import ( CondDB, CondDBAccessSvc , DDDBConf)
useXML = False
inpath  = "/afs/cern.ch/user/c/chombach/VELOUpgrade/DDDB/"
tags = {
    "DDDB":"dddb-20141121",
    "CondDB":"sim-20140204-vc-md100",
           }

conddb = CondDB()
privatedbPathdb = inpath+"myDDDB"
privatedbPathsc = inpath+"mySIMCOND"
LHCbApp().Simulation = True
LHCbApp().DataType = 'Upgrade'
conddb.Upgrade     = True

detectors = ['VP', 'UT' , 'FT', 'Ecal', 'Hcal', 'Muon', 'Magnet', 'Tr' ]
LHCbApp().Detectors  = detectors
LHCbApp().DDDBtag    = tags['DDDB']
LHCbApp().CondDBtag  = tags['CondDB']

if useXML:
    privatedb_pathdb = "/afs/cern.ch/user/c/chombach/VELOUpgrade/DDDB/myDDDBXml/"
    #privatedb_pathdb = "/afs/cern.ch/user/c/chombach/public/forWouter/VeloPixAlign/xml/Conditions/VP/Alignment/Global.xml "
    DDDBConf(DbRoot = privatedb_pathdb + "lhcb.xml")

db = inpath+"DDDB-new.db"
sc = inpath+"SIMCOND-new.db"
conddb.addLayer(dbFile = db, dbName = "DDDB" )
conddb.addLayer(dbFile = sc, dbName = "SIMCOND" )
#db = '/afs/cern.ch/user/c/chombach/VELOUpgrade/GiulioScripts/MisGM2.db'
#from Configurables import ( CondDB, CondDBAccessSvc )
#alignCond = CondDBAccessSvc( 'AlignDDDB'  )
#alignCond.ConnectionString = 'sqlite_file:' + db + '/DDDB'
#CondDB().addLayer( alignCond )


Escher().InputType = "DIGI"
Escher().Kalman = True
Escher().Millepede = False
#Escher().TrackContainer = "Rec/Track/Velo" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = -1
Escher().MoniSequence = []
Escher().RecoSequence = ["Decoding","VP", "VELOPIX","Tr","Vertex","CALO","PROTO"]
Escher().WithMC = True
Escher().Simulation = True
Escher().DataType = 'Upgrade'
Escher().Upgrade  = True
RecSysConf().Detectors = detectors
RecMoniConf().Detectors = detectors

Escher().ExpertTracking = ["kalmanSmoother","simplifiedGeometry"]


TrackSys().TrackExtraInfoAlgorithms = []
TrackSys().DataType = 'Upgrade'
TrackSys().Simulation   = True
TrackSys().WithMC       = True
#TrackSys().TrackTypes   = ["Velo"]
TrackSys().TrackPatRecAlgorithms = ["VP","Forward","TsaSeed","Match","Downstream"]
from Configurables import GaudiSequencer, DstConf
DstConf().Detectors = detectors



from Configurables import PrPixelTracking, PrPixelStoreClusters, PrLHCbID2MCParticle, GaudiSequencer

hvpseq = GaudiSequencer("VPSeq")
prLHCbID2mc = PrLHCbID2MCParticle()
prPixTr = PrPixelTracking()
prPixStCl = PrPixelStoreClusters()

hvpseq.Members += [prPixStCl,prPixTr,prLHCbID2mc]

from Configurables import TAlignment
from TAlignment.Alignables import *
from TAlignment.TrackSelections import *

from Configurables import AlignAlgorithm
AlignAlgorithm("Alignment" ).ForcedInitialTime = 1

elements = Alignables()
print dir(elements)
elements.VPModules("TxTyTzRxRyRz")
#elements.VPLeft("TxTy")
#elements.VPRight("TxTyRz")
#constraints = ["Ty","Tx","Rz","Ry","Rx","Szx","Szy","SRz"]
constraints = ["Tx","Ty","Tz","Szx","Szy"] 
#constraints = []
constraints.append("VPFixModule00 : VP/VPLeft/Module00WithSupport: Tx Ty Tz Rx Ry Rz")
constraints.append("VPFixModule01 : VP/VPRight/Module01WithSupport: Tx Ty Tz Rx Ry Rz")
constraints.append("VPFixModule40 : VP/VPLeft/Module40WithSupport: Tx Ty Tz Rx Ry Rz")
constraints.append("VPFixModule41 : VP/VPRight/Module41WithSupport: Tx Ty Tz Rx Ry Rz")
#constraints = []
print "aligning elements ", elements
from Configurables import VPTrackSelector
class myVPTracksOverlap( TrackRefiner ):
    def __init__(self, Name="VPOverlapTracks", InputLocation = "Rec/Track/Best", Fitted = True ):
        TrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation, Fitted = Fitted)
    def configureSelector( self, a ):
        
        a.Selector = VPTrackSelector()
        a.Selector.MinHitsASide = 1
        a.Selector.MinHitsCSide = 1
        a.Selector.TrackTypes = ["Velo","Long"]

class myVPTracks( TrackRefiner ):
    def __init__(self, Name="VPOverlapTracks", InputLocation = "Rec/Track/Best", Fitted = True ):
        TrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation, Fitted = Fitted)
    def configureSelector( self, a ):

        a.Selector = VPTrackSelector()
        a.Selector.TrackTypes = ["Velo","Long"]

TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/Best"
TAlignment().VertexLocation       = ""#Rec/Vertex/Primary"
TAlignment().Constraints          = constraints
TAlignment().TrackSelections      = [ myVPTracks() ]
TAlignment().WriteCondSubDetList  = ['VP']
TAlignment().EigenValueThreshold  = 100
TAlignment().UsePreconditioning   = True
TAlignment().UseLocalFrame        = True
#include survey constraints
from TAlignment.SurveyConstraints import *

surveyconstraints = SurveyConstraints()
surveyconstraints.VP()

"""
# still set up a track selection
from Configurables import (GaudiSequencer,TrackContainerCopy,TrackSelector,TrackMonitor) 
trackFilterSeq = GaudiSequencer("TrackFilterSeq")
alignSelector  = TrackContainerCopy("AlignSelector",
                                    inputLocation = "Rec/Track/Best",
                                    outputLocation = "Rec/Track/AlignTracks")
alignSelector.addTool( TrackSelector("Selector",
                                     MaxChi2Cut = 10,
)
                       ) 
trackFilterSeq.Members.append( alignSelector )

GaudiSequencer("RecoDecodingSeq").Members = [ "UnpackMCVertex",
                                              "UnpackMCParticle",
                                              ]
"""
      
# Convert Calo ReadoutStatus to ProcStatus
from Configurables import CaloDigitConf,CaloProcessor,GlobalRecoConf
caloBanks=GaudiSequencer("CaloBanksHandler")
caloDetectors = [det for det in ['Spd','Prs','Ecal','Hcal'] if det in detectors]
CaloDigitConf(ReadoutStatusConvert=True,Sequence=caloBanks,Detectors=caloDetectors)
GaudiSequencer("EscherSequencer").Members += [caloBanks]

# New NoSPDPRS switches
noSPDPRS = False
if [det for det in ['Spd', 'Prs'] if det not in detectors]:
    noSPDPRS = True
CaloProcessor().setProp("NoSpdPrs", noSPDPRS)
GlobalRecoConf().setProp("NoSpdPrs", noSPDPRS)
import os, sys
#indir = '/afs/cern.ch/work/c/chombach/public/VP/BooleOutput/'
inputdata = []
#files = os.listdir(indir)
#for file in files:
#    inputdata.append("DATAFILE='PFN:"+indir+file+"'")
for i in range(209):
    
    inputdata.append("root://eoslhcb.cern.ch//eos/lhcb/user/c/chombach/ganga/2745/%i/BooleOutput.digi-Extended.digi" % i)
inputFiles = inputdata
#inputFiles = ['/eos/lhcb/user/c/chombach/ganga/2744/0/BrunelTest.xdst']

def doMyChanges():
    GaudiSequencer("HltFilterSeq").Members = []
from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction(doMyChanges)

from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(inputFiles)
