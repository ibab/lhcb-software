### @file
#
#  Basic particles
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.16 $"
# =============================================================================
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers
from HltLine.HltReco import HltRecoSequence
from Configurables import NoPIDsParticleMaker, CombinedParticleMaker, TrackSelector
from Configurables import PhotonMaker, PhotonMakerAlg
from Configurables import ProtoParticleCALOFilter, ProtoParticleMUONFilter
from Configurables import Hlt2PID, GaudiSequencer
from GaudiKernel.SystemOfUnits import MeV
#
# These are all based on unfitted tracks
# No RICH info can be added until the fast-fit is done
# For particles with RICH info see TFBasicParticles
#
##########################################################################
# Make the pions
#
Hlt2NoCutsPions = NoPIDsParticleMaker("Hlt2NoCutsPions")
Hlt2NoCutsPions.Input =  "Hlt/ProtoP/Charged"
Hlt2NoCutsPions.Particle =  "pion" 
Hlt2NoCutsPions.WriteP2PVRelations = False
##########################################################################
# Make the kaons
#
Hlt2NoCutsKaons = Hlt2NoCutsPions.clone("Hlt2NoCutsKaons")
Hlt2NoCutsKaons.Particle =  "kaon" 
##########################################################################
# Make the protons
#
Hlt2NoCutsProtons = Hlt2NoCutsPions.clone("Hlt2NoCutsProtons")
Hlt2NoCutsProtons.Particle =  "proton" 

##########################################################################
# Make the Muons
#
Hlt2Muons = CombinedParticleMaker("Hlt2Muons")
Hlt2Muons.Particle = "muon" 
Hlt2Muons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2Muons.Muon.Selection = ["RequiresDet='MUON' IsMuon=True" ]
Hlt2Muons.addTool(TrackSelector)
Hlt2Muons.TrackSelector.TrackTypes = ["Long"] 
Hlt2Muons.Input =  "Hlt/ProtoP/Charged"
Hlt2Muons.WriteP2PVRelations = False
##########################################################################
# Make the electrons
#
Hlt2Electrons = CombinedParticleMaker("Hlt2Electrons")
Hlt2Electrons.Particle =   "electron" 
Hlt2Electrons.Input =  "Hlt/ProtoP/Charged" 
Hlt2Electrons.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2Electrons.Electron.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'-2.0'" ]
##########################################################################
# Make the photons
#
Hlt2Photons = PhotonMakerAlg("Hlt2Photons")
Hlt2Photons.addTool(PhotonMaker)
Hlt2Photons.PhotonMaker.Input= "Hlt/ProtoP/Neutrals"
Hlt2Photons.PhotonMaker.ConvertedPhotons = True  
Hlt2Photons.PhotonMaker.UnconvertedPhotons = True  
Hlt2Photons.PhotonMaker.PtCut = 200.* MeV 
##########################################################################
#
# Charged protoparticles -> pulls all the pid
#
caloProtos = Hlt2PID().hlt2ChargedCaloProtos( )
muonProtos = Hlt2PID().hlt2MuonProtos( )
hadronProtos = Hlt2PID().hlt2ChargedHadronProtos( )
##########################################################################
#
# Charged protoparticles
#
NeutralProtos = Hlt2PID().hlt2NeutralProtos()
##########################################################################
#
# 
#
##########################################################################
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.BasicParticles import Muons
#

__all__ = ( 'NoCutsPions', 'NoCutsKaons', 'NoCutsProtons', 'Muons', 'RichPIDsKaons', 'Electrons', 'Photons' )

NoCutsPions   = bindMembers( None, [ hadronProtos, Hlt2NoCutsPions ] )
NoCutsKaons   = bindMembers( None, [ hadronProtos, Hlt2NoCutsKaons ] )
NoCutsProtons = bindMembers( None, [ hadronProtos, Hlt2NoCutsProtons ] )
Muons         = bindMembers( None, [ muonProtos , Hlt2Muons ] )
Electrons     = bindMembers( None, [ caloProtos, Hlt2Electrons ] )
Photons       = bindMembers( None, [ NeutralProtos, Hlt2Photons ] )
