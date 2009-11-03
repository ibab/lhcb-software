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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.13 $"
# =============================================================================
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers
from Configurables import NoPIDsParticleMaker, CombinedParticleMaker, TrackSelector
from Configurables import PhotonMaker, PhotonMakerAlg
from Configurables import ProtoParticleCALOFilter, ProtoParticleMUONFilter
from Configurables import Hlt2PID, GaudiSequencer
from GaudiKernel.SystemOfUnits import MeV
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
# Make the RICH kaons
#
Hlt2RichPIDsKaons = CombinedParticleMaker("HltRichPIDsKaons")
Hlt2RichPIDsKaons.Input = "Hlt/ProtoP/Charged" 
Hlt2RichPIDsKaons.Particle =   "kaon" 
Hlt2RichPIDsKaons.addTool(TrackSelector)
Hlt2RichPIDsKaons.TrackSelector.TrackTypes =  [ "Long" ]
Hlt2RichPIDsKaons.addTool(ProtoParticleCALOFilter('Kaon'))
Hlt2RichPIDsKaons.Kaon.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'-5.0'" ] 
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
# Now all PID
# 
##########################################################################
#
# Charged protoparticles
#
ChargedProtoMaker = Hlt2PID().hlt2ChargedProtos( )
##########################################################################
#
# Charged protoparticles
#
NeutralProtoMaker = Hlt2PID().hlt2NeutralProtos()
##########################################################################
#
# Calo reco (initialised in HltConf.Hlt2)
#
Hlt2CaloSeq = GaudiSequencer("Hlt2CaloSeq")

##########################################################################
#
# Muon reco
#
Hlt2MuonIDSeq = Hlt2PID().hlt2Muon()  
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
# @todo : Charged protos will be split
#

__all__ = ( 'NoCutsPions', 'NoCutsKaons', 'Muons', 'RichPIDsKaons', 'Electrons', 'Photons',
            'ChargedProtos', 'NeutralProtos' )

ChargedProtos = bindMembers( None, [ Hlt2CaloSeq, Hlt2MuonIDSeq, ChargedProtoMaker ] )
NeutralProtos = bindMembers( None, [ Hlt2CaloSeq, NeutralProtoMaker ] )

NoCutsPions   = bindMembers( None, [ ChargedProtos, Hlt2NoCutsPions ] )
NoCutsKaons   = bindMembers( None, [ ChargedProtos, Hlt2NoCutsKaons ] )
Muons         = bindMembers( None, [ ChargedProtos, Hlt2Muons ] )
Electrons     = bindMembers( None, [ ChargedProtos, Hlt2Electrons ] )
RichPIDsKaons = bindMembers( None, [ Hlt2RichPIDsKaons ] ) # TODO: add Rich reco as dependency!!!
Photons       = bindMembers( None, [ NeutralProtos, Hlt2Photons ] )
