__author__ = 'Conor Fitzpatrick'
__date__ = '16/06/2010'
__version__ = '$Revision: 1.1 $'

'''
D+, Ds -> phi pi stripping selection using LoKi::Hybrid and python
configurables.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand

class StrippingDs2piPhiConf(LHCbConfigurableUser):
    """
    Definition of LOOSE Ds to phi pi stripping. 
    """
    __slots__ = { 
			"DauTRCHI2"		: 10.0		# adimensional 
		,	"DsComboMassUpper"	: 2070.0		# MeV
		,	"DsComboMassLower"	: 1770.0		# MeV
		#,	"DsMassUpper"		: 2070.0	# MeV
		#,	"DsMassLower"		: 1770.0	# MeV
		,	"phiComboMassUpper"	: 1040.0	# MeV
		,	"phiComboMassLower"	: 1000.0	# MeV
		#,	"phiMassUpper"		: 1040.0	# MeV
		#,	"phiMassLower"		: 1000.0	# MeV
		,	"pionMIPCHI2"		: 1.0		# adimensional
		,	"KaonMIPCHI2"		: 1.0		# adimensional
		,	"KaonPIDKPI"		: 5.0		# adimensional
		,	"DsBPVDIRA"		: 0.9999	# adimensional
		,	"DsBPVVDCHI2"		: 30.0		# adimensional
		,	"DsVCHI2VDOF"		: 10.0		# adimensional
                }
    
    def line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Ds2piPhiSel = self.Ds2piPhi()
	return StrippingLine('Ds2piPhiLine', prescale = 0.05, algos = [Ds2piPhiSel])
     
	
    def Ds2piPhi( self ):
	StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
	StdNoPIDsPions = DataOnDemand(Location = "Phys/StdNoPIDsPions")

	Phi2KKforDs2piPhi = CombineParticles("Phi2KKforDs2piPhi")
	Phi2KKforDs2piPhi.DecayDescriptor = "phi(1020) -> K+ K-"
	
	kaonCuts = " (TRCHI2DOF< %(DauTRCHI2)s ) "\
		"& (MIPCHI2DV(PRIMARY) > %(KaonMIPCHI2)s) "\
		"& ((PIDK-PIDpi) >  %(KaonPIDKPI)s)" % self.getProps()
	
	Phi2KKforDs2piPhi.CombinationCut = "(AM < %(phiComboMassUpper)s *MeV) "\
		"& (AM > %(phiComboMassLower)s *MeV)" % self.getProps()
	#Phi2KKforDs2piPhi.MotherCut = "(M < %(phiMassUpper)s *MeV)"\
 	#	"& (M > %(phiMassLower)s *MeV)" % self.getProps()
	Phi2KKforDs2piPhi.MotherCut = "ALL"
	Phi2KKforDs2piPhi.DaughtersCuts = {"K-" : kaonCuts}
	
	Phi2KKforDs2piPhiSel =  Selection("SelPhi2KKforDs2piPhi",
                        Algorithm = Phi2KKforDs2piPhi,
                        RequiredSelections = [StdNoPIDsKaons])

	
	Ds2piPhi = CombineParticles("Ds2piPhi")
	Ds2piPhi.DecayDescriptor = "[D_s+ -> pi+ phi(1020)]cc"
	
	pionCuts = "  (TRCHI2DOF< %(DauTRCHI2)s ) "\
	"& (MIPCHI2DV(PRIMARY) > %(pionMIPCHI2)s) " % self.getProps()

	Ds2piPhi.CombinationCut = "  (AM > %(DsComboMassLower)s *MeV)"\
		"& (AM < %(DsComboMassUpper)s *MeV) " % self.getProps()

	#Ds2piPhi.MotherCut = " (M > %(DsMassLower)s *MeV) "\
	#"& (M < %(DsMassUpper)s *MeV)"\
	
	Ds2piPhi.MotherCut = " (BPVDIRA > %(DsBPVDIRA)s) "\
	"& (BPVVDCHI2 > %(DsBPVVDCHI2)s)"\
	"& (VFASPF(VCHI2/VDOF) < %(DsVCHI2VDOF)s)" % self.getProps()
	
	Ds2piPhi.DaughtersCuts = {"pi+" : pionCuts}

	Ds2piPhiSel = Selection("SelDs2piPhi",
                 	Algorithm = Ds2piPhi,
                 	RequiredSelections = [StdNoPIDsPions,Phi2KKforDs2piPhiSel])
	return Ds2piPhiSel

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
