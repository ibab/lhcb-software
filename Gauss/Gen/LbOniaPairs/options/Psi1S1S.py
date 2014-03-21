from Configurables import Generation, Gauss
from GaudiKernel import SystemOfUnits
Generation().Special.OniaPairsProduction.Ecm = 2 * Gauss().BeamMomentum / SystemOfUnits.GeV
Generation().Special.OniaPairsProduction.Psi1S1S = 1
Generation().Special.OniaPairsProduction.Psi1S2S = 1
Generation().Special.OniaPairsProduction.Psi2S2S = 1
