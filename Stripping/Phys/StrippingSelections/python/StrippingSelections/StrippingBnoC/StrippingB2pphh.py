# $Id: StrippingBu2pphh.py,v 1.0 2013-12-19$
'''
Module for construction of B->pphh 
   ppKK
   pppipi
   ppKpi
'''

__author__ = ['Roberta Cardinale']
__date__ = '18/12/2013'
__version__ = '$Revision: 1.0 $'

__all__ = ('B2pphhConf',
           'default_config'
           )


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
#from Configurables import DaVinci__N4BodyDecays
from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays

from StandardParticles import StdAllNoPIDsKaons, StdAllNoPIDsPions, StdAllNoPIDsProtons
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter as VoidFilter

default_config = {
    'NAME' : 'B2pphh',
    'WGs'  : ['BnoC'],
    'BUILDERTYPE' : 'B2pphhConf',
    'CONFIG' : { 'MaxTrSIZE'             : 10000 ,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
    'CombMass12Max_kk'         : 4700.0,
    'CombMass12Max_kpi'         : 5000.0,
    'CombMass12Max_pipi'         : 5350.0,
    'd_achi2doca12'             : 20.0,
    'd_achi2doca13'             : 20.0,
    'd_achi2doca14'             : 20.0,
    'd_achi2doca23'             : 20.0,
    'd_achi2doca24'             : 20.0,
    'd_achi2doca34'             : 20.0,
    'p_PTSUM'               : 750.0,
    'p_PSUM'               : 7000.0,
    'p_PTMIN'              : 400.0,
    'p_PMIN'              : 4.0,
    'p_PROBNNpPROD'        : 0.01,
    'CombMass123Max'         : 5600.0,
    'CombMassMax'         : 5.60,
    'CombMassMin'         : 5.00,
    'MassMax'             : 5.55,
    'MassMin'             : 5.05,
    '4h_PTSUM'            : 3000.0,
    'B_DIRA'              : .9999 ,   
    '4h_AMAXDOCA'                : 0.3 ,     
    'B_CHI2'              : 30.0 ,    
    'B_PT'                 : 1000.,
    'B_MIPDV'              : 0.2,
    'PionCuts' : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 4.0) & (TRGHOSTPROB < 0.5) & (PROBNNpi > 0.05)",
    'KaonCuts' : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 3.0) & (TRGHOSTPROB < 0.5) & (PROBNNk > 0.05)",
    'ProtonCuts' : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 2.0) & (TRGHOSTPROB < 0.5) & (PROBNNp > 0.05)",
    'Prescaleppkk'  : 1.0,
    'Prescalepppipi'  : 1.0,
    'Prescaleppkpi'  : 1.0
    },
    'STREAMS' : {'Bhadron' : ['StrippingB2pphh_kkLine',
                              'StrippingB2pphh_kpiLine',
                              'StrippingB2pphh_pipiLine']}
    }


default_name = "B2pphh"

class B2pphhConf(LineBuilder) :
    __configuration_keys__ = ( 'MaxTrSIZE',      
                               'CombMass12Max_kk',
                               'CombMass12Max_kpi',
                               'CombMass12Max_pipi',
                               'd_achi2doca12',
                               'd_achi2doca13',
                               'd_achi2doca14',
                               'd_achi2doca23',
                               'd_achi2doca24',
                               'd_achi2doca34',
                               'p_PTSUM',
                               'p_PSUM',
                               'p_PTMIN',
                               'p_PMIN',
                               'p_PROBNNpPROD',
                               'CombMass123Max',
                               'CombMassMax',
                               'CombMassMin',
                               'MassMax',
                               'MassMin',
                               '4h_PTSUM',
                               'B_DIRA',
                               '4h_AMAXDOCA',
                               'B_CHI2',
                               'B_PT',
                               'B_MIPDV',
                               'PionCuts',
                               'ProtonCuts',
                               'KaonCuts',
                               'Prescaleppkk',
                               'Prescalepppipi',
                               'Prescaleppkpi'
                               )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = name
        self.config = config

        self.PionForB2pphh = self.createSubSel( OutputList = "PionFor" + self.name,
                                                InputList = DataOnDemand(Location = 'Phys/StdAllNoPIDsPions/Particles'),
                                                Cuts = config['PionCuts'])
        self.ProtonForB2pphh = self.createSubSel( OutputList = "ProtonFor" + self.name,
                                                  InputList = DataOnDemand(Location = 'Phys/StdAllNoPIDsProtons/Particles'),
                                                  Cuts = config['ProtonCuts'])

        self.KaonForB2pphh = self.createSubSel( OutputList = "KaonFor" + self.name,
                                                InputList = DataOnDemand(Location = 'Phys/StdAllNoPIDsKaons/Particles'),
                                                Cuts = config['KaonCuts'])
        
        self.makeBpppipi()
        self.makeBppkpi()
        self.makeBppkk()
        

        
    def createSubSel( self, OutputList, InputList, Cuts ) :
        '''create a selection using a FilterDesktop'''
        filter = FilterDesktop(Code = Cuts)
        return Selection( OutputList,
                          Algorithm = filter,
                          RequiredSelections = [ InputList ] )
    
    def createCombinationSel (self, OutputList,
                              DecayDescriptor,
                              DaughterLists,
                              DaughterCuts = {},
                              Combination12Cut = {},
                              Combination123Cut = {},
                              PreVertexCuts = "AALL",
                              PostVertexCuts = "ALL",
                              ReFitPVs = True) :
        
        combiner = DaVinci__N4BodyDecays( DecayDescriptor = DecayDescriptor,
                                          DaughtersCuts = DaughterCuts,
                                          MotherCut = PostVertexCuts,
                                          CombinationCut = PreVertexCuts,
                                          Combination12Cut = Combination12Cut,
                                          Combination123Cut = Combination123Cut,
                                          ReFitPVs = ReFitPVs)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
    
    


    def makeBppkk(self):
        Bppkk = self.createCombinationSel (OutputList = "ppkk" + self.name,
                                           DecayDescriptor  = "B_s0 -> p+ p~- K+ K-",
                                           DaughterLists = [self.ProtonForB2pphh, self.KaonForB2pphh],
                                           Combination12Cut = "((AM < %(CombMass12Max_kk)s*MeV) & (ASUM(PT)> %(p_PTSUM)s * MeV) & (ASUM(P)> %(p_PSUM)s*MeV) & ( (  AFUNCHILD(1,P) > %(p_PMIN)s * GeV) | ( AFUNCHILD(2,P) > %(p_PMIN)s * GeV ) ) & ( (  AFUNCHILD(1,PT) > %(p_PTMIN)s * MeV) | ( AFUNCHILD(2,PT) > %(p_PTMIN)s * MeV ) ) & (AFUNCHILD(1,PROBNNp) * AFUNCHILD(2,PROBNNp) > %(p_PROBNNpPROD)s ) & (ACHI2DOCA(1,2) < %(d_achi2doca12)s ))"%self.config,
                                           Combination123Cut = "((AM < %(CombMass123Max)s * MeV)  & (ACHI2DOCA(1,3) < %(d_achi2doca13)s )  & (ACHI2DOCA(2,3) < %(d_achi2doca23)s ))"%self.config,
                                           PreVertexCuts = " ((AMAXDOCA('LoKi::TrgDistanceCalculator') < %(4h_AMAXDOCA)s) & (in_range ( %(CombMassMin)s * GeV , AM , %(CombMassMax)s * GeV)) & (ACHI2DOCA(1,4) < %(d_achi2doca14)s )  & (ACHI2DOCA(2,4) < %(d_achi2doca24)s )  & (ACHI2DOCA(3,4) < %(d_achi2doca34)s ))" %self.config,
                                           PostVertexCuts ="((BPVDIRA > %(B_DIRA)s) & (VFASPF(VCHI2) < %(B_CHI2)s) & (SUMTREE(PT,((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-')),0.0) > %(4h_PTSUM)s*MeV) & (PT > %(B_PT)s * MeV) & (MIPDV(PRIMARY) < %(B_MIPDV)s*mm) & (in_range (%(MassMin)s * GeV , M , %(MassMax)s * GeV )))" %self.config,
                                           ReFitPVs = True)
        
        BppkkLine = StrippingLine( self.name + '_kkLine',
                                   prescale   = 1,
                                   #HLT = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))",
                                   FILTER = {"Code":"(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 200 )",
                                             "Preambulo":["from LoKiTracks.decorators import *"]},
                                   algos     = [ Bppkk ],
                                   EnableFlavourTagging = True,
                                   MDSTFlag = True)
        
        self.registerLine(BppkkLine)
        
        
        
    def makeBppkpi(self):
        Bppkpi = self.createCombinationSel (OutputList = "ppkpi" + self.name,
                                            DecayDescriptor  = "[B0 -> p+ p~- K+ pi-]cc",
                                            DaughterLists = [self.ProtonForB2pphh, self.PionForB2pphh, self.KaonForB2pphh],
                                            Combination12Cut = "((AM < %(CombMass12Max_kpi)s*MeV) & (ASUM(PT)> %(p_PTSUM)s*MeV) & (ASUM(P)> %(p_PSUM)s* MeV) & ( (  AFUNCHILD(1,P) > %(p_PMIN)s * GeV) | ( AFUNCHILD(2,P) > %(p_PMIN)s * GeV ) ) & ( (  AFUNCHILD(1,PT) > %(p_PTMIN)s * MeV) | ( AFUNCHILD(2,PT) > %(p_PTMIN)s * MeV ) ) & (AFUNCHILD(1,PROBNNp) * AFUNCHILD(2,PROBNNp) > %(p_PROBNNpPROD)s )  & (ACHI2DOCA(1,2) < %(d_achi2doca12)s ))" %self.config,
                                            Combination123Cut = "((AM < %(CombMass123Max)s * MeV)  & (ACHI2DOCA(1,3) < %(d_achi2doca13)s )  & (ACHI2DOCA(2,3) < %(d_achi2doca23)s ))" %self.config,
                                            PreVertexCuts = "((AMAXDOCA('LoKi::TrgDistanceCalculator') < %(4h_AMAXDOCA)s) & (in_range (%(CombMassMin)s * GeV , AM , %(CombMassMax)s * GeV))  & (ACHI2DOCA(1,4) < %(d_achi2doca14)s )  & (ACHI2DOCA(2,4) < %(d_achi2doca24)s )  & (ACHI2DOCA(3, 4) < %(d_achi2doca34)s ))" %self.config,
                                            PostVertexCuts ="((BPVDIRA > %(B_DIRA)s) & (VFASPF(VCHI2) < %(B_CHI2)s) & (SUMTREE(PT,((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='K+') | (ABSID=='K-') | (ABSID=='pi+') | (ABSID=='pi-')),0.0) > %(4h_PTSUM)s*MeV) & (PT > %(B_PT)s*MeV) & (MIPDV(PRIMARY) < %(B_MIPDV)s*mm) & (in_range (%(MassMin)s * GeV , M , %(MassMax)s * GeV)))" %self.config,
                                            ReFitPVs = True)         
        BppkpiLine = StrippingLine( self.name + '_kpiLine',
                                    prescale   = 1,
                                    #HLT = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))",
                                    FILTER = {"Code":"(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 200 )",
                                              "Preambulo":["from LoKiTracks.decorators import *"]},
                                    algos     = [ Bppkpi ],
                                    EnableFlavourTagging = True,
                                    MDSTFlag = True)
        
        self.registerLine(BppkpiLine)
        
    def makeBpppipi(self):
        Bpppipi = self.createCombinationSel (OutputList = "Bpppipi" + self.name,
                                             DecayDescriptor  = "B0 -> p+ p~- pi+ pi-",
                                             DaughterLists = [self.ProtonForB2pphh, self.PionForB2pphh],
                                             Combination12Cut = "((AM < %(CombMass12Max_pipi)s*MeV) & (ASUM(PT)> %(p_PTSUM)s*MeV) & (ASUM(P)> %(p_PSUM)s* MeV) & ( (  AFUNCHILD(1,P) > %(p_PMIN)s * GeV) | ( AFUNCHILD(2,P) > %(p_PMIN)s * GeV ) ) & ( (  AFUNCHILD(1,PT) > %(p_PTMIN)s * MeV) | ( AFUNCHILD(2,PT) > %(p_PTMIN)s * MeV ) ) & (AFUNCHILD(1,PROBNNp) * AFUNCHILD(2,PROBNNp) > %(p_PROBNNpPROD)s )  & (ACHI2DOCA(1,2) < %(d_achi2doca12)s ))" %self.config,
                                             Combination123Cut = "((AM < %(CombMass123Max)s * MeV) & (ACHI2DOCA(1,3) < %(d_achi2doca13)s )  & (ACHI2DOCA(2,3) < %(d_achi2doca23)s ))" %self.config,
                                             PreVertexCuts = "((AMAXDOCA('LoKi::TrgDistanceCalculator') < %(4h_AMAXDOCA)s) & (in_range( %(CombMassMin)s *GeV, AM , %(CombMassMax)s *GeV))  & (ACHI2DOCA(1,4) < %(d_achi2doca14)s )  & (ACHI2DOCA(2,4) < %(d_achi2doca24)s )  & (ACHI2DOCA(3, 4) < %(d_achi2doca34)s ))" %self.config,
                                             PostVertexCuts ="((BPVDIRA > %(B_DIRA)s) & (VFASPF(VCHI2) < %(B_CHI2)s) & (SUMTREE(PT,((ABSID=='p+') |(ABSID=='p~-') |(ABSID=='pi+') | (ABSID=='pi-')),0.0) > %(4h_PTSUM)s*MeV) & (PT > %(B_PT)s* MeV) & (MIPDV(PRIMARY) < %(B_MIPDV)s*mm) & (in_range ( %(MassMin)s * GeV , M , %(MassMax)s * GeV)))" %self.config,
                                             ReFitPVs = True)
        
        BpppipiLine = StrippingLine( self.name + '_pipiLine',
                                     prescale   = 1,
                                     #HLT = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))",
                                     FILTER = {"Code":"(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 200 )",
                                               "Preambulo":["from LoKiTracks.decorators import *"]},
                                     algos      = [ Bpppipi ],
                                     EnableFlavourTagging = True,
                                     MDSTFlag = True)
        
        self.registerLine(BpppipiLine) 
       
       
       
       
