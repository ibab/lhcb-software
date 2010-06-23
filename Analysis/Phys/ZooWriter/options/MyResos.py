from Gaudi.Configuration import *
from Configurables import CombineParticles
from Configurables import TrackSelector, NoPIDsParticleMaker
from CommonParticles.Utils import *

# modify default particles to include anything with a little momentum information
def patchNoPIDsParticles():
#	trackSelector(NoPIDsParticleMaker("StdNoPIDsPions"), tracks=TrackSelector, trackTypes=[ "Long", "Upstream", "Downstream", "Ttrack" ])
	trackSelector(NoPIDsParticleMaker("StdNoPIDsPions"), tracks=TrackSelector, trackTypes=[ "Long", "Upstream", "Downstream", "Ttrack"  ])
	trackSelector(NoPIDsParticleMaker("StdNoPIDsKaons"), tracks=TrackSelector, trackTypes=[ "Long", "Upstream", "Downstream", "Ttrack" ])
	trackSelector(NoPIDsParticleMaker("StdNoPIDsMuons"), tracks=TrackSelector, trackTypes=[ "Long", "Upstream", "Downstream", "Ttrack" ])
	trackSelector(NoPIDsParticleMaker("StdNoPIDsProtons"), tracks=TrackSelector, trackTypes=[ "Long", "Upstream", "Downstream", "Ttrack" ])
appendPostConfigAction(patchNoPIDsParticles)

# do our own selections
MyKaons = CombineParticles("MyKaons")
MyKaons.InputLocations = [ "StdNoPIDsPions" ]
MyKaons.DecayDescriptor = "KS0 -> pi+ pi-"
MyKaons.DaughtersCuts = {
#		"pi+": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 500 * MeV))",
#		"pi-": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 500 * MeV))"
		"pi+": "((TRCHI2DOF < 25) & (PT > 50*MeV) & (P > 2000 * MeV))",
		"pi-": "((TRCHI2DOF < 25) & (PT > 50*MeV) & (P > 2000 * MeV))"
		}
MyKaons.CombinationCut = "((AMAXDOCA('') < 20*mm) & (ADAMASS('KS0') < 200 * MeV))"
MyKaons.MotherCut = "((VFASPF(VCHI2/VDOF) < 25) & (ADMASS('KS0') < 100 * MeV) & (VFASPF(VZ) < 220 * cm) & ((VFASPF(VZ) - P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2  < 150 * mm * 150 * mm) & ((VFASPF(VX) - PX/PZ * P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2+(VFASPF(VY) - PY/PZ * P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2 < 6 * mm) & ((VFASPF(VX)*PX+VFASPF(VY)*PY+PZ*(P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))) / (P*(VFASPF(VX)**2+VFASPF(VY)**2+(P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2)**0.5) > 0.99995))"

MyLooseDownKaons = CombineParticles("MyLooseDownKaons")
MyLooseDownKaons.InputLocations = [ "MyStdNoPIDsDownPions" ]
MyLooseDownKaons.DecayDescriptor = "KS0 -> pi+ pi-"
MyLooseDownKaons.DaughtersCuts = {
#		"pi+": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 500 * MeV))",
#		"pi-": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 500 * MeV))"
		"pi+": "((PT > 50*MeV))",
		"pi-": "((PT > 50*MeV))"
		}
MyLooseDownKaons.CombinationCut = "(ADAMASS('KS0') < 200 * MeV)"
MyLooseDownKaons.MotherCut = "(ADMASS('KS0') < 200 * MeV)"

MyDownKaons = CombineParticles("MyDownKaons")
MyDownKaons.InputLocations = [ "MyStdNoPIDsDownPions" ]
MyDownKaons.DecayDescriptor = "KS0 -> pi+ pi-"
MyDownKaons.DaughtersCuts = {
#		"pi+": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 500 * MeV))",
#		"pi-": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 500 * MeV))"
		"pi+": "((TRCHI2DOF < 25) & (PT > 50*MeV) & (P > 2000 * MeV))",
		"pi-": "((TRCHI2DOF < 25) & (PT > 50*MeV) & (P > 2000 * MeV))"
		}
MyDownKaons.CombinationCut = "((AMAXDOCA('') < 20*mm) & (ADAMASS('KS0') < 200 * MeV))"
MyDownKaons.MotherCut = "((VFASPF(VCHI2/VDOF) < 25) & (ADMASS('KS0') < 100 * MeV) & (VFASPF(VZ) < 220 * cm) & ((VFASPF(VZ) - P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2  < 150 * mm * 150 * mm) & ((VFASPF(VX) - PX/PZ * P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2+(VFASPF(VY) - PY/PZ * P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2 < 6 * mm) & ((VFASPF(VX)*PX+VFASPF(VY)*PY+PZ*(P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))) / (P*(VFASPF(VX)**2+VFASPF(VY)**2+(P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2)**0.5) > 0.99995))"


MyLambdas = CombineParticles("MyLambdas")
MyLambdas = CombineParticles("MyLambdas")
MyLambdas.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsProtons" ]
MyLambdas.DecayDescriptor = "[ Lambda0 -> p+ pi- ]cc"
MyLambdas.DaughtersCuts = {
#		"pi+": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 500 * MeV))",
#		"pi-": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 500 * MeV))"
"pi+": "((TRCHI2DOF < 35) & (PT > 25*MeV) & (P > 1000 * MeV))",
		"pi-": "((TRCHI2DOF < 35) & (PT > 25*MeV) & (P > 1000 * MeV))"
		}
MyLambdas.CombinationCut = "((AMAXDOCA('') < 20*mm) & (ADAMASS('Lambda0') < 200 * MeV))"
MyLambdas.MotherCut = "((VFASPF(VCHI2/VDOF) < 35) & (ADMASS('Lambda0') < 100 * MeV) & (VFASPF(VZ) < 220 * cm) & ((VFASPF(VZ) - P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2  < 300 * mm * 300 * mm) & ((VFASPF(VX) - PX/PZ * P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2+(VFASPF(VY) - PY/PZ * P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2 < 20 * mm) & ((VFASPF(VX)*PX+VFASPF(VY)*PY+PZ*(P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))) / (P*(VFASPF(VX)**2+VFASPF(VY)**2+(P*P*(VFASPF(VX)*PX/PZ+VFASPF(VY)*PY/PZ)/(PX*PX+PY*PY))**2)**0.5) > 0.995))"

MyTightLambdas = CombineParticles("MyTightLambdas")
MyTightLambdas.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsProtons" ]
MyTightLambdas.DecayDescriptor = "[ Lambda0 -> p+ pi- ]cc"
MyTightLambdas.DaughtersCuts = {
		"p+": "((TRCHI2DOF < 25) & (PT > 150*MeV) & (P > 500 * MeV))",
		"pi-": "((TRCHI2DOF < 25) & (PT > 150*MeV) & (P > 500 * MeV))"
		}
MyTightLambdas.CombinationCut = "((AMAXDOCA('') < 20*mm) & (ADAMASS('Lambda0') < 200 * MeV))"
MyTightLambdas.MotherCut = "((VFASPF(VCHI2/VDOF) < 25) & (ADMASS('Lambda0') < 30 * MeV) & (VFASPF(VZ) < 220 * cm))"

MyPhis = CombineParticles("MyPhis")
MyPhis.InputLocations = [ "StdNoPIDsKaons", "StdNoPIDsKaons" ]
MyPhis.DecayDescriptor = "phi(1020) -> K+ K-"
MyPhis.DaughtersCuts = {
		"K+": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 150 * MeV))",
		"K-": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 150 * MeV))"
		}
MyPhis.CombinationCut = "((AMAXDOCA('') < 20*mm) & (ADAMASS('phi(1020)') < 500 * MeV))"
MyPhis.MotherCut = "((VFASPF(VCHI2/VDOF) < 25) & (ADMASS('phi(1020)') < 200 * MeV) & (VFASPF(VZ) < 100 * cm) & (VFASPF(VZ) > -100 * cm))"

MyXis = CombineParticles("MyXis")
MyXis.InputLocations = [ "MyTightLambdas", "StdNoPIDsPions" ]
MyXis.DecayDescriptor = "[ Xi- -> Lambda0 pi- ]cc"
MyXis.DaughtersCuts = {
		"Lambda0": "((VFASPF(VCHI2/VDOF) < 25) & (PT > 150*MeV) & (P > 700 * MeV))",
		"pi-": "((TRCHI2DOF < 25) & (PT > 0*MeV) & (P > 150 * MeV))"
		}
MyXis.CombinationCut = "((AMAXDOCA('') < 20*mm) & (ADAMASS('Xi-') < 500 * MeV))"
MyXis.MotherCut = "((VFASPF(VCHI2/VDOF) < 25) & (ADMASS('Xi-') < 200 * MeV) & (VFASPF(VZ) < 220 * cm))"

MyJPsis = CombineParticles("MyJPsis")
MyJPsis.InputLocations = [ "StdNoPIDsMuons", "StdNoPIDsMuons" ]
MyJPsis.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
MyJPsis.DaughtersCuts = {
		"mu+": "((TRCHI2DOF < 25) & (PT > 250*MeV) & (P > 1500 * MeV))",
		"mu-": "((TRCHI2DOF < 25) & (PT > 250*MeV) & (P > 1500 * MeV))"
		}
MyJPsis.CombinationCut = "((AMAXDOCA('') < 20*mm) & (ADAMASS('J/psi(1S)') < 1000 * MeV))"
MyJPsis.MotherCut = "((VFASPF(VCHI2/VDOF) < 25) & (ADMASS('J/psi(1S)') < 500 * MeV) & (VFASPF(VZ) < 100 * cm) & (VFASPF(VZ) > -100 * cm))"
