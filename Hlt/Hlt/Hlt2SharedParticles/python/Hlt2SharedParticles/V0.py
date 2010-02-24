### @file
#
#  Standard V0
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *
from Hlt2SharedParticles.BasicParticles import NoCutsPions
from Configurables import GaudiSequencer, CreateHltVzero, HltV0ParticleMakerAlg
from HltLine.HltLine import bindMembers
from HltKshort.HltKshort import createHltV0DD, createHltV0LL, createHltV0DDFit

#
# We need a new instance of the Hlt2Tracking configurable
# once for the fitted and once for the unfitted 
#
from HltLine.HltTrackNames import Hlt2DownstreamTracksName, HltBiDirectionalKalmanFitSuffix

from Configurables import Hlt2Tracking
class Hlt2V0Tracking(Hlt2Tracking) :
    __used_configurables__ = []
    __slots__ = []

Hlt2V0Tracking = Hlt2V0Tracking()
Hlt2V0Tracking.Prefix = Hlt2Tracking().getProp("Prefix")
Hlt2V0Tracking.Suffix = Hlt2Tracking().getProp("Suffix")
Hlt2V0Tracking.Hlt2Tracks = Hlt2DownstreamTracksName
Hlt2V0Tracking.UseRICH = False
Hlt2V0Tracking.UseCALO = False
Hlt2V0Tracking.DataType = Hlt2Tracking().getProp("DataType")

class Hlt2TFV0Tracking(Hlt2Tracking) :
    __used_configurables__ = []
    __slots__ = []

Hlt2TFV0Tracking = Hlt2TFV0Tracking()
Hlt2TFV0Tracking.Prefix = Hlt2Tracking().getProp("Prefix")
Hlt2TFV0Tracking.Suffix = HltBiDirectionalKalmanFitSuffix
Hlt2TFV0Tracking.Hlt2Tracks = Hlt2DownstreamTracksName
Hlt2TFV0Tracking.UseRICH = False
Hlt2TFV0Tracking.UseCALO = False
Hlt2TFV0Tracking.DataType = Hlt2Tracking().getProp("DataType")

#---------------------------------------------------------------------
# Special case for Vzero particles  @todo TO BE REVISED
#---------------------------------------------------------------------

Hlt2V0LL = createHltV0LL( 'Hlt2V0LL', input = (Hlt2Tracking().hlt2Tracking()).outputSelection() )

Hlt2KsLLParticles = HltV0ParticleMakerAlg('Hlt2KsLLParticles')
Hlt2KsLLParticles.V0Location = Hlt2V0LL.OutputVertexContainer
Hlt2KsLLParticles.RefitVertex = True   #/// @todo Remove when covariance is fixed
Hlt2KsLLParticles.MakeKs = True 
Hlt2KsLLParticles.MakeLambda = False 
Hlt2KsLLParticles.InputLocations = [ NoCutsPions.outputSelection() ]
# Must specify the protoparticles location to propagate dependencies correctly! 
Hlt2KsLLParticles.ProtoParticlesLocation = (Hlt2Tracking().hlt2ChargedHadronProtos()).outputSelection()

Hlt2V0DD = createHltV0DD( 'Hlt2V0DD' )
Hlt2KsDDParticles = HltV0ParticleMakerAlg("Hlt2KsDDParticles")
Hlt2KsDDParticles.V0Location = Hlt2V0DD.OutputVertexContainer
Hlt2KsDDParticles.RefitVertex = True
Hlt2KsDDParticles.MakeKs = True
Hlt2KsDDParticles.MakeLambda = False
# Must specify the protoparticles location to propagate dependencies correctly!
Hlt2KsDDParticles.ProtoParticlesLocation = (Hlt2V0Tracking.hlt2ChargedHadronProtos()).outputSelection()

#Hlt2V0DDFit = Hlt2V0DD.clone("Hlt2V0DDFit")
#Hlt2V0DDFit.InputTrackContainer = (Hlt2TFV0Tracking.hlt2Tracking()).outputSelection()
#Hlt2V0DDFit.OutputVertexContainer = "Hlt/Vertex/KsDDFit"

Hlt2V0DDFit = createHltV0DDFit( 'Hlt2V0DDFit')
Hlt2V0DDFit.InputTrackContainer = (Hlt2TFV0Tracking.hlt2Tracking()).outputSelection()
Hlt2V0DDFit.OutputVertexContainer = "Hlt/Vertex/KsDDFit"

Hlt2KsDDFitParticles = HltV0ParticleMakerAlg("Hlt2KsDDFitParticles")
Hlt2KsDDFitParticles.V0Location = Hlt2V0DDFit.OutputVertexContainer
Hlt2KsDDFitParticles.RefitVertex = True
Hlt2KsDDFitParticles.MakeKs = True
Hlt2KsDDFitParticles.MakeLambda = False
# Must specify the protoparticles location to propagate dependencies correctly!
Hlt2KsDDFitParticles.ProtoParticlesLocation = (Hlt2TFV0Tracking.hlt2ChargedHadronProtos()).outputSelection()

###@TODO/@FIXME there are no Protons...
#Hlt2LambdaLLParticles = HltV0ParticleMakerAlg('Hlt2LambdaLLParticles')
#Hlt2LambdaLLParticles.V0Location = "Hlt/Vertex/KsLL" 
#Hlt2LambdaLLParticles.RefitVertex = True   #/// @todo Remove when covariance is fixe
#Hlt2LambdaLLParticles.MakeKs = False 
#Hlt2LambdaLLParticles.MakeLambda = True 
#Hlt2LambdaLLParticles.InputLocations = [ NoCutsPions.outputSelection() 
#                                       , Protons.outputSelection()
#                                       ]
#Hlt2LambdaLLParticles.ProtoParticlesLocation = (Hlt2TFV0Tracking.hlt2ChargedHadronProtos()).outputSelection()


#LambdaLL = bindMembers(None, [ NoCutsPions, Protons, Hlt2V0LL, Hlt2LambdaLLParticles ] )
KsLL     = bindMembers(None, [ NoCutsPions, Hlt2V0LL, Hlt2KsLLParticles ] )
KsDD     = bindMembers(None, [ Hlt2V0Tracking.hlt2Tracking(), Hlt2V0DD, Hlt2KsDDParticles ] )
KsDDFit  = bindMembers(None, [ Hlt2TFV0Tracking.hlt2StagedFastFit(), Hlt2V0DDFit, Hlt2KsDDFitParticles] )



__all__ = ( KsLL, KsDD, KsDDFit )
