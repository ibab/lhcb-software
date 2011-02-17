"""
Module for construct B semileptonic inclusive channels:
B->D0XMuNu, D+XMuNu, Ds+XMuNu, Lc+XMuNu with
D0->Kpi, D+->Kpipi, KKPi, Ds+ -> KKPi and Lc+->PKPi
"""
__author__ = ['Liming Zhang']
__date__ = '23/07/2010'
__version__ = '$Revision: 1.4 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


__all__ = ('B2DMuNuXAllLinesConf',
           'makeb2DMuX',
           'confdict')

confdict = {
    "MINIPCHI2"      : 9.0    # adimensiional
    ,"TRCHI2"        : 5.0    # adimensiional
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"MuonIPCHI2"    : 4.00   # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensiional
    ,"DsFDCHI2"      : 100.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"DsIP"          : 7.4    #mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"PIDmu"         : -0.0   # adimensiional
    ,"BDIRA"         : 0.999  #adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"DZ"            : 0.0    #mm
    ,"DDocaChi2Max"  : 20     #adimensiional
    }
   

class B2DMuNuXAllLinesConf(LineBuilder) :
    """
    """

    __configuration_keys__ = (
         "MINIPCHI2"     
        ,"TRCHI2"        
        ,"KaonPIDK"      
        ,"PionPIDK"      
        ,"MuonIPCHI2"    
        ,"MuonPT"        
        ,"KPiPT"               
        ,"DsDIRA"        
        ,"DsFDCHI2"      
        ,"DsMassWin"     
        ,"DsAMassWin"    
        ,"DsIP"          
        ,"DsVCHI2DOF"    
        ,"PIDmu"         
        ,"BDIRA"         
        ,"BVCHI2DOF"     
        ,"DZ"            
        ,"DDocaChi2Max"  
        )

    __confdict__={}
        

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config

        self.selmuon = self._muonFilter()
        self.selKaon = self._kaonFilter()
        self.selPion = self._pionFilter()
        self.selb2D0MuX = makeb2DMuX(OutputList = "b2D0MuX",
                                 DecayDescriptors = [ '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc' ],
                                 DaughterLists = [ self.selmuon, self._D02KPiFilter()],
                                 BVCHI2DOF = config['BVCHI2DOF'],
                                 BDIRA = config['BDIRA'],
                                 DZ = config['DZ']
                                 )

        self.selb2DpMuX = makeb2DMuX(OutputList = "b2DpMuX",
                                 DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                 DaughterLists = [ self.selmuon, self._Dp2KPiPiFilter()],
                                 BVCHI2DOF = config['BVCHI2DOF'],
                                 BDIRA = config['BDIRA'],
                                 DZ = config['DZ']
                                 )

        self.selb2DsMuX = makeb2DMuX(OutputList = "b2DsMuX",
                                 DecayDescriptors = [ '[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc' ],
                                 DaughterLists = [ self.selmuon, self._Ds2KKPiFilter()],
                                 BVCHI2DOF = config['BVCHI2DOF'],
                                 BDIRA = config['BDIRA'],
                                 DZ = config['DZ']
                                 )
        self.selb2LcMuX = makeb2DMuX(OutputList = "b2LcMuX",
                                 DecayDescriptors = [ '[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc'],
                                 DaughterLists = [ self.selmuon, self._Lc2PKPiFilter()],
                                 BVCHI2DOF = config['BVCHI2DOF'],
                                 BDIRA = config['BDIRA'],
                                 DZ = config['DZ']
                                 )         
        
        self.b2D0MuXLine = StrippingLine('b2D0MuXLine'
                                          #, HLT     = " HLT_PASS_RE('Hlt.*(Muon|MuTrack).*Decision') "
                                          , prescale = 1
                                          , selection = self.selb2D0MuX
                                          )

        self.b2DpMuXLine = StrippingLine('b2DpMuXLine'
                                         , prescale = 1
                                         , selection = self.selb2DpMuX
                                         )
        
        self.b2DsMuXLine = StrippingLine('b2DsMuXLine'
                                         , prescale = 1
                                         , selection = self.selb2DsMuX
                                         )
        
        self.b2LcMuXLine = StrippingLine('b2LcMuXLine'
                                         , prescale = 1
                                         , selection = self.selb2LcMuX
                                         )

        
        self.registerLine(self.b2D0MuXLine)
        self.registerLine(self.b2DpMuXLine)
        self.registerLine(self.b2DsMuXLine)
        self.registerLine(self.b2LcMuXLine)



    def _muonFilter( self ):
        from Configurables import FilterDesktop
        _mu = FilterDesktop("FilterMu_forb2DMuX")
        _mu.Code = "(PT > %(MuonPT)s *MeV) & (P> 3.0*GeV) & (TRCHI2DOF< %(TRCHI2)s) & (MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s) & (PIDmu > %(PIDmu)s)" % self.__confdict__
        return Selection( "Mu_forb2DMuX", Algorithm = _mu, RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseMuons")])
        

    def _pionFilter( self ):
        from Configurables import FilterDesktop
        _pi = FilterDesktop("FilterPi_forb2DMuX")
        _pi.Code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                   "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDK)s)" % self.__confdict__
        return Selection( "Pi_forb2DMuX", Algorithm = _pi, RequiredSelections = [DataOnDemand(Location = "Phys/StdLoosePions")])

    def _kaonFilter( self ):
        from Configurables import FilterDesktop
        _ka = FilterDesktop("FilterK_forb2DMuX")
        _ka.Code = "  (TRCHI2DOF < %(TRCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                   "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK> %(KaonPIDK)s)" % self.__confdict__
        return Selection( "K_forb2DMuX", Algorithm = _ka, RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKaons")])
        
                   
  
    def _D02KPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _d02kpi = CombineParticles("CombineD02KPi_forb2DMuX")
        _d02kpi.DecayDescriptors = [ '[D0 -> K- pi+]cc' ]
        _d02kpi.CombinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > 1400.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _d02kpi.MotherCut = "(SUMTREE( PT,  ISBASIC )>1400.*MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.__confdict__
        return Selection( "D0KPi_forb2DMuX", Algorithm = _d02kpi, RequiredSelections = [self.selKaon, self.selPion] )

    def _Dp2KPiPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _dp2kpipi = CombineParticles("CombineDp2KPiPi_forb2DMuX")
        _dp2kpipi.DecayDescriptors = [ '[D+ -> K- pi+ pi+]cc' ]
        _dp2kpipi.CombinationCut = "(ADAMASS('D+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _dp2kpipi.MotherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(ADMASS('D+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.__confdict__
        return Selection( "Dp2KPiPi_forb2DMuX", Algorithm = _dp2kpipi, RequiredSelections = [self.selKaon, self.selPion] )

    def _Ds2KKPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _ds2kkpi = CombineParticles("CombineDs2KKPi_forb2DMuX")
        _ds2kkpi.DecayDescriptors = [ '[D+ -> K+ K- pi+]cc' ]
        _ds2kkpi.CombinationCut = "(DAMASS('D_s+') < %(DsAMassWin)s *MeV) & (DAMASS('D+')> -%(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _ds2kkpi.MotherCut = "(SUMTREE( PT,  ISBASIC )>1800.*MeV) &(DMASS('D_s+') < %(DsMassWin)s *MeV) & (DMASS('D+') > -%(DsMassWin)s *MeV)"\
                             "& (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                             "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.__confdict__
        return Selection( "Ds2KKPi_forb2DMuX", Algorithm = _ds2kkpi, RequiredSelections = [self.selKaon, self.selPion] )


    def _Lc2PKPiFilter( self ):
        from Configurables import FilterDesktop, CombineParticles
        _lambdac = CombineParticles("CombineLc2PKPi_forb2DMuX")
        _lambdac.DecayDescriptors = [ '[Lambda_c+ -> K- p+ pi+]cc' ]
        _lambdac.DaughtersCuts = {  "p+" :  "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(KPiPT)s *MeV) & (P>2.0*GeV) "\
                                    "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s)  &  (PIDp> %(KaonPIDK)s) & (PIDp-PIDK>1.0e-10)" % self.__confdict__}
        _lambdac.CombinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1800.*MeV) & (ADOCACHI2CUT( %(DDocaChi2Max)s, ''))" % self.__confdict__
        _lambdac.MotherCut = "(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) & (SUMTREE( PT,  ISBASIC )>1800.*MeV) & (BPVDIRA> %(DsDIRA)s) & (BPVIP()< %(DsIP)s *mm)"  % self.__confdict__
        return Selection( "Lc2PKPi_forb2DMuX", Algorithm = _lambdac, RequiredSelections = [self.selKaon, self.selPion, DataOnDemand(Location = "Phys/StdLooseProtons") ] )
        


def makeb2DMuX(OutputList,
               DecayDescriptors,
               DaughterLists,
               BVCHI2DOF,
               BDIRA,
               DZ):
    from Configurables import CombineParticles, OfflineVertexFitter
    combinerName = OutputList + "_forb2DMuX"
    _B = CombineParticles( combinerName )
    _B.DecayDescriptors = DecayDescriptors
    _B.CombinationCut = "(AM<6.2*GeV)"
    _B.MotherCut = "  (MM<6.0*GeV) & (MM>2.5*GeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)  " \
                   "& (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > %(DZ)s *mm ) "  % locals()
    #        _B.ReFitPVs = True
    return Selection (OutputList,
                      Algorithm = _B,
                      RequiredSelections = DaughterLists)
