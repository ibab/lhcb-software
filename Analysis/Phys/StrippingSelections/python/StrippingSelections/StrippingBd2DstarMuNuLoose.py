# $Id: StrippingBd2DstarMuNuLoose.py,v 1.8 2010-08-10 15:40:46 gcowan Exp $

__author__ = 'Greig Cowan, Marta Calvi'
__date__ = '10/12/2009'
__version__ = '$Revision: 1.8 $'

'''
Bd->Dstar mu nu loose stripping selection using LoKi::Hybrid and python
configurables.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from GaudiKernel.SystemOfUnits import MeV, mm

class StrippingBd2DstarMuNuLooseConf(LHCbConfigurableUser):
    """
    Definition of LOOSE Bd->D*MuNu stripping. 
    """
    __slots__ = { 
			"MuonTRCHI2"		: 10.0		
		,	"MuonPT"		: 500.0		*MeV
		,	"MuonMIPDV"		: 0.04		*mm
		,	"D0PT"			: 1600.		*MeV
		,	"D0BPVVDCHI2"		: 50.		
		,	"KaonPT"		: 350.0		*MeV  
		,	"PionPT"		: 350.0		*MeV  
		,	"BdDeltaMassLower"	: -2279.0	*MeV
		,	"BdDeltaMassUpper"	: 0.0		*MeV
		,	"BdVCHI2" 		: 10.0	
		,	"BdDIRA" 		: 0.999	
                }
    
    def line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	Bd2DstarMuNuSel = self.Bd2DstarMuNu()
	Bd2DstarMuNuSeq = SelectionSequence("SeqBd2DstarMuNuLoose", TopSelection = Bd2DstarMuNuSel)
	return StrippingLine('Bd2DstarMuNuLooseLine', prescale = 1, algos = [Bd2DstarMuNuSeq])
     	
    def Bd2DstarMuNu( self ):
	stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons")
	stdVeryLooseDstar = DataOnDemand(Location = "Phys/StdVeryLooseDstarWithD02KPi")
	
	Bd2DstarMu = CombineParticles("Bd2DstarMuNuLoose")
	Bd2DstarMu.DecayDescriptors = ["[B0 -> D*(2010)- mu+]cc", "[B0 -> D*(2010)+ mu+]cc"] # includes wrong sign
	
	# D* has the following decay chain:  D*+ -> ( D0 -> K pi ) pi 
	muonCuts = "  (ISLONG)"\
           	   "& (PT > %(MuonPT)s)"\
           	   "& (MIPDV(PRIMARY) > %(MuonMIPDV)s)"\
           	   "& (TRCHI2DOF < %(MuonTRCHI2)s)" % self.getProps()

	DstarCutsD0 = "CHILDCUT("\
             	      "    (PT > %(D0PT)s)"\
		      "  & (BPVVDCHI2 > %(D0BPVVDCHI2)s)"\
             	      ",1)" % self.getProps()

	DstarCutsK = "& CHILDCUT("\
                     "   CHILDCUT("\
                     "       (ISLONG)"\
                     "     & (PT > %(KaonPT)s)"\
                     "   ,1)"\
                     ",1)" % self.getProps()

	DstarCutsPi= "& CHILDCUT("\
                     "   CHILDCUT("\
             	     "       (ISLONG)"\
             	     "     & (PT > %(PionPT)s)"\
             	     "   ,2)"\
             	     ",1)" % self.getProps()

	finalDstarCuts = DstarCutsD0 + DstarCutsK + DstarCutsPi 

	Bd2DstarMu.DaughtersCuts = { "mu-" : muonCuts,
                             	     "D*(2010)+" : finalDstarCuts}

	Bd2DstarMu.CombinationCut = "  (DAMASS('B0') > %(BdDeltaMassLower)s)"\
                            	    "& (DAMASS('B0') < %(BdDeltaMassUpper)s)" % self.getProps()

	Bd2DstarMu.MotherCut = "  (BPVDIRA > %(BdDIRA)s)"\
			       "& (VFASPF(VCHI2/VDOF) < %(BdVCHI2)s)" % self.getProps()
	
	Bd2DstarMuSel = Selection("SelBd2DstarMuNuLoose",
                 	Algorithm = Bd2DstarMu,
                 	RequiredSelections = [stdVeryLooseDstar, stdLooseMuons])
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
