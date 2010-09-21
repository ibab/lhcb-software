__author__ = ['Liming Zhang']
__date__ = '24/03/2010'
__version__ = '$Revision: 1.2 $'
"""
Bs->phiMuMu and f0(980)MuMu selections
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingBs2PhiMuMuConf(LHCbConfigurableUser):
    __slots__ = {
         "MINIPCHI2"     : 4.00   # adimensiional
        ,"TRCHI2"        : 10.0   # adimensiional
        ,"KaonPIDK"      : 1e-10  # adimensiional
        ,"PhiPT"         : 100    # MeV
        ,"MuonMINIPCHI2" : 2.25   # adimensiional
        ,"MuonPIDmu"     : -5.0  # adimensiional
        ,"MuonTRCHI2"    : 10.0   # adimensiional
        ,"BsMassWin"     : 250.0  # MeV
        ,"BsVCHI2DOF"    : 8.0   # adimensiional
        ,"BsDIRA"        : 0.9993  # adimensiional
        ,"BsFDCHI2"      : 25.0   # adimensiional
        ,"PionPIDK"      : 10.0   # adimensiional
        ,"f0MassWin"     : 200.0  # MeV
        ,"VCHI2"         : 10.0   # adimensiional
        ,"BsIPCHI2"      : 36.0  # adimensiional
        ,"DocaMax"       : 0.5  #mm
        }

    def _phiFilter( self ):
        from Configurables import FilterDesktop
        from CommonParticles.StdLooseDetachedPhi import StdLooseDetachedPhi2KK
        _phi = FilterDesktop("phi_Bs2PhiMuMu", InputLocations = [ "StdLooseDetachedPhi2KK" ] )
        _phi.Code = "(PT > %(PhiPT)s *MeV) & (VFASPF(VCHI2) < %(VCHI2)s) & (2==NINTREE( ('K+'==ABSID) & (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s ) & (TRCHI2DOF < %(TRCHI2)s) & (PIDK> %(KaonPIDK)s)))" % self.getProps()
        return _phi

    def _diMuonFilter( self ):
        from Configurables import FilterDesktop
        from CommonParticles.StdLooseDiMuon import StdLooseDiMuon        
        _diMuon = FilterDesktop("diMuon_Bs2PhiMuMu", InputLocations = [ "StdLooseDiMuon" ])
        _diMuon.Code = "(VFASPF(VCHI2) < %(VCHI2)s) & (2==NINTREE( ('mu+'==ABSID) & (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s) & (HASMUON) & (PIDmu> %(MuonPIDmu)s) & (TRCHI2DOF< %(MuonTRCHI2)s))) & (BPVDIRA > 0.0)" % self.getProps()
        return _diMuon

    def _Bs2PhiMuMu( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _Bs = CombineParticles("Bs2PhiMuMu")
        _Bs.InputLocations = [ "diMuon_Bs2PhiMuMu", "phi_Bs2PhiMuMu" ]
        _Bs.DecayDescriptors = [ 'B_s0 -> J/psi(1S) phi(1020)']
        _Bs.CombinationCut = "ADAMASS('B_s0') < %(BsMassWin)s *MeV" % self.getProps()
        _Bs.MotherCut = "  (VFASPF(VCHI2/VDOF) < %(BsVCHI2DOF)s)" \
                        "& (BPVDIRA > %(BsDIRA)s)" \
                        "& (BPVIPCHI2() < %(BsIPCHI2)s)" \
                        "& (BPVVDCHI2 > %(BsFDCHI2)s)" % self.getProps()
#        _Bs.ReFitPVs = True
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
        _f0.CombinationCut = "(ADAMASS('f_0(980)') < %(f0MassWin)s *MeV) & (APT > %(PhiPT)s) & (ACUTDOCA( %(DocaMax)s *mm, ''))" % self.getProps()
        _f0.DaughtersCuts = { "pi+" : "  (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s )"\
                                      "& (TRCHI2DOF < %(TRCHI2)s) & (PIDK< %(PionPIDK)s)" % self.getProps() }
        _f0.MotherCut = "(VFASPF(VCHI2) < %(VCHI2)s)"  % self.getProps()                                       
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

    
        
        
    
