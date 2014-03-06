from Configurables import Generation
Generation().EventType = 49000001
# This event type does not meet the event-type convention

from Configurables import Special
Generation().SampleGenerationTool = "Special"
Generation().addTool( Special )

from Configurables import OniaPairsProduction
Generation().Special.ProductionTool = "OniaPairsProduction"
Generation().Special.addTool( OniaPairsProduction )

# Set pp center of mass energy
from Configurables import Gauss
from GaudiKernel import SystemOfUnits
Generation().Special.OniaPairsProduction.Ecm = 2 * Gauss().BeamMomentum / SystemOfUnits.GeV

# Turn on main process and psi(2S) feed-down
Generation().Special.OniaPairsProduction.Psi1S1S = 1
Generation().Special.OniaPairsProduction.Psi1S2S = 1
Generation().Special.OniaPairsProduction.Psi2S2S = 1

# Force decays
from Configurables import ToolSvc, EvtGenDecay
ToolSvc().addTool( EvtGenDecay )
ToolSvc().EvtGenDecay.UserDecayFile = "$LBONIAPAIRSROOT/options/OniaPairs.dec"

# Cut tool to select di-psi in acceptance from gluon fusion subprocess 
Generation().Special.CutTool = ""
from Configurables import LoKi__FullGenEventCut
Generation().addTool( LoKi__FullGenEventCut, "PsiPairInAcc" )
Generation().FullGenEventCutTool = "LoKi::FullGenEventCut/PsiPairInAcc"
Generation().PsiPairInAcc.Code = " count ( isPsi & inY & isGood ) > 1.5 "
Generation().PsiPairInAcc.Preambulo += [
    "isPsi         = ( 'J/psi(1S)' == GID ) " , 
    "inY           = in_range ( 2. , GY , 4.5 ) " , 
    "FromGluFusion = ( ~GHAS( ('cluster' == GID) | ('string' == GID) , HepMC.ancestors ) ) " , 
    "notFromB     = 0 == GNINTREE ( GBEAUTY , HepMC.ancestors ) " , 
    "isGood        = ( 3 != GSTATUS ) & FromGluFusion & notFromB "
    ]

from Configurables import FixedNInteractions
Generation().PileUpTool = 'FixedNInteractions'
Generation().addTool( FixedNInteractions )
Generation().FixedNInteractions.NInteractions = 1

