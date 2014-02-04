"""
Control channels for B-> D tau nu, tau-> mu analyses:
Author is a strong term - code ripped off shamelessly from the B2DMuX line
"""
__author__ = ['Greg Ciezarek']
__date__ = '22/08/2012'
__version__ = '$Revision: 1.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdAllLooseMuons, StdLooseKaons, StdLooseProtons, StdAllLoosePions, StdLooseMergedPi0,StdLooseResolvedPi0,StdAllNoPIDsMuons, StdLooseANNUpPions
from Configurables import ConjugateNeutralPID
from PhysSelPython.Wrappers import MergedSelection

__all__ = ('B2DHForTauMuconf',
           'makeb2DHX',
           'confdict')

confdict = {
 
     "MINIPCHI2"     : 45.0    # adimensiional 
    ,"MINIPCHI2LOOSE"     : 9.0    # adimensiional 
    ,"GhostProb"     : 0.5     # adimensional  
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDKTight" : 2.0    # adimensiional
    ,"MuonIPCHI2"    : 45.00   # adimensiional
    ,"KPiPT"         : 300.0  # MeV
    ,"DSumPT"        : 1400.0 # MeV
    ,"DsDIRA"        : 0.9998   # adimensiional
    ,"DsFDCHI2"      : 250.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"DsVCHI2DOF"    : 4.0    # adimensiional
    ,"PIDmu"         : 2.0   # adimensiional
    ,"BDIRA"         : 0.9995  #adimensiional
    ,"BFDCHI2"         : 50.0  #adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"SPDmax"        : 600    # adimensiional
    }

class B2DHForTauMuconf(LineBuilder) :
    """
    """
    
    __configuration_keys__ = (

        "MINIPCHI2" 
        ,"MINIPCHI2LOOSE" 
        ,"GhostProb"  
        ,"KaonPIDK"
        ,"PionPIDKTight"
        ,"MuonIPCHI2"
        ,"KPiPT"
        ,"DSumPT"
        ,"DsDIRA"
        ,"DsFDCHI2"
        ,"DsMassWin"
        ,"DsAMassWin"
        ,"DsVCHI2DOF"
        ,"PIDmu"
        ,"BDIRA"
        ,"BFDCHI2"
        ,"BVCHI2DOF"
        ,"SPDmax")
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config

        

        ############### MUON SELECTIONS ###################
        self.selProbe = Selection( "Probefor" + name,
                                  Algorithm = self._probeFilter(),
                                  RequiredSelections = [StdAllLoosePions])
                                  


        ############### KAON AND PION SELECTIONS ################
        
        self.selKaon = Selection( "Kfor" + name,
                                  Algorithm = self._kaonFilter(),
                                  RequiredSelections = [StdLooseKaons])
        
        self.selPion = Selection( "Pifor" + name,
                                  Algorithm = self._pionFilter(),
                                  RequiredSelections = [StdLoosePions])
                                  
                                  

        ################ D0 -> HH SELECTION ##########################
        
        self.seld02kpi = Selection( "D02KPifor" + name,
                                    Algorithm = self._D02KPiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion] )

        self.seldst2d0pi = Selection( "Dst2D0Pifor" + name,
                                    Algorithm = self._Dst2D0PiFilter(),
                                    RequiredSelections = [self.seld02kpi, StdAllLoosePions] )


        self.sel3pi = Selection( "Pi3for" + name,
                                    Algorithm = self._Pi3Filter(),
                                    RequiredSelections = [self.selProbe] )
        self.sel2pi = Selection( "Pi2for" + name,
                                    Algorithm = self._Pi2Filter(),
                                    RequiredSelections = [self.selProbe] )

        self.selb2D0HX = makeb2DHX('b2D0HX' + name,
                                     DecayDescriptors = ['[B0 -> D*(2010)+ pi-]cc','[B0 -> D*(2010)+ rho(770)0]cc' ,'[B0 -> D*(2010)+ a_1(1260)-]cc', '[B0 -> D0 pi-]cc','[B0 -> D0 rho(770)0]cc' ,'[B0 -> D0 a_1(1260)-]cc'],
                                     MuSel = self.selProbe, 
                                     DSel = self.seld02kpi,
                                     DstSel = self.seldst2d0pi,
                                     RhoSel = self.sel2pi,
                                     A1Sel = self.sel3pi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA'],
                                     BFDCHI2 = config['BFDCHI2'],
                                     #MuonIPCHI2 = config['MuonIPCHI2'],
                                     #MuonIPCHI2LOOSE = config['MuonIPCHI2LOOSE'],
                                     #GhostProb = config['GhostProb']
                                     )

        ################# DECLARE THE STRIPPING LINES #################################

        GECs = { "Code":" ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < %(SPDmax)s )" % config,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}

        ########## D0 -> HH ###########
        self.B2DHForTauMu = StrippingLine('b2D0H' + name+ 'Line', prescale = 1, selection = self.selb2D0HX, FILTER  = GECs)
        self.registerLine(self.B2DHForTauMu)


    def _probeFilter( self ):
        _code = "(MIPCHI2DV(PRIMARY)> %(MINIPCHI2LOOSE)s) &(TRGHOSTPROB < %(GhostProb)s) & (P> 3.0*GeV) " % self.__confdict__
        _mu = FilterDesktop( Code = _code )
        return _mu 

    def _pionFilter( self ):
        _code = "(P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                   "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDKTight)s) &(TRGHOSTPROB < %(GhostProb)s) " % self.__confdict__
        _pi = FilterDesktop( Code = _code )
        return _pi

    def _kaonFilter( self ):
        _code = "(PIDK> %(KaonPIDK)s) & (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                   "    & (TRGHOSTPROB < %(GhostProb)s)" % self.__confdict__
        _ka = FilterDesktop( Code = _code )
        return _ka     
   
    def _D02KPiFilter( self ):
        _decayDescriptors = [ '[D0 -> K- pi+]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > %(DSumPT)s *MeV) " % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )> %(DSumPT)s * MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.__confdict__
        _d02kpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _d02kpi
  


    def _Dst2D0PiFilter( self ):
        _decayDescriptors = [ '[D*(2010)+ -> D0 pi+]cc' ]
        _combinationCut = "((AM - ACHILD(M,1) < 175 *MeV))" % self.__confdict__
        _motherCut = "(VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s)" \
                            "& ((M - CHILD(M,1) < 147 *MeV)) & ((M - CHILD(M,1) > 143 *MeV))"  % self.__confdict__
        _Dst2D0pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _Dst2D0pi


    def _Pi3Filter( self ):
        _decayDescriptors = [ '[a_1(1260)+ -> pi+ pi- pi+]cc' ]
        _combinationCut = "(ADOCACHI2CUT( 20 , ''))" % self.__confdict__
        _motherCut = "(VFASPF(VCHI2/VDOF) < 15) " \
                            "& (BPVVDCHI2 > 30) & (CHILD(MIPCHI2DV(PRIMARY),2) > %(MuonIPCHI2)s) & (CHILD(P,2) > 3.0*GeV)"  % self.__confdict__
        _pi3 = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _pi3


    def _Pi2Filter( self ):
        _decayDescriptors = [ 'rho(770)0 -> pi+ pi-' ]
        _combinationCut = "(ADOCACHI2CUT( 20 , ''))" % self.__confdict__
        _motherCut = "(VFASPF(VCHI2/VDOF) < 15) " \
                            "& (BPVVDCHI2 > 30)& (CHILD(MIPCHI2DV(PRIMARY),2) > %(MuonIPCHI2)s) & (CHILD(P,2) > 3.0*GeV)"  % self.__confdict__
        _pi2 = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _pi2

def makeb2DHX(name,
               DecayDescriptors,
               MuSel,
               DSel,
               DstSel,
               RhoSel,
               A1Sel,
               BVCHI2DOF,
               BDIRA,
               BFDCHI2):
    _combinationCut = "(AM<7.2*GeV) &(AM>4.5*GeV)"
    _motherCut = "  (MM<7*GeV) & (MM>4.9*GeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s) & (BPVVDCHI2 > %(BFDCHI2)s)" \
                   " "  % locals()
 
    _B = CombineParticles(DecayDescriptors = DecayDescriptors,
                          CombinationCut = _combinationCut,
                          MotherCut = _motherCut)
                          
    return Selection (name,
                      Algorithm = _B,
                      RequiredSelections = [MuSel, DSel,DstSel,RhoSel,A1Sel])

