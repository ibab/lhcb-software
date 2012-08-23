"""
Selection for B-> D tau nu, tau-> mu analyses
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
from StandardParticles import StdLoosePions, StdAllLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdLooseMergedPi0,StdLooseResolvedPi0
from Configurables import ConjugateNeutralPID

__all__ = ('B2DMuForTauMuconf',
           'makeb2DMuX',
           'confdict')

confdict = {
 
     "MINIPCHI2"     : 45.0    # adimensiional 
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
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"SPDmax"        : 600    # adimensiional
    }

class B2DMuForTauMuconf(LineBuilder) :
    """
    """
    
    __configuration_keys__ = (

        "MINIPCHI2"
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
        ,"BVCHI2DOF"
        ,"SPDmax"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config

        

        ############### MUON SELECTIONS ###################
        self.selmuon = Selection( "Mufor" + name,
                                  Algorithm = self._muonFilter(),
                                  RequiredSelections = [StdAllLooseMuons])


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

        self.selb2D0MuX = makeb2DMuX('b2D0MuX' + name,
                                     DecayDescriptors = [ '[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                     MuSel = self.selmuon, 
                                     DSel = self.seld02kpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )
        
        
        ################# DECLARE THE STRIPPING LINES #################################

        GECs = { "Code":" ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < %(SPDmax)s )" % config,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}

        ########## D0 -> HH ###########
        self.B2DMuForTauMu = StrippingLine('mystripping' + 'Line', prescale = 1, selection = self.selb2D0MuX, FILTER  = GECs)
        self.registerLine(self.B2DMuForTauMu)        


    def _muonFilter( self ):
        _code = "(MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s) &(TRGHOSTPROB < %(GhostProb)s) & (PIDmu > %(PIDmu)s)   & (P> 3.0*GeV)" % self.__confdict__
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
  



def makeb2DMuX(name,
               DecayDescriptors,
               MuSel,
               DSel,
               BVCHI2DOF,
               BDIRA):
               #due to tight cuts, B sideband adds virtually zero events, free
    _combinationCut = "(AM<10.2*GeV)"
    _motherCut = "  (MM<10.0*GeV) & (MM>0.0*GeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)  " \
                   " "  % locals()
    _B = CombineParticles(DecayDescriptors = DecayDescriptors,
                          CombinationCut = _combinationCut,
                          MotherCut = _motherCut)
                          
    return Selection (name,
                      Algorithm = _B,
                      RequiredSelections = [MuSel, DSel])





