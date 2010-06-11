#$Id: StrippingD0ForBXX.py,v 1.2 2010-06-11 03:17:42 lzhang Exp $
__author__ = ['Liming Zhang']
__date__ = '20/05/2010'
__version__ = '$Revision: 1.2 $'
"""
D0->Kpi and D0->K3pi selections for bb-x-section
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingD0forBXXConf(LHCbConfigurableUser):
    __slots__ = {
         "MINIPCHI2"      : 9.0   # adimensiional
         ,"TRCHI2"        : 10.0   # adimensiional
         ,"KaonPIDK"      : 1.0e-10  # adimensiional
         ,"KPiPT"         : 250.0  # MeV
        ,"D0DIRA"        : 0.9    # adimensiional
        ,"D0DIRAK3Pi"    : 0.999    # adimensiional        
        ,"FDCHI2"        : 49.0   # adimensiional
        ,"D0MassWin"     : 100.0  # MeV
        ,"D0IP"          : 7.4    #mm
        }


    def _D02KPi( self ):
        from Configurables import CombineParticles
        _d02kpi = CombineParticles("D02KPiForBXX")
        _d02kpi.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons" ]
        _d02kpi.DecayDescriptor = '[D0 -> pi+ K-]cc'
        _d02kpi.CombinationCut = "(ADAMASS('D0') < 150 *MeV)" % self.getProps()
        _d02kpi.DaughtersCuts = { "pi+" : "  (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (PT > %(KPiPT)s *MeV) "\
                                      "& (TRCHI2DOF < %(TRCHI2)s)" % self.getProps(),
                              "K+" :  "  (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (PT > %(KPiPT)s *MeV) "\
                                      "& (TRCHI2DOF < %(TRCHI2)s) & (PIDK> %(KaonPIDK)s)" % self.getProps() }
        _d02kpi.MotherCut = "(VFASPF(VCHI2/VDOF) < 20.0) & (SUMTREE( PT,  ISBASIC )>1200.*MeV) & (BPVIP()< %(D0IP)s *mm)" \
                        "& (BPVVDCHI2 > %(FDCHI2)s) & (BPVDIRA> %(D0DIRA)s) & (ADMASS('D0') < %(D0MassWin)s *MeV)"  % self.getProps()
        return _d02kpi
    
    def _D02K3Pi( self ):
        from Configurables import CombineParticles
        _d02k3pi = CombineParticles("D02K3PiForBXX")
        _d02k3pi.InputLocations = [ "Phys/StdLoosePions", "Phys/StdLooseKaons" ]
        _d02k3pi.DecayDescriptor = '[D0 -> K- pi+ pi- pi+]cc'
        _d02k3pi.CombinationCut = "(ADAMASS('D0') < 150 *MeV)" % self.getProps()
        _d02k3pi.DaughtersCuts = { "pi+" : "  (PT > %(KPiPT)s *MeV) "\
                                      "& (TRCHI2DOF < %(TRCHI2)s)" % self.getProps(),
                              "K+" :  "  (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (PT > %(KPiPT)s *MeV) "\
                                      "& (TRCHI2DOF < %(TRCHI2)s) & (PIDK> %(KaonPIDK)s)" % self.getProps() }
        _d02k3pi.MotherCut = "(VFASPF(VCHI2/VDOF) < 20.0) & (SUMTREE( PT,  ISBASIC )>1500.*MeV) & (BPVIP()< %(D0IP)s *mm) " \
                            "& (INTREE((ABSID=='pi+')&(MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)))" \
                        "& (BPVVDCHI2 > %(FDCHI2)s) & (BPVDIRA> %(D0DIRAK3Pi)s) & (ADMASS('D0') < %(D0MassWin)s *MeV)"  % self.getProps()                                      
        return _d02k3pi
       
    def D02KPiforBXXLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('D02KPiforBXXLine'
                             , prescale = 1
                             , algos = [ self._D02KPi() ]
                             )

    def D02K3PiforBXXLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('D02K3PiforBXXLine'
                             , prescale = 1
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

    
        
        
    
