
__author__ = ['Phillip Urquijo']
__date__ = '21/05/2010'
__version__ = '$Revision: 1.3 $'

'''
B->Xu mu mu , Xu=pi/K channels
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingB2XMuMuSSConf(LHCbConfigurableUser):
    """
    Definition of nominal B->Xu mu nu stripping
    """
    
    __slots__ = {
        #Muons
        "MuonTRCHI2"    : 10.   ,#adimensional
        "MuonP"         : 3000. ,#MeV
        "MuonPT"        : 500. ,#MeV
        #"MuonPIDK"      : -5.   ,#adimensional
        #"MuonPIDmu"     : -5.   ,#adimensional
        #        "MuonPIDp"      : -5.   ,#adimensional
        "MuonMINIPCHI2" : 4     ,#adminensional
        #Xu
        #Pi Channel
        "PionTRCHI2"    : 10.   ,#adimensional
        "PionP"         : 2000. ,#MeV
        "PionPT"        : 300.  ,#MeV
        #"PionPIDK"      : -11.  ,#adimensional
        #"PionPIDmu"     : -10.  ,#adimensional
        #"PionPIDp"      : -10.  ,#adimensional
        "PionMINIPCHI2" : 4     ,#adminensional
        #K Channel
        "KaonTRCHI2"    : 10.   ,#adimensional
        "KaonP"         : 2000. ,#MeV
        "KaonPT"        : 300.  ,#MeV
        "KaonPIDK"      : -1.   ,#adimensional
        "KaonMINIPCHI2" : 4     ,#adminensional
        #B
        "BVCHI2DOF"     : 10    ,#adminensional
        #"BFDCHI2"       : 9     ,#adimensional
        "BDIRA"         : 0.998 ,#adimensional
        'BMassLow'      : 5050  ,# MeV, low mass cut
        'BMassWin'      :  500   # MeV, high mass window
        }


    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
      
    def _NominalPiSelection( self ):
        return "(TRCHI2DOF < %(PionTRCHI2)s )&  (P> %(PionP)s *MeV) &  (PT> %(PionPT)s *MeV)"\
               "& (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s )"
      
    def _NominalKSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def PiSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('B2PiMuMuSSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._B2PiMuMuSS()])

    def PiOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('B2PiMuMuOSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._B2PiMuMuOS()])

    def KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('B2KMuMuSSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._B2KMuMuSS()])

    def KOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('B2KMuMuOSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._B2KMuMuOS()])

    def _muonFilter( self ):
        from Configurables import FilterDesktop
        _mu = FilterDesktop("Mu_forB2XMuMu",
                            InputLocations = ["Phys/StdLooseMuons"])
        _mu.Code = self._NominalMuSelection() % self.getProps()
        return _mu

    def _pionFilter( self ):
        from Configurables import FilterDesktop
        _pi = FilterDesktop("Pi_forB2XMuMu",
                            InputLocations = ["Phys/StdLoosePions"])
        _pi.Code = self._NominalPiSelection() % self.getProps()
        return _pi

    def _kaonFilter( self ):
        from Configurables import FilterDesktop
        _ka = FilterDesktop("K_forB2XMuMu",
                            InputLocations = ["Phys/StdLooseKaons"])
        _ka.Code = self._NominalKSelection() % self.getProps()
        return _ka

    
    def _B2PiMuMuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMuMu = CombineParticles("B2PiMuMuSS")
        _PiMuMu.InputLocations = ["Phys/Mu_forB2XMuMu","Phys/Pi_forB2XMuMu"]
        _PiMuMu.DecayDescriptors = ["[B- -> pi+ mu- mu-]cc"]
        _PiMuMu.CombinationCut = "(AM>%(BMassLow)s*MeV)  & (ADAMASS('B-') < %(BMassWin)s *MeV)" % self.getProps()
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )"\
                            "&(BPVDIRA> %(BDIRA)s )" % self.getProps()
        _PiMuMu.ReFitPVs = True
        return _PiMuMu
    

    def _B2PiMuMuOS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMuMu = CombineParticles("B2PiMuMuOS")
        _PiMuMu.InputLocations = ["Phys/Mu_forB2XMuMu","Phys/Pi_forB2XMuMu"]
        _PiMuMu.DecayDescriptors = ["[B+ -> pi+ mu+ mu-]cc"]
        _PiMuMu.CombinationCut = "(AM>%(BMassLow)s*MeV)  & (ADAMASS('B+') < %(BMassWin)s *MeV)" % self.getProps()
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )"\
                            "&(BPVDIRA> %(BDIRA)s )" % self.getProps()
        _PiMuMu.ReFitPVs = True
        return _PiMuMu

    
    def _B2KMuMuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMuMu = CombineParticles("B2KMuMuSS")
        _KMuMu.InputLocations = ["Phys/Mu_forB2XMuMu","Phys/K_forB2XMuMu"]
        _KMuMu.DecayDescriptors = ["[B- -> K+ mu- mu-]cc"]
        _KMuMu.CombinationCut = "(AM>%(BMassLow)s*MeV)  & (ADAMASS('B-') < %(BMassWin)s *MeV)" % self.getProps()
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )"\
                          "&(BPVDIRA> %(BDIRA)s )" % self.getProps()
        _KMuMu.ReFitPVs = True
        return _KMuMu
    

    def _B2KMuMuOS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMuMu = CombineParticles("B2KMuMuOS")
        _KMuMu.InputLocations = ["Phys/Mu_forB2XMuMu","Phys/K_forB2XMuMu"]
        _KMuMu.DecayDescriptors = ["[B+ -> K+ mu+ mu-]cc"]
        _KMuMu.CombinationCut = "(AM>%(BMassLow)s*MeV)  & (ADAMASS('B+') < %(BMassWin)s *MeV)" % self.getProps()
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )"\
                          "&(BPVDIRA> %(BDIRA)s )" % self.getProps()
        _KMuMu.ReFitPVs = True
        return _KMuMu
    
    
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
        
