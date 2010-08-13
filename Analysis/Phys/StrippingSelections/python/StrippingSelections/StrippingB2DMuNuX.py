# $Id: StrippingB2DMuNuX.py,v 1.2 2010-08-13 20:13:13 lzhang Exp $
__author__ = ['Liming Zhang']
__date__ = '23/07/2010'
__version__ = '$Revision: 1.2 $'
"""
B->DMuNuX inclusive selections
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingB2DMuNuXConf(LHCbConfigurableUser):
    __slots__ = {
        "KPiMINIPCHI2"   : 9.0    # adimensiional
        ,"MINIPCHI2"     : 4.0    # adimensiional
        ,"TRCHI2"        : 10.0   # adimensiional
        ,"KaonPIDK"      : 4.0    # adimensiional
        ,"MuonIPCHI2"    : 4.00   # adimensiional
        ,"MuonPT"        : 800.0   # MeV
        ,"KPiPT"         : 300.0  # MeV
        ,"PT"            : 300.0  # MeV
        ,"DsDIRA"        : 0.9    # adimensiional
        ,"DsFDCHI2"      : 64.0   # adimensiional
        ,"DsMassWin"     : 100.0  # MeV
        ,"DsAMassWin"    : 120.0  # MeV
        ,"DsIP"          : 7.4    #mm
        ,"DsVCHI2DOF"    : 10.0  # adimensiional
        ,"DDocaMax"      : 0.5    #mm
        }

    def _D02KPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _d02kpi = CombineParticles("D02KPi_forb2DMuX")
        _d02kpi.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons" ]
        _d02kpi.DecayDescriptors = [ '[D0 -> K- pi+]cc' ]
        _d02kpi.DaughtersCuts = { "pi+" : "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                                  "& (MIPCHI2DV(PRIMARY)> %(KPiMINIPCHI2)s)" % self.getProps(),
                                  "K+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(KPiPT)s *MeV) & (P>2.0*GeV) "\
                                  "& (MIPCHI2DV(PRIMARY)> %(KPiMINIPCHI2)s)  &  (PIDK> %(KaonPIDK)s)" % self.getProps() }
        _d02kpi.CombinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1400.*MeV) & (ACUTDOCA( %(DDocaMax)s *mm, ''))" % self.getProps()
        _d02kpi.MotherCut = "(SUMTREE( PT,  ISBASIC )>1400.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.getProps()        
        return _d02kpi

    def _Dp2KPiPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _dp2kpipi = CombineParticles("Dp2KPiPi_forb2DMuX")
        _dp2kpipi.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons" ]
        _dp2kpipi.DecayDescriptors = [ '[D+ -> K- pi+ pi+]cc' ]
        _dp2kpipi.DaughtersCuts = { "pi+" : "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(PT)s *MeV)"\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" % self.getProps(),
                                    "K+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDK> %(KaonPIDK)s)" % self.getProps() }
        _dp2kpipi.CombinationCut = "(ADAMASS('D+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ACUTDOCA( %(DDocaMax)s *mm, ''))" % self.getProps()
        _dp2kpipi.MotherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('D+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.getProps()
        return _dp2kpipi
    
    def _Ds2KKPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _ds2kkpi = CombineParticles("Ds2KKPi_forb2DMuX")
        _ds2kkpi.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons" ]
        _ds2kkpi.DecayDescriptors = [ '[D+ -> K+ K- pi+]cc' ]
        _ds2kkpi.DaughtersCuts = { "pi+" : "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(PT)s *MeV)"\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" % self.getProps(),
                                    "K+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDK> %(KaonPIDK)s)" % self.getProps() }
        _ds2kkpi.CombinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ACUTDOCA( %(DDocaMax)s *mm, ''))" % self.getProps()
        _ds2kkpi.MotherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                             "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                             "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.getProps()
        return _ds2kkpi
        
    
    def _Lc2PKPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _lambdac = CombineParticles("Lc2PKPi_forb2DMuX")
        _lambdac.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons", "Phys/StdLooseProtons" ]
        _lambdac.DecayDescriptors = [ '[Lambda_c+ -> K- p+ pi+]cc' ]
        _lambdac.DaughtersCuts = { "pi+" : "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(PT)s *MeV)"\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)" % self.getProps(),
                                    "K+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDK> %(KaonPIDK)s)" % self.getProps(),
                                    "p+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(PT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>%(KaonPIDK)s)" % self.getProps()}
        _lambdac.CombinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ACUTDOCA( %(DDocaMax)s *mm, ''))" % self.getProps()
        _lambdac.MotherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) & (SUMTREE( PT,  ISBASIC )>1800.*MeV) & (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.getProps()
        return _lambdac
     

    def _muonFilter( self ):
        from Configurables import FilterDesktop
        _mu = FilterDesktop("Mu_forb2DMuX", InputLocations = [ "Phys/StdLooseMuons" ])
        _mu.Code = "(PT > %(MuonPT)s *MeV) & (P> 3.0*GeV) & (TRCHI2DOF< %(TRCHI2)s) & (MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s)" %self.getProps()
        return _mu
    
    def _b2D0MuX( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _B = CombineParticles("b2D0MuX")
        _B.InputLocations = [ "Phys/D02KPi_forb2DMuX", "Phys/Mu_forb2DMuX" ]
        _B.DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ]
#        _B.DaughtersCuts = {'mu+': ALL}
        _B.CombinationCut = "(AM<6.2*GeV)" % self.getProps()
        _B.MotherCut = "  (MM<6.0*GeV) & (MM>2.0*GeV) & (VFASPF(VCHI2/VDOF)<10.0) & (BPVDIRA>0.998)  " \
                        "& (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > -1.0*mm ) "  % self.getProps()
#        _B.ReFitPVs = True
        return _B
    
    def _b2DpMuX( self ):
        _B = self._b2D0MuX().clone("b2DpMuX",
                                   InputLocations = [ "Phys/Dp2KPiPi_forb2DMuX", "Phys/Mu_forb2DMuX" ],
                                   DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ])
        return _B

    def _b2DsMuX( self ):
        _B = self._b2DpMuX().clone("b2DsMuX",
                                   InputLocations = [ "Phys/Ds2KKPi_forb2DMuX", "Phys/Mu_forb2DMuX" ])
        return _B
    
    def _b2LcMuX( self ):
        _B = self._b2D0MuX().clone("b2LcMuX",
                                   InputLocations = ["Phys/Lc2PKPi_forb2DMuX", "Phys/Mu_forb2DMuX" ],
                                   DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc' ])
        return _B
              
                                      
    def b2D0MuXLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('b2D0MuXLine'
                             , prescale = 1
                             , algos = [ self._muonFilter(),
                                         self._D02KPiFilter(),
                                         self._b2D0MuX() ]
                             )
    def b2DpMuXLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('b2DpMuXLine'
                             , prescale = 1
                             , algos = [ self._muonFilter(),
                                         self._Dp2KPiPiFilter(),
                                         self._b2DpMuX() ]
                             )
    def b2DsMuXLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('b2DsMuXLine'
                             , prescale = 1
                             , algos = [ self._muonFilter(),
                                         self._Ds2KKPiFilter(),
                                         self._b2DsMuX() ]
                             )    
    
    def b2LcMuXLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('b2LcMuXLine'
                             , prescale = 1
                             , algos = [ self._muonFilter(),
                                         self._Lc2PKPiFilter(),
                                         self._b2LcMuX() ]
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

    
        
        
    
