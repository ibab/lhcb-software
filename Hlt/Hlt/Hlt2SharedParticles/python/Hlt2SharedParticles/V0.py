### @file
#
#  Standard V0
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.BasicParticles import NoCutsPions
from Configurables import GaudiSequencer, CreateHltVzero, HltV0ParticleMakerAlg
from HltLine.HltLine import bindMembers
from HltConf.HltReco import Seed, SeedKF
from HltKshort.HltKshort import createHltV0DD, createHltV0LL

#---------------------------------------------------------------------
# Special case for Vzero particles  @todo TO BE REVISED
#---------------------------------------------------------------------

Hlt2V0LL = createHltV0LL( 'Hlt2V0LL', input = 'Hlt/Track/Forward' )

Hlt2KsLLParticles = HltV0ParticleMakerAlg('Hlt2KsLLParticles')
Hlt2KsLLParticles.V0Location = Hlt2V0LL.OutputVertexContainer
Hlt2KsLLParticles.RefitVertex = True   #/// @todo Remove when covariance is fixed
Hlt2KsLLParticles.MakeKs = True 
Hlt2KsLLParticles.MakeLambda = False 
Hlt2KsLLParticles.InputLocations = [ NoCutsPions.outputSelection() ]


Hlt2V0DD = createHltV0DD( 'Hlt2V0DD', input = 'Hlt/Track/Forward' )
Hlt2KsDDParticles = HltV0ParticleMakerAlg("Hlt2KsDDParticles")
Hlt2KsDDParticles.V0Location = Hlt2V0DD.OutputVertexContainer
Hlt2KsDDParticles.RefitVertex = True
Hlt2KsDDParticles.MakeKs = True
Hlt2KsDDParticles.MakeLambda = False


Hlt2V0DDFit = Hlt2V0DD.clone('Hlt2V0DDFit')
Hlt2V0DDFit.InputTrackContainer = SeedKF.outputSelection() 
Hlt2V0DDFit.OutputVertexContainer = "Hlt/Vertex/KsDDFit"

Hlt2KsDDFitParticles = Hlt2KsDDParticles.clone('Hlt2KsDDFitParticles')
Hlt2KsDDFitParticles.V0Location = Hlt2V0DDFit.OutputVertexContainer

###@TODO/@FIXME there are no Protons...
#Hlt2LambdaLLParticles = HltV0ParticleMakerAlg('Hlt2LambdaLLParticles')
#Hlt2LambdaLLParticles.V0Location = "Hlt/Vertex/KsLL" 
#Hlt2LambdaLLParticles.RefitVertex = True   #/// @todo Remove when covariance is fixe
#Hlt2LambdaLLParticles.MakeKs = False 
#Hlt2LambdaLLParticles.MakeLambda = True 
#Hlt2LambdaLLParticles.InputLocations = [ NoCutsPions.outputSelection() 
#                                       , Protons.outputSelection()
#                                       ]


#LambdaLL = bindMembers(None, [ NoCutsPions, Protons, Hlt2V0LL, Hlt2LambdaLLParticles ] )
KsLL     = bindMembers(None, [ NoCutsPions, Hlt2V0LL, Hlt2KsLLParticles ] )
KsDD     = bindMembers(None, [ Seed, Hlt2V0DD, Hlt2KsDDParticles ] )
KsDDFit  = bindMembers(None, [ SeedKF, Hlt2V0DDFit, Hlt2KsDDFitParticles] )


__all__ = ( KsLL, KsDD )
