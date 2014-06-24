from Configurables import Gauss
from Configurables import Generation
from Configurables import Special
from GaudiKernel   import SystemOfUnits

Generation().SampleGenerationTool = "Special"
Generation().addTool( Special )
Generation().Special.ProductionTool = "OniaPairsProduction"
Generation().Special.addTool( OniaPairsProduction )
Generation().Special.OniaPairsProduction.Ecm = 2 * Gauss().BeamMomentum / SystemOfUnits.GeV

