
__author__ = ['Phillip Urquijo']
__date__ = '08/05/2010'
__version__ = '$Revision: 1.1 $'

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
        "MuonTRCHI2"          : 10.   ,#adimensional
        "MuonP"               : 3000. ,#MeV
        "MuonPT"              : 1000. ,#MeV
        "MuonPIDK"            : 0.    ,#adimensional
        "MuonPIDmu"           : 0.    ,#adimensional
        "MuonPIDp"            : 0.    ,#adimensional
#        "MuonMINIP"           : 0.05  ,#mm
        "MuonMINIPCHI2"       : 16     ,#adminensional
        #Xu
        #Pi Channel
        "PionTRCHI2"          : 10.   ,#adimensional
        "PionP"               : 3000. ,#MeV
        "PionPT"              : 800.  ,#MeV
        "PionPIDK"            : 0.    ,#adimensional
        "PionPIDmu"           : 0.    ,#adimensional
        "PionPIDp"            : 0.    ,#adimensional
#        "PionMINIP"           : 0.05  ,#mm
        "PionMINIPCHI2"       : 16     ,#adminensional
        #K channel
        "KaonTRCHI2"          : 10.   ,#adimensional
        "KaonP"               : 3000. ,#MeV
        "KaonPT"              : 800.  ,#MeV
        "KaonPIDK"            : 0.    ,#adimensional
        "KaonPIDmu"           : 0.    ,#adimensional
        "KaonPIDp"            : 0.    ,#adimensional
#        "KaonMINIP"           : 0.05  ,#mm
        "KaonMINIPCHI2"       : 16     ,#adminensional
        #Rho channel
        "RhoMassWindow"       : 250.  ,#MeV
        "RhoVCHI2DOF"         : 10    ,#adimensional
        "RhoPT"               : 400.  ,#MeV
        "RhoLeadingPionPT"    : 800.  ,#MeV
        "RhoMINIP"            : 0.05  ,#mm 
        "RhoMINIPCHI2"        : 2     ,#adimensional
        "RhoChPionPT"         : 100.  ,#MeV
        "RhoChPionPIDK"       : -1.   ,#adminensional
#        "RhoChPionMINIP"      : 0.05  ,#mm
        "RhoChPionMINIPCHI2"  : 6     ,#adimensional
        #K* channel
        "KSMassWindow"        : 300.  ,#MeV
        "KSVCHI2DOF"          : 16    ,#adimensional
        "KSPT"                : 400.  ,#MeV
#        "KSMINIP"             : 0.05  ,#mm 
        "KSMINIPCHI2"         : 4     ,#adimensional
        "KstarMassWindow"     : 300.  ,#MeV
        "KstarVCHI2DOF"       : 16    ,#adimensional
        "KstarPT"             : 400.  ,#MeV
#        "KstarMINIP"          : 0.05  ,#mm 
        "KstarMINIPCHI2"      : 4     ,#adimensional
        "KstarChPionPT"       : 100.  ,#MeV
        "KstarChPionPIDK"     : -1.   ,#adminensional
#        "KstarChPionMINIP"    : 0.05  ,#mm
        "KstarChPionMINIPCHI2": 6     ,#adimensional
        "KstarLeadingPionPT"  : 800.  ,#MeV
        #Xu
        "PiMuMassLowLoose"    : 1200. ,#MeV
        "KMuMassLowTight"     : 1350. ,#MeV
        "RhoMuMassLowTight"   : 1350. ,#MeV
        "KstarMuMassLowTight" : 1400. ,#MeV
        #B
        "BVCHI2DOF"           : 4     ,#adminensional
        "BFD"                 : 1.5   #mm
        }


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
               "& (PIDK-PIDmu> %(KaonPIDmu)s ) & (PIDK-PIDpi> %(KaonPIDK)s ) & (PIDK-PIDp> %(KaonPIDp)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def Pi_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bd2PiMuNuLine', prescale = 1., algos = [self._Pi_line_sequence()])
    
    def PiSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bd2PiMuNuSSLine', prescale = 1., algos = [self._PiSS_line_sequence()])

    def Rho_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bu2RhoMuNuLine', prescale = 1., algos = [self._Rho_line_sequence()])
        
    def K_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2KMuNuLine', prescale = 1., algos = [self._K_line_sequence()])
    
    def KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2KMuNuSSLine', prescale = 1., algos = [self._KSS_line_sequence()])

    def Kstar_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2KstarMuNuLine', prescale = 1., algos = [self._Kstar_line_sequence()])
    
    def KstarSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine('Bs2KstarMuNuSSLine', prescale = 1., algos = [self._KstarSS_line_sequence()])
    
    def _Pi_line_sequence( self ):
        return SelectionSequence('SeqBd2PiMuNu', TopSelection = self._Bd2PiMuNu())

    def _PiSS_line_sequence( self ):
        return SelectionSequence('SeqBd2PiMuNuSS', TopSelection = self._Bd2PiMuNuSS())

    def _Rho_line_sequence( self ):
        return SelectionSequence('SeqBu2RhoMuNu', TopSelection = self._Bu2RhoMuNu())

    def _K_line_sequence( self ):
        return SelectionSequence('SeqBs2KMuNu', TopSelection = self._Bs2KMuNu())
    
    def _KSS_line_sequence( self ):
        return SelectionSequence('SeqBs2KMuNuSS', TopSelection = self._Bs2KMuNuSS())

    def _Kstar_line_sequence( self ):
        return SelectionSequence('SeqBs2KstarMuNu', TopSelection = self._Bs2KstarMuNu())
    
    def _KstarSS_line_sequence( self ):
        return SelectionSequence('SeqBs2KstarMuNuSS', TopSelection = self._Bs2KstarMuNuSS())
    

    
    def _Bd2PiMuNu( self ):
        StdLoosePions = DataOnDemand("StdLoosePions",Location="Phys/StdLoosePions")
        StdLooseMuons = DataOnDemand("StdLooseMuons",Location="Phys/StdLooseMuons")
        
        _PiMu = CombineParticles("PiMu")
        _PiMu.DecayDescriptors = ["[B~0 -> pi+ mu-]cc"]
        _PiMu.CombinationCut = "(AM>%(PiMuMassLowLoose)s*MeV)" % self.getProps()
        _PiMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "pi+": self._NominalPiSelection()  % self.getProps()
            }
        _PiMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVD > %(BFD)s *mm)" % self.getProps()
        PiMu = Selection("SelBd2PiMuNu",
                         Algorithm = _PiMu,
                         RequiredSelections = [StdLooseMuons, StdLoosePions])
        return PiMu

    def _Bd2PiMuNuSS( self ):
        StdLoosePions = DataOnDemand("StdLoosePions",Location="Phys/StdLoosePions")
        StdLooseMuons = DataOnDemand("StdLooseMuons",Location="Phys/StdLooseMuons")
        
        _PiMu = CombineParticles("PiMu")
        _PiMu.DecayDescriptors = ["[B~0 -> pi- mu-]cc","[B~0 -> pi+ mu+]cc"]
        _PiMu.CombinationCut = "(AM>%(PiMuMassLowLoose)s*MeV)" % self.getProps()
        _PiMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "pi+": self._NominalPiSelection()  % self.getProps()
            }
        _PiMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVD > %(BFD)s *mm)" % self.getProps()
        PiMu = Selection("SelBd2PiMuNuSS",
                         Algorithm = _PiMu,
                         RequiredSelections = [StdLooseMuons, StdLoosePions])
        return PiMu

    
    def _Bs2KMuNu( self ):
        StdLooseKaons = DataOnDemand("StdLooseKaons",Location="Phys/StdLooseKaons")
        StdLooseMuons = DataOnDemand("StdLooseMuons",Location="Phys/StdLooseMuons")
        
        _KMu = CombineParticles("KMu")
        _KMu.DecayDescriptors = ["[B_s~0 -> K+ mu-]cc"]
        _KMu.CombinationCut = "(AM>%(KMuMassLowTight)s*MeV)" % self.getProps()
        _KMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "K+" : self._NominalKSelection()  % self.getProps()
            }
        _KMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVD > %(BFD)s *mm)" % self.getProps()
        KMu = Selection("SelBd2KMuNu",
                        Algorithm = _KMu,
                        RequiredSelections = [StdLooseMuons, StdLooseKaons])
        return KMu
    
    def _Bs2KMuNuSS( self ):
        StdLooseKaons = DataOnDemand("StdLooseKaons",Location="Phys/StdLooseKaons")
        StdLooseMuons = DataOnDemand("StdLooseMuons",Location="Phys/StdLooseMuons")
        
        _KMu = CombineParticles("KMu")
        _KMu.DecayDescriptors = ["[B_s~0 -> K- mu-]cc","[B_s~0 -> K+ mu+]cc"]
        _KMu.CombinationCut = "(AM>%(KMuMassLowTight)s*MeV)" % self.getProps()
        _KMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "K+" : self._NominalKSelection()  % self.getProps()
            }
        _KMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVD > %(BFD)s *mm)" % self.getProps()
        KMu = Selection("SelBd2KMuNuSS",
                        Algorithm = _KMu,
                        RequiredSelections = [StdLooseMuons, StdLooseKaons])
        return KMu
    

    def _rho02PiPi( self ):
        StdLoosePions = DataOnDemand("StdLoosePions", Location="Phys/StdLoosePions")
        _rho770 = CombineParticles("Rho02PiPi")
        _rho770.DecayDescriptors = ["[rho(770)0 -> pi- pi+]cc","[rho(770)0 -> pi+ pi+]cc","[rho(770)0 -> pi- pi-]cc"] 
        _rho770.CombinationCut = "(ADAMASS('rho(770)0')< %(RhoMassWindow)s)" % self.getProps()
        _rho770.DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV)"\
                                   "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(RhoChPionPIDK)s )" % self.getProps()
                                   }
        _rho770.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
                                  "& (INTREE( (ID=='pi+') & (PT>%(RhoLeadingPionPT)s *MeV) ) )"\
                                  "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s )" %self.getProps() 
        rho770 = Selection("SelRho02PiPi",
                           Algorithm = _rho770,
                           RequiredSelections = [StdLoosePions])
        return rho770

    def _KS02PiPi( self, trail="" ):
        StdLoosePions = DataOnDemand("StdLoosePions", Location="Phys/StdLoosePions")

        _KS = CombineParticles("KS02PiPi")
        _KS.DecayDescriptors = ["[KS0 -> pi- pi+]cc","[KS0 -> pi+ pi+]cc","[KS0 -> pi- pi-]cc"] 
        _KS.CombinationCut = "(ADAMASS('KS0')< %(KSMassWindow)s)" % self.getProps()
        _KS.DaughtersCuts   = {"pi+":"(PT> %(KstarChPionPT)s *MeV)"\
                               "& (MIPCHI2DV(PRIMARY)> %(KstarChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(KstarChPionPIDK)s )" % self.getProps()
                               }
        _KS.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(KSVCHI2DOF)s ) & (PT > %(KSPT)s *MeV) "\
                              "& (MIPCHI2DV(PRIMARY)> %(KSMINIPCHI2)s )" % self.getProps()
        KS = Selection("SelKS02PiPi"+trail,
                       Algorithm = _KS,
                       RequiredSelections = [StdLoosePions])
        return KS

    def _Kstar2KSPi( self, trail="" ):
        StdLoosePions = DataOnDemand("StdLoosePions", Location="Phys/StdLoosePions")
        KS = self._KS02PiPi(trail)
        _Kstar = CombineParticles("Kstar2KSPi")
        _Kstar.DecayDescriptors = ["[K*(892)+ -> KS0 pi+]cc"] 
        _Kstar.CombinationCut = "(ADAMASS('K*(892)+')< %(KstarMassWindow)s)" % self.getProps()
        _Kstar.DaughtersCuts   = {"pi+":"(PT> %(KstarChPionPT)s *MeV)"\
                                  "& (MIPCHI2DV(PRIMARY)>%(KstarChPionMINIPCHI2)s) & (PIDpi-PIDK>%(KstarChPionPIDK)s)" % self.getProps()
                                  }
        _Kstar.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(KstarVCHI2DOF)s ) & (PT > %(KstarPT)s *MeV) "\
                                 "& (INTREE( (ID=='pi+') & (PT>%(KstarLeadingPionPT)s *MeV) ) )"\
                                 "& (MIPCHI2DV(PRIMARY)> %(KstarMINIPCHI2)s )" % self.getProps() 
        Kstar = Selection("SelKstar2KSPi"+trail,
                          Algorithm = _Kstar,
                          RequiredSelections = [StdLoosePions, KS])
        return KS

        
    def _Bu2RhoMuNu( self ):
        StdLooseMuons = DataOnDemand("StdLooseMuons",Location="Phys/StdLooseMuons")
        Rho = self._rho02PiPi()

        _RhoMu = CombineParticles("RhoMu")
        _RhoMu.DecayDescriptors = ["[B+ -> rho(770)0 mu-]cc"]
        _RhoMu.CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV)" % self.getProps()
        _RhoMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "rho(770)0": "ALL"  % self.getProps()
            }
        _RhoMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVD > %(BFD)s *mm)" % self.getProps()

        RhoMu = Selection("SelBu2RhoMuNu",
                         Algorithm = _RhoMu,
                         RequiredSelections = [StdLooseMuons, Rho])
        return RhoMu

    def _Bs2KstarMuNu( self ):
        StdLooseMuons = DataOnDemand("StdLooseMuons",Location="Phys/StdLooseMuons")
        Kstar = self._Kstar2KSPi("OS")

        _KstarMu = CombineParticles("KstarMu")
        _KstarMu.DecayDescriptors = ["[B_s~0 -> K*(892)+ mu-]cc"]
        _KstarMu.CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV)" % self.getProps()
        _KstarMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "K*(892)+": "ALL"  % self.getProps()
            }
        _KstarMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVD > %(BFD)s *mm)" % self.getProps()

        KstarMu = Selection("SelBs2KstarMuNu",
                            Algorithm = _KstarMu,
                            RequiredSelections = [StdLooseMuons, Kstar])
        return KstarMu
    

    def _Bs2KstarMuNuSS( self ):
        StdLooseMuons = DataOnDemand("StdLooseMuons",Location="Phys/StdLooseMuons")
        Kstar = self._Kstar2KSPi("SS")

        _KstarMu = CombineParticles("KstarMuSS")
        _KstarMu.DecayDescriptors = ["[B_s~0 -> K*(892)- mu-]cc","[B_s~0 -> K*(892)+ mu+]cc"]
        _KstarMu.CombinationCut = "(AM>%(KstarMuMassLowTight)s*MeV)" % self.getProps()
        _KstarMu.DaughtersCuts ={
            "mu-": self._NominalMuSelection() % self.getProps(),
            "K*(892)+": "ALL"  % self.getProps()
            }
        _KstarMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVD > %(BFD)s *mm)" % self.getProps()
        
        KstarMu = Selection("SelBs2KstarMuNuSS",
                            Algorithm = _KstarMu,
                            RequiredSelections = [StdLooseMuons, Kstar])
        return KstarMu
    

    
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
