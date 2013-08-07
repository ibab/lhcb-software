"""
Configuration file for 'Standard Dsplus with MC assoc'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"

__all__ = (
		'StdMCDsplus2KPiPi' ,
		'StdMCDsplus2KKPi' ,
		'StdMCDsplus2PiPiPi' ,
		'locations'
	  )
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCKaons, StandardMCPions

#locations of nopidsparticles: 
npkaons =  "Phys/StdMCKaons/Particles"
nppions = "Phys/StdMCPions/Particles"
#matchKaons = "mcMatch( '[K+]cc' )"
#matchPions = "mcMatch( '[pi+]cc' )"

#Create matching strings
matchDsPiPiPi = "(mcMatch('[D_s+ ==> pi- pi+ pi+]CC'))"
matchDsKPiPi = "(mcMatch('[D_s- ==> K- pi+ pi-]CC'))"
matchDsKKPi = "(mcMatch('[D_s- ==> K- K+ pi-]CC'))"


#Algorithms
StdMCDsplus2PiPiPi=CombineParticles ('StdMCDsplus2PiPiPi')
StdMCDsplus2PiPiPi.Inputs = [ nppions]
StdMCDsplus2PiPiPi.DecayDescriptor = "[D_s+ -> pi- pi- pi+]cc"
StdMCDsplus2PiPiPi.MotherCut =  matchDsPiPiPi
#StdMCDsplus2PiPiPi.DaughtersCuts = {
#	"pi+" : matchPions
#	}
StdMCDsplus2PiPiPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCDsplus2PiPiPi )


StdMCDsplus2KPiPi=CombineParticles ('StdMCDsplus2KPiPi')
StdMCDsplus2KPiPi.Inputs = [ npkaons, nppions]
StdMCDsplus2KPiPi.DecayDescriptor = "[D_s+ -> K+ pi+ pi-]cc"
StdMCDsplus2KPiPi.MotherCut =  matchDsKPiPi
#StdMCDsplus2KPiPi.DaughtersCuts = {
#	                "K+" : matchKaons,
#	                "pi+" : matchPions
#	}
StdMCDsplus2KPiPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCDsplus2KPiPi )


StdMCDsplus2KKPi=CombineParticles ('StdMCDsplus2KKPi')
StdMCDsplus2KKPi.Inputs = [ npkaons, nppions]
StdMCDsplus2KKPi.DecayDescriptor = "[D_s+ -> K+ K- pi+]cc"
StdMCDsplus2KKPi.MotherCut =  matchDsKKPi
#StdMCDsplus2KKPi.DaughtersCuts = {
#	                "K+" : matchKaons,
#	                "pi+" : matchPions
#	}
StdMCDsplus2KKPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations = updateDoD ( StdMCDsplus2KKPi )
## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

