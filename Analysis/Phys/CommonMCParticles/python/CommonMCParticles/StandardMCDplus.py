"""
Configuration file for 'Standard Dplus with MC assoc'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"

__all__ = (
		'StdMCDplus2KPiPi' ,
		'StdMCDplus2KKPi' ,
		'StdMCDplus2PiPiPi' ,
		'StdMCDplus2KPiPiOppSignPi' ,
		'StdMCDplus2KKK',
		'StdMCDplus2KKPiOppSignK',
		'locations'
	  )
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCKaons, StandardMCPions
#locations of nopidsparticles: 
npkaons =  "Phys/StdMCKaons/Particles"
nppions = "Phys/StdMCPions/Particles"
#matchKaons = "mcMatch('[K+]cc')"
#matchPions = "mcMatch('[pi+]cc')"

locations={}

#Create matching strings
matchDpKPiPi   = "(mcMatch('[D- ==> K+ pi- pi-]CC'))"
matchDpKKPi   = "(mcMatch('[D+ ==> K+ K+ pi-]CC'))"
matchDpPiPiPi = "(mcMatch('[D+ ==> pi- pi+ pi+]CC'))"
matchDpKKK = "(mcMatch('[D+ ==> K- K+ K+]CC'))"
matchDpKPiPiOppSignPi = "(mcMatch('[D- ==> K- pi+ pi-]CC'))"
matchDpKKPiOppSignK = "(mcMatch('[D- ==> K- K+ pi-]CC'))"

#Algorithms
StdMCDplus2KPiPi=CombineParticles ('StdMCDplus2KPiPi')
StdMCDplus2KPiPi.Inputs = [ npkaons, nppions]
StdMCDplus2KPiPi.DecayDescriptor = "[D+ -> K- pi+ pi+]cc"
StdMCDplus2KPiPi.MotherCut =  matchDpKPiPi
#StdMCDplus2KPiPi.DaughtersCuts = {
#	"K+" : matchKaons,
#	"pi+" : matchPions
#	}
StdMCDplus2KPiPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update( updateDoD ( StdMCDplus2KPiPi ) )


StdMCDplus2KKPi=CombineParticles ('StdMCDplus2KKPi')
StdMCDplus2KKPi.Inputs = [ npkaons, nppions]
StdMCDplus2KKPi.DecayDescriptor = "[D+ -> K+ K+ pi-]cc"
StdMCDplus2KKPi.MotherCut =  matchDpKKPi
#StdMCDplus2KKPi.DaughtersCuts = {
#	"K+" : matchKaons,
#	"pi+" : matchPions	
#	}
StdMCDplus2KKPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update( updateDoD ( StdMCDplus2KKPi ) )


StdMCDplus2KKK=CombineParticles ('StdMCDplus2KKK')
StdMCDplus2KKK.Inputs = [ npkaons]
StdMCDplus2KKK.DecayDescriptor = "[D+ -> K- K- K+]cc"
StdMCDplus2KKK.MotherCut =  matchDpKKK
#StdMCDplus2KKK.DaughtersCuts = {
#	"K+" : matchKaons,
#	}
StdMCDplus2KKK.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update( updateDoD ( StdMCDplus2KKK ) )



StdMCDplus2PiPiPi=CombineParticles ('StdMCDplus2PiPiPi')
StdMCDplus2PiPiPi.Inputs = [ nppions]
StdMCDplus2PiPiPi.DecayDescriptor = "[D+ -> pi- pi- pi+]cc"
StdMCDplus2PiPiPi.MotherCut =  matchDpPiPiPi
#StdMCDplus2PiPiPi.DaughtersCuts = {
#	"pi+" :	matchPions
#	}
StdMCDplus2PiPiPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update( updateDoD ( StdMCDplus2PiPiPi ) )

StdMCDplus2KPiPiOppSignPi=CombineParticles ('StdMCDplus2KPiPiOppSignPi')
StdMCDplus2KPiPiOppSignPi.Inputs = [ npkaons, nppions]
StdMCDplus2KPiPiOppSignPi.DecayDescriptor = "[D+ -> K+ pi+ pi-]cc"
StdMCDplus2KPiPiOppSignPi.MotherCut =  matchDpKPiPiOppSignPi
#StdMCDplus2KPiPiOppSignPi.DaughtersCuts = {
#	       "K+" : matchKaons,
#	       "pi+" : matchPions
#	}
StdMCDplus2KPiPiOppSignPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update( updateDoD ( StdMCDplus2KPiPiOppSignPi ) )



StdMCDplus2KKPiOppSignK=CombineParticles ('StdMCDplus2KKPiOppSignK')
StdMCDplus2KKPiOppSignK.Inputs = [ npkaons, nppions]
StdMCDplus2KKPiOppSignK.DecayDescriptor = "[D+ -> K+ K- pi+]cc"
StdMCDplus2KKPiOppSignK.MotherCut =  matchDpKKPiOppSignK
#StdMCDplus2KKPiOppSignK.DaughtersCuts = {
#	             "K+" : matchKaons,
#		     "pi+" : matchPions
#	}
StdMCDplus2KKPiOppSignK.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update( updateDoD ( StdMCDplus2KKPiOppSignK ) )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

