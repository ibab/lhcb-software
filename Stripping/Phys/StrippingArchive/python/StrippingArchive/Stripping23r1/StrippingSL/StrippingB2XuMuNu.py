
__author__ = ['Phillip Urquijo, Alessandra Borgia']
__date__ = '08/05/2010'
__version__ = '$Revision: 1.6 $'

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
default_config = {
    'B2XuMuNu' : {
        'WGs'         : ['Semileptonic'],
	'BUILDERTYPE' : 'B2XuMuNuBuilder',
        'CONFIG'      :  {
	"GEC_nLongTrk"        : 250.  , #adimensional
	"TRGHOSTPROB"         : 0.5    ,#adimensional
	#Muons
	"MuonGHOSTPROB"       : 0.35  ,#adimensional
	"MuonTRCHI2"          : 4.    ,#adimensional
	"MuonP"               : 3000. ,#MeV
	"MuonPTight"          : 6000. ,#MeV
	"MuonPT"              : 1000. ,#MeV
	"MuonPTTight"         : 1500. ,#MeV
	"MuonPIDK"            : 0.    ,#adimensional
	"MuonPIDmu"           : 3.    ,#adimensional
	"MuonPIDp"            : 0.    ,#adimensional
	"MuonMINIPCHI2"       : 12    ,#adminensional
	#Xu
	#K channel
	"KaonTRCHI2"          : 6.     ,#adimensional
	"KaonP"               : 3000.  ,#MeV
	"KaonPTight"          : 10000. ,#MeV
	"KaonPT"              : 800.   ,#MeV
	"KaonPIDK"            : 5.     ,#adimensional 
	"KaonPIDmu"           : 5.     ,#adimensional
	"KaonPIDp"            : 5.     ,#adimensional
	"KaonPIDK_phi"        : 0.     ,#adimensional 
	"KaonPIDmu_phi"       : -2.    ,#adimensional
	"KaonPIDp_phi"        : -2.    ,#adimensional
	"KaonMINIPCHI2"       : 16     ,#adminensional


	"PhiMassWindow"       : 20.  ,#MeV
	"PhiVCHI2DOF"         : 6     ,#adimensional
	"PhiPT"               : 500.  ,#MeV
	"PhiMINIPCHI2"        : 4     ,#adimensional
	"PhiDIRA"             : 0.9   ,#adimensional

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
				       #    "RhoFDCHI2"           : 100.  ,#adimensional
				       #    "RhoFD"               : 6     ,#mm
				       #    "RhoIPMin"            : 0.3   ,#mm    
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
	"BDIRAMed"            : 0.994 ,#adminensional
	"BDIRATight"          : 0.999 ,#adminensional
	"BFDCHI2HIGH"         : 100.  ,#adimensional
	"BFDCHI2Tight"        : 120.  ,#adimensional
	"BFDCHI2ForPhi"       : 50.   ,#adimensional
	#B Mass Minima
	"KMuMassLowTight"     : 1500. ,#MeV
	"PhiMu_MCORR"         : 2500. ,#MeV
	"RhoMuMassLowTight"   : 2000. ,#MeV
	"KshMuMassLowTight"   : 3000. ,#MeV
	"KstarMuMassLowTight" : 2500. ,#MeV
	#
	"XMuMassUpper"        : 5500. ,#MeV
	"XMuMassUpperHigh"    : 6500. ,#MeV
	"Enu"                 : 1850. ,#MeV
	"EnuK"                : 2000. ,#MeV
	'KshZ'                : 0.    ,#mm
	'PiMuNu_prescale'     : 0.1 
	  },
      'STREAMS'     : ['Semileptonic']	  
      }
    }
from Gaudi.Configuration import *
from StrippingUtils.Utils import LineBuilder

import logging
def makeTOSFilter(name,specs):
    from Configurables import TisTosParticleTagger
    tosFilter = TisTosParticleTagger(name+'TOSFilter')
    tosFilter.TisTosSpecs = specs
    tosFilter.ProjectTracksToCalo = False
    tosFilter.CaloClustForCharged = False
    tosFilter.CaloClustForNeutral = False
    tosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tosFilter

def tosSelection(sel,specs):
    from PhysSelPython.Wrappers import Selection 
    '''Filters Selection sel to be TOS.'''
    tosFilter = makeTOSFilter(sel.name(),specs)
    return Selection(sel.name()+'TOS', Algorithm=tosFilter,
                     RequiredSelections=[sel])

default_name="B2XuMuNu"

class B2XuMuNuBuilder(LineBuilder):
    """
    Definition of B->Xu mu nu (Xu=rho,k,k*+) stripping
    and where B ->KS0 mu nu (KS0->MuPi) stripping
    """
    
    __configuration_keys__ = [
        "GEC_nLongTrk"
        ,"MuonGHOSTPROB"
        ,"TRGHOSTPROB"          
        ,"MuonTRCHI2"          
        ,"MuonP"
        ,"MuonPTight"               
        ,"MuonPT"
        ,"MuonPTTight"              
        ,"MuonPIDK"            
        ,"MuonPIDmu"           
        ,"MuonPIDp"            
        ,"MuonMINIPCHI2"       
        ,"KaonTRCHI2"          
        ,"KaonP"               
        ,"KaonPTight"               
        ,"KaonPT"              
        ,"KaonPIDK"             
        ,"KaonPIDmu"           
        ,"KaonPIDp"            
        ,"KaonPIDK_phi"
        ,"KaonPIDmu_phi"
        ,"KaonPIDp_phi"
        ,"KaonMINIPCHI2"
        ,"PhiMassWindow"       
        ,"PhiVCHI2DOF"         
        ,"PhiPT"               
        ,"PhiMINIPCHI2"               
        ,"PhiDIRA"               
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
        #        ,"RhoFDCHI2"           
        #,"RhoFD"               
        # ,"RhoIPMin"            
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
        ,"BDIRAMed"
        ,"BDIRATight"               
        ,"BFDCHI2HIGH"
        ,"BFDCHI2Tight"         
        ,"BFDCHI2ForPhi"         
        ,"KMuMassLowTight"     
        ,"PhiMu_MCORR"     
        ,"RhoMuMassLowTight"   
        ,"KshMuMassLowTight"   
        ,"KstarMuMassLowTight" 
        ,"XMuMassUpper"        
        ,"XMuMassUpperHigh"
        ,"Enu"                 
        ,"EnuK"                
        ,'KshZ'                
        ,'PiMuNu_prescale'                
        ]

    def __init__(self,name,config):
        LineBuilder.__init__(self, name, config)
        self.config=config
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        self._stdLooseKsLL = DataOnDemand("Phys/StdLooseKsLL/Particles")

        self.GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                      "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        self._muonSel=None
        self._muonFilter()

        self._muonSelTOS=None
        self._muonFilterTOS()

        self._muonSelTight=None
        self._muonFilterTight()

        self._MajoranaLineMuSel=None
        self._MajoranaLineMuFilter()

        self._pionSel=None
        self._kaonSel=None
        self._kaonForPhiSel=None
        self._kaonFilter()
        self._pionFilter()
        self._kaonForPhiFilter()
        
        self._rho770Sel=None
        self._rho770WSSel=None
        self._rho770SBSel=None

        self._phi1020Sel=None
        
        self._Rho02PiPiFilter()
        self._Rho02PiPiWSFilter()
        self._Rho02PiPiSBFilter()

        self._KshMajoranaSSMuSel=None
        self._KshMajoranaSSMuFilter()

        self._KshMajoranaOSMuSel=None
        self._KshMajoranaOSMuFilter()

        self._KSLLSel=None
        self._KsLLFilter()

        self.registerLine(self._Pi_line())
        self.registerLine(self._PiSS_line())
        self.registerLine(self._K_line())
        self.registerLine(self._KSS_line())
        self.registerLine(self._Phi_line())
        self.registerLine(self._Rho_line())
        self.registerLine(self._RhoWS_line())
        self.registerLine(self._RhoSB_line())
        self.registerLine(self._KshMajoranaSSMu_line())
        self.registerLine(self._KshMajoranaOSMu_line())
        self.registerLine(self._KstarLL_line())
        self.registerLine(self._KstarLLSS_line())
        
    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
               "& (PIDmu-PIDpi> %(MuonPIDmu)s )"\
               "& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "& (PIDmu-PIDK> %(MuonPIDK)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"

    def _NominalMuSelectionTight( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonPTight)s *MeV) &  (PT> %(MuonPTTight)s* MeV)"\
               "& (TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
               "& (PIDmu-PIDpi> %(MuonPIDmu)s )"\
               "& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "& (PIDmu-PIDK> %(MuonPIDK)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
   
    
    def _MajoranaLineMuSelection( self ):
        return "(P > %(KS0DaugP)s) & (PT > %(KS0DaugPT)s)"\
               "& (TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
               "&(TRCHI2DOF < %(KS0DaugTrackChi2)s ) & (PIDmu-PIDpi> %(MuonPIDmu)s )& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "&(PIDmu-PIDK> %(MuonPIDK)s )&(MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s)"\
               
    def _NominalKSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonPTight)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s )& (PIDK-PIDp> %(KaonPIDp)s )& (PIDK-PIDmu> %(KaonPIDmu)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def _NominalPiSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
               "& (PIDK-PIDpi< -%(KaonPIDK)s )& (PIDp < -%(KaonPIDp)s )& (PIDmu < -%(KaonPIDmu)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def _KforPhiSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
               "& (PIDK-PIDpi> %(KaonPIDK_phi)s )& (PIDK-PIDp> %(KaonPIDp_phi)s )& (PIDK-PIDmu> %(KaonPIDmu_phi)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"
    
    
    def _NominalKsLLSelection( self ):
        return " (BPVVDZ < %(KsLLMaxDz)s * mm) &(BPVVD >%(KsLLCutFD)s*mm)& (MM>%(KSLLMassLow)s*MeV)&(MM<%(KSLLMassHigh)s*MeV)  & (BPVVDCHI2> %(KSLLCutFDChi2)s ) & (PT > %(KS0PT)s*MeV) & (VFASPF(VCHI2PDOF) < %(KS0VertexChi2)s) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),1) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),2) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),1) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),2) & (MIPCHI2DV(PRIMARY) > %(KS0MIPChi2)s)"
    
    
    ######Kaon Line######
    def _Pi_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'B2PiLine', prescale = self.config["PiMuNu_prescale"],
                             FILTER=self.GECs,
                             algos = [ self._B2PiMuNu()])
   
    def _PiSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'B2PiSSLine', prescale = self.config["PiMuNu_prescale"],
                             FILTER=self.GECs,
                             algos = [ self._B2PiMuNuSS()])

    def _K_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bs2KMuNu()])

    def _Phi_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'B2PhiLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._B2PhiMuNu()])
    
    def _KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KSSLine', prescale = 0.5,
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
        return StrippingLine(self._name+'Bu2RhoSBLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bu2RhoMuNuSB()])

    ######KshortMajoranaLine######
    def _KshMajoranaSSMu_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bu2KshSSMu_SSMuminusLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bu2KshSSMuNu_SSMuminus()])
    
    def _KshMajoranaOSMu_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bu2KshOSMu_SSMuplusLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bu2KshOSMuNu_SSMuplus()])

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
    
    ######--######
    def _muonFilterTight( self ):
        if self._muonSelTight is not None:
            return self._muonSelTight
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseMuons
        _muTight = FilterDesktop( Code = self._NominalMuSelectionTight() % self._config )
        _muTightSel=Selection("MuTightCuts_for"+self._name,
                         Algorithm=_muTight,
                         RequiredSelections = [StdLooseMuons])
        
        self._muonSelTight=_muTightSel
        
        return _muTightSel

    ######--######
    def _muonFilterTOS( self ):
        if self._muonSelTOS is not None:
            return self._muonSelTOS
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseMuons
        _muTOS = FilterDesktop( Code = self._NominalMuSelectionTight() % self._config )
        _muSelTOS=Selection("MuL0TOS_for"+self._name,
                            Algorithm=_muTOS,
                         RequiredSelections = [StdLooseMuons])
        _muSelTOS = tosSelection(_muSelTOS,{'L0.*Muon.*Decision%TOS':0})

        self._muonSelTOS=_muSelTOS
        
        return _muSelTOS

    def _pionFilter( self ):
        if self._pionSel is not None:
            return self._pionSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _ka = FilterDesktop( Code = self._NominalPiSelection() % self._config )
        _kaSel=Selection("Pi_for"+self._name,
                         Algorithm=_ka,
                         RequiredSelections = [StdLoosePions])
        self._pionSel=_kaSel
        return _kaSel

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


    def _kaonForPhiFilter( self ):
        if self._kaonForPhiSel is not None:
            return self._kaonForPhiSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseKaons
        
        _ka = FilterDesktop( Code = self._KforPhiSelection() % self._config )
        _kaSel=Selection("K_Phi"+self._name,
                         Algorithm=_ka,
                         RequiredSelections = [StdLooseKaons])
        self._kaonForPhiSel=_kaSel
        return _kaSel

    ######Majorana Muon Filter######

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
    def _Phi2KKFilter( self ):
        if self._phi1020Sel is not None:
            return self._phi1020Sel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _phi1020 = CombineParticles(
            DecayDescriptors = ["phi(1020) -> K- K+"] ,
            CombinationCut = "(ADAMASS('phi(1020)')< %(PhiMassWindow)s)" % self._config,
            MotherCut       = "(VFASPF(VCHI2/VDOF) < %(PhiVCHI2DOF)s ) & (PT > %(PhiPT)s *MeV) "\
            "& (MIPCHI2DV(PRIMARY)> %(PhiMINIPCHI2)s ) & (BPVDIRA> %(PhiDIRA)s)" %self._config
            )
        _phi1020Sel=Selection("PhiKK_for"+self._name,
                             Algorithm=_phi1020,
                             RequiredSelections = [self._kaonForPhiFilter()])
        
        self._phi1020Sel=_phi1020Sel
        
        return _phi1020Sel

    def _Rho02PiPiFilter( self ):
        if self._rho770Sel is not None:
            return self._rho770Sel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _rho770 = CombineParticles(
            DecayDescriptors = ["rho(770)0 -> pi- pi+"] ,
            DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV) &   (TRCHI2DOF < %(RhoChPionTRCHI2)s )"\
                               "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(RhoChPionPIDK)s ) & (TRGHOSTPROB < %(TRGHOSTPROB)s)" % self._config
                               },
            CombinationCut = "(ADAMASS('rho(770)0')< %(RhoMassWindow)s)" % self._config,
            MotherCut       = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV )"\
            "& (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
            "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s ) & (BPVDIRA> %(RhoDIRA)s)" %self._config
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
                               "& (PIDpi-PIDK> %(RhoChPionPIDK)s ) & (TRGHOSTPROB < %(TRGHOSTPROB)s)" % self._config
                               },
            CombinationCut = "(ADAMASS('rho(770)0')< %(RhoMassWindow)s)" % self._config,
            MotherCut      = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV )"\
            "& (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
            "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s ) & (BPVDIRA> %(RhoDIRA)s)"%self._config
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
            DecayDescriptors = ["rho(770)0 -> pi+ pi-"] ,
            DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV) & (TRCHI2DOF < %(RhoChPionTRCHI2)s )"\
                               "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(RhoChPionPIDK)s ) & (TRGHOSTPROB < %(TRGHOSTPROB)s)" % self._config
                               },
            CombinationCut = "((AM > %(RhoMassWindowMin1SB)s) & (AM < %(RhoMassWindowMax1SB)s))"\
            " | ((AM > %(RhoMassWindowMin2SB)s) & (AM < %(RhoMassWindowMax2SB)s))" % self._config,
            
            MotherCut       = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV )"\
            "& (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
            "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s ) & (BPVDIRA> %(RhoDIRA)s)"%self._config
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
    

    def _B2PiMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KMu = CombineParticles(
            DecayDescriptors = ["[B0 -> pi+ mu-]cc"],
            CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOFTight)s) & (BPVDIRA> %(BDIRATight)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2Tight)s)  & (ratio > 0.4)"
            % self._config,
            ReFitPVs = True
            )
        _KMu.Preambulo = [ "from LoKiPhys.decorators import *",
                           "dx = (VFASPF(VX)-BPV(VX))",
                           "dy = (VFASPF(VY)-BPV(VY))",
                           "dz = (VFASPF(VZ)-BPV(VZ))",
                           "norm = (max(sqrt(dx*dx+dy*dy+dz*dz),0))",
                           "B_xdir  = ((dx)/norm)",
                           "B_ydir  = ((dy)/norm)",
                           "B_zdir  = ((dz)/norm)",
                           "Pxkmu   = (CHILD(PX,1)+CHILD(PX,2))",
                           "Pykmu   = (CHILD(PY,1)+CHILD(PY,2))",
                           "Pzkmu   = (CHILD(PZ,1)+CHILD(PZ,2))",
                           "Bflight = (B_xdir*Pxkmu + B_ydir*Pykmu+ B_zdir*Pzkmu)",
                           "mB  = 5366.77",
                           "M_2 = (mB*mB + M12*M12)",
                           "kmu_E  = (CHILD(E,1)+CHILD(E,2))",
                           "quada = (Bflight*Bflight - kmu_E*kmu_E)",
                           "quadb = (M_2*Bflight)",
                           "quadc = (((M_2*M_2)/4) - (kmu_E*kmu_E*mB*mB))",
                           "Discriminant = ((quadb*quadb)-(4*quada*quadc))",
                           "solution_a = ((-quadb + sqrt(max(Discriminant,0)))/(2*quada))",
                           "solution_b = ((-quadb - sqrt(max(Discriminant,0)))/(2*quada))",
                           "ratio = (solution_a/solution_b)"
                           ]
        
        _KMuSel=Selection("PiMu_for"+self._name,
                         Algorithm=_KMu,
                         RequiredSelections = [self._muonFilterTOS(), self._pionFilter()])
        _KMuSel = tosSelection(_KMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _KMuSel
    
    def _B2PiMuNuSS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KMuSS = CombineParticles(
            DecayDescriptors = ["[B0 -> pi- mu-]cc"],
            CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = " (VFASPF(VCHI2/VDOF)< %(BVCHI2DOFTight)s) & (BPVDIRA> %(BDIRATight)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2Tight)s)  & (ratio > 0.4)" % self._config,
            ReFitPVs = True
            )
        _KMuSS.Preambulo = [ "from LoKiPhys.decorators import *",
                           "dx = (VFASPF(VX)-BPV(VX))",
                           "dy = (VFASPF(VY)-BPV(VY))",
                           "dz = (VFASPF(VZ)-BPV(VZ))",
                           "norm = (max(sqrt(dx*dx+dy*dy+dz*dz),0))",
                           "B_xdir  = ((dx)/norm)",
                           "B_ydir  = ((dy)/norm)",
                           "B_zdir  = ((dz)/norm)",
                           "Pxkmu   = (CHILD(PX,1)+CHILD(PX,2))",
                           "Pykmu   = (CHILD(PY,1)+CHILD(PY,2))",
                           "Pzkmu   = (CHILD(PZ,1)+CHILD(PZ,2))",
                           "Bflight = (B_xdir*Pxkmu + B_ydir*Pykmu+ B_zdir*Pzkmu)",
                           "mB  = 5366.77",
                           "M_2 = (mB*mB + M12*M12)",
                           "kmu_E  = (CHILD(E,1)+CHILD(E,2))",
                           "quada = (Bflight*Bflight - kmu_E*kmu_E)",
                           "quadb = (M_2*Bflight)",
                           "quadc = (((M_2*M_2)/4) - (kmu_E*kmu_E*mB*mB))",
                           "Discriminant = ((quadb*quadb)-(4*quada*quadc))",
                           "solution_a = ((-quadb + sqrt(max(Discriminant,0)))/(2*quada))",
                           "solution_b = ((-quadb - sqrt(max(Discriminant,0)))/(2*quada))",
                           "ratio = (solution_a/solution_b)"
                           ]
        _KMuSelSS=Selection("PiMuSS_for"+self._name,
                          Algorithm=_KMuSS,
                          RequiredSelections = [self._muonFilterTOS(), self._pionFilter()])
        _KMuSelSS = tosSelection(_KMuSelSS,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _KMuSelSS


    ######Bs->KMuNu######
    def _Bs2KMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KMu = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> K+ mu-]cc"],
            CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRAMed)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2Tight)s)"
            % self._config,
            ReFitPVs = True
            )
        _KMu.Preambulo = [ "from LoKiPhys.decorators import *",
                           "dx = (VFASPF(VX)-BPV(VX))",
                           "dy = (VFASPF(VY)-BPV(VY))",
                           "dz = (VFASPF(VZ)-BPV(VZ))",
                           "norm = (max(sqrt(dx*dx+dy*dy+dz*dz),0))",
                           "B_xdir  = ((dx)/norm)",
                           "B_ydir  = ((dy)/norm)",
                           "B_zdir  = ((dz)/norm)",
                           "Pxkmu   = (CHILD(PX,1)+CHILD(PX,2))",
                           "Pykmu   = (CHILD(PY,1)+CHILD(PY,2))",
                           "Pzkmu   = (CHILD(PZ,1)+CHILD(PZ,2))",
                           "Bflight = (B_xdir*Pxkmu + B_ydir*Pykmu+ B_zdir*Pzkmu)",
                           "mB  = 5366.77",
                           "M_2 = (mB*mB + M12*M12)",
                           "kmu_E  = (CHILD(E,1)+CHILD(E,2))",
                           "quada = (Bflight*Bflight - kmu_E*kmu_E)",
                           "quadb = (M_2*Bflight)",
                           "quadc = (((M_2*M_2)/4) - (kmu_E*kmu_E*mB*mB))",
                           "Discriminant = ((quadb*quadb)-(4*quada*quadc))",
                           "solution_a = ((-quadb + sqrt(max(Discriminant,0)))/(2*quada))",
                           "solution_b = ((-quadb - sqrt(max(Discriminant,0)))/(2*quada))",
                           "ratio = (solution_a/solution_b)"
                           ]
        
        _KMuSel=Selection("KMu_for"+self._name,
                         Algorithm=_KMu,
                         RequiredSelections = [self._muonFilterTOS(), self._kaonFilter()])
        _KMuSel = tosSelection(_KMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _KMuSel
    
    def _Bs2KMuNuSS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KMuSS = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> K- mu-]cc"],
            CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRAMed)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2Tight)s)"
            % self._config,
            ReFitPVs = True
            )
        _KMuSS.Preambulo = [ "from LoKiPhys.decorators import *",
                           "dx = (VFASPF(VX)-BPV(VX))",
                           "dy = (VFASPF(VY)-BPV(VY))",
                           "dz = (VFASPF(VZ)-BPV(VZ))",
                           "norm = (max(sqrt(dx*dx+dy*dy+dz*dz),0))",
                           "B_xdir  = ((dx)/norm)",
                           "B_ydir  = ((dy)/norm)",
                           "B_zdir  = ((dz)/norm)",
                           "Pxkmu   = (CHILD(PX,1)+CHILD(PX,2))",
                           "Pykmu   = (CHILD(PY,1)+CHILD(PY,2))",
                           "Pzkmu   = (CHILD(PZ,1)+CHILD(PZ,2))",
                           "Bflight = (B_xdir*Pxkmu + B_ydir*Pykmu+ B_zdir*Pzkmu)",
                           "mB  = 5366.77",
                           "M_2 = (mB*mB + M12*M12)",
                           "kmu_E  = (CHILD(E,1)+CHILD(E,2))",
                           "quada = (Bflight*Bflight - kmu_E*kmu_E)",
                           "quadb = (M_2*Bflight)",
                           "quadc = (((M_2*M_2)/4) - (kmu_E*kmu_E*mB*mB))",
                           "Discriminant = ((quadb*quadb)-(4*quada*quadc))",
                           "solution_a = ((-quadb + sqrt(max(Discriminant,0)))/(2*quada))",
                           "solution_b = ((-quadb - sqrt(max(Discriminant,0)))/(2*quada))",
                           "ratio = (solution_a/solution_b)"
                           ]
        _KMuSelSS=Selection("KMuSS_for"+self._name,
                          Algorithm=_KMuSS,
                          RequiredSelections = [self._muonFilterTOS(), self._kaonFilter()])
        _KMuSelSS = tosSelection(_KMuSelSS,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _KMuSelSS

    def _B2PhiMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _PhiMu = CombineParticles(
            DecayDescriptors = ["[B+ -> phi(1020) mu+]cc"],
            CombinationCut = "(AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(BPVCORRM>%(PhiMu_MCORR)s*MeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOFTight)s) & (BPVDIRA> %(BDIRATight)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2ForPhi)s)"
            % self._config,
            ReFitPVs = True
            )
        _PhiMuSel=Selection("PhiMu_for"+self._name,
                         Algorithm=_PhiMu,
                         RequiredSelections = [self._muonFilterTOS(), self._Phi2KKFilter()])
            # _KMuSel = tosSelection(_KMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        _PhiMuSel = tosSelection(_PhiMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*TopoMu3Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _PhiMuSel
    
    ######Bu->RhoMuNu ######
    def _Bu2RhoMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _RhoMu = CombineParticles(
            DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"],
            CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOFTight)s) & (BPVDIRA> %(BDIRATight)s) & (BPVVDCHI2 >%(BFDCHI2Tight)s) & (ratio >0.4)"% self._config,
            #"( ((((5279.17*5279.17) -(MASS(1,2))*(MASS(1,2))))/(2*5279.17)) < %(Enu)s*MeV)"\
            ReFitPVs = True
            )

        _RhoMu.Preambulo = [ "from LoKiPhys.decorators import *",
                           "dx = (VFASPF(VX)-BPV(VX))",
                           "dy = (VFASPF(VY)-BPV(VY))",
                           "dz = (VFASPF(VZ)-BPV(VZ))",
                           "norm = (max(sqrt(dx*dx+dy*dy+dz*dz),0))",
                           "B_xdir  = ((dx)/norm)",
                           "B_ydir  = ((dy)/norm)",
                           "B_zdir  = ((dz)/norm)",
                           "Pxrhomu   = (CHILD(PX,1)+CHILD(PX,2))",
                           "Pyrhomu   = (CHILD(PY,1)+CHILD(PY,2))",
                           "Pzrhomu   = (CHILD(PZ,1)+CHILD(PZ,2))",
                           "Bflight = (B_xdir*Pxrhomu + B_ydir*Pyrhomu+ B_zdir*Pzrhomu)",
                           "mB  = 5279.25",
                           "M_2 = (mB*mB + M12*M12)",
                           "rhomu_E  = (CHILD(E,1)+CHILD(E,2))",
                           "quada = (Bflight*Bflight - rhomu_E*rhomu_E)",
                           "quadb = (M_2*Bflight)",
                           "quadc = (((M_2*M_2)/4) - (rhomu_E*rhomu_E*mB*mB))",
                           "Discriminant = ((quadb*quadb)-(4*quada*quadc))",
                           "solution_a = ((-quadb + sqrt(max(Discriminant,0)))/(2*quada))",
                           "solution_b = ((-quadb - sqrt(max(Discriminant,0)))/(2*quada))",
                           "ratio = (solution_a/solution_b)"
                           ]
         
        _RhoMuSel=Selection("RhoMu_for"+self._name,
                         Algorithm=_RhoMu,
                         RequiredSelections = [self._muonFilterTight(), self._Rho02PiPiFilter()])
        _RhoMuSel = tosSelection(_RhoMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*TopoMu3Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _RhoMuSel

    def _Bu2RhoMuNuWS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _RhoMu = CombineParticles(
            DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"],
            CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOFTight)s) & (BPVDIRA> %(BDIRATight)s) & (BPVVDCHI2 >%(BFDCHI2Tight)s) & (ratio >0.4)"% self._config,
            #" ( ((((5279.17*5279.17) -(MASS(1,2))*(MASS(1,2))))/(2*5279.17)) < %(Enu)s*MeV)"\
            
            ReFitPVs = True
            )
        
        _RhoMu.Preambulo = [ "from LoKiPhys.decorators import *",
                           "dx = (VFASPF(VX)-BPV(VX))",
                           "dy = (VFASPF(VY)-BPV(VY))",
                           "dz = (VFASPF(VZ)-BPV(VZ))",
                           "norm = (max(sqrt(dx*dx+dy*dy+dz*dz),0))",
                           "B_xdir  = ((dx)/norm)",
                           "B_ydir  = ((dy)/norm)",
                           "B_zdir  = ((dz)/norm)",
                           "Pxrhomu   = (CHILD(PX,1)+CHILD(PX,2))",
                           "Pyrhomu   = (CHILD(PY,1)+CHILD(PY,2))",
                           "Pzrhomu   = (CHILD(PZ,1)+CHILD(PZ,2))",
                           "Bflight = (B_xdir*Pxrhomu + B_ydir*Pyrhomu+ B_zdir*Pzrhomu)",
                           "mB  = 5279.25",
                           "M_2 = (mB*mB + M12*M12)",
                           "rhomu_E  = (CHILD(E,1)+CHILD(E,2))",
                           "quada = (Bflight*Bflight - rhomu_E*rhomu_E)",
                           "quadb = (M_2*Bflight)",
                           "quadc = (((M_2*M_2)/4) - (rhomu_E*rhomu_E*mB*mB))",
                           "Discriminant = ((quadb*quadb)-(4*quada*quadc))",
                           "solution_a = ((-quadb + sqrt(max(Discriminant,0)))/(2*quada))",
                           "solution_b = ((-quadb - sqrt(max(Discriminant,0)))/(2*quada))",
                           "ratio = (solution_a/solution_b)"
                           ]
        
        _RhoMuSel=Selection("RhoMuWS_for"+self._name,
                         Algorithm=_RhoMu,
                         RequiredSelections = [self._muonFilterTight(), self._Rho02PiPiWSFilter()])
        _RhoMuSel = tosSelection(_RhoMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*TopoMu3Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _RhoMuSel

    
    def _Bu2RhoMuNuSB( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _RhoMu = CombineParticles(
            DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"],
            CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut ="(VFASPF(VCHI2/VDOF)< %(BVCHI2DOFTight)s) & (BPVDIRA> %(BDIRATight)s) & (BPVVDCHI2 >%(BFDCHI2Tight)s) & (ratio >0.4)"% self._config,
            #" ( ((((5279.17*5279.17) -(MASS(1,2))*(MASS(1,2))))/(2*5279.17)) < %(Enu)s*MeV)"\

            ReFitPVs = True
            )
        
        _RhoMu.Preambulo = [ "from LoKiPhys.decorators import *",
                           "dx = (VFASPF(VX)-BPV(VX))",
                           "dy = (VFASPF(VY)-BPV(VY))",
                           "dz = (VFASPF(VZ)-BPV(VZ))",
                           "norm = (max(sqrt(dx*dx+dy*dy+dz*dz),0))",
                           "B_xdir  = ((dx)/norm)",
                           "B_ydir  = ((dy)/norm)",
                           "B_zdir  = ((dz)/norm)",
                           "Pxrhomu   = (CHILD(PX,1)+CHILD(PX,2))",
                           "Pyrhomu   = (CHILD(PY,1)+CHILD(PY,2))",
                           "Pzrhomu   = (CHILD(PZ,1)+CHILD(PZ,2))",
                           "Bflight = (B_xdir*Pxrhomu + B_ydir*Pyrhomu+ B_zdir*Pzrhomu)",
                           "mB  = 5279.25",
                           "M_2 = (mB*mB + M12*M12)",
                           "rhomu_E  = (CHILD(E,1)+CHILD(E,2))",
                           "quada = (Bflight*Bflight - rhomu_E*rhomu_E)",
                           "quadb = (M_2*Bflight)",
                           "quadc = (((M_2*M_2)/4) - (rhomu_E*rhomu_E*mB*mB))",
                           "Discriminant = ((quadb*quadb)-(4*quada*quadc))",
                           "solution_a = ((-quadb + sqrt(max(Discriminant,0)))/(2*quada))",
                           "solution_b = ((-quadb - sqrt(max(Discriminant,0)))/(2*quada))",
                           "ratio = (solution_a/solution_b)"
                           ]
        
        _RhoMuSel=Selection("RhoMuSB_for"+self._name,
                         Algorithm=_RhoMu,
                         RequiredSelections = [self._muonFilterTight(), self._Rho02PiPiSBFilter()])
        _RhoMuSel = tosSelection(_RhoMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*TopoMu3Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _RhoMuSel


    ######Bu->KshMuNu SS & OS######
    def _Bu2KshSSMuNu_SSMuminus( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KshSSMu_SSMuminus = CombineParticles(
            DecayDescriptors = ["[B- -> KS0 mu-]cc"],
            CombinationCut = "(AM>%(KshMuMassLowTight)s*MeV) & (AM<%(XMuMassUpperHigh)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (MINTREE( (ABSID=='KS0') , VFASPF(VZ))-VFASPF(VZ) > %(KshZ)s *mm )" % self._config,
            ReFitPVs = True
            )
        
        _KshSSMu_SSMuminusSel=Selection("KshSSMu_SSMuminus_for"+self._name,
                              Algorithm=_KshSSMu_SSMuminus,
                              RequiredSelections = [self._muonFilter(), self._KshMajoranaSSMuFilter()])
        return _KshSSMu_SSMuminusSel
    
    
    def _Bu2KshOSMuNu_SSMuplus( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KshOSMu_SSMuplus = CombineParticles(
            DecayDescriptors = ["[B- -> KS0 mu-]cc"],
            CombinationCut = "(AM>%(KshMuMassLowTight)s*MeV) & (AM<%(XMuMassUpperHigh)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (MINTREE( (ABSID=='KS0') , VFASPF(VZ))-VFASPF(VZ) > %(KshZ)s *mm )" % self._config,
            ReFitPVs = True
            )
        
        _KshOSMu_SSMuplusSel=Selection("KshOSMu_SSMuplus_for"+self._name,
                              Algorithm=_KshOSMu_SSMuplus,
                              RequiredSelections = [self._muonFilter(), self._KshMajoranaOSMuFilter()])
        return _KshOSMu_SSMuplusSel

########Comment out for now. Will reinstate next round of stripping.####### 
#    ###Combine Bs->K*(892)+MuNu, K*->KS0Pi###
    def _Bs2KstarLLMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
#        
#        
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
        _KstarLLMuSel = tosSelection(_KstarLLMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*TopoMu3Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
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
        _KstarLLMuSSSel = tosSelection(_KstarLLMuSSSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*TopoMu3Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _KstarLLMuSSSel

