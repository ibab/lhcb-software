
__author__ = ['Liang Sun','Adam Davis']
__date__ = '09/11/2015'
__version__ = '$Revision: 0.01 $'

'''
D* -> D0 pis, D0 -> h- mu+(e+) nu exclusive reconstruction 
'''
# =============================================================================
##
#  D->h- mu+(e+) nu exclusive reconstruction
#

"""
Stripping lines for semileptonic D0 decays
to final states with hadron+muon, where the hadron is
a charged track.
The two hadronic signal modes for D+ from D*+ -> D0 pi+ are:

D0 -> K-  mu+(e+) nu, 
D0 -> pi-  mu+(e+) nu, 

Last modification $Date: 12/9/2015 $
               by $Author: adavis$
"""
__all__ = ('D2HLepNuBuilder',
           'TOSFilter',
           'default_config')

default_config = {
    'D2HMuNu' : {
        'WGs'         : ['Charm'],
        'BUILDERTYPE' : 'D2HLepNuBuilder',
        'CONFIG'      :  {
            "KLepMassLow" : 500 , #MeV
            "KLepMassHigh" : 2000 , #MeV
            "DELTA_MASS_MAX" : 400  , #MeV
            "GEC_nLongTrk"        : 160.  , #adimensional
            "TRGHOSTPROB"         : 0.35    ,#adimensional
            #Muons
            "MuonGHOSTPROB"       : 0.35  ,#adimensional
            #	"MuonP"               : 3000. ,#MeV
            "MuonPT"              : 500. ,#MeV
            "MuonPIDK"            : 0.    ,#adimensional
            "MuonPIDmu"           : 3.    ,#adimensional
            "MuonPIDp"            : 0.    ,#adimensional
            "ElectronPIDe"  : 0.0 ,
            "ElectronPT"    : 500,#MeV
            #Xu
            #K channel
            "KaonP"               : 3000.  ,#MeV
            "KaonPT"              : 800.   ,#MeV
            "KaonPIDK"            : 5.     ,#adimensional 
            "KaonPIDmu"           : 5.     ,#adimensional
            "KaonPIDp"            : 5.     ,#adimensional
            "KaonMINIPCHI2"          : 9      ,#adimensional
            "BVCHI2DOF"       : 20    ,#adminensional
            "BDIRA"       : 0.999    ,#adminensional
            "BFDCHI2HIGH"         : 100.  ,#adimensional
            #slow pion
            "Slowpion_PT" : 300 #MeV
            ,"Slowpion_P" : 1000 #MeV
            ,"Slowpion_TRGHOSTPROB" : 0.35 #adimensional
            ,"Slowpion_PIDe" : 5 #adimensional
            ,"useTOS" : True #adimensional
            ,"TOSFilter" : { 'Hlt2CharmHad.*HHX.*Decision%TOS' : 0}  #adimensional
            },
        'STREAMS'     : ['Charm']	  
        }
    }
from Gaudi.Configuration import *
from StrippingUtils.Utils import LineBuilder

import logging
#default_name="D2KMuNu"

class D2HLepNuBuilder(LineBuilder):
    """
    Definition of D* tagged D0 -> H- mu+(e+) nu stripping
    """
    
    __configuration_keys__ = [
        "KLepMassLow"  
        , "KLepMassHigh"  
        , "DELTA_MASS_MAX"
        , "GEC_nLongTrk"
        ,"MuonGHOSTPROB"
        ,"TRGHOSTPROB"          
        #        ,"MuonTRCHI2"          
        #        ,"MuonP"
        ,"MuonPT"
        ,"MuonPIDK"            
        ,"MuonPIDmu"           
        ,"MuonPIDp"           
        ,"ElectronPIDe"
        ,"ElectronPT"
        #        ,"MuonMINIPCHI2"       
        #        ,"KaonTRCHI2"          
        ,"KaonP"               
        ,"KaonPT"              
        ,"KaonPIDK"             
        ,"KaonPIDmu"           
        ,"KaonPIDp"

        ,"KaonMINIPCHI2"
        ,"Slowpion_PT"
        ,"Slowpion_P"
        ,"Slowpion_TRGHOSTPROB"
        ,"Slowpion_PIDe"
        #        ,"useHLT2" 
        ,"useTOS"
        ,"TOSFilter"
        #        ,"Hlt2Filter"
        , "BVCHI2DOF"
        , "BDIRA"
        , "BFDCHI2HIGH"
#        , "VDCut"
        ]
    
    def __init__(self,name,config):
        LineBuilder.__init__(self, name, config)
        self._config=config
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        
        self.GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                      "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        self._muonSel=None
        self._muonFilter()
        
        self._electronSel=None
        self._electronFilter()
        
        self._slowpionSel =None
        self._pionSel=None
        self._kaonSel=None
        self._kaonFilter()
        self._pionFilter()
        self._slowpionFilter()
        
        self.registerLine(self._D2PiMuNuLine())
        self.registerLine(self._D2KMuNuLine())
        self.registerLine(self._D2PiENuLine())
        self.registerLine(self._D2KENuLine())
        
    def _NominalMuSelection( self ):
        #        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
        return " (PT> %(MuonPT)s* MeV)"\
               "& (TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
               "& (PIDmu-PIDpi> %(MuonPIDmu)s )"\
               "& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "& (PIDmu-PIDK> %(MuonPIDK)s )"
#               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"

    def _NominalKSelection( self ):
        #        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonPTight)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
        return " (PT> %(KaonPT)s *MeV)"\
               "& (P> %(KaonP)s)"\
               "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s )& (PIDK-PIDp> %(KaonPIDp)s )& (PIDK-PIDmu> %(KaonPIDmu)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def _NominalPiSelection( self ):
        #        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
        return " (PT> %(KaonPT)s *MeV)"\
               "& (P> %(KaonP)s)"\
               "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
               "& (PIDK-PIDpi< -%(KaonPIDK)s )& (PIDp < -%(KaonPIDp)s )& (PIDmu < -%(KaonPIDmu)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def _NominalSlowPiSelection( self ):
        #        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
        return " (PT> %(Slowpion_PT)s *MeV)"\
               "& (P > %(Slowpion_P)s)"\
               "& (TRGHOSTPROB < %(Slowpion_TRGHOSTPROB)s)"\
               "& (PIDe < %(Slowpion_PIDe)s) & (MIPCHI2DV(PRIMARY)< 9.0) "

    def _NominalElectronSelection( self ):
        return " (PT> %(ElectronPT)s *MeV) & (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                "& (PIDe > %(ElectronPIDe)s)"

    ######--######
    def _electronFilter( self ):
        if self._electronSel is not None:
            return self._electronSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseElectrons
        _el = FilterDesktop( Code = self._NominalElectronSelection() % self._config )
        _electronSel=Selection("Electron_for"+self._name,
                         Algorithm=_el,
                         RequiredSelections = [StdLooseElectrons])
        
        self._electronSel=_electronSel
        
        return _electronSel

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

    ######Slow Pion Filter######
    def _slowpionFilter( self ):
        if self._slowpionSel is not None:
            return self._slowpionSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdAllLoosePions
        
        _ka = FilterDesktop( Code = self._NominalSlowPiSelection() % self._config )
        _kaSel=Selection("pis_for"+self._name,
                         Algorithm=_ka,
                         RequiredSelections = [StdAllLoosePions])
        self._slowpionSel=_kaSel
        return _kaSel

    def _D2PiMuNuLine( self ):
        return self.DstarMaker("D2pimu", ["[D0 -> pi- mu+]cc"], self._pionFilter(), self._muonFilter())

    def _D2KMuNuLine( self ):
        return self.DstarMaker("D2Kmu",["[D0 -> K- mu+]cc"], self._kaonFilter(), self._muonFilter())

    def _D2PiENuLine( self ):
        return self.DstarMaker("D2piE", ["[D0 -> pi- e+]cc"], self._pionFilter(), self._electronFilter())

    def _D2KENuLine( self ):
        return self.DstarMaker("D2KE",["[D0 -> K- e+]cc"], self._kaonFilter(), self._electronFilter())

    def DstarMaker(self, _name, _D0Decays, hfiler, lfilter):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StrippingConf.StrippingLine import StrippingLine
        
        _KMu = CombineParticles(
            DecayDescriptors = _D0Decays,
            CombinationCut = "(AM>%(KLepMassLow)s*MeV) & (AM<%(KLepMassHigh)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s )"\
            "& (BPVVDCHI2 >%(BFDCHI2HIGH)s)"\
            "& (BPVDIRA > %(BDIRA)s)"
            #            "& (BPVVD > %(VDCut)s)"
            #" & (ratio > 0.0)"
            % self._config,
            ReFitPVs = True
            )
        # _KMu.Preambulo = [ "from LoKiPhys.decorators import *",
        #                    "dx = (VFASPF(VX)-BPV(VX))",
        #                    "dy = (VFASPF(VY)-BPV(VY))",
        #                    "dz = (VFASPF(VZ)-BPV(VZ))",
        #                    "norm = (max(sqrt(dx*dx+dy*dy+dz*dz),0))",
        #                    "D_xdir  = ((dx)/norm)",
        #                    "D_ydir  = ((dy)/norm)",
        #                    "D_zdir  = ((dz)/norm)",
        #                    "Pxkmu   = (CHILD(PX,1)+CHILD(PX,2))",
        #                    "Pykmu   = (CHILD(PY,1)+CHILD(PY,2))",
        #                    "Pzkmu   = (CHILD(PZ,1)+CHILD(PZ,2))",
        #                    "Dflight = (D_xdir*Pxkmu + D_ydir*Pykmu+ D_zdir*Pzkmu)",
        #                    "mD  = 1864.84",
        #                    "M_2 = (mD*mD + M12*M12)",
        #                    "kmu_E  = (CHILD(E,1)+CHILD(E,2))",
        #                    "quada = (Dflight*Dflight - kmu_E*kmu_E)",
        #                    "quadb = (M_2*Dflight)",
        #                    "quadc = (((M_2*M_2)/4) - (kmu_E*kmu_E*mD*mD))",
        #                    "Discriminant = ((quadb*quadb)-(4*quada*quadc))",
        #                    "solution_a = ((-quadb + sqrt(max(Discriminant,0)))/(2*quada))",
        #                    "solution_b = ((-quadb - sqrt(max(Discriminant,0)))/(2*quada))",
        #                    "ratio = (solution_a/solution_b)"
        #                    ]
        
        _D0Sel=Selection("SelD0_for"+_name,
                         Algorithm=_KMu,
                         RequiredSelections = [lfilter, hfiler])


        DstComb = CombineParticles( #name = "CombDst"+_name,
                DecayDescriptors = ['[D*(2010)+ -> D0 pi+]cc'],
                CombinationCut = "(AM - ACHILD(M,1) < %(DELTA_MASS_MAX)s+5 *MeV) & (ADOCACHI2CUT(20,''))" %self._config,
                MotherCut = "(M - CHILD(M,1) < %(DELTA_MASS_MAX)s *MeV) & (VFASPF(VCHI2/VDOF) < 9.0)" %self._config
                )
        DstSel = Selection("SelDst"+_name,
                Algorithm = DstComb,
                RequiredSelections = [_D0Sel,self._slowpionFilter()])

        _tosFilter = self._config['TOSFilter']
        DstSelTOS = TOSFilter( "SelDstHMu_Hlt2TOS"+_name
                ,DstSel
                ,_tosFilter)
    
        hlt2 = ""        
        if self._config["useTOS"] == True: # and _name.find('E') < 0:
            Line = StrippingLine(_name+'Line',
                    prescale = 1.0,
                    FILTER=self.GECs,
                    HLT2 = hlt2,
                    selection = DstSelTOS) 
        else:
            Line = StrippingLine(_name+'Line',
                    prescale = 1.0,
                    FILTER=self.GECs,
                    HLT2 = hlt2,
                    selection = DstSel) 
        
        return Line
                                                    
def TOSFilter( name, _input, _specs ) :
    from Configurables import TisTosParticleTagger
    from PhysSelPython.Wrappers import Selection 
    _tisTosFilter = TisTosParticleTagger( name + "Tagger" )
    _tisTosFilter.TisTosSpecs = _specs 
    return Selection( name
                      , Algorithm = _tisTosFilter
                      , RequiredSelections = [ _input ]
                      )        
