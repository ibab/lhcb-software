"""
Configuration file for 'Standard MC Dstar WithD02HH'. 
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCDstarWithD02KPi' ,
    'StdMCDstarWithD02KPiDCS' ,
    'StdMCDstarWithD02PiPi' ,
    'StdMCDstarWithD02KK' ,
    'locations'
    )

from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCD0, StandardMCPions
#locations of nopidsparticles: 

nppions = "Phys/StdMCPions/Particles"
mcD02KPi = "Phys/StdMCD02KPi/Particles"
mcD02KPiDCS = "Phys/StdMCD02KPiDCS/Particles"
mcD02PiPi = "Phys/StdMCD02PiPi/Particles"
mcD02KK = "Phys/StdMCD02KK/Particles"
locations ={}
#matchPions = "mcMatch( '[pi+]cc' )"
#matchD0 = "mcMatch( '[D0]cc' )"

#mothercut
mothercut = "ALL"

#Create matching strings

matchDstPiD0 = "(mcMatch('[D*(2010)+ ==> pi+ D0]CC'))"

## create the algorithm 
StdMCDstarWithD02KPiDCS = CombineParticles ( 'StdMCDstarWithD02KPiDCS' )
StdMCDstarWithD02KPiDCS.Inputs = [ nppions, mcD02KPiDCS]
StdMCDstarWithD02KPiDCS.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc"
StdMCDstarWithD02KPiDCS.MotherCut =  matchDstPiD0
#StdMCDstarWithD02KPiDCS.DaughtersCuts = {
#        "pi+" : matchPions,
#	"D0" : matchD0
#	}
StdMCDstarWithD02KPiDCS.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update(updateDoD ( StdMCDstarWithD02KPiDCS ))



## create the algorithm 
StdMCDstarWithD02KPi = CombineParticles ( 'StdMCDstarWithD02KPi' )
StdMCDstarWithD02KPi.Inputs = [ nppions,mcD02KPi]
StdMCDstarWithD02KPi.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc"
StdMCDstarWithD02KPi.MotherCut = matchDstPiD0
#StdMCDstarWithD02KPi.DaughtersCuts = {
#                "pi+" : matchPions,
#		"D0" : matchD0
#	}
StdMCDstarWithD02KPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update(updateDoD ( StdMCDstarWithD02KPi ))


## create the algorithm 
StdMCDstarWithD02KK = CombineParticles ( 'StdMCDstarWithD02KK' )
StdMCDstarWithD02KK.Inputs = [ nppions,mcD02KK]
StdMCDstarWithD02KK.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc"
StdMCDstarWithD02KK.MotherCut = matchDstPiD0
#StdMCDstarWithD02KK.DaughtersCuts = {
#                       "pi+" : matchPions,
#		       "D0" : matchD0
#	}
StdMCDstarWithD02KK.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update(updateDoD ( StdMCDstarWithD02KK ))



## create the algorithm 
StdMCDstarWithD02PiPi = CombineParticles ( 'StdMCDstarWithD02PiPi' )
StdMCDstarWithD02PiPi.Inputs = [ nppions,mcD02PiPi]
StdMCDstarWithD02PiPi.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc"
StdMCDstarWithD02PiPi.MotherCut = matchDstPiD0
#StdMCDstarWithD02PiPi.DaughtersCuts = {
#                       "pi+" : matchPions,
#		       "D0" : matchD0
#        }
StdMCDstarWithD02PiPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update(updateDoD ( StdMCDstarWithD02PiPi ))

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

