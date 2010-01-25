## #####################################################################
# A stripping selection for pp -> pp  mu+ mu- decays for indirect luminosity
# measurment
# @authors J.Keaveney, D.Moran
# @date 2010-1-24
# 
## #####################################################################

name = "PP2PPMuMu"

__all__ = ('name', 'diPhoton', 'sequence')

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop, NumberOfTracksFilter
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence

# Create pp -> pp  mumu candidates out of std loose muons
## ############################################################
_muons =  DataOnDemand('stdLooseMuons', Location = 'Phys/StdLooseMuons')

mucut = '(PT>400*MeV) '

FilterNumMuons = NumberOfTracksFilter("FilterNumMuons")
FilterNumMuons.MinTracks = 2
FilterNumMuons.MaxTracks = 2
FilterNumMuons.TrackLocations  = [ "Rec/Track/Muon" ]

#Using J/psi(1S) as dimuon container
_diPhoton = CombineParticles(name,
                         DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                         DaughtersCuts = { 'mu+' : mucut , 
                                           'mu-' : mucut },
                         CombinationCut =
                         "(AM>1000*MeV)&(APT<900*MeV)&(AMAXDOCA('')<0.15)" ,
                         MotherCut = "ALL",
                         WriteP2PVRelations = False
                         )
                         
diPhoton = Selection( "Sel"+name,
                  Algorithm = _diPhoton,
                  RequiredSelections = [_muons] 
                  )

# build the SelectionSequence
sequence = SelectionSequence("Seq"+name,
                             TopSelection = diPhoton,
                             EventPreSelector = [FilterNumMuons]
                             )
# Define the line
## ############################################################
line = StrippingLine('PP2PPMuMu'
                           , prescale = 1.
                           , algos = [ sequence ]
                           )

