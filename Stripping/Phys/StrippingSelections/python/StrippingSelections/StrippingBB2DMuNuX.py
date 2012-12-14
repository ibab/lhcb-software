"""
Module for constructing B semileptonic inclusive channel:
B->D0XMuNu
"""
__author__ = ['Yury Shcheglov']
__date__ = '23/10/2012'
__version__ = '$Revision: 1.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdLooseMergedPi0,StdLooseResolvedPi0
from Configurables import ConjugateNeutralPID

__all__ = ('StrippingBB2DMuNuXConf',
           'makebb2DMuX',
           'confdict')

confdict = {
    "TRGHOSTPROB" : 0.5
    ,"MuonGHOSTPROB" : 0.5
    ,"PrescaleD0Mu"    : 1.0
    ,"MINIPCHI2"     : 25.0 
    ,"TRCHI2"        : 5.0
    ,"KaonPIDK"      : 4.0 
    ,"PionPIDK"      : 10.0
    ,"MuonIPCHI2"    : 25.00  
    ,"MuonPT"        : 800.0    # MeV   800
    ,"KPiPT"         : 300.0    # MeV   300
    ,"D0FDCHI2"      : 100.0     # 100
    ,"D0AMassWin"    : 100.0    # MeV   100
    ,"D0VCHI2DOF"    : 10.0            #6.0
    ,"PIDmu"         : 0.0   
    ,"BVCHI2DOF"     : 100.0            #6.0    
    ,"Comb_AM_MIN"   : 0.
    ,"Comb_AM_MAX"   : 90000.0  # MeV
            }


class StrippingBB2DMuNuXConf(LineBuilder) :
    """
    """
    
    __configuration_keys__ = (
        "TRGHOSTPROB"
        ,"MuonGHOSTPROB"
        ,"PrescaleD0Mu"
        ,"MINIPCHI2"     
        ,"TRCHI2"     
        ,"KaonPIDK"
        ,"PionPIDK"
        ,"MuonIPCHI2"    
        ,"MuonPT"        
        ,"KPiPT"               
        ,"D0FDCHI2"      
        ,"D0AMassWin"    
        ,"D0VCHI2DOF"    
        ,"PIDmu"         
        ,"BVCHI2DOF"     
        ,"Comb_AM_MIN"
        ,"Comb_AM_MAX"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config


        ############### MUON SELECTIONS ###################
        self.selmuon = Selection( "Mufor" + name,
                                  Algorithm = self._muonFilter(),
                                  RequiredSelections = [StdLooseMuons])


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
        

        ################ B CANDIDATES SELECTION #################################


        self.selb2D0MuX = Selection('b2D0MuX' + name,
                                    Algorithm = self._b2D0MuXFilter(),
                                    RequiredSelections = [self.selmuon, self.seld02kpi])


        ################ MAKE THE B CANDIDATES #################################

        self.selbb2D0MuX = makebb2D0MuX('bb2D0MuX' + name
                                        , inputSel = self.selb2D0MuX
                                        , Comb_AM_MIN = config['Comb_AM_MIN']
                                        , Comb_AM_MAX = config['Comb_AM_MAX']
                                        )

        ########## D0 -> HH ###########
        self.bb2D0MuXLine = StrippingLine('bb2D0MuX' + name + 'Line', prescale = config['PrescaleD0Mu'], selection = self.selbb2D0MuX)

        self.registerLine(self.bb2D0MuXLine)        

    def _muonFilter( self ):
        _code = "(PT > %(MuonPT)s *MeV) & (P> 3.0*GeV)"\
                "&(TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
                "& (TRCHI2DOF< %(TRCHI2)s) & (MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s) & (PIDmu > %(PIDmu)s)" % self.__confdict__
        _mu = FilterDesktop( Code = _code )
        return _mu        

    def _pionFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "&(TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDK)s)" % self.__confdict__
        _pi = FilterDesktop( Code = _code )
        return _pi

    def _kaonFilter( self ):
        _code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                "&(TRGHOSTPROB < %(TRGHOSTPROB)s)"\
                "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK> %(KaonPIDK)s)" % self.__confdict__
        _ka = FilterDesktop( Code = _code )
        return _ka 
  

    def _D02KPiFilter( self ):

        _decayDescriptors = [ '[D0 -> K- pi+]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(D0AMassWin)s *MeV)" % self.__confdict__
        _motherCut = "(SUMTREE( PT,  ISBASIC )>1400.*MeV) & (VFASPF(VCHI2/VDOF) < %(D0VCHI2DOF)s) " \
                     "& (BPVVDCHI2 > %(D0FDCHI2)s)"  % self.__confdict__
        _d02kpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _d02kpi
  
    
    def _b2D0MuXFilter(self):
        _decayDescriptors =[ '[B- -> D0 mu-]cc' ]
        _combinationCut = "(AM<6.2*GeV)"
        _motherCut = "(MM<7.0*GeV) & (MM>1.5*GeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s)" % self.__confdict__
        _B = CombineParticles(DecayDescriptors = _decayDescriptors,
                              CombinationCut = _combinationCut,
                              MotherCut = _motherCut)
                          
        return _B



def makebb2D0MuX( name
                 , inputSel
                 , Comb_AM_MIN
                 , Comb_AM_MAX
                 , _decDescriptors  = [ '[Z0 -> B+ B-]cc' ]
                ):

    _combinationCut = "(AM < %(Comb_AM_MAX)s)" \
                "& (AM > %(Comb_AM_MIN)s)" % locals()
    _motherCuts = "(ALL)"

    _BB = CombineParticles(DecayDescriptors = _decDescriptors
                           , CombinationCut   = _combinationCut
                           , MotherCut = _motherCuts
                          )

    return Selection( name,
                      Algorithm          = _BB,
                      RequiredSelections = [inputSel]
                    )

