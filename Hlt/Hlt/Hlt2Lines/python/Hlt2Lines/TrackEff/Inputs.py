__author__  = 'Mika Vesterinen'
__date__    = '$Date: 2015-02-23$'
__version__ = '$Revision: 0.0 $'
from Hlt2SharedParticles.TrackFittedBasicParticles import Hlt2BiKalmanFittedPions       as Hlt2SlowPions
from Hlt2SharedParticles.TrackFittedBasicParticles import Hlt2BiKalmanFittedRichPions   as Hlt2LoosePions
from Hlt2SharedParticles.TrackFittedBasicParticles import Hlt2BiKalmanFittedRichKaons   as Hlt2LooseKaons
from Hlt2SharedParticles.TrackFittedBasicParticles import Hlt2BiKalmanFittedRichProtons as Hlt2LooseProtons

#from Hlt2SharedParticles.TagAndProbeParticles import ProbeVeloPions as Hlt2ProbeVeloPions
#from Hlt2SharedParticles.TagAndProbeParticles import ProbeVeloKaons as Hlt2ProbeVeloKaons
#from Hlt2SharedParticles.TrackFittedBasicParticles import Hlt2BiKalmanFittedPions       as Hlt2ProbeVeloPions
#from Hlt2SharedParticles.TrackFittedBasicParticles import Hlt2BiKalmanFittedKaons       as Hlt2ProbeVeloKaons

## velo probes made here for now ##
from HltTracking.HltTrackNames import HltSharedTrackLoc, HltDefaultFitSuffix, _baseProtoPLocation, TrackName, Hlt2TrackEffRoot
from HltLine.HltLine import bindMembers
from Configurables import CombinedParticleMaker, ChargedProtoParticleMaker, BestPIDParticleMaker, NoPIDsParticleMaker
from Configurables import DelegatingTrackSelector
from Configurables import TrackContainerCopy,TrackStateInitAlg,DelegatingTrackSelector, DecodeVeloRawBuffer

DecodeVelo = DecodeVeloRawBuffer('DecodeVelo') 
DecodeVelo.DecodeToVeloLiteClusters = True
DecodeVelo.DecodeToVeloClusters = False
from TrackFitter.ConfiguredFitters import (ConfiguredEventFitter,
                                           ConfiguredForwardStraightLineEventFitter)

copyVelo = TrackContainerCopy( "CopyVeloForFitForTrackEff" )
copyVelo.inputLocation = HltSharedTrackLoc["Velo"]
copyVelo.outputLocation = Hlt2TrackEffRoot+'/FittedVelo/VeloTracks'
preve = TrackStateInitAlg("PrepareVeloTracks",TrackLocation = copyVelo.outputLocation )
preve.StateInitTool.VeloFitterName = "FastVeloFitLHCbIDs"
MyVeloFit = ConfiguredEventFitter("VeloRefitterAlgForTrackEff",
                                  TracksInContainer=copyVelo.outputLocation,
                                  LiteClusters=True,
                                  SimplifiedGeometry = True)

Hlt2VeloProtos = ChargedProtoParticleMaker('Hlt2VeloProtosForTrackEff')
#Hlt2VeloProtos.Inputs = [ HltSharedTrackLoc["Velo"]]
Hlt2VeloProtos.Inputs = [copyVelo.outputLocation]
Hlt2VeloProtos.Output = _baseProtoPLocation("Hlt2", Hlt2TrackEffRoot+"/"+TrackName["Velo"])
Hlt2VeloProtos.addTool(DelegatingTrackSelector, name='delTrackSelVelo')
Hlt2VeloProtos.delTrackSelVelo.TrackTypes = ['Velo']
Hlt2VeloPionParts = NoPIDsParticleMaker("Hlt2VeloPionParts")
Hlt2VeloPionParts.Particle = 'pion'
Hlt2VeloPionParts.Input =  Hlt2VeloProtos.Output
Hlt2VeloPionParts.Output =  Hlt2TrackEffRoot+"/Hlt2VeloPions/Particles"
Hlt2VeloKaonParts = NoPIDsParticleMaker("Hlt2VeloKaonParts")
Hlt2VeloKaonParts.Particle = 'kaon'
Hlt2VeloKaonParts.Input =  Hlt2VeloProtos.Output
Hlt2VeloKaonParts.Output =  Hlt2TrackEffRoot+"/Hlt2VeloKaons/Particles"
ProbeVeloProtos  = bindMembers( None, [ DecodeVelo,copyVelo,preve,MyVeloFit,Hlt2VeloProtos ])
Hlt2ProbeVeloPions  = bindMembers( None, [ ProbeVeloProtos , Hlt2VeloPionParts ] )
Hlt2ProbeVeloKaons  = bindMembers( None, [ ProbeVeloProtos , Hlt2VeloKaonParts ] )



