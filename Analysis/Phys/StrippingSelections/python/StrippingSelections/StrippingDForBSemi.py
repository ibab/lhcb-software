# $Id: StrippingDForBSemi.py,v 1.1 2010-08-13 20:13:13 lzhang Exp $
__author__ = ['Liming Zhang']
__date__ = '10/08/2010'
__version__ = '$Revision: 1.1 $'
"""
D+, Ds, /\c selections as control sample for B->DMuNuX 
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingDforBSemiConf(LHCbConfigurableUser):
    __slots__ = {
        "PreScale"       : 0.1
        ,"MINIPCHI2"     : 4.0    # adimensiional
        ,"TRCHI2"        : 10.0   # adimensiional
        ,"KaonPIDK"      : 4.0  # adimensiional
        ,"PT"            : 300.0  # MeV
        ,"DsDIRA"        : 0.9    # adimensiional
        ,"DsFDCHI2"      : 64.0   # adimensiional
        ,"DsMassWin"     : 100.0  # MeV
        ,"DsAMassWin"    : 120.0  # MeV
        ,"DsIP"          : 7.4    #mm
        ,"DsVCHI2DOF"    : 10.0
        ,"DDocaMax"      : 0.5    #mm        
        }

    def _Dp2KPiPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _dp2kpipi = CombineParticles("Dp2KPiPi_forBSemi")
        _dp2kpipi.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons" ]
        _dp2kpipi.DecayDescriptors = [ '[D+ -> K- pi+ pi+]cc' ]
        _dp2kpipi.DaughtersCuts = { "pi+" : "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(PT)s *MeV)"\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" % self.getProps(),
                                    "K+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDK> %(KaonPIDK)s)" % self.getProps() }
        _dp2kpipi.CombinationCut = "(ADAMASS('D+') < %(DsAMassWin)s *MeV)  & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ACUTDOCA( %(DDocaMax)s *mm, ''))" % self.getProps()
        _dp2kpipi.MotherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) & (ADMASS('D+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) & (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.getProps()
        return _dp2kpipi
    
    def _Ds2KKPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _ds2kkpi = CombineParticles("Ds2KKPi_forBSemi")
        _ds2kkpi.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons" ]
        _ds2kkpi.DecayDescriptors = [ '[D+ -> K+ K- pi+]cc' ]
        _ds2kkpi.DaughtersCuts = { "pi+" : "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(PT)s *MeV)"\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" % self.getProps(),
                                    "K+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDK> %(KaonPIDK)s)" % self.getProps() }
        _ds2kkpi.CombinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV)  & (ACUTDOCA( %(DDocaMax)s *mm, ''))" % self.getProps()
        _ds2kkpi.MotherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) & (DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                             "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                             "& (BPVVDCHI2 > %(DsFDCHI2)s) & (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.getProps()
        return _ds2kkpi
        
    
    def _Lc2PKPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _lambdac = CombineParticles("Lc2PKPi_forBSemi")
        _lambdac.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons", "Phys/StdLooseProtons" ]
        _lambdac.DecayDescriptors = [ '[Lambda_c+ -> K- p+ pi+]cc' ]
        _lambdac.DaughtersCuts = { "pi+" : "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(PT)s *MeV)"\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" % self.getProps(),
                                    "K+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDK> %(KaonPIDK)s)" % self.getProps(),
                                    "p+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>%(KaonPIDK)s)" % self.getProps()}
        _lambdac.CombinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ACUTDOCA( %(DDocaMax)s *mm, ''))" % self.getProps()
        _lambdac.MotherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) & (BPVDIRA> %(DsDIRA)s) &   (BPVIP()< %(DsIP)s *mm)"  % self.getProps()
        return _lambdac
     
                                      
 
    def DpforBSemiLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        props = self.getProps()
        return StrippingLine('DpforBSemiLine'
                             , prescale =  props['PreScale']
                             , algos = [ self._Dp2KPiPiFilter() ]
                             )

    def DsforBSemiLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        props = self.getProps()
        return StrippingLine('DsforBSemiLine'
                             , prescale =  props['PreScale']
                             , algos = [ self._Ds2KKPiFilter()]
                                         
                             )    
    
    def LcforBSemiLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        props = self.getProps()
        return StrippingLine('LcforBSemiLine'
                             , prescale =  props['PreScale']
                             , algos = [ self._Lc2PKPiFilter() ]
                                         
                             )
    
#
#
####################################################################################################
#
#
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

    
        
        
    
