'''
(1) Bd,s->Jpsi h+ h-, h=pi or K with biased selection
(2) Bd->Jpsi K*(892) similar selection for decay time acceptance
(3) prompt Jpsi h+h- for time resolution [prescaled]
'''

__author__ = ['Liming Zhang']
__date__ = '15/10/2015'

__all__ = ('B2JpsiHHConf','default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from CommonParticles.Utils import updateDoD
from StandardParticles import StdLoosePions
from StandardParticles import StdLooseKaons
from StandardParticles import StdAllLooseKaons
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV

default_config = {
    'NAME'              : 'B2JpsiHH',
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2JpsiHHConf',
    'CONFIG'    : {
                         'TRCHI2DOF'                 :       5
                 ,       'JpsiMassWindow'            :       80
                 ,       'VCHI2PDOF'                 :       10
                 ,       'HLTCuts'                   :       "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')"
                 ,       'Bs2Jpsif0Prescale'         :       0.10     # 2011: 0.13, 2012: 0.62
                         },
    'STREAMS' : {
        'Leptonic' : [
          'StrippingB2JpsiHHBs2JpsiKstarLine',
          'StrippingB2JpsiHHBs2Jpsif0PrescaledLine'
        ],
        'Dimuon' : [
         'StrippingB2JpsiHHBs2Jpsif0Line'
        ]
    }
    }

### Lines stored in this file:

class B2JpsiHHConf(LineBuilder) :
    __configuration_keys__ = ('TRCHI2DOF',
                              'JpsiMassWindow',
                              'VCHI2PDOF',
                              'HLTCuts',
                              'Bs2Jpsif0Prescale')

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = name
        self.config = config
	
        #### Input selections:

        # ------------------------------------------------------------ J/Psi:
        
        self.WideJpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles")

        self.DetachedJpsiList = self.createSubSel( OutputList = 'NarrowDetachedJpsiForJpsiHH' + self.name,
                                           InputList = self.WideJpsiList,
                                           Cuts = "(PFUNA(ADAMASS('J/psi(1S)')) < %(JpsiMassWindow)s * MeV) & (BPVDIRA >0.9) & (BPVVD > 1.5 *mm)" %self.config)

        # ------------------------------------------------------------ Kaons:
       
        self.NoIPKaonList = self.createSubSel( OutputList = "NoIPKaonsForJpsiHH" + self.name,
                                               InputList = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles"),
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & (PIDK > 0) & (PT > 250.*MeV) & (TRGHOSTPROB < 0.8)" % self.config )

        self.NoIPPionList = self.createSubSel( OutputList = "NoIPPionsForJpsiHH" + self.name,
                                               InputList = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles"),
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & (PT > 250.*MeV) & (TRGHOSTPROB < 0.8)" % self.config )

        self.DetachedKstarList = self.createCombinationSel( OutputList = "DetachedKstarListForJpsiHH" + self.name,
                                                                 DaughterLists = [ self.NoIPKaonList, self.NoIPPionList ],
                                                                 DecayDescriptor =  "[K*(892)0 -> K+ pi-]cc" ,
                                                                 PreVertexCuts = "(ACHILD(PT,1)+ACHILD(PT,2) > 900.*MeV) & (ADAMASS('K*(892)0') < 200 *MeV) & (ADOCACHI2CUT(20., ''))",
                                                                 PostVertexCuts = "(VFASPF(VCHI2) < 16) & (BPVDIRA >0.9) & (BPVVD > 1.5 *mm)",                                                                 
                                                                 ReFitPVs = False )

        self.Detachedf0List = self.createCombinationsSel( OutputList = "Detachedf02HHForJpsiHH" + self.name,
                                                  DaughterLists = [ self.NoIPKaonList, self.NoIPPionList ],
                                                  DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+", "f_0(980) -> K+ K-"],
#                                                  DaughterCuts = { "pi+" : " (MIPCHI2DV(PaRIMARY)>9) " % self.config,
#                                                                   "K+"  : " (MIPCHI2DV(PRIMARY)>9) " % self.config },
                                                  PreVertexCuts = "(ACHILD(PT,1)+ACHILD(PT,2) > 900.*MeV) & (AM < 2700 *MeV) & (ADOCACHI2CUT(20., ''))",
                                                  PostVertexCuts = "(VFASPF(VCHI2) < 16) & (BPVDIRA >0.9) & (BPVVD > 1.5 *mm)",
                                                  ReFitPVs = False ) # Note that this is false to save CPU time, for the future check with Liming if the kaon list is fine

#unbiased f0
        self.Unbiasedf0List = self.createCombinationsSel( OutputList = "Unbiasedf02HHForJpsiHH" + self.name,
                                                  DaughterLists = [ self.NoIPKaonList, self.NoIPPionList ],
                                                  DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> K+ K-"],
                                                  PreVertexCuts = "(ACHILD(PT,1)+ACHILD(PT,2) > 900.*MeV) & (AM < 2700 *MeV) & (ADOCACHI2CUT(20., ''))",
                                                  PostVertexCuts = "(VFASPF(VCHI2) < 16)",
                                                  ReFitPVs = False ) # Note that this is false to save CPU time, for the future check with Liming if the kaon list is fine

        self.makeBs2Jpsif0()
        self.makeBs2JpsiKstar()


    ### Stripping lines:

    # ------------------------- Bs->J/Psif0:
    def makeBs2Jpsif0( self ):
        Bs2Jpsif0 = self.createCombinationSel( OutputList = "Bs2Jpsif0" + self.name,
                                   DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)",
                                   DaughterLists  = [ self.DetachedJpsiList, self.Detachedf0List ],
                                   PreVertexCuts = "in_range(5100,AM,5720)",
	                           PostVertexCuts = "in_range(5150,M,5670) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s ) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" % self.config ) # for the other particles is 10.

        Bs2Jpsif0Line  = StrippingLine( self.name + "Bs2Jpsif0Line", algos = [ Bs2Jpsif0 ], EnableFlavourTagging = False )
        
        Bs2Jpsif0Prescaled = self.createCombinationSel( OutputList = "Bs2Jpsif0Prescale" + self.name,
                                   DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)",
                                   DaughterLists  = [ self.WideJpsiList, self.Unbiasedf0List ],
                                   PreVertexCuts = "in_range(5170,AM,5470)",
	                           PostVertexCuts = "(VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s )" % self.config ) # for the other particles is 10.

        Bs2Jpsif0PrescaledLine  = StrippingLine( self.name + "Bs2Jpsif0PrescaledLine", algos = [ Bs2Jpsif0Prescaled ], MDSTFlag = True, EnableFlavourTagging = False, HLT2 = self.config['HLTCuts'], prescale = self.config["Bs2Jpsif0Prescale"]  )

        self.registerLine(Bs2Jpsif0Line)
        self.registerLine(Bs2Jpsif0PrescaledLine)

    # -------------------------- Bs->J/PsiK*:
    def makeBs2JpsiKstar( self ):
        Bs2JpsiKstar      = self.createCombinationSel( OutputList = "Bs2JpsiKstar" + self.name,
                                DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc",
                                DaughterLists  = [ self.DetachedJpsiList, self.DetachedKstarList ],
                                PreVertexCuts = "in_range(4900,AM,5600)",
                                PostVertexCuts = "in_range(5000,M,5500) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" %self.config )

        Bs2JpsiKstarLine = StrippingLine( self.name + "Bs2JpsiKstarLine", algos = [ Bs2JpsiKstar ], MDSTFlag = True, EnableFlavourTagging = True )

        self.registerLine(Bs2JpsiKstarLine)

    ### Common tools:

    def createSubSel( self, OutputList, InputList, Cuts ) :
        '''create a selection using a FilterDesktop'''
        filter = FilterDesktop(Code = Cuts)
        return Selection( OutputList,
                      Algorithm = filter,
                      RequiredSelections = [ InputList ] )

    def createCombinationSel( self, OutputList,
                          DecayDescriptor,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL",
                          ReFitPVs = True ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = ReFitPVs)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
        
    def createCombinationsSel( self, OutputList,
	                  DecayDescriptors,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL",
                          ReFitPVs = True ) :
        '''For taking in multiple decay descriptors'''
        combiner = CombineParticles( DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = ReFitPVs)
        return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)

    def filterTisTos( self, name, DiMuonInput, myTisTosSpecs ) :
        from Configurables import TisTosParticleTagger
    
        myTagger = TisTosParticleTagger(name + "_TisTosTagger")
        myTagger.TisTosSpecs = myTisTosSpecs
        
        # To speed it up, TisTos only with tracking system:
        myTagger.ProjectTracksToCalo = False
        myTagger.CaloClustForCharged = False
        myTagger.CaloClustForNeutral = False
        myTagger.TOSFrac = { 4:0.0, 5:0.0 }
        
        return Selection(name + "_SelTisTos",
                     Algorithm = myTagger,
                     RequiredSelections = [ DiMuonInput ] )

