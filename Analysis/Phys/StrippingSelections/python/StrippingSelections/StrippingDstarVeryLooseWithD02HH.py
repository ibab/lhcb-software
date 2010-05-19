# $Id: StrippingDstarVeryLooseWithD02HH.py,v 1.1 2010-05-19 14:17:46 gcowan Exp $

__author__ = 'Greig Cowan, Marta Calvi'
__date__ = '18/05/2010'
__version__ = '$Revision: 1.1 $'

'''
Prompt Dstar->D0(Kpi)pi stripping selection for studying Dstar in Bd->Dstar mu nu.
Cuts on the Dstar are the same as in the Bd stripping, with inclusion of a pointing
angle cut to ensure that we look at the prompt component. 
'''

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import MeV
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingDstarVeryLooseWithD02HHConf(LHCbConfigurableUser):
    __slots__ = { 
			"D0PT"			: 1200.*MeV
		,	"D0BPVVDCHI2"		: 40.
		,	"D0BPVDIRA"		: 0.9995
		,	"KaonPT"		: 350.*MeV
		,	"PionPT"		: 350.*MeV
                }
    
    def line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	DstarSel = self.DstarVeryLooseWithD02HH()
	DstarSeq = SelectionSequence("SeqDstarVeryLooseWithD02HH", TopSelection = DstarSel)
	return StrippingLine('DstarVeryLooseWithD02HHLine', prescale = 1, algos = [DstarSeq])
     	
    def DstarVeryLooseWithD02HH( self ):
	stdVeryLooseDstar = DataOnDemand(Location = "Phys/StdVeryLooseDstarWithD02KPi")
	stdVeryLooseDstarDCS = DataOnDemand(Location = "Phys/StdVeryLooseDstarWithD02KPiDCS")
	
	# D* has the following decay chain:  D*+ -> ( D0 -> K pi ) pi 
	Dstar = FilterDesktop("FilterDstarVeryLooseWithD02HH")
	
	DstarCutsD0 = "CHILDCUT("\
             	      "    (PT > %(D0PT)s)"\
             	      "  & (BPVDIRA > %(D0BPVDIRA)s)"\
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

	Dstar.Code = DstarCutsD0 + DstarCutsK + DstarCutsPi 

	DstarSel = Selection("DstarVeryLooseWithD02HH",
                 	Algorithm = Dstar,
                 	RequiredSelections = [stdVeryLooseDstar, stdVeryLooseDstarDCS])
	return DstarSel

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
