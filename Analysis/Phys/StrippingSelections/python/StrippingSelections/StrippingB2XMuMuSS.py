
__author__ = ['Phillip Urquijo']
__date__ = '21/05/2010'
__version__ = '$Revision: 1.1 $'

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
        "MuonPIDK"      : -5.   ,#adimensional
        "MuonPIDmu"     : -5.   ,#adimensional
        "MuonPIDp"      : -5.   ,#adimensional
        "MuonMINIPCHI2" : 4     ,#adminensional
        #Xu
        #Pi Channel
        "PionTRCHI2"    : 10.   ,#adimensional
        "PionP"         : 2000. ,#MeV
        "PionPT"        : 500.  ,#MeV
        "PionPIDK"      : -10.  ,#adimensional
        "PionPIDmu"     : -10.  ,#adimensional
        "PionPIDp"      : -10.  ,#adimensional
        "PionMINIPCHI2" : 4     ,#adminensional
        #K Channel
        "KaonTRCHI2"    : 10.   ,#adimensional
        "KaonP"         : 2000. ,#MeV
        "KaonPT"        : 500.  ,#MeV
        "KaonPIDK"      : -1.   ,#adimensional
        "KaonMINIPCHI2" : 4     ,#adminensional
        #B
        "BVCHI2DOF"     : 36    ,#adminensional
        "BFDCHI2"       : 9     ,#adimensional
        "BDIRA"         : 0.99 ,#adimensional
        'BMassLow'      : 5050  ,# MeV, low mass cut
        'BMassWin'      :  500   # MeV, high mass window
        }

    def PiSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('B2PiMuMuSSLine', prescale = 1., algos = [self._PiSS_line_sequence()])
    
    def _PiSS_line_sequence( self ):
        return SelectionSequence('SeqB2PiMuMuSS', TopSelection = self._B2PiMuMuSS())


    def PiOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('B2PiMuMuOSLine', prescale = 1., algos = [self._PiOS_line_sequence()])
    
    def _PiOS_line_sequence( self ):
        return SelectionSequence('SeqB2PiMuMuOS', TopSelection = self._B2PiMuMuOS())

    def KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('B2KMuMuSSLine', prescale = 1., algos = [self._KSS_line_sequence()])
    
    def _KSS_line_sequence( self ):
        return SelectionSequence('SeqB2KMuMuSS', TopSelection = self._B2KMuMuSS())


    def KOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('B2KMuMuOSLine', prescale = 1., algos = [self._KOS_line_sequence()])
    
    def _KOS_line_sequence( self ):
        return SelectionSequence('SeqB2KMuMuOS', TopSelection = self._B2KMuMuOS())



    def _NominalMuSelection( self ):
        return "(ISLONG) &  (TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (PIDmu-PIDpi> %(MuonPIDmu)s ) & (PIDmu-PIDK> %(MuonPIDK)s ) & (PIDmu-PIDp> %(MuonPIDp)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
      
    def _NominalPiSelection( self ):
        return "(ISLONG) &  (TRCHI2DOF < %(PionTRCHI2)s )&  (P> %(PionP)s *MeV) &  (PT> %(PionPT)s *MeV)"\
               "& (PIDpi-PIDmu> %(PionPIDmu)s ) & (PIDpi-PIDK> %(PionPIDK)s ) & (PIDpi-PIDp> %(PionPIDp)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s )"
      
    def _NominalKSelection( self ):
        return "(ISLONG) &  (TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"
    
    def _B2PiMuMuSS( self ):
        StdLoosePions = DataOnDemand("StdLoosePions","StdLoosePions")
        StdLooseMuons = DataOnDemand("StdLooseMuons","StdLooseMuons")
        
        _PiMuMu = CombineParticles("PiMuMuSS")
        _PiMuMu.DecayDescriptors = ["[B- -> pi+ mu- mu-]cc"]
        _PiMuMu.CombinationCut = "(AM>%(BMassLow)s*MeV)  & (ADAMASS('B-') < %(BMassWin)s *MeV)" % self.getProps()
        _PiMuMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "pi+": self._NominalPiSelection()  % self.getProps()
            }
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVDCHI2 > %(BFDCHI2)s)"\
                          "&(BPVDIRA> %(BDIRA)s )" % self.getProps()
        PiMuMu = Selection("SelB2PiMuMuSS",
                           Algorithm = _PiMuMu,
                           RequiredSelections = [StdLooseMuons, StdLoosePions])
        return PiMuMu
    
    

    def _B2PiMuMuOS( self ):
        StdLoosePions = DataOnDemand("StdLoosePions","StdLoosePions")
        StdLooseMuons = DataOnDemand("StdLooseMuons","StdLooseMuons")
        
        _PiMuMu = CombineParticles("PiMuMuOS")
        _PiMuMu.DecayDescriptors = ["[B+ -> pi+ mu+ mu-]cc"]
        _PiMuMu.CombinationCut = "(AM>%(BMassLow)s*MeV)  & (ADAMASS('B+') < %(BMassWin)s *MeV)" % self.getProps()
        _PiMuMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "pi+": self._NominalPiSelection()  % self.getProps()
            }
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVDCHI2 > %(BFDCHI2)s *mm)"\
                          "&(BPVDIRA> %(BDIRA)s )" % self.getProps()
        PiMuMu = Selection("SelB2PiMuMuOS",
                           Algorithm = _PiMuMu,
                           RequiredSelections = [StdLooseMuons, StdLoosePions])
        return PiMuMu
    
    def _B2KMuMuSS( self ):
        StdLooseKaons = DataOnDemand("StdLooseKaons","StdLooseKaons")
        StdLooseMuons = DataOnDemand("StdLooseMuons","StdLooseMuons")
        
        _KMuMu = CombineParticles("KMuMuSS")
        _KMuMu.DecayDescriptors = ["[B- -> K+ mu- mu-]cc"]
        _KMuMu.CombinationCut = "(AM>%(BMassLow)s*MeV)  & (ADAMASS('B-') < %(BMassWin)s *MeV)" % self.getProps()
        _KMuMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "K+": self._NominalKSelection()  % self.getProps()
            }
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVDCHI2 > %(BFDCHI2)s *mm)"\
                          "&(BPVDIRA> %(BDIRA)s )" % self.getProps()
        KMuMu = Selection("SelB2KMuMuSS",
                           Algorithm = _KMuMu,
                           RequiredSelections = [StdLooseMuons, StdLooseKaons])
        return KMuMu
    
    

    def _B2KMuMuOS( self ):
        StdLooseKaons = DataOnDemand("StdLooseKaons","StdLooseKaons")
        StdLooseMuons = DataOnDemand("StdLooseMuons","StdLooseMuons")
        
        _KMuMu = CombineParticles("KMuMuOS")
        _KMuMu.DecayDescriptors = ["[B+ -> K+ mu+ mu-]cc"]
        _KMuMu.CombinationCut = "(AM>%(BMassLow)s*MeV)  & (ADAMASS('B+') < %(BMassWin)s *MeV)" % self.getProps()
        _KMuMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "K+": self._NominalKSelection()  % self.getProps()
            }
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVDCHI2 > %(BFDCHI2)s *mm)"\
                          "&(BPVDIRA> %(BDIRA)s )" % self.getProps()
        KMuMu = Selection("SelB2KMuMuOS",
                           Algorithm = _KMuMu,
                           RequiredSelections = [StdLooseMuons, StdLooseKaons])
        return KMuMu
    
    
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
        
