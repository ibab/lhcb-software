from Gaudi.Configuration import *
from PhysSelPython.Wrappers import Selection , SelectionSequence, DataOnDemand
from Configurables import CombineParticles, FilterDesktop


#from MakeMyParticles import *
#makeMyProtoP("Best")
#makeMyParticles("SmrdLoosePions", "Best", "pion")
#makeMyKaons("SmrdLooseKaons", "Best", "kaon")
#makeAllParticles("SmrdNoPIDPions", "Best", "pion")
#makeAllParticles("SmrdNoPIDKaons", "Best", "kaon")

from Configurables import TrackSmeared
output = "Smeared" #/Rec/Track/Smeared
TrackSmeared("TrackSmearing").OutputLocation = output
TrackSmeared("TrackSmearing").smear = True
TrackSmeared("TrackSmearing").makePlots = True
TrackSmeared("TrackSmearing").OutputLevel = 2
TrackSmeared("TrackSmearing").smearProto = True
TrackSmearingSeq = GaudiSequencer("TrackSmearingSeq")

#Preserve MC
from Configurables import TrackToDST, ChargedPP2MC, TrackAssociator
assoctr = TrackAssociator(output+"AssocTr")
assoctr.TracksInContainer = "Rec/Track/"+output
assocpp=ChargedPP2MC(output+"AssocPP")
assocpp.TrackLocations = [ "Rec/Track/"+output ]
assocpp.InputData = [ "Rec/ProtoP/"+output+"ProtoPMaker" ]
assocpp.OutputTable = "Relations/Rec/ProtoP/"+output+"ProtoPMaker"

TrackSmearingSeq.Members = [ TrackSmeared("TrackSmearing") , assoctr, assocpp ]
from Configurables       import CombinedParticleMaker
from CommonParticles.Utils import *
from Configurables       import ProtoParticleCALOFilter
## create the algorithm 
algorithm =  CombinedParticleMaker ( 'SmrdLoosePions'  ,
                                     Particle =  'pion'  )

# configure the track selector
selector = trackSelector ( algorithm ) 
## add this line,
## this corresonds to the output of the smearing tool 'Rec/ProtoP/' is hard coded
algorithm.Input = 'Rec/ProtoP/Smeared'
### protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Pion' )
fltr.Selection = [ "" ]
## configure Data-On-Demand service 
locations = updateDoD ( algorithm )
## finally: define the symbol 
SmrdLoosePions = algorithm 


#Truth matching
matchpion = "(mcMatch('[^pi-]CC'))" 
#matchpion = "ALL" 

#######################################
# pions
_pions = DataOnDemand(Location='Phys/StdLoosePions')
_filter = FilterDesktop("filterPions", Code = matchpion)
_filter.Preambulo = [
    "from LoKiPhysMC.decorators import *" ,
    "from PartProp.Nodes import CC"    
    ]
_selPions = Selection("SelFilterPions",
                        Algorithm = _filter,
                        RequiredSelections = [_pions])
_seqPions = SelectionSequence("SeqFilterPions", TopSelection = _selPions)

#######################################
# pions
_pions2 = DataOnDemand(Location='Phys/SmrdLoosePions')
_filter2 = FilterDesktop("filterPions", Code = matchpion)
_filter2.Preambulo = [
    "from LoKiPhysMC.decorators import *" ,
    "from PartProp.Nodes import CC"    
    ]
_selPions2 = Selection("SelFilterPions2",
                        Algorithm = _filter2,
                        RequiredSelections = [_pions2])
_seqPions2 = SelectionSequence("SeqFilterPions2", TopSelection = _selPions2)


#######################################################################
#
# Decay Tree tuple
#

from Configurables import DecayTreeTuple
decaytuple = DecayTreeTuple("OriginalPionsTuple")
decaytuple.InputLocations = [_seqPions.outputLocation() ]
decaytuple.ToolList +=  [
    "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolTrackInfo"
    , "TupleToolPid"
    ]
decaytuple.Decay = "[pi+]cc"

decaytuple2 = DecayTreeTuple("SmearedPionsTuple")
decaytuple2.InputLocations = [_seqPions2.outputLocation() ]
decaytuple2.ToolList +=  [
       "TupleToolGeometry"
       , "TupleToolKinematic"
       , "TupleToolTrackInfo"
       , "TupleToolPid"
       ]
decaytuple2.Decay = "[pi+]cc"


#######################################################################
#
# Configure the application
#

#remove the welcome message
from Configurables import LoKiSvc
LoKiSvc().Welcome = False

from Configurables import EventSelector
EventSelector().PrintFreq = 500


from Configurables import DaVinci


DaVinci().TupleFile = "SmrdPions.root"
DaVinci().EvtMax = 10
DaVinci().HistogramFile = "DVHistos.root"
DaVinci().DataType = '2010'
DaVinci().Simulation = True


# MC Options
DaVinci().DDDBtag = 'head-20101206'
DaVinci().CondDBtag = "sim-20101210-vc-md100"






DaVinci().UserAlgorithms += [ 
    TrackSmearingSeq
    ,SmrdLoosePions
    ,_seqPions.sequence()
    ,decaytuple
    ,_seqPions2.sequence()
    ,decaytuple2
    ]
    

DaVinci().MainOptions  = ""

EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000025_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"] 

