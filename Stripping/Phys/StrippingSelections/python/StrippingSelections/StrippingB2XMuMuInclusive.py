
__author__ = 'P. Owen, K. Petridis'
__date__ = '26/06/20114'
__version__ = '$Revision: 1.0 $'

__all__ = ( 'B2XMuMuInclusiveConf' )

"""
Stripping selection for inclusively selected b->s,dmumu decays. Seperate lines for high, low and charmonium q2 regions.
"""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import  CombineParticles, FilterDesktop

from PhysSelPython.Wrappers import Selection, AutomaticData, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from LHCbKernel.Configuration import *  #check if needed
from Configurables import SubstitutePID
from Configurables import SubPIDMMFilter


#################
#
#  Define Cuts here
#
#################


defaultConfig = {

    # Incl (dimu) cuts
    'IPCHI2_LOWQ2'     :   9.0,
    'FlightChi2_LOWQ2'     :   100.0,
    'DIRA_LOWQ2'           :   -0.90,
    'VertexCHI2_LOWQ2'     :     2.0,
    'FlightChi2_HIGHQ2'     :   100.0,
    'DIRA_HIGHQ2'           :    0.99,
    'VertexCHI2_HIGHQ2'     :     4.0,
    'CCbarPrescale'  :    0.07,
    'LOWERMASS_LOWQ2'       :  1400.0, # MeV
    'UPPERMASS_LOWQ2'       :  2500.0, # MeV
    'LOWERMASS_CCbar'       :  2500.0, # MeV
    'UPPERMASS_CCbar'       :  3870.0, # MeV
    'LOWERMASS_HIGHQ2'       :  3870.0, # MeV
    'UPPERMASS_HIGHQ2'       :  5800.0, # MeV
    'CORRM_MIN'       :     0.0, # MeV
    'CORRM_MAX'       : 15000.0, # MeV
    # Track cuts
    'Track_CHI2nDOF'      :    3.0,
    'Track_GhostProb'     :    0.35,  

    # Muon cuts
    'Muon_MinIPCHI2_LOWQ2'   :    16.0,
    'Muon_PIDmu_LOWQ2'       :    2.0,
    'Muon_PIDmuK_LOWQ2'       :    2.0,
    'Muon_PT_LOWQ2'          :    800,
    'Muon_MinIPCHI2_HIGHQ2'  :    16.0,
    'Muon_PIDmu_HIGHQ2'      :    0.0,
    'Muon_PIDmuK_HIGHQ2'     :    0.0,
    'Muon_PT_HIGHQ2'         :    250,

    # Wrong sign combinations
    'WS'            :   False,

    # GEC
    'SpdMult'             :  600,
    'HLT_FILTER'             :  "HLT_PASS_RE('Hlt2DiMuonDetachedDecision')|HLT_PASS_RE('Hlt2DiMuonDetachedHeavyDecision')|HLT_PASS_RE('Hlt2SingleMuonDecision')",

      }


#################
#
#  Make line here
#
#################

defaultName = "B2XMuMuInclusive"


class B2XMuMuInclusiveConf(LineBuilder) :

    __configuration_keys__ = (
       # Incl (dimu) cuts
        'IPCHI2_LOWQ2',
        'FlightChi2_LOWQ2',
        'VertexCHI2_LOWQ2',
        'DIRA_LOWQ2',
        'FlightChi2_HIGHQ2',
        'VertexCHI2_HIGHQ2',
        'DIRA_HIGHQ2',
        'CCbarPrescale',
        'LOWERMASS_HIGHQ2',
        'UPPERMASS_HIGHQ2',
        'LOWERMASS_CCbar',
        'UPPERMASS_CCbar',
        'LOWERMASS_LOWQ2',
        'UPPERMASS_LOWQ2',
        'CORRM_MIN',
        'CORRM_MAX',

        # Track cuts
        'Track_CHI2nDOF',
        'Track_GhostProb',
        
        # Muon cuts
        'Muon_MinIPCHI2_LOWQ2',
        'Muon_PIDmu_LOWQ2',
        'Muon_PIDmuK_LOWQ2',
        'Muon_PT_LOWQ2',
        'Muon_MinIPCHI2_HIGHQ2',
        'Muon_PIDmu_HIGHQ2',
        'Muon_PIDmuK_HIGHQ2',
        'Muon_PT_HIGHQ2',

        # Choose WS combinations
        'WS',

        #GEC
        'SpdMult',
        'HLT_FILTER',

        )
    
    def __init__(self, name, config) :


        LineBuilder.__init__(self, name, config)

        self.name = name
        

        self.InclDiMuLowQ2CombCut = "(AM > %(LOWERMASS_LOWQ2)s *MeV) & " \
                                     "(AM < %(UPPERMASS_LOWQ2)s *MeV)" %config
        self.InclDiMuHighQ2CombCut = "(AM > %(LOWERMASS_HIGHQ2)s *MeV) & " \
                                     "(AM < %(UPPERMASS_HIGHQ2)s *MeV)" %config
        self.InclDiMuCut = "(BPVCORRM > %(CORRM_MIN)s *MeV) & " \
                           "(BPVCORRM < %(CORRM_MAX)s *MeV)" %config
        self.InclDiMuLowQ2Cut = "(BPVDIRA > %(DIRA_LOWQ2)s) & " \
                                "(BPVIPCHI2() > %(IPCHI2_LOWQ2)s) & " \
                                "(BPVVDCHI2 > %(FlightChi2_LOWQ2)s) & " \
                                "(VFASPF(VCHI2/VDOF) < %(VertexCHI2_LOWQ2)s) & " \
                                "(M > %(LOWERMASS_LOWQ2)s) & " \
                                "(M < %(UPPERMASS_LOWQ2)s)" %config
        self.InclDiMuHighQ2Cut = "(BPVDIRA > %(DIRA_HIGHQ2)s) & " \
                                 "(BPVVDCHI2 > %(FlightChi2_HIGHQ2)s) & " \
                                 "(VFASPF(VCHI2/VDOF) < %(VertexCHI2_HIGHQ2)s) & " \
                                 "(M > %(LOWERMASS_HIGHQ2)s) & " \
                                 "(M < %(UPPERMASS_HIGHQ2)s)" %config
        self.InclDiMuCCbarCut = "(BPVDIRA > %(DIRA_HIGHQ2)s) & " \
                                "(BPVVDCHI2 > %(FlightChi2_HIGHQ2)s) & " \
                                "(VFASPF(VCHI2/VDOF) < %(VertexCHI2_HIGHQ2)s) & " \
                                "(M > %(LOWERMASS_CCbar)s) & " \
                                " (M < %(UPPERMASS_CCbar)s)" %config

        self.TrackCuts = "(TRCHI2DOF < %(Track_CHI2nDOF)s) & (TRGHP < %(Track_GhostProb)s)" %config
        #self.TrackCuts = "(TRCHI2DOF < %(Track_CHI2nDOF)s)" %config
        
        self.MuonCutHighQ2 = self.TrackCuts + " & (MIPCHI2DV(PRIMARY) > %(Muon_MinIPCHI2_HIGHQ2)s) & " \
                                              " (PIDmu> %(Muon_PIDmu_HIGHQ2)s) & "\
                                              " (PIDmu-PIDK> %(Muon_PIDmuK_HIGHQ2)s) & "\
                                              " (PT > %(Muon_PT_HIGHQ2)s)" %config
        self.MuonCutLowQ2 = self.TrackCuts + " & (MIPCHI2DV(PRIMARY) > %(Muon_MinIPCHI2_LOWQ2)s) & " \
                                             " (PIDmu> %(Muon_PIDmu_LOWQ2)s) & " \
                                             " (PIDmu-PIDK> %(Muon_PIDmuK_LOWQ2)s) & "\
                                             " (PT > %(Muon_PT_LOWQ2)s)" %config

        #self.KstarFilterCut  = self.KstarCut + " & (INTREE(ABSID=='K+') & " + self.KaonCut + ") & (INTREE(ABSID=='pi+') & " + self.PionCut + ")"

        self.LowQ2Muons = self.__LowQ2Muons__(config)
        self.HighQ2Muons = self.__HighQ2Muons__(config)
        self.InclDimuHighQ2 = self.__InclDimuHighQ2__(config,doWS=False,
                                                      doCCbar=False)
        self.InclDimuLowQ2 = self.__InclDimuLowQ2__(config,doWS=False)
        self.InclDimuCCbar = self.__InclDimuHighQ2__(config,doWS=False,
                                                     doCCbar=True)        

        

        self.inclusive_DiMuHighQ2_line =  StrippingLine(
            self.name+"_InclDiMuHighQ2Line",
            prescale = 1,
            FILTER = {
            'Code' : " ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < %(SpdMult)s )" %config ,
            'Preambulo' : [
            "from LoKiNumbers.decorators import *", "from LoKiCore.basic import LHCb"
            ]
            },
            HLT=config["HLT_FILTER"],
            algos=[self.InclDimuHighQ2]
            )
        self.registerLine( self.inclusive_DiMuHighQ2_line )

        # inclusive dimuon line
        self.inclusive_DiMuLowQ2_line =  StrippingLine(
            self.name+"_InclDiMuLowQ2Line",
            prescale = 1,
            FILTER = {
            'Code' : " ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < %(SpdMult)s )" %config ,
            'Preambulo' : [
            "from LoKiNumbers.decorators import *", "from LoKiCore.basic import LHCb"
            ]
            },
            HLT=config["HLT_FILTER"],
            algos=[self.InclDimuLowQ2]
            )

        self.registerLine( self.inclusive_DiMuLowQ2_line )

        # inclusive dimuon line around jpsi and psi2s
        self.inclusive_DiMuCCbar_line =  StrippingLine(
            self.name+"_InclDiMuCCbarLine",
            prescale = config['CCbarPrescale'],
            FILTER = {
            'Code' : " ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < %(SpdMult)s )" %config ,
            'Preambulo' : [
            "from LoKiNumbers.decorators import *", "from LoKiCore.basic import LHCb"
            ]
            },
            HLT=config["HLT_FILTER"],
            algos=[self.InclDimuCCbar]
            )

        self.registerLine( self.inclusive_DiMuCCbar_line )

    def __LowQ2Muons__(self, conf):
        """
        Filter muons from StdLooseMuons
        """  
        _muons = AutomaticData(Location = 'Phys/StdLooseMuons/Particles')
        _filter = FilterDesktop(Code = self.MuonCutLowQ2)
        _sel = Selection("Selection_"+self.name+"_LowQ2Muons",
                         RequiredSelections = [ _muons ] ,
                         Algorithm = _filter)
        return _sel
 
    def __HighQ2Muons__(self, conf):
        """
        Filter muons from StdLooseMuons
        """  
        _muons = AutomaticData(Location = 'Phys/StdLooseMuons/Particles')
        _filter = FilterDesktop(Code = self.MuonCutHighQ2)
        _sel = Selection("Selection_"+self.name+"_HighQ2Muons",
                         RequiredSelections = [ _muons ] ,
                         Algorithm = _filter)
        return _sel
        

    def __InclDimuHighQ2__(self, conf, doWS=False, doCCbar=False):
        '''
        Create a new dimuon for high q2 inclusive B->Xmumu
        '''
        from  GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineDiMuon = CombineParticles()
        CombineDiMuon.DecayDescriptors = ["B0 -> mu- mu+"]
        sel_name="InclDiMuHighQ2"
        CombineDiMuon.MotherCut     = self.InclDiMuHighQ2Cut
        # choose
        if doCCbar == True:
            # make sure cant run WS combinations if running ccbar
            sel_name += "Jpsi"
            CombineDiMuon.CombinationCut = "AM > 0 *MeV"
            CombineDiMuon.MotherCut = self.InclDiMuCCbarCut
        if doWS == True:
            CombineDiMuon.DecayDescriptors = ["B0 -> mu- mu+", "B0 -> mu- mu-", "B0 -> mu+ mu+"]

        
        from PhysSelPython.Wrappers import Selection
        SelDiMuon = Selection("Sel_" + self.name + "_"+sel_name, Algorithm = CombineDiMuon,
                              RequiredSelections = [ self.HighQ2Muons ] )
        return SelDiMuon

    def __InclDimuLowQ2__(self, conf, doWS=False):
        '''
        Create a new dimuon for high q2 inclusive B->Xmumu
        '''
        from  GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineDiMuon = CombineParticles()
        CombineDiMuon.DecayDescriptors = ["B0 -> mu- mu+"]
        sel_name="InclDiMuLowQ2"
        #CombineDiMuon.CombinationCut = self.InclDiMuLowQ2CombCut
        CombineDiMuon.MotherCut     = self.InclDiMuCut+"&"+self.InclDiMuLowQ2Cut
        # choose
        if doWS == True:
            CombineDiMuon.DecayDescriptors = ["B0 -> mu- mu+", "B0 -> mu- mu-", "B0 -> mu+ mu+"]

        from PhysSelPython.Wrappers import Selection
        SelDiMuon = Selection("Sel_" + self.name + "_"+sel_name, Algorithm = CombineDiMuon,
                              RequiredSelections = [ self.LowQ2Muons ] )
        return SelDiMuon
    
