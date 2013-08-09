"""
Configuration file for 'Standard D02HH with MC assoc'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"

__all__ = (
    'StdMCD02KPi' ,
    'StdMCD02PiPi' ,
    'StdMCD02KK' ,
    'StdMCD02KPiDCS' ,
    'locations'	
	  )
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCKs, StandardMCKaons, StandardMCPions

#locations of nopidsparticles: 
npkaons =  "Phys/StdMCKaons/Particles"
nppions = "Phys/StdMCPions/Particles"
locations={}
#matchKaons = "mcMatch( '[K+]cc' )"
#matchPions = "mcMatch( '[pi+]cc' )"


#Create matching strings
matchD02KPi   = "(mcMatch('[D0 ==> K- pi+ ]CC'))"
matchD02KK = "(mcMatch('[D0 ==> K- K+ ]CC'))"
matchD02PiPi = "(mcMatch('[D0 ==> pi- pi+ ]CC'))"
matchD02KPiDCS = "(mcMatch('[D0 ==> K+ pi- ]CC'))"

#Algorithms
StdMCD02KPi=CombineParticles ('StdMCD02KPi')
StdMCD02KPi.Inputs = [ npkaons, nppions]
StdMCD02KPi.DecayDescriptor = "[D0 -> K- pi+ ]cc"
StdMCD02KPi.MotherCut =  matchD02KPi
#StdMCD02KPi.DaughtersCuts = {
#		"K+" : matchKaons,
#		"pi+" : matchPions
#	}
StdMCD02KPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update(updateDoD ( StdMCD02KPi ))

StdMCD02KK=CombineParticles ('StdMCD02KK')
StdMCD02KK.Inputs = [ npkaons]
StdMCD02KK.DecayDescriptor = "[D0 -> K- K+ ]cc"
StdMCD02KK.MotherCut =  matchD02KK
#StdMCD02KK.DaughtersCuts = {
#		"K+" : matchKaons
#	}
StdMCD02KK.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update(updateDoD ( StdMCD02KK ))

StdMCD02PiPi=CombineParticles ('StdMCD02PiPi')
StdMCD02PiPi.Inputs = [ nppions]
StdMCD02PiPi.DecayDescriptor = "[D0 -> pi- pi+ ]cc"
StdMCD02PiPi.MotherCut =  matchD02PiPi
#StdMCD02PiPi.DaughtersCuts = {
#	"pi+" : matchPions
#	}
StdMCD02PiPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update(updateDoD ( StdMCD02PiPi ))


StdMCD02KPiDCS=CombineParticles ('StdMCD02KPiDCS')
StdMCD02KPiDCS.Inputs = [ npkaons, nppions]
StdMCD02KPiDCS.DecayDescriptor = "[D0 -> K+ pi- ]cc"
StdMCD02KPiDCS.MotherCut = matchD02KPiDCS 
#StdMCD02KPiDCS.DaughtersCuts = {
#		"K+" : matchKaons,
#		"pi+" : matchPions
#	}
StdMCD02KPiDCS.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
locations.update(updateDoD ( StdMCD02KPiDCS ))

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

