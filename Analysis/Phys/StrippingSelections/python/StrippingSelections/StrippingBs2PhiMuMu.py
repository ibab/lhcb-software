__author__ = ['Liming Zhang']
__date__ = '24/03/2010'
__version__ = '$Revision: 1.1 $'
"""
Bs->phiMuMu and f0(980)MuMu selections
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingBs2PhiMuMuConf(LHCbConfigurableUser):
    __slots__ = {
         "MINIP"         : 0.05   # mm
        ,"MINIPCHI2"     : 2.25   # adimensiional
        ,"TRCHI2"        : 10.0   # adimensiional
        ,"KaonPIDK"      : -10.0  # adimensiional
        ,"PhiPT"         : 100    # MeV
        ,"MuonMINIP"     : 0.00   # mm
        ,"MuonMINIPCHI2" : 0.00   # adimensiional
        ,"MuonPIDmu"     : -10.0  # adimensiional
        ,"MuonTRCHI2"    : 10.0   # adimensiional
        ,"BsMassWin"     : 250.0  # MeV
        ,"BsVCHI2"       : 40.0   # adimensiional
        ,"BsDIRA"        : 0.999  # adimensiional
        ,"BsFDCHI2"      : 16.0   # adimensiional
        ,"PionPIDK"      : 10.0   # adimensiional
        ,"f0MassWin"     : 500.0  # MeV
        }

    def _phiFilter( self ):
        from Configurables import FilterDesktop
        from CommonParticles.StdVeryLooseDiMuon import StdVeryLooseDiMuon
        _phi = FilterDesktop("phi_Bs2PhiMuMu", InputLocations = [ "StdLoosePhi2KK" ] )
        _phi.Code = "(PT > %(PhiPT)s *MeV) & (2==NINTREE( ('K+'==ABSID) & ((MIPDV(PRIMARY)> %(MINIP)s *mm) | (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) ) & (TRCHI2DOF < %(TRCHI2)s) & (PIDK> %(KaonPIDK)s)))" % self.getProps()
        return _phi

    def _diMuonFilter( self ):
        from Configurables import FilterDesktop
        from CommonParticles.StdLoosePhi import StdLoosePhi2KK
        _diMuon = FilterDesktop("diMuon_Bs2PhiMuMu", InputLocations = [ "StdVeryLooseDiMuon" ])
        _diMuon.Code = "(2==NINTREE( ('mu+'==ABSID) & ((MIPDV(PRIMARY)> %(MuonMINIP)s *mm) | (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s) ) & (HASMUON) & (PIDmu> %(MuonPIDmu)s) & (TRCHI2DOF< %(MuonTRCHI2)s))) & (BPVDIRA > 0.0)" % self.getProps()
        return _diMuon

    def _Bs2PhiMuMu( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _Bs = CombineParticles("Bs2PhiMuMu")
        _Bs.InputLocations = [ "diMuon_Bs2PhiMuMu", "phi_Bs2PhiMuMu" ]
        _Bs.DecayDescriptors = [ 'B_s0 -> J/psi(1S) phi(1020)']
        _Bs.CombinationCut = "ADAMASS('B_s0') < %(BsMassWin)s *MeV" % self.getProps()
        _Bs.MotherCut = "  (VFASPF(VCHI2) < %(BsVCHI2)s)" \
                        "& (BPVDIRA > %(BsDIRA)s)" \
                        "& (BPVVDCHI2 > %(BsFDCHI2)s)" % self.getProps()
        _Bs.ReFitPVs = True
        # Set the OfflineVertexFitter to keep the 4 tracks
        _Bs.addTool( OfflineVertexFitter() )
        _Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        _Bs.OfflineVertexFitter.useResonanceVertex = False
        return _Bs

    def _f02PiPi( self ):
        from Configurables import CombineParticles
        _f0 = CombineParticles("f0_Bs2f0MuMu")
        _f0.InputLocations = [ "StdLoosePions" ]
        _f0.DecayDescriptors = ["f_0(980) -> pi+ pi-"]
        _f0.CombinationCut = "(ADAMASS('f_0(980)') < %(f0MassWin)s *MeV) & (APT > %(PhiPT)s)" % self.getProps()
        _f0.DaughtersCuts = { "pi+" : "  ((MIPDV(PRIMARY)> %(MINIP)s *mm) | (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) )"\
                                      "& (TRCHI2DOF < %(TRCHI2)s) & (PIDK< %(PionPIDK)s)" % self.getProps() }
        _f0.MotherCut = "(VFASPF(VCHI2) < 25.0)"                                       
        return _f0
    
    def _Bs2f0MuMu( self ):
        _Bs = self._Bs2PhiMuMu().clone("Bs2f0MuMu",
                                      InputLocations = ["diMuon_Bs2PhiMuMu", "f0_Bs2f0MuMu" ],
                                      DecayDescriptors = [ 'B_s0 -> J/psi(1S) f_0(980)' ])
        return _Bs
                                      
                                      
    def Bs2PhiMuMuLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('Bs2PhiMuMuLine'
                             , prescale = 1
                             , algos = [ self._diMuonFilter(),
                                         self._phiFilter(),
                                         self._Bs2PhiMuMu() ]
                             )

    def Bs2f0MuMuLine( self ):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('Bs2f0MuMuLine'
                             , prescale = 1
                             , algos = [ self._diMuonFilter(),
                                         self._f02PiPi(),
                                         self._Bs2f0MuMu() ]
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

    
        
        
    
