
__author__ = ['Phillip Urquijo, Alessandra Borgia']
__date__ = '08/05/2010'
__version__ = '$Revision: 1.5 $'

'''
B->Xu mu nu exclusive reconstruction in Xu=rho/K/K*+ channels
'''
# =============================================================================
##
#  B->Xu mu nu exclusive reconstruction in Xu=pi/rho/K/K*+ channels
#
#  Stripping lines for charmless semileptonic B+/B0/Bs decays
#  to final states with hadron+muon, where the hadron is
#  either a charged track or decays to 2 or 3 charged tracks.
#  The charmless hadronic signal modes are:
#
#  Bs0 -> K+  mu- nu, 
#  B-  -> rho0 mu- nu, 
#  Bs0 -> K*+ mu- nu, (with K*+->KS0 Pi)
#
#  The lines will lead to measurements of SU3 symmetry tests
#  in B decays, and to the exclusive determination of Vub in
#  B+/B0/Bs decays. The priority is toward Bs decay
#  semileptonics.  
#
#  The nature of the topology leads to large backgrounds,
#  hence large data samples are required to accurately
#  determine the background. We anticipate that this analysis
#  will require of order 1fb-1.
############
#  Also added are 2 new lines for Majorana Neutrino decays.
#  B- -> KS0 mu- nu, with KS0 ->pi+ mu- (SS = Same Sign)
#  B- -> KS0 mu- nu, with KS0 ->pi- mu+ (OS = Opposite Sign)
############
#
#  Stripping XX, with requirements that the
#  rate <0.05% and timing <1ms/evt.
## 

"""
Stripping lines for charmless semileptonic B+/B0/Bs decays
to final states with hadron+muon, where the hadron is
either a charged track or decays to 2 or 3 charged tracks.
The charmless hadronic signal modes are:

Bs0 -> K+  mu- nu, 
B+  -> rho0 mu+ nu, 
Bs0 -> K*+ mu- nu, 

The lines will lead to measurements of SU3 symmetry tests
in B decays, and to the exclusive determination of Vub in
B+/B0/Bs decays. The priority is toward Bs decay
semileptonics.  

The nature of the topology leads to large backgrounds,
hence large data samples are required to accurately
determine the background. We anticipate that this analysis
will require of order 1fb-1.

Also added are 2 new lines for Majorana Neutrino decays.
B- -> KS0 mu- nu, with KS0 ->pi+ mu- (SS = Same Sign)
B- -> KS0 mu- nu, with KS0 ->pi- mu+ (OS = Opposite Sign)

Stripping XX, with requirements that the
rate <0.05% and timing <1ms/evt.
Last modification $Date: 2012-January-27 $
               by $Author: aborgia $
"""

confdict= {
    "GEC_nLongTrk"        : 250.  , #adimensional
    #Muons
    "MuonTRCHI2"          : 4.    ,#adimensional
    "MuonP"               : 3000. ,#MeV
    "MuonPT"              : 1000. ,#MeV
    "MuonPIDK"            : 0.    ,#adimensional
    "MuonPIDmu"           : 3.    ,#adimensional
    "MuonPIDp"            : 0.    ,#adimensional
    "MuonMINIPCHI2"       : 12    ,#adminensional
    #Xu
    #K channel
    "KaonTRCHI2"          : 6.   ,#adimensional
    "KaonP"               : 3000. ,#MeV
    "KaonPT"              : 800.  ,#MeV
    "KaonPIDK"            : 5.    ,#adimensional 
    "KaonPIDmu"           : 0.    ,#adimensional
    "KaonPIDp"            : 0.    ,#adimensional
    "KaonMINIPCHI2"       : 16     ,#adminensional
    #Rho channel
    "RhoMassWindow"       : 150.  ,#MeV
    "RhoMassWindowMin1SB" : 300.  ,#MeV
    "RhoMassWindowMax1SB" : 620.  ,#MeV
    "RhoMassWindowMin2SB" : 920.  ,#MeV
    "RhoMassWindowMax2SB" : 1200. ,#MeV
    "RhoVCHI2DOF"         : 6     ,#adimensional
    "RhoPT"               : 500.  ,#MeV
    "RhoLeadingPionPT"    : 800.  ,#MeV
    "RhoMINIPCHI2"        : 4     ,#adimensional
    "RhoChPionPT"         : 300.  ,#MeV
    "RhoChPionTRCHI2"     : 10.   ,#MeV
    "RhoChPionPIDK"       : -10.  ,#adminensional
    "RhoFDCHI2"           : 100.  ,#adimensional
    "RhoFD"               : 6     ,#mm
    "RhoIPMin"            : 0.3   ,#mm    
    "RhoDIRA"             : 0.9   ,#adimensional
    "RhoChPionMINIPCHI2"  : 9.    ,#adimensional
    #Kshort Daughter Cuts
    "KS0DaugP"            : 2000. ,#MeV
    "KS0DaugPT"           : 250.  ,#MeV
    "KS0DaugTrackChi2"    : 4.    ,#adimensional
    "KS0DaugMIPChi2"      : 50.   ,#adimensional
    #Kshort cuts
    "KSMajoranaCutFDChi2" : 100.  ,#adimensional
    "KS0VertexChi2"       : 10.   ,#adimensional
    "KS0PT"               : 700.  ,#adimensional
    "KsLLMaxDz"           : 650.  ,#mm
    "KsLLCutFD"           : 20.   ,#mm
    "KSLLMassLow"         : 456.  ,#MeV
    "KSLLMassHigh"        : 536.  ,#MeV
    "KSLLCutFDChi2"       : 100.  ,#adimensional
    "KS0MIPChi2"          : 8.    ,#adimensional
    #Kstar Pion Daughter Cuts
    "KstarChPionPT"       : 100.  ,#MeV
    "KstarChPionP"        : 2000. ,#MeV
    "KstarChPionTRCHI2"   : 10.   ,#adimensional
    "KstarChPionMINIPCHI2": 9.    ,#adimensional
    "KstarChPionPIDK"     : -10.  ,#adminensional
    #B Mother Cuts
    "BVCHI2DOF"           : 4.    ,#adminensional
    "BVCHI2DOFTight"      : 2.    ,#adminensional
    "BDIRA"               : 0.99  ,#adminensional
    "BFDCHI2HIGH"         : 100.  ,#adimensional
    #B Mass Minima
    "KMuMassLowTight"     : 1500. ,#MeV
    "RhoMuMassLowTight"   : 2000. ,#MeV
    "KshMuMassLowTight"   : 3000. ,#MeV
    "KstarMuMassLowTight" : 2500. ,#MeV
    #
    "XMuMassUpper"        : 5500. ,#MeV
    "XMuMassUpperHigh"    : 6500. ,#MeV
    "Enu"                 : 1850. ,#MeV
    "EnuK"                : 2000. ,#MeV
    'KshZ'                : 0.    #mm
    }

from Gaudi.Configuration import *
from StrippingUtils.Utils import LineBuilder

import logging

default_name="B2XuMuNu"

class B2XuMuNuBuilder(LineBuilder):
    """
    Definition of B->Xu mu nu (Xu=rho,k,k*+) stripping
    and where B ->KS0 mu nu (KS0->MuPi) stripping
    """
    
    __configuration_keys__ = [
        "GEC_nLongTrk"
        ,"MuonTRCHI2"          
        ,"MuonP"               
        ,"MuonPT"              
        ,"MuonPIDK"            
        ,"MuonPIDmu"           
        ,"MuonPIDp"            
        ,"MuonMINIPCHI2"       
        ,"KaonTRCHI2"          
        ,"KaonP"               
        ,"KaonPT"              
        ,"KaonPIDK"             
        ,"KaonPIDmu"           
        ,"KaonPIDp"            
        ,"KaonMINIPCHI2"
        ,"RhoMassWindow"       
        ,"RhoMassWindowMin1SB" 
        ,"RhoMassWindowMax1SB" 
        ,"RhoMassWindowMin2SB" 
        ,"RhoMassWindowMax2SB" 
        ,"RhoVCHI2DOF"         
        ,"RhoPT"               
        ,"RhoLeadingPionPT"    
        ,"RhoMINIPCHI2"        
        ,"RhoChPionPT"         
        ,"RhoChPionTRCHI2"     
        ,"RhoChPionPIDK"
        ,"RhoFDCHI2"           
        ,"RhoFD"               
        ,"RhoIPMin"            
        ,"RhoDIRA"             
        ,"RhoChPionMINIPCHI2"  
        ,"KS0DaugP"            
        ,"KS0DaugPT"           
        ,"KS0DaugTrackChi2"    
        ,"KS0DaugMIPChi2"      
        ,"KSMajoranaCutFDChi2" 
        ,"KS0VertexChi2"       
        ,"KS0PT"               
        ,"KsLLMaxDz"           
        ,"KsLLCutFD"           
        ,"KSLLMassLow"         
        ,"KSLLMassHigh"        
        ,"KSLLCutFDChi2"       
        ,"KS0MIPChi2"          
        ,"KstarChPionPT"       
        ,"KstarChPionP"        
        ,"KstarChPionTRCHI2"   
        ,"KstarChPionMINIPCHI2"
        ,"KstarChPionPIDK"     
        ,"BVCHI2DOF"
        ,"BVCHI2DOFTight"      
        ,"BDIRA"               
        ,"BFDCHI2HIGH"         
        ,"KMuMassLowTight"     
        ,"RhoMuMassLowTight"   
        ,"KshMuMassLowTight"   
        ,"KstarMuMassLowTight" 
        ,"XMuMassUpper"        
        ,"XMuMassUpperHigh"
        ,"Enu"                 
        ,"EnuK"                
        ,'KshZ'                
        ]

    def __init__(self,name,config):
        LineBuilder.__init__(self, name, config)

        from PhysSelPython.Wrappers import Selection, DataOnDemand
        self._stdLooseKsLL = DataOnDemand("Phys/StdLooseKsLL/Particles")

        self.GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                      "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        self._muonSel=None
        self._muonFilter()

        self._MajoranaLineMuSel=None
        self._MajoranaLineMuFilter()

        self._kaonSel=None
        self._kaonFilter()
        
        self._rho770Sel=None
        self._rho770WSSel=None
        self._rho770SBSel=None
        
        self._Rho02PiPiFilter()
        self._Rho02PiPiWSFilter()
        self._Rho02PiPiSBFilter()

        self._KshMajoranaSSMuSel=None
        self._KshMajoranaSSMuFilter()

        self._KshMajoranaOSMuSel=None
        self._KshMajoranaOSMuFilter()

        self._KSLLSel=None
        self._KsLLFilter()

        self.registerLine(self._K_line())
        self.registerLine(self._KSS_line())
        self.registerLine(self._Rho_line())
        self.registerLine(self._RhoWS_line())
        self.registerLine(self._RhoSB_line())
        self.registerLine(self._KshMajoranaSSMu_line())
        self.registerLine(self._KshMajoranaOSMu_line())
        self.registerLine(self._KstarLL_line())
        self.registerLine(self._KstarLLSS_line())
        
    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (PIDmu-PIDpi> %(MuonPIDmu)s )"\
               "& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "& (PIDmu-PIDK> %(MuonPIDK)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
    
    def _MajoranaLineMuSelection( self ):
        return "(P > %(KS0DaugP)s) & (PT > %(KS0DaugPT)s)"\
               "&(TRCHI2DOF < %(KS0DaugTrackChi2)s ) & (PIDmu-PIDpi> %(MuonPIDmu)s )& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "&(PIDmu-PIDK> %(MuonPIDK)s )&(MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s)"\
               
    def _NominalKSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s )& (PIDK-PIDp> %(KaonPIDp)s )& (PIDK-PIDmu> %(KaonPIDmu)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"
    
    def _NominalKsLLSelection( self ):
        return " (BPVVDZ < %(KsLLMaxDz)s * mm) &(BPVVD >%(KsLLCutFD)s*mm)& (MM>%(KSLLMassLow)s*MeV)&(MM<%(KSLLMassHigh)s*MeV)  & (BPVVDCHI2> %(KSLLCutFDChi2)s ) & (PT > %(KS0PT)s*MeV) & (VFASPF(VCHI2PDOF) < %(KS0VertexChi2)s) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),1) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),2) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),1) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),2) & (MIPCHI2DV(PRIMARY) > %(KS0MIPChi2)s)"
    
    
    ######Kaon Line######
    def _K_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KLine', prescale = 0.5,
                             FILTER=self.GECs,
                             algos = [ self._Bs2KMuNu()])
    
    def _KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KSSLine', prescale = 0.25,
                             FILTER=self.GECs,
                             algos = [ self._Bs2KMuNuSS()])
    ######Rho Line######

    def _Rho_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bu2RhoLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bu2RhoMuNu()])
    def _RhoWS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bu2RhoWSLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bu2RhoMuNuWS()])
    def _RhoSB_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bu2RhoSBLine', prescale = 0.5,
                             FILTER=self.GECs,
                             algos = [ self._Bu2RhoMuNuSB()])

    ######KshortMajoranaLine######
    def _KshMajoranaSSMu_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bu2KshSSMuLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bu2KshSSMuNu()])
    
    def _KshMajoranaOSMu_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bu2KshOSMuLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bu2KshOSMuNu()])

    ######K*LL Line######
    def _KstarLL_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KstarLLLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bs2KstarLLMuNu()])
    
    
    def _KstarLLSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KstarLLSSLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bs2KstarLLMuNuSS()])

    ######--######
    def _muonFilter( self ):
        if self._muonSel is not None:
            return self._muonSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseMuons
        _mu = FilterDesktop( Code = self._NominalMuSelection() % self._config )
        _muSel=Selection("Mu_for"+self._name,
                         Algorithm=_mu,
                         RequiredSelections = [StdLooseMuons])
        
        self._muonSel=_muSel
        
        return _muSel

    ######Kaon Filter######
    def _kaonFilter( self ):
        if self._kaonSel is not None:
            return self._kaonSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseKaons
        
        _ka = FilterDesktop( Code = self._NominalKSelection() % self._config )
        _kaSel=Selection("K_for"+self._name,
                         Algorithm=_ka,
                         RequiredSelections = [StdLooseKaons])
        
        self._kaonSel=_kaSel
        
        return _kaSel

    ######Majorana Muon Filter######
    def _MajoranaLineMuFilter( self ):
        if self._MajoranaLineMuSel is not None:
            return self._MajoranaLineMuSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseMuons
        _mumajorana = FilterDesktop( Code = self._MajoranaLineMuSelection() % self._config )
        _mumajoranaSel=Selection("MuMajorana_for"+self._name,
                                 Algorithm=_mumajorana,
                                 RequiredSelections = [StdLooseMuons])
        
        self._MajoranaLineMuSel=_mumajoranaSel
        
        return _mumajoranaSel
    
    
    #####Kshort Filter for K* Line######
    def _KsLLFilter( self ):
        if self._KSLLSel is not None:
            return self._KSLLSel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        
        _code =  FilterDesktop( Code = self._NominalKsLLSelection() % self._config )
        
        _KsLLSel=Selection("KSLL02PiPi_for"+self._name,
                           Algorithm= _code,
                           RequiredSelections = [self._stdLooseKsLL])
        
        self._KSLLSel = _KsLLSel
        
        return _KsLLSel

    #####Make the Rho######
    def _Rho02PiPiFilter( self ):
        if self._rho770Sel is not None:
            return self._rho770Sel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _rho770 = CombineParticles(
            DecayDescriptors = ["[rho(770)0 -> pi- pi+]cc"] ,
            DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV) &   (TRCHI2DOF < %(RhoChPionTRCHI2)s )"\
                               "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(RhoChPionPIDK)s )" % self._config
                               },
            CombinationCut = "(ADAMASS('rho(770)0')< %(RhoMassWindow)s)" % self._config,
            MotherCut       = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV )"\
            "& (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
            "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s ) & (BPVVDCHI2 > %(RhoFDCHI2)s)"\
            "& (BPVVD > %(RhoFD)s *mm)& (BPVDIRA> %(RhoDIRA)s) & (BPVIP()> %(RhoIPMin)s *mm)" %self._config
            )
        _rho770Sel=Selection("Rho02PiPi_for"+self._name,
                             Algorithm=_rho770,
                             RequiredSelections = [StdLoosePions])
        
        self._rho770Sel=_rho770Sel
        
        return _rho770Sel

    def _Rho02PiPiWSFilter( self ):
        if self._rho770WSSel is not None:
            return self._rho770WSSel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _rho770 = CombineParticles(
            DecayDescriptors = ["[rho(770)0 -> pi+ pi+]cc"] ,
            DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV) & (TRCHI2DOF < %(RhoChPionTRCHI2)s )"\
                               "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s )"\
                               "& (PIDpi-PIDK> %(RhoChPionPIDK)s )" % self._config
                               },
            CombinationCut = "(ADAMASS('rho(770)0')< %(RhoMassWindow)s)" % self._config,
            MotherCut      = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV )"\
            "& (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
            "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s ) & (BPVVDCHI2 > %(RhoFDCHI2)s)"\
            "& (BPVVD > %(RhoFD)s *mm) & (BPVDIRA> %(RhoDIRA)s) & (BPVIP()> %(RhoIPMin)s *mm)"%self._config
            )
        _rho770Sel=Selection("Rho02PiPiWS_for"+self._name,
                             Algorithm=_rho770,
                             RequiredSelections = [StdLoosePions])
        
        self._rho770WSSel=_rho770Sel
        return _rho770Sel
    
    def _Rho02PiPiSBFilter( self ):
        if self._rho770SBSel is not None:
            return self._rho770SBSel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _rho770SB = CombineParticles(
            DecayDescriptors = ["[rho(770)0 -> pi+ pi-]cc"] ,
            DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV) & (TRCHI2DOF < %(RhoChPionTRCHI2)s )"\
                               "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s )"\
                               "& (PIDpi-PIDK> %(RhoChPionPIDK)s )" % self._config
                               },
            CombinationCut = "((AM > %(RhoMassWindowMin1SB)s) & (AM < %(RhoMassWindowMax1SB)s))" \
            " | ((AM > %(RhoMassWindowMin2SB)s) & (AM < %(RhoMassWindowMax2SB)s))"

            % self._config,
            MotherCut       = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV )"\
            "& (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
            "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s ) & (BPVVDCHI2 > %(RhoFDCHI2)s)"\
            "& (BPVVD > %(RhoFD)s *mm) & (BPVDIRA> %(RhoDIRA)s) & (BPVIP()> %(RhoIPMin)s *mm)"%self._config
            )
        _rho770SelSB=Selection("Rho02PiPiSB_for"+self._name,
                               Algorithm=_rho770SB,
                               RequiredSelections = [StdLoosePions])
        
        self._rho770SBSel=_rho770SelSB
        return _rho770SelSB

    ######Make the Kshort->MuPi Same Sign for Majorana####
    def _KshMajoranaSSMuFilter( self ):
        if self._KshMajoranaSSMuSel is not None:
            return self._KshMajoranaSSMuSel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions, StdLooseMuons
     
        _Ksh = CombineParticles(
            DecayDescriptor = "KS0 -> mu- pi+",
            DaughtersCuts   = {"pi+":"(P > %(KS0DaugP)s)& (PT > %(KS0DaugPT)s)"\
                               "& (TRCHI2DOF < %(KS0DaugTrackChi2)s)" \
                               "& (MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s)"   % self._config
                               },
            CombinationCut  = "(ADOCACHI2CUT(25, ''))"% self._config,
            
            MotherCut       = "(BPVVDCHI2> %(KSMajoranaCutFDChi2)s )& (VFASPF(VCHI2/VDOF) < %(KS0VertexChi2)s)&(PT > %(KS0PT)s*MeV)" % self._config 
            )
        
        #_Ksh.ReFitPVs = True        
        _KshMajoranaSSMuSel=Selection("KshMajoranaSSMu_for"+self._name,
                                      Algorithm=_Ksh,
                                      RequiredSelections = [StdLoosePions, self._MajoranaLineMuFilter()])
        self._KshMajoranaSSMuSel=_KshMajoranaSSMuSel
        
        return _KshMajoranaSSMuSel
    
    ######Make the Kshort->MuPi Opposite Sign for Majorana####
    def _KshMajoranaOSMuFilter( self ):
        if self._KshMajoranaOSMuSel is not None:
            return self._KshMajoranaOSMuSel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions, StdLooseMuons
        
        _Ksh = CombineParticles(
            DecayDescriptor = "KS0 -> mu+ pi-",
            DaughtersCuts   = {"pi-":"(P > %(KS0DaugP)s)& (PT > %(KS0DaugPT)s)"\
                               "&(TRCHI2DOF < %(KS0DaugTrackChi2)s)"\
                               "&(MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s)"   % self._config
                               },
            CombinationCut  = "(ADOCACHI2CUT(25, ''))"% self._config,
            
            MotherCut       = "(BPVVDCHI2> %(KSMajoranaCutFDChi2)s )& (VFASPF(VCHI2/VDOF) < %(KS0VertexChi2)s)&(PT > %(KS0PT)s*MeV)" % self._config 
            )
        # _Ksh.ReFitPVs = True
        _KshMajoranaOSMuSel=Selection("KshMajoranaOSMu_for"+self._name,
                                      Algorithm=_Ksh,
                                      RequiredSelections = [StdLoosePions, self._MajoranaLineMuFilter()])
        self._KshMajoranaOSMuSel=_KshMajoranaOSMuSel
        
        return _KshMajoranaOSMuSel
    

    ######Bs->KMuNu######
    def _Bs2KMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KMu = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> K+ mu-]cc"],
            CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "( ((((5366.3*5366.3) -(MASS(1,2))*(MASS(1,2))))/(2*5366.3)) < %(EnuK)s*MeV)"\
            "& (VFASPF(VCHI2/VDOF)< %(BVCHI2DOFTight)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2HIGH)s)" % self._config,
            ReFitPVs = True
            )
        
        _KMuSel=Selection("KMu_for"+self._name,
                         Algorithm=_KMu,
                         RequiredSelections = [self._muonFilter(), self._kaonFilter()])
        return _KMuSel
    
    def _Bs2KMuNuSS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KMu = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> K- mu-]cc"],
            CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "( ((((5366.3*5366.3) -(MASS(1,2))*(MASS(1,2))))/(2*5366.3)) < %(EnuK)s*MeV)"\
            "& (VFASPF(VCHI2/VDOF)< %(BVCHI2DOFTight)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2HIGH)s)" % self._config,
            ReFitPVs = True
            )
        _KMuSel=Selection("KMuSS_for"+self._name,
                          Algorithm=_KMu,
                          RequiredSelections = [self._muonFilter(), self._kaonFilter()])
        return _KMuSel
    
    ######Bu->RhoMuNu ######
    def _Bu2RhoMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _RhoMu = CombineParticles(
            DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"],
            CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = " ( ((((5279.17*5279.17) -(MASS(1,2))*(MASS(1,2))))/(2*5279.17)) < %(Enu)s*MeV)"\
            "& (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"% self._config,

            ReFitPVs = True
            )
        
        _RhoMuSel=Selection("RhoMu_for"+self._name,
                         Algorithm=_RhoMu,
                         RequiredSelections = [self._muonFilter(), self._Rho02PiPiFilter()])
        return _RhoMuSel

    def _Bu2RhoMuNuWS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _RhoMu = CombineParticles(
            DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"],
            CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = " ( ((((5279.17*5279.17) -(MASS(1,2))*(MASS(1,2))))/(2*5279.17)) < %(Enu)s*MeV)"\
            "& (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"% self._config,

            ReFitPVs = True
            )
        
        _RhoMuSel=Selection("RhoMuWS_for"+self._name,
                         Algorithm=_RhoMu,
                         RequiredSelections = [self._muonFilter(), self._Rho02PiPiWSFilter()])
        return _RhoMuSel

    
    def _Bu2RhoMuNuSB( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _RhoMu = CombineParticles(
            DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"],
            CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = " ( ((((5279.17*5279.17) -(MASS(1,2))*(MASS(1,2))))/(2*5279.17)) < %(Enu)s*MeV)"\
            "& (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"% self._config,

            ReFitPVs = True
            )
        
        _RhoMuSel=Selection("RhoMuSB_for"+self._name,
                         Algorithm=_RhoMu,
                         RequiredSelections = [self._muonFilter(), self._Rho02PiPiSBFilter()])
        return _RhoMuSel


    ######Bu->KshMuNu SS & OS######
    def _Bu2KshSSMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KshSSMu = CombineParticles(
            DecayDescriptors = ["[B- -> KS0 mu-]cc"],
            CombinationCut = "(AM>%(KshMuMassLowTight)s*MeV) & (AM<%(XMuMassUpperHigh)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (MINTREE( (ABSID=='KS0') , VFASPF(VZ))-VFASPF(VZ) > %(KshZ)s *mm )" % self._config,
            ReFitPVs = True
            )
        
        _KshSSMuSel=Selection("KshSSMu_for"+self._name,
                              Algorithm=_KshSSMu,
                              RequiredSelections = [self._muonFilter(), self._KshMajoranaSSMuFilter()])
        return _KshSSMuSel
    
    
    def _Bu2KshOSMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KshOSMu = CombineParticles(
            DecayDescriptors = ["[B- -> KS0 mu-]cc"],
            CombinationCut = "(AM>%(KshMuMassLowTight)s*MeV) & (AM<%(XMuMassUpperHigh)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (MINTREE( (ABSID=='KS0') , VFASPF(VZ))-VFASPF(VZ) > %(KshZ)s *mm )" % self._config,
            ReFitPVs = True
            )
        
        _KshOSMuSel=Selection("KshOSMu_for"+self._name,
                              Algorithm=_KshOSMu,
                              RequiredSelections = [self._muonFilter(), self._KshMajoranaOSMuFilter()])
        return _KshOSMuSel

    ###Combine Bs->K*(892)+MuNu, K*->KS0Pi###
    def _Bs2KstarLLMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        
        _KstarLLMu = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> KS0 pi+ mu-]cc"],
            DaughtersCuts   = {"pi+":"(PT> %(KstarChPionPT)s*MeV) &(P> %(KstarChPionP)s*MeV)"\
                               "&  (TRCHI2DOF < %(KstarChPionTRCHI2)s )& (MIPCHI2DV(PRIMARY)>%(KstarChPionMINIPCHI2)s)"\
                               "& (PIDpi-PIDK>%(KstarChPionPIDK)s)" % self._config
                               },
            CombinationCut = "(AM>%(KstarMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "( ((((5366.3*5366.3) -(MASS(1,2,3))*(MASS(1,2,3))))/(2*5366.3)) < %(Enu)s*MeV)"\
            "& (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)" % self._config,
            ReFitPVs = True
            )
        
        _KstarLLMuSel=Selection("KstarLLMu_for"+self._name,
                                Algorithm=_KstarLLMu,
                                RequiredSelections = [self._muonFilter(), StdLoosePions, self._KsLLFilter()])
        
        return _KstarLLMuSel
    
    def _Bs2KstarLLMuNuSS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _KstarLLMuSS = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> KS0 pi- mu-]cc"],
            DaughtersCuts   = {"pi-":"(PT> %(KstarChPionPT)s*MeV)  &(P> %(KstarChPionP)s*MeV)"\
                               "& (TRCHI2DOF < %(KstarChPionTRCHI2)s) & (MIPCHI2DV(PRIMARY)>%(KstarChPionMINIPCHI2)s)"\
                               "& (PIDpi-PIDK>%(KstarChPionPIDK)s)" % self._config
                               },
            CombinationCut = "(AM>%(KstarMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "( ((((5366.3*5366.3) -(MASS(1,2,3))*(MASS(1,2,3))))/(2*5366.3)) < %(Enu)s*MeV)"\
            "& (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)" % self._config,
            ReFitPVs = True
            )
        
        _KstarLLMuSSSel=Selection("KstarLLMuSS_for"+self._name,
                                  Algorithm=_KstarLLMuSS,
                                  RequiredSelections = [self._muonFilter(), StdLoosePions, self._KsLLFilter()])
        
        return _KstarLLMuSSSel
    
