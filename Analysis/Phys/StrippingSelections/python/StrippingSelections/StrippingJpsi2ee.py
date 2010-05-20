# $Id: StrippingJpsi2ee.py,v 1.1 2010-05-20 13:24:28 pkoppenb Exp $
## #####################################################################
# A stripping selection for inclusive J/psi(1S) -> e+ e- decays
#
# @authors P. Koppenburg, Jibo He
# @date 2010-05-20
# 
## #####################################################################

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingDiElectronConf(LHCbConfigurableUser):
    """
    Definition of Jpsi -> e e stripping.
    """
    __slots__ = {
        "electronPT" : 300 # MeV
		}

    def loose_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
	_jpsi =  DataOnDemand(Location = 'Phys/StdLooseJpsi2ee')
        _refine = FilterDesktop("RefineJpsi2ee", Code = "(MINTREE('e-'==ABSID,PT)> %(electronPT)s *MeV)" % self.getProps() ) 
        
	JpsiSel = Selection("SelJpsi2ee",
                  Algorithm = _refine,
                  RequiredSelections = [_jpsi]                           
                  )
	JpsiSeq = SelectionSequence("SeqJpsi2ee", TopSelection = JpsiSel )
	return StrippingLine('Jpsi2eeLine', prescale = 1, algos = [JpsiSeq])

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

