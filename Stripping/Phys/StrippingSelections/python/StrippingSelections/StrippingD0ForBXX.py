#$Id: StrippingD0ForBXX.py,v 1.4 2010-09-03 02:09:28 lzhang Exp $
__author__ = ['Liming Zhang']
__date__ = '20/05/2010'
__version__ = '$Revision: 1.4 $'
"""
D0->Kpi and D0->K3pi selections for bb-x-section
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingD0forBXXConf(LHCbConfigurableUser):
    __slots__ = {
        "PreScale"       : 0.02   # adimensiional
        ,"MINIPCHI2"     : 9.0   # adimensiional
        ,"TRCHI2"        : 5.0   # adimensiional
        ,"KaonPIDK"      : 4.0   # adimensiional
        ,"KPiPT"         : 300.0  # MeV
        ,"D0DIRA"        : 0.9    # adimensiional
        ,"D0DIRAK3Pi"    : 0.9999    # adimensiional        
        ,"FDCHI2"        : 64.0   # adimensiional
        ,"D0AMassWin"    : 100.0   # MeV
        ,"D0MassWin"     : 80.0  # MeV
        ,"D0IP"          : 7.4    #mm
        ,"D0K3PiIP"      : 0.2    #mm        
        ,"D0VCHI2DOF"    : 6.0   # adimensiional
        ,"DDocaMax"      : 0.5    #mm
        }


    def _D02KPi( self ):
        from Configurables import CombineParticles
        _d02kpi = CombineParticles("D02KPiForBXX")
        _d02kpi.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons" ]
        _d02kpi.DecayDescriptor = '[D0 -> pi+ K-]cc'
        _d02kpi.CombinationCut = "(ADAMASS('D0') < %(D0AMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1400*MeV) & (ACUTDOCA( %(DDocaMax)s *mm, ''))" % self.getProps()
        _d02kpi.DaughtersCuts = { "pi+" : "(P>2.0*GeV)&  (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (PT > %(KPiPT)s *MeV) "\
                                      "& (TRCHI2DOF < %(TRCHI2)s)" % self.getProps(),
                              "K+" :  "(P>2.0*GeV)&  (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (PT > %(KPiPT)s *MeV) "\
                                      "& (TRCHI2DOF < %(TRCHI2)s) & (PIDK> %(KaonPIDK)s)" % self.getProps() }
        _d02kpi.MotherCut = "(VFASPF(VCHI2/VDOF) < %(D0VCHI2DOF)s) & (SUMTREE( PT,  ISBASIC )>1400.*MeV)  & (BPVIP()< %(D0IP)s *mm)" \
                        "& (BPVVDCHI2 > %(FDCHI2)s) & (BPVDIRA> %(D0DIRA)s) & (ADMASS('D0') < %(D0MassWin)s *MeV)"  % self.getProps()
        return _d02kpi
    
    def _D02K3Pi( self ):
        from Configurables import CombineParticles
        _d02k3pi = CombineParticles("D02K3PiForBXX")
        _d02k3pi.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons" ]
        _d02k3pi.DecayDescriptor = '[D0 -> K- pi+ pi- pi+]cc'
        _d02k3pi.CombinationCut = " (ADAMASS('D0') < %(D0AMassWin)s  *MeV) & (APT>1.0*GeV) & (ACUTDOCA( %(DDocaMax)s *mm, ''))"  % self.getProps()
        _d02k3pi.DaughtersCuts = { "pi+" : "  (PT > %(KPiPT)s *MeV) "\
                                      "& (TRCHI2DOF < %(TRCHI2)s)" % self.getProps(),
                              "K+" :  "(P>2.0*GeV)&  (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (PT > %(KPiPT)s *MeV) "\
                                      "& (TRCHI2DOF < %(TRCHI2)s) & (PIDK> %(KaonPIDK)s)" % self.getProps() }
        _d02k3pi.MotherCut = "(PT>1.0*GeV) & (VFASPF(VCHI2/VDOF) < %(D0VCHI2DOF)s)  & (BPVIP()< %(D0K3PiIP)s *mm) " \
                                    "& (INTREE((ABSID=='pi+')&(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)))" \
                        "& (BPVVDCHI2 > %(FDCHI2)s) & (BPVDIRA> %(D0DIRAK3Pi)s) & (ADMASS('D0') < %(D0MassWin)s *MeV)"  % self.getProps()                                      
        return _d02k3pi
       
    def D02KPiforBXXLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        props = self.getProps()
        return StrippingLine('D02KPiforBXXLine'
                             , prescale = props['PreScale']
                             , algos = [ self._D02KPi() ]
                             )

    def D02K3PiforBXXLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        props = self.getProps()
        return StrippingLine('D02K3PiforBXXLine'
                             , prescale = props['PreScale']
                             , algos = [ self._D02K3Pi() ]
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

    
        
        
    
