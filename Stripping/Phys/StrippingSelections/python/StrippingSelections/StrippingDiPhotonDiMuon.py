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
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, EventSelection

# Create pp -> pp  mumu candidates out of std loose muons
## ############################################################
_muons =  DataOnDemand(Location = 'Phys/StdLooseMuons')

mucut = '(PT>400*MeV) '

from GaudiConfUtils.ConfigurableGenerators import NumberOfTracksFilter
_filterNumMuons = EventSelection("FilterNumMuons",
                                 Algorithm=NumberOfTracksFilter(MinTracks = 2,
                                                                MaxTracks = 2,
                                                                TrackLocations  = [ "Rec/Track/Muon" ]))

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
                  RequiredSelections = [_filterNumMuons, _muons] 
                  )

# Define the line
## ############################################################
line = StrippingLine('PP2PPMuMu'
                     , prescale = 1.
                     , checkPV  = False
                     , algos = [ diPhoton ]
                     )

line_2 = StrippingLine('PP2PPMuMu_2'
                     , prescale = 1.
                     , checkPV  = False
                     , L0DU = "L0_CHANNEL('Muon,lowMult')|L0_CHANNEL('DiMuon,lowMult')" 
                  )
