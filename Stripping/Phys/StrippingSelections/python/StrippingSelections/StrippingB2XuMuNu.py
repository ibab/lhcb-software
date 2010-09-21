
__author__ = ['Phillip Urquijo']
__date__ = '08/05/2010'
__version__ = '$Revision: 1.4 $'

'''
B->Xu mu nu exclusive reconstruction in Xu=pi/rho/K/K* channels
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingB2XuMuNuConf(LHCbConfigurableUser):
    """
    Definition of B->Xu mu nu (Xu=pi,rho,k,k*) stripping
    """
    
    __slots__ = {
        #Muons
        "MuonTRCHI2"          : 6.   ,#adimensional
        "MuonP"               : 3000. ,#MeV
        "MuonPT"              : 1000. ,#MeV
        "MuonPIDK"            : 0.    ,#adimensional
        "MuonPIDmu"           : 3.    ,#adimensional
        "MuonPIDp"            : 0.    ,#adimensional
        #        "MuonMINIP"           : 0.05  ,#mm
        "MuonMINIPCHI2"       : 12     ,#adminensional
        #Xu
        #Pi Channel
        "PionTRCHI2"          : 6.   ,#adimensional
        "PionP"               : 3000. ,#MeV
        "PionPT"              : 800.  ,#MeV
        "PionPIDK"            : 0.    ,#adimensional
        "PionPIDmu"           : 0.    ,#adimensional
        "PionPIDp"            : 0.    ,#adimensional
        #        "PionMINIP"           : 0.05  ,#mm
        "PionMINIPCHI2"       : 16     ,#adminensional
        #K channel
        "KaonTRCHI2"          : 6.   ,#adimensional
        "KaonP"               : 3000. ,#MeV
        "KaonPT"              : 800.  ,#MeV
        "KaonPIDK"            : 2.    ,#adimensional
        "KaonPIDmu"           : 0.    ,#adimensional
        "KaonPIDp"            : 0.    ,#adimensional
        #        "KaonMINIP"          : 0.05  ,#mm
        "KaonMINIPCHI2"       : 16     ,#adminensional
        #Rho channel
        "RhoMassWindow"       : 190.  ,#MeV
        "RhoVCHI2DOF"         : 6    ,#adimensional
        "RhoPT"               : 500.  ,#MeV
        "RhoLeadingPionPT"    : 800.  ,#MeV
        #        "RhoMINIP"           : 0.05  ,#mm 
        "RhoMINIPCHI2"        : 4     ,#adimensional
        "RhoChPionPT"         : 300.  ,#MeV
        "RhoChPionTRCHI2"     : 10.  ,#MeV
        "RhoChPionPIDK"       : -10.   ,#adminensional
        #        "RhoChPionMINIP"     : 0.05  ,#mm
        "RhoChPionMINIPCHI2"  : 9     ,#adimensional
        #K* channel
        "KSMassWindow"        : 300.  ,#MeV
        "KSVCHI2DOF"          : 6    ,#adimensional
        "KSPT"                : 400.  ,#MeV
        #        "KSMINIP"             : 0.05  ,#mm 
        "KSMINIPCHI2"         : 4     ,#adimensional
        "KstarMassWindow"     : 100.  ,#MeV
        "KstarVCHI2DOF"       : 6    ,#adimensional
        "KstarPT"             : 400.  ,#MeV
        #        "KstarMINIP"          : 0.05  ,#mm 
        "KstarMINIPCHI2"      : 4     ,#adimensional
        "KstarChPionPT"       : 100.  ,#MeV
        "KstarChPionTRCHI2"   : 10.  ,#MeV
        "KstarChPionPIDK"     : -10.   ,#adminensional
        #        "KstarChPionMINIP"    : 0.05  ,#mm
        "KstarChPionMINIPCHI2": 9     ,#adimensional
        "KstarLeadingPionPT"  : 800.  ,#MeV
        #Xu
        "PiMuMassLowLoose"    : 1230. ,#MeV
        "KMuMassLowTight"     : 1370. ,#MeV
        "RhoMuMassLowTight"   : 1360. ,#MeV
        "KstarMuMassLowTight" : 1400. ,#MeV
        #B
        "BVCHI2DOF"           : 4     ,#adminensional
        "BFDCHI2LOW"             : 15,   #adimensional
        "BFDCHI2HIGH"             : 225,   #adimensional
        "XMuMassUpper"        : 5500 # MeV
        }


    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (PIDmu-PIDpi> %(MuonPIDmu)s )"\
               "& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "& (PIDmu-PIDK> %(MuonPIDK)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
    
    def _NominalPiSelection( self ):
        return "(TRCHI2DOF < %(PionTRCHI2)s )&  (P> %(PionP)s *MeV) &  (PT> %(PionPT)s *MeV)"\
               "& (PIDpi-PIDK> %(PionPIDK)s )& (PIDpi-PIDp> %(PionPIDp)s )& (PIDpi-PIDmu> %(PionPIDmu)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s )"
    
    def _NominalKSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s )& (PIDK-PIDp> %(KaonPIDp)s )& (PIDK-PIDmu> %(KaonPIDmu)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def Pi_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine('Bd2PiMuNuLine',
                             HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2MuTrackDecision')",
                             prescale = 1.,
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._Bd2PiMuNu()
                                      ])
    
    def PiSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bd2PiMuNuSSLine', prescale = 0.5,
                             HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2MuTrackDecision')",
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._Bd2PiMuNuSS()])

    def Rho_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bu2RhoMuNuLine', prescale = 1.,
                             HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2MuTrackDecision')",
                             algos = [self._muonFilter(),
                                      self._Rho02PiPiFilter(),
                                      self._Bu2RhoMuNu()])
        
    def RhoWS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bu2RhoMuNuWSLine', prescale = 0.5,
                             HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2MuTrackDecision')",
                             algos = [self._muonFilter(),
                                      self._Rho02PiPiWSFilter(),
                                      self._Bu2RhoMuNuWS()])
    
    def K_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2KMuNuLine', prescale = 1.,
                             HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2MuTrackDecision')",
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._Bs2KMuNu()])
    
    def KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2KMuNuSSLine', prescale = 0.5,
                             HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2MuTrackDecision')",
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._Bs2KMuNuSS()])

    def Kstar_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2KstarMuNuLine', prescale = 1.,
                             HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2MuTrackDecision')",
                             algos = [self._muonFilter(),
                                      self._KS02PiPiFilter(),
                                      self._Kstar2KSPiFilter(),
                                      self._Bs2KstarMuNu()])

    def KstarSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2KstarMuNuSSLine', prescale = 0.5,
                             HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2MuTrackDecision')",
                             algos = [self._muonFilter(),
                                      self._KS02PiPiFilter(),
                                      self._Kstar2KSPiFilter(),
                                      self._Bs2KstarMuNuSS()])

    def _muonFilter( self ):
        from Configurables import FilterDesktop
        _mu = FilterDesktop("Mu_forB2XuMuNu",
                            InputLocations = ["Phys/StdLooseMuons"])
        _mu.Code = self._NominalMuSelection() % self.getProps()
        return _mu

    def _pionFilter( self ):
        from Configurables import FilterDesktop
        _pi = FilterDesktop("Pi_forB2XuMuNu",
                            InputLocations = ["Phys/StdLoosePions"])
        _pi.Code = self._NominalPiSelection() % self.getProps()
        return _pi

    def _kaonFilter( self ):
        from Configurables import FilterDesktop
        _ka = FilterDesktop("K_forB2XuMuNu",
                            InputLocations = ["Phys/StdLooseKaons"])
        _ka.Code = self._NominalKSelection() % self.getProps()
        return _ka

    def _Rho02PiPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _rho770 = CombineParticles("Rho02PiPi_forB2XuMuNu")
        _rho770.InputLocations = ["Phys/StdLoosePions"]
        _rho770.DecayDescriptors = ["[rho(770)0 -> pi- pi+]cc"] 
        _rho770.DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV) &   (TRCHI2DOF < %(RhoChPionTRCHI2)s )"\
                                   "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(RhoChPionPIDK)s )" % self.getProps()
                                   }
        _rho770.CombinationCut = "(ADAMASS('rho(770)0')< %(RhoMassWindow)s)" % self.getProps()
        _rho770.MotherCut       = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV ) & (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
                                  "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s )" %self.getProps() 
        return _rho770

    def _Rho02PiPiWSFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _rho770 = CombineParticles("Rho02PiPiWS_forB2XuMuNu")
        _rho770.InputLocations = ["Phys/StdLoosePions"]
        _rho770.DecayDescriptors = ["[rho(770)0 -> pi+ pi+]cc"] 
        _rho770.DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV) &   (TRCHI2DOF < %(RhoChPionTRCHI2)s )"\
                                   "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(RhoChPionPIDK)s )" % self.getProps()
                                   }
        _rho770.CombinationCut = "(ADAMASS('rho(770)0')< %(RhoMassWindow)s)" % self.getProps()
        _rho770.MotherCut       = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV ) & (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
                                  "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s )" %self.getProps() 
        return _rho770

    def _KS02PiPiFilter( self, trail="" ):
        from Configurables import FilterDesktop, CombineParticles
        _KS = CombineParticles("KS02PiPi_forB2XuMuNu")
        _KS.InputLocations = ["Phys/StdLoosePions"]
        _KS.DecayDescriptors = ["[KS0 -> pi- pi+]cc"] 
        _KS.CombinationCut = "(ADAMASS('KS0')< %(KSMassWindow)s)" % self.getProps()
        _KS.DaughtersCuts   = {"pi+":"(PT> %(KstarChPionPT)s *MeV) &  (TRCHI2DOF < %(KstarChPionTRCHI2)s )"\
                                   "& (MIPCHI2DV(PRIMARY)> %(KstarChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(KstarChPionPIDK)s )" % self.getProps()
                               }
        _KS.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(KSVCHI2DOF)s ) & (PT > %(KSPT)s *MeV) "\
                              "& (MIPCHI2DV(PRIMARY)> %(KSMINIPCHI2)s )" % self.getProps()
        return _KS

    def _Kstar2KSPiFilter( self, trail="" ):
        from Configurables import FilterDesktop, CombineParticles
        _Kstar = CombineParticles("Kstar2KSPi_forB2XuMuNu")
        _Kstar.InputLocations = ["Phys/StdLoosePions", "Phys/KS02PiPi_forB2XuMuNu"]
        _Kstar.DecayDescriptors = ["[K*(892)+ -> KS0 pi+]cc"] 
        _Kstar.CombinationCut = "(ADAMASS('K*(892)+')< %(KstarMassWindow)s)" % self.getProps()
        _Kstar.DaughtersCuts   = {"pi+":"(PT> %(KstarChPionPT)s *MeV) &  (TRCHI2DOF < %(KstarChPionTRCHI2)s )"\
                                  "& (MIPCHI2DV(PRIMARY)>%(KstarChPionMINIPCHI2)s) & (PIDpi-PIDK>%(KstarChPionPIDK)s)" % self.getProps()
                                  }
        _Kstar.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(KstarVCHI2DOF)s ) & (PT > %(KstarPT)s *MeV) "\
                                 "& (MAXTREE('pi+'==ABSID,PT)>%(KstarLeadingPionPT)s *MeV )"\
                                 "& (MIPCHI2DV(PRIMARY)> %(KstarMINIPCHI2)s )" % self.getProps() 
        return _Kstar
    
    def _Bd2PiMuNu( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMu = CombineParticles("PiMu")
        _PiMu.InputLocations=["Phys/Pi_forB2XuMuNu","Phys/Mu_forB2XuMuNu"]
        _PiMu.DecayDescriptors = ["[B~0 -> pi+ mu-]cc"]
        _PiMu.CombinationCut = "(AM>%(PiMuMassLowLoose)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self.getProps()
        _PiMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVDCHI2> %(BFDCHI2LOW)s)" % self.getProps()
        _PiMu.ReFitPVs = True
        return _PiMu

    def _Bd2PiMuNuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMu = CombineParticles("PiMuSS")
        _PiMu.InputLocations=["Phys/Pi_forB2XuMuNu","Phys/Mu_forB2XuMuNu"]
        _PiMu.DecayDescriptors = ["[B~0 -> pi- mu-]cc"]
        _PiMu.CombinationCut = "(AM>%(PiMuMassLowLoose)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self.getProps()
        _PiMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVDCHI2> %(BFDCHI2LOW)s)" % self.getProps()
        _PiMu.ReFitPVs = True
        return _PiMu

    
    def _Bs2KMuNu( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMu = CombineParticles("KMu")
        _KMu.InputLocations=["Phys/K_forB2XuMuNu","Phys/Mu_forB2XuMuNu"]
        _KMu.DecayDescriptors = ["[B_s~0 -> K+ mu-]cc"]
        _KMu.CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self.getProps()
        _KMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )" % self.getProps()
        _KMu.ReFitPVs = True
        return _KMu
    
    def _Bs2KMuNuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMu = CombineParticles("KMuSS")
        _KMu.InputLocations=["Phys/K_forB2XuMuNu","Phys/Mu_forB2XuMuNu"]
        _KMu.DecayDescriptors = ["[B_s~0 -> K- mu-]cc"]
        _KMu.CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self.getProps()
        _KMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )" % self.getProps()
        _KMu.ReFitPVs = True
        return _KMu
        
    def _Bu2RhoMuNu( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _RhoMu = CombineParticles("RhoMu")
        _RhoMu.InputLocations=["Phys/Mu_forB2XuMuNu","Phys/Rho02PiPi_forB2XuMuNu"]
        _RhoMu.DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"]
        _RhoMu.CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self.getProps()
        _RhoMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )" % self.getProps()
        _RhoMu.ReFitPVs = True
        return _RhoMu

    def _Bu2RhoMuNuWS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _RhoMu = CombineParticles("RhoMuWS")
        _RhoMu.InputLocations=["Phys/Mu_forB2XuMuNu","Phys/Rho02PiPiWS_forB2XuMuNu"]
        _RhoMu.DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"]
        _RhoMu.CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self.getProps()
        _RhoMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )" % self.getProps()
        _RhoMu.ReFitPVs = True
        return _RhoMu

    def _Bs2KstarMuNu( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KstarMu = CombineParticles("KstarMu")
        _KstarMu.InputLocations=["Phys/Mu_forB2XuMuNu","Phys/Kstar2KSPi_forB2XuMuNu"]
        _KstarMu.DecayDescriptors = ["[B_s~0 -> K*(892)+ mu-]cc"]
        _KstarMu.CombinationCut = "(AM>%(KstarMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self.getProps()
        _KstarMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )" % self.getProps()
        _KstarMu.ReFitPVs = True
        return _KstarMu

    def _Bs2KstarMuNuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KstarMu = CombineParticles("KstarMuSS")
        _KstarMu.InputLocations=["Phys/Mu_forB2XuMuNu","Phys/Kstar2KSPi_forB2XuMuNu"]
        _KstarMu.DecayDescriptors = ["[B_s~0 -> K*(892)+ mu+]cc"]
        _KstarMu.CombinationCut = "(AM>%(KstarMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self.getProps()
        _KstarMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )" % self.getProps()
        _KstarMu.ReFitPVs = True
        return _KstarMu
    
    
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
