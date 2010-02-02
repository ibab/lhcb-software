# $Id: StrippingBd2DstarMuNuLoose.py,v 1.3 2010-02-02 21:24:47 gcowan Exp $

__author__ = 'Greig Cowan, Marta Calvi'
__date__ = '10/12/2009'
__version__ = '$Revision: 1.3 $'

'''
Bd->Dstar mu nu loose stripping selection using LoKi::Hybrid and python
configurables.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingBd2DstarMuNuLooseConf(LHCbConfigurableUser):
    """
    Definition of LOOSE Bd->D*MuNu stripping. 
    """
    __slots__ = { 
			"MuonTRCHI2"		: 10.0		# adimensional 
		,	"MuonPT"		: 800.0		# MeV 
		,	"MuonMIPDV"		: 0.04		# mm
		,	"D0PT"			: 1600.		# MeV
		,	"KaonPT"		: 350.0		# MeV  
		,	"PionPT"		: 350.0		# MeV  
		,	"BdDeltaMassLower"	: -2279.0	# MeV
		,	"BdDeltaMassUpper"	: 0.0		# MeV
		,	"BdVCHI2" 		: 20.0		# adimensional
                }
    
    def line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bd2DstarMuNuSel = self.Bd2DstarMuNu()
	Bd2DstarMuNuSeq = SelectionSequence("SeqBd2DstarMuNuLoose", TopSelection = Bd2DstarMuNuSel)
	return StrippingLine('Bd2DstarMuNuLooseLine', prescale = 1, algos = [Bd2DstarMuNuSeq])
     	
    def Bd2DstarMuNu( self ):
	stdVeryLooseMuons = DataOnDemand("stdVeryLooseMuons", Location = "StdVeryLooseMuons")
	stdVeryLooseDstar = DataOnDemand("stdVeryLooseDstarWithD02KPi", Location = "StdVeryLooseDstarWithD02KPi")
	stdVeryLooseDstarDCS = DataOnDemand("stdVeryLooseDstarWithD02KPiDCS", Location = "StdVeryLooseDstarWithD02KPiDCS")
	
	Bd2DstarMu = CombineParticles("Bd2DstarMuNuLoose")
	Bd2DstarMu.DecayDescriptors = ["[B0 -> D*(2010)- mu+]cc", "[B0 -> D*(2010)+ mu+]cc"] # includes wrong sign
	
	# D* has the following decay chain:  D*+ -> ( D0 -> K pi ) pi 
	muonCuts = "  (ISLONG)"\
           	   "& (PT > %(MuonPT)s *MeV)"\
           	   "& (MIPDV(PRIMARY) > %(MuonMIPDV)s *mm)"\
           	   "& (TRCHI2DOF < %(MuonTRCHI2)s)" % self.getProps()

	DstarCutsD0 = "CHILDCUT("\
             	      "    (PT > %(D0PT)s *MeV)"\
             	      ",1)" % self.getProps()

	DstarCutsK = "& CHILDCUT("\
                     "   CHILDCUT("\
                     "       (ISLONG)"\
                     "     & (PT > %(KaonPT)s *MeV)"\
                     "   ,1)"\
                     ",1)" % self.getProps()

	DstarCutsPi= "& CHILDCUT("\
                     "   CHILDCUT("\
             	     "       (ISLONG)"\
             	     "     & (PT > %(PionPT)s *MeV)"\
             	     "   ,2)"\
             	     ",1)" % self.getProps()

	finalDstarCuts = DstarCutsD0 + DstarCutsK + DstarCutsPi 

	Bd2DstarMu.DaughtersCuts = { "mu-" : muonCuts,
                             	     "D*(2010)+" : finalDstarCuts}

	Bd2DstarMu.CombinationCut = "  (DAMASS('B0') > %(BdDeltaMassLower)s *MeV)"\
                            	    "& (DAMASS('B0') < %(BdDeltaMassUpper)s *MeV)" % self.getProps()

	Bd2DstarMu.MotherCut = "  (VFASPF(VCHI2) < %(BdVCHI2)s)" % self.getProps()
	
	Bd2DstarMuSel = Selection("SelBd2DstarMuNuLoose",
                 	Algorithm = Bd2DstarMu,
                 	RequiredSelections = [stdVeryLooseMuons, stdVeryLooseDstar, stdVeryLooseDstarDCS])
	return Bd2DstarMuSel

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
