"""
Selection for B-> D tau nu, tau-> mu analyses
Author is a strong term - code ripped off shamelessly from the B2DMuX line
"""
__author__ = ['Greg Ciezarek']
__date__ = '05/11/2015'
__version__ = '$Revision: 2.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdAllLooseMuons, StdLooseKaons, StdLooseProtons, StdAllLoosePions, StdLooseMergedPi0,StdLooseResolvedPi0,StdAllNoPIDsMuons, StdLooseANNUpPions
from Configurables import ConjugateNeutralPID
from PhysSelPython.Wrappers import MergedSelection

__all__ = ('B2DMuForTauMuconf',
           #'makeb2DMuX',
           'confdict')
default_config = {
    'B2DMuForTauMu' : {
        'WGs'         : ['Semileptonic'],
	'BUILDERTYPE' : 'B2DMuForTauMuconf',
        'CONFIG'      : {
 
	 "MINIPCHI2"     : 9.0    # adimensiional 
	,"GhostProb"     : 0.5     # adimensional  
	,"KaonPIDK"      : 4.0    # adimensiional
	,"ProtonPIDp"    : 0.0    # adimensiional
	,"PionPIDKTight" : 2.0    # adimensiional
	,"MuonIPCHI2"    : 16.00   # adimensiional
	,"KPiPT"         : 300.0  # MeV
	,"DSumPT"        : 2500.0 # MeV
	,"DsDIRA"        : 0.999   # adimensiional
	,"DsFDCHI2"      : 25.0  # adimensiional
	,"DsMassWin"     : 80.0   # MeV
	,"DsAMassWin"    : 100.0  # MeV
	,"DsVCHI2DOF"    : 4.0    # adimensiional
	,"PIDmu"         : -200.0   # adimensiional
	,"BDIRA"         : 0.999  #adimensiional
	,"BVCHI2DOF"     : 6.0    # adimensiional
	,"SPDmax"        : 600    # adimensiional
	,"Hlt2Line"        : "HLT_PASS_RE('Hlt2XcMuXForTauB2XcMuDecision')"    # adimensiional
	,"Hlt2LineFake"        : "HLT_PASS_RE('Hlt2XcMuXForTauB2XcFakeMuDecision')"    # adimensiional
#	,"Hlt2Line"        : "HLT_PASS_RE('Hlt2XcMuXForTau.*Decision')"    # adimensiional
#	,"Hlt2LineFake"        : "HLT_PASS_RE('Hlt2XcMuXForTau.*Decision')"    # adimensiional

	  },
      'STREAMS'     : ['Semileptonic']	  
      }
    }

class B2DMuForTauMuconf(LineBuilder) :
    """
    """
    
    __configuration_keys__ = (

        "MINIPCHI2"
        ,"GhostProb"    
        ,"KaonPIDK"    
        ,"ProtonPIDp"     
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
        ,"Hlt2Line"
        ,"Hlt2LineFake"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.config=config

        

        ############### MUON SELECTIONS ###################
        self.selmuon = Selection( "Mufor" + name,
                                  Algorithm = self._muonFilter(),
                                  RequiredSelections = [StdAllLooseMuons])
                                  
                                  
        self.selFakemuon = Selection( "FakeMufor" + name,
                                  Algorithm = self._FakemuonFilter(),
                                  RequiredSelections = [StdAllNoPIDsMuons])                                  


        ############### KAON AND PION SELECTIONS ################
        
        self.selKaon = Selection( "Kfor" + name,
                                  Algorithm = self._kaonFilter(),
                                  RequiredSelections = [StdLooseKaons])
        
        self.selPion = Selection( "Pifor" + name,
                                  Algorithm = self._pionFilter(),
                                  RequiredSelections = [StdLoosePions])
                                  
        
        self.selProton = Selection( "Pfor" + name,
                                  Algorithm = self._protonFilter(),
                                  RequiredSelections = [StdLooseProtons])
                                                                          

        ################ D0 -> HH SELECTION ##########################
        
        self.seld02kpi = Selection( "D02KPifor" + name,
                                    Algorithm = self._D02KPiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion] )

        self.seld02k3pi = Selection( "D02K3Pifor" + name,
                                    Algorithm = self._D02K3PiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion] )
                                    
        self.seldp2k2pi = Selection( "Dp2K2Pifor" + name,
                                    Algorithm = self._Dp2K2PiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion] )                                    
                                    
        self.selds2kkpi = Selection( "Ds2KKPifor" + name,
                                    Algorithm = self._Dsp2KKPiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion] )
                                    
        self.sellc2pkpi = Selection( "Lc2pKPifor" + name,
                                    Algorithm = self._LcpKPiFilter(),
                                    RequiredSelections = [self.selKaon, self.selPion, self.selProton] )                                    

        self.selb2D0MuX = makeb2DMuX('b2D0MuX' + name,
                                     DecayDescriptors = [ '[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                     MuSel = self.selmuon, 
                                     DSel = self.seld02kpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )


        self.selb2D03MuX = makeb2DMuX('b2D0K3piMuX' + name,
                                     DecayDescriptors = [ '[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                     MuSel = self.selmuon, 
                                     DSel = self.seld02k3pi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )

        self.selb2DpMuX = makeb2DMuX('b2DpMuX' + name,
                                     DecayDescriptors = [ '[B- -> D+ mu-]cc','[B+ -> D+ mu+]cc'],
                                     MuSel = self.selmuon, 
                                     DSel = self.seldp2k2pi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )                

        self.selb2DsMuX = makeb2DMuX('b2DsMuX' + name,
                                     DecayDescriptors = [ '[B- -> D_s+ mu-]cc','[B+ -> D_s+ mu+]cc'],
                                     MuSel = self.selmuon, 
                                     DSel = self.selds2kkpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )                                     
                                     
        self.selb2LcMuX = makeb2DMuX('b2LcMuX' + name,
                                     DecayDescriptors = [ '[B- -> Lambda_c+ mu-]cc','[B+ -> Lambda_c+ mu+]cc'],
                                     MuSel = self.selmuon, 
                                     DSel = self.sellc2pkpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )                                     
                                                                          
        self.selFakeb2D0MuX = makeFakeb2DMuX('b2D0MuXFake' + name,
                                     DecayDescriptors = [ '[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                     FakeMuSel = self.selFakemuon, 
                                     DSel = self.seld02kpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )        


        self.selFakeb2D03MuX = makeFakeb2DMuX('b2D0K3piMuXFake' + name,
                                     DecayDescriptors = [ '[B- -> D0 mu-]cc','[B+ -> D0 mu+]cc'],
                                     FakeMuSel = self.selFakemuon, 
                                     DSel = self.seld02k3pi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )

        self.selFakeb2DpMuX = makeFakeb2DMuX('b2DpMuXFake' + name,
                                     DecayDescriptors = [ '[B- -> D+ mu-]cc','[B+ -> D+ mu+]cc'],
                                     FakeMuSel = self.selFakemuon, 
                                     DSel = self.seldp2k2pi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )                

        self.selFakeb2DsMuX = makeFakeb2DMuX('b2DsMuXFake' + name,
                                     DecayDescriptors = [ '[B- -> D_s+ mu-]cc','[B+ -> D_s+ mu+]cc'],
                                     FakeMuSel = self.selFakemuon, 
                                     DSel = self.selds2kkpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )                                     
                                     
        self.selFakeb2LcMuX = makeFakeb2DMuX('b2LcMuXFake' + name,
                                     DecayDescriptors = [ '[B- -> Lambda_c+ mu-]cc','[B+ -> Lambda_c+ mu+]cc'],
                                     FakeMuSel = self.selFakemuon, 
                                     DSel = self.sellc2pkpi,
                                     BVCHI2DOF = config['BVCHI2DOF'],
                                     BDIRA = config['BDIRA']
                                     )                                     

        
        ################# DECLARE THE STRIPPING LINES #################################

        GECs = { "Code":" ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < %(SPDmax)s )" % config,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}

        ########## D0 -> HH ###########
        self.B2DMuForTauMu = StrippingLine('b2D0MuX' + name+ 'Line', prescale = 1, selection = self.selb2D0MuX, FILTER  = GECs,HLT2 = config["Hlt2Line"])
        self.registerLine(self.B2DMuForTauMu)
        
        self.B2DK3piMuForTauMu = StrippingLine('b2D0K3piMuX' + name+ 'Line', prescale = 1, selection = self.selb2D03MuX, FILTER  = GECs,HLT2 = config["Hlt2Line"])
        self.registerLine(self.B2DK3piMuForTauMu)
        
        self.B2DpMuForTauMu = StrippingLine('b2DpMuX' + name+ 'Line', prescale = 1, selection = self.selb2DpMuX, FILTER  = GECs,HLT2 = config["Hlt2Line"])
        self.registerLine(self.B2DpMuForTauMu)        

        self.B2DsMuForTauMu = StrippingLine('b2DsMuX' + name+ 'Line', prescale = 1, selection = self.selb2DsMuX, FILTER  = GECs,HLT2 = config["Hlt2Line"])
        self.registerLine(self.B2DsMuForTauMu) 
        
        self.B2LcMuForTauMu = StrippingLine('b2LcMuX' + name+ 'Line', prescale = 1, selection = self.selb2LcMuX, FILTER  = GECs,HLT2 = config["Hlt2Line"])
        self.registerLine(self.B2LcMuForTauMu)     


        self.FakeB2DMuForTauMu = StrippingLine('b2D0MuXFake' + name+ 'Line', prescale = 1., selection = self.selFakeb2D0MuX, FILTER  = GECs,HLT2 = config["Hlt2LineFake"])
        self.registerLine(self.FakeB2DMuForTauMu)   

        self.FakeB2DK3piMuForTauMu = StrippingLine('b2D0K3piMuXFake' + name+ 'Line', prescale = 1., selection = self.selFakeb2D03MuX, FILTER  = GECs,HLT2 = config["Hlt2LineFake"])
        self.registerLine(self.FakeB2DK3piMuForTauMu)
        
        self.FakeB2DpMuForTauMu = StrippingLine('b2DpMuXFake' + name+ 'Line', prescale = 1., selection = self.selFakeb2DpMuX, FILTER  = GECs,HLT2 = config["Hlt2LineFake"])
        self.registerLine(self.FakeB2DpMuForTauMu)        

        self.FakeB2DsMuForTauMu = StrippingLine('b2DsMuXFake' + name+ 'Line', prescale = 1., selection = self.selFakeb2DsMuX, FILTER  = GECs,HLT2 = config["Hlt2LineFake"])
        self.registerLine(self.FakeB2DsMuForTauMu) 
        
        self.FakeB2LcMuForTauMu = StrippingLine('b2LcMuXFake' + name+ 'Line', prescale = 1., selection = self.selFakeb2LcMuX, FILTER  = GECs,HLT2 = config["Hlt2LineFake"])
        self.registerLine(self.FakeB2LcMuForTauMu)     

    def _muonFilter( self ):
        _code = "(MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s) &(TRGHOSTPROB < %(GhostProb)s) & (PIDmu > %(PIDmu)s)   & (P> 3.0*GeV)" % self.config
        _mu = FilterDesktop( Code = _code )
        return _mu 
        
    def _FakemuonFilter( self ):
        _code = "(MIPCHI2DV(PRIMARY)> %(MuonIPCHI2)s) &(TRGHOSTPROB < %(GhostProb)s) & (P> 3.0*GeV) & (~ISMUON) & (INMUON)" % self.config
        _Fakemu = FilterDesktop( Code = _code )
        return _Fakemu               

    def _pionFilter( self ):
        _code = "(P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                   "& (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) &  (PIDK< %(PionPIDKTight)s) &(TRGHOSTPROB < %(GhostProb)s) " % self.config
        _pi = FilterDesktop( Code = _code )
        return _pi

    def _kaonFilter( self ):
        _code = "(PIDK> %(KaonPIDK)s) & (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                   "    & (TRGHOSTPROB < %(GhostProb)s)" % self.config
        _ka = FilterDesktop( Code = _code )
        return _ka     

    def _protonFilter( self ):
        _code = "(PIDp> %(ProtonPIDp)s) & (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s) & (P>2.0*GeV) & (PT > %(KPiPT)s *MeV)"\
                   "    & (TRGHOSTPROB < %(GhostProb)s)" % self.config
        _pr = FilterDesktop( Code = _code )
        return _pr 
           
    def _D02KPiFilter( self ):
        _decayDescriptors = [ '[D0 -> K- pi+]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2) > %(DSumPT)s *MeV) " % self.config
        _motherCut = "(SUMTREE( PT,  ISBASIC )> %(DSumPT)s * MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.config
        _d02kpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _d02kpi
  
    
    def _D02K3PiFilter( self ):
        _decayDescriptors = [ '[D0 -> K- pi+ pi- pi+]cc' ]
        _combinationCut = "(ADAMASS('D0') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(DSumPT)s *MeV) " % self.config
        _motherCut = "(SUMTREE( PT,  ISBASIC )> %(DSumPT)s * MeV) &(ADMASS('D0') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.config
        _d02k3pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _d02k3pi
  

    def _Dp2K2PiFilter( self ):
        _decayDescriptors = [ '[D+ -> K- pi+ pi+]cc' ]
        _combinationCut = "(ADAMASS('D+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(DSumPT)s *MeV) " % self.config
        _motherCut = "(SUMTREE( PT,  ISBASIC )> %(DSumPT)s * MeV) &(ADMASS('D+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.config
        _dp2k2pi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _dp2k2pi


    def _Dsp2KKPiFilter( self ):
        _decayDescriptors = [ '[D_s+ -> K- K+ pi+]cc' ]
        _combinationCut = "(ADAMASS('D_s+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(DSumPT)s *MeV) " % self.config
        _motherCut = "(SUMTREE( PT,  ISBASIC )> %(DSumPT)s * MeV) &(ADMASS('D_s+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.config
        _dsp2kkpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _dsp2kkpi

    def _LcpKPiFilter( self ):
        _decayDescriptors = [ '[Lambda_c+ -> p+ K- pi+]cc' ]
        _combinationCut = "(ADAMASS('Lambda_c+') < %(DsAMassWin)s *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(DSumPT)s *MeV) " % self.config
        _motherCut = "(SUMTREE( PT,  ISBASIC )> %(DSumPT)s * MeV) &(ADMASS('Lambda_c+') < %(DsMassWin)s *MeV) & (VFASPF(VCHI2/VDOF) < %(DsVCHI2DOF)s) " \
                            "& (BPVVDCHI2 > %(DsFDCHI2)s) &  (BPVDIRA> %(DsDIRA)s)"  % self.config
        _lcpkpi = CombineParticles( DecayDescriptors = _decayDescriptors,
                                    CombinationCut = _combinationCut,
                                    MotherCut = _motherCut)                            
        return _lcpkpi


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




def makeFakeb2DMuX(name,
               DecayDescriptors,
               FakeMuSel,
               DSel,
               BVCHI2DOF,
               BDIRA):
               #due to tight cuts, B sideband adds virtually zero events, free
    _combinationCut = "(AM<10.2*GeV)"
    _motherCut = "  (MM<10.0*GeV) & (MM>0.0*GeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)  " \
                   " "  % locals()
    _FakeB = CombineParticles(DecayDescriptors = DecayDescriptors,
                          CombinationCut = _combinationCut,
                          MotherCut = _motherCut)
                          
    return Selection (name,
                      Algorithm = _FakeB,
                      RequiredSelections = [FakeMuSel, DSel])

