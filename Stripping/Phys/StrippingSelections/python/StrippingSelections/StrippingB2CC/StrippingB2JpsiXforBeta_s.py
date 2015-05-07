# $Id: StrippingB2JpsiXforBeta_s.py,v 1.3 2010-09-05 21:31:13 wouter Exp $
'''
Module for construction of B->JpsiX roadmap lifetime unbiased 
stripping Selections and StrippingLine.
Provides functions to build Bs, Jpsi, Phi selections.
Provides class Bs2JpsiPhiConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   B2JpsiXLines
'''

__author__ = ['Greig Cowan','Juan Palacios','Francesca Dordei', 'Carlos Vazquez Sierra']
__date__ = '21/08/2014'
__version__ = '$Revision: 1.4 $'

__all__ = ('B2JpsiXforBeta_sConf','default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from CommonParticles.Utils import updateDoD
from StandardParticles import StdLoosePions
from StandardParticles import StdLooseKaons
from StandardParticles import StdAllLooseKaons
#from StandardParticles import StdLooseResolvedPi0
#from StandardParticles import StdLooseMergedPi0
from StandardParticles import StdNoPIDsPions
from StandardParticles import StdLooseProtons
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV

default_config = {
    'NAME'              : 'BetaS',
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2JpsiXforBeta_sConf',
    'CONFIG'    : {
                         'TRCHI2DOF'                 :       5
                 ,       'BPVLTIME'                  :       0.2
                 ,       'JpsiMassWindow'            :       80
                 ,       'DaughterPT'                :       1000
                 ,       'VCHI2PDOF'                 :       10
                 ,       'Jpsi2MuMuPrescale'         :       0.075   # 2011: 0.014, 2012: 0.075
                 ,       'Bd2JpsiKstarPrescale'      :       0.29    # 2011: 0.065, 2012: 0.29
                 ,       'Bd2JpsiKsPrescale'         :       1.0     # 2011: 1.0, 2012: 1.0
                 ,       'Bs2JpsiPhiPrescale'        :       0.62    # 2011: 0.13, 2012: 0.62
#                 ,       'Bs2JpsiPi0Prescale'        :       0.9     # 2011: 0.185, 2012: 0.9
                 ,       'Bu2JpsiKPrescale'          :       0.2     # 2011: 0.04, 2012: 0.2
                         },
    'STREAMS' : {
        'Leptonic' : [
         'StrippingBetaSJpsi2MuMuLine',
         'StrippingBetaSBu2JpsiKPrescaledLine',
         'StrippingBetaSBs2JpsiPhiPrescaledLine',
         'StrippingBetaSBd2JpsiKstarPrescaledLine',
         'StrippingBetaSBd2JpsiKsPrescaledLine',
         'StrippingBetaSBd2JpsiKsDetachedLine',
         'StrippingBetaSBd2JpsiKsLDDetachedLine',
         'StrippingBetaSBs2JpsiKstarWideLine',
         'StrippingBetaSLambdab2JpsiLambdaUnbiasedLine',
#         'StrippingBetaSBd2JpsiPi0PrescaledLine',
#         'StrippingBetaSBd2JpsiPi0DetachedLine',
         'StrippingBetaSLambdab2JpsippiDetachedLine'
        ],
        'Dimuon' : [
         'StrippingBetaSBu2JpsiKDetachedLine',
         'StrippingBetaSBd2JpsiKstarDetachedLine',
         'StrippingBetaSBs2JpsiPhiDetachedLine'
        ]
    }
    }

### Lines stored in this file:
# StrippingBetaSJpsi2MuMuLine
# StrippingBetaSBu2JpsiKDetachedLine
# StrippingBetaSBu2JpsiKPrescaledLine
# StrippingBetaSBs2JpsiPhiPrescaledLine
# StrippingBetaSBs2JpsiPhiDetachedLine
# StrippingBetaSBd2JpsiKstarPrescaledLine
# StrippingBetaSBd2JpsiKstarDetachedLine
# StrippingBetaSBd2JpsiKsPrescaledLine
# StrippingBetaSBd2JpsiKsDetachedLine
# StrippingBetaSBd2JpsiKsLDDetachedLine
# StrippingBetaSBs2JpsiKstarWideLine
# StrippingBetaSLambdab2JpsiLambdaUnbiasedLine
# StrippingBetaSBd2JpsiPi0PrescaledLine - moved
# StrippingBetaSBd2JpsiPi0DetachedLine - moved
# StrippingBetaSLambdab2JpsippiDetachedLine

class B2JpsiXforBeta_sConf(LineBuilder) :
    __configuration_keys__ = ('TRCHI2DOF',
                              'BPVLTIME',
                              'JpsiMassWindow',
                              'DaughterPT',
                              'VCHI2PDOF',
                              'Jpsi2MuMuPrescale',
                              'Bd2JpsiKstarPrescale',
                              'Bd2JpsiKsPrescale',
                              'Bs2JpsiPhiPrescale',
#                              'Bs2JpsiPi0Prescale',
                              'Bu2JpsiKPrescale')

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = name
        self.config = config
	
        # Define input daughter lists for various B -> J/psi X selections:

        self.WideJpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles")

        self.JpsiList = self.createSubSel( OutputList = 'NarrowJpsiForBetaS' + self.name,
                                           InputList = self.WideJpsiList,
                                           Cuts = "(PFUNA(ADAMASS('J/psi(1S)')) < %(JpsiMassWindow)s * MeV)" % self.config)
        
        self.KaonList = self.createSubSel( OutputList = "KaonsForBetaS" + self.name,
                                           InputList = DataOnDemand(Location = "Phys/StdLooseKaons/Particles"),
                                           Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & (PIDK >-2)" % self.config )  

        self.NoIPKaonList = self.createSubSel( OutputList = "NoIPKaonsForBetaS" + self.name,
                                               InputList = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles"),
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & (PIDK > 0)" % self.config )    
        
        self.NoPIDPionList = self.createSubSel( OutputList = "NoPIDPionsForBetaS" + self.name,
                                                InputList = DataOnDemand(Location = "Phys/StdNoPIDsPions/Particles"),
                                                Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s )" % self.config )
        
        self.ProtonList = self.createSubSel( OutputList = "ProtonsForBetaS" + self.name,
                                        InputList = DataOnDemand(Location = "Phys/StdLooseProtons/Particles"),
                                        Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s )" % self.config )
        
        self.PhiList = self.createSubSel( OutputList = "Phi2KKForBetaS" + self.name,
                                          InputList = DataOnDemand(Location = "Phys/StdLoosePhi2KK/Particles"),
                                          Cuts = "(in_range(980,M,1050))" \
                                          "& (PT > 500.*MeV) " \
                                          "& (VFASPF(VCHI2) < 25)" \
                                          "& (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(TRCHI2DOF)s )" \
                                          "& (MINTREE('K+'==ABSID, PIDK) > 0)" % self.config)
        
        self.KstarList = self.createSubSel( OutputList = "Kstar2KpiForBetaS" + self.name,
                                            InputList = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi/Particles"),
                                            Cuts = "(in_range(826,M,966))" \
                                            "& (PT > 1300.*MeV) " \
                                            "& (VFASPF(VCHI2) < 25)" \
                                            "& (MAXTREE('K+'==ABSID,  TRCHI2DOF) < %(TRCHI2DOF)s )" \
                                            "& (MAXTREE('pi-'==ABSID, TRCHI2DOF) < %(TRCHI2DOF)s )" \
                                            "& (MINTREE('K+'==ABSID, PIDK) > 0)" % self.config)
        
        self.KsListLoose = MergedSelection("StdLooseKsMergedForBetaS" + self.name,
                                           RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD/Particles"),
                                                                 DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")] )

        self.KsList = self.createSubSel(OutputList = "KsForBetaS" + self.name,
                                        InputList = self.KsListLoose,
                                        Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)")

        self.KsLDList = self.createSubSel( OutputList =  "KsLDForBetaS" + self.name,
                                           InputList = DataOnDemand(Location = "Phys/StdLooseKsLD/Particles"),
                                           Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)")

        self.DetachedKstarWideList = self.createCombinationsSel( OutputList = "DetachedKstarWideListForBetaS" + self.name,
                                                                 DaughterLists = [ StdLooseKaons, StdLoosePions ],
                                                                 DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc", "[K*_0(1430)0 -> K+ pi-]cc"  ],
                                                                 DaughterCuts = { "pi-" : " (PT > 500 *MeV) & (PIDK < 0) & (TRGHOSTPROB < 0.8)",
                                                                                  "K+"  : " (PT > 500 *MeV) & (PIDK > 0) & (TRGHOSTPROB < 0.8)"},
                                                                 PreVertexCuts = "(in_range(750,AM,1900))  & (ADOCACHI2CUT(30, ''))",
                                                                 PostVertexCuts = "(VFASPF(VCHI2) < 25)",
                                                                 ReFitPVs = False )

        self.LambdaListLoose = MergedSelection("StdLooseLambdaMergedForBetaS" + self.name,
                                               RequiredSelections =  [DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles"),
                                                                      DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")])
        self.LambdaList =  self.createSubSel(OutputList = "LambdaForBetaS" + self.name,
                                             InputList = self.LambdaListLoose ,
                                             Cuts = "(MAXTREE('p+'==ABSID, PT) > 500.*MeV) & (MAXTREE('pi-'==ABSID, PT) > 100.*MeV) & (ADMASS('Lambda0') < 15.*MeV) & (VFASPF(VCHI2) < 20)")

#        self.Pi0ListLoose = MergedSelection("StdLooseCocktailPi0ForBetaS" + self.name,
#                                             RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseResolvedPi0/Particles"),
#                                                                   DataOnDemand(Location = "Phys/StdLooseMergedPi0/Particles")])

#        self.Pi0List = self.createSubSel( OutputList = "Pi0ForBetaS" + self.name,
#                                          InputList = self.Pi0ListLoose,
#                                          Cuts = "(PT > 1500.*MeV)")
        
        self.makeInclJpsi()
        self.makeBd2JpsiKsLD()
        self.makeBu2JpsiK()
        self.makeBs2JpsiPhi()
        self.makeBs2JpsiKstarWide()
        self.makeBd2JpsiKstar()
        self.makeBd2JpsiKs()
        self.makeLambdab2Jpsippi() 
        self.makeLambdab2JpsiLambda()
#        self.makeBd2JpsiPi0()

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

    def makeInclJpsi( self ):
        '''Inclusive J/psi. We keep it for as long as we can.'''
        Jpsi2MuMuForBetasLine = StrippingLine( self.name + "Jpsi2MuMuLine", algos = [ self.WideJpsiList ], HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config["Jpsi2MuMuPrescale"] )
        self.registerLine(Jpsi2MuMuForBetasLine)

    def makeBu2JpsiK( self ):
        Bu2JpsiK = self.createCombinationSel( OutputList = "Bu2JpsiK" + self.name,
                                 DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",
                                 DaughterLists = [ self.WideJpsiList, self.NoIPKaonList ],
                                 DaughterCuts  = {"K+": "(PT > 500.*MeV)" },
                                 PreVertexCuts = "in_range(5050,AM,5550)",
                                 PostVertexCuts = "in_range(5150,M,5450) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config )

        Bu2JpsiKPrescaledLine = StrippingLine( self.name + "Bu2JpsiKPrescaledLine", algos = [ Bu2JpsiK ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config["Bu2JpsiKPrescale"], EnableFlavourTagging = True, MDSTFlag = True )

        Bu2JpsiKDetached = self.createSubSel( InputList = Bu2JpsiK, OutputList = Bu2JpsiK.name() + "Detached" + self.name,
                                              Cuts = "(CHILD('Beauty -> ^J/psi(1S) X', PFUNA(ADAMASS('J/psi(1S)'))) < %(JpsiMassWindow)s * MeV) & "\
                                              "(BPVLTIME() > %(BPVLTIME)s*ps) & "\
                                              "(MINTREE('K+'==ABSID, PT) > 500.*MeV)" % self.config )

        Bu2JpsiKDetachedLine  = StrippingLine( self.name + "Bu2JpsiKDetachedLine", algos = [ Bu2JpsiKDetached ], EnableFlavourTagging = True )
    
        self.registerLine(Bu2JpsiKDetachedLine)
        self.registerLine(Bu2JpsiKPrescaledLine)

    def makeBs2JpsiPhi( self ):
        Bs2JpsiPhi = self.createCombinationSel( OutputList = "Bs2JpsiPhi" + self.name,
                                   DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                                   DaughterLists  = [ self.WideJpsiList, self.PhiList ],
                                   PreVertexCuts = "in_range(5050,AM,5650)",
                                   #PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config )
				                           PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < 20)" % self.config ) # for the other particles is 10.
				   
        Bs2JpsiPhiPrescaledLine = StrippingLine( self.name + "Bs2JpsiPhiPrescaledLine", algos = [ Bs2JpsiPhi ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bs2JpsiPhiPrescale'], MDSTFlag = True, EnableFlavourTagging = True )

        Bs2JpsiPhiDetached = self.createSubSel( InputList = Bs2JpsiPhi,
                                                OutputList = Bs2JpsiPhi.name() + "Detached" + self.name,
                                                Cuts = "(CHILD('Beauty -> ^J/psi(1S) X', PFUNA(ADAMASS('J/psi(1S)'))) < %(JpsiMassWindow)s * MeV) & "\
                                                "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )

        Bs2JpsiPhiDetachedLine  = StrippingLine( self.name + "Bs2JpsiPhiDetachedLine", algos = [ Bs2JpsiPhiDetached ], EnableFlavourTagging = True )
        
        self.registerLine(Bs2JpsiPhiPrescaledLine)
        self.registerLine(Bs2JpsiPhiDetachedLine)

    def makeBd2JpsiKstar( self ):
        Bd2JpsiKstar = self.createCombinationSel( OutputList = "Bd2JpsiKstar" + self.name,
                                     DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc",
                                     DaughterLists  = [ self.WideJpsiList, self.KstarList ],
                                     PreVertexCuts = "in_range(5050,AM,5550)",
                                     #PostVertexCuts = "in_range(5150,M,5450) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config)
                                     PostVertexCuts = "in_range(5150,M,5450) & (VFASPF(VCHI2PDOF) < 20)" % self.config) # for the other particles is 10.

        Bd2JpsiKstarPrescaledLine = StrippingLine( self.name + "Bd2JpsiKstarPrescaledLine", algos = [ Bd2JpsiKstar ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bd2JpsiKstarPrescale'], MDSTFlag = True, EnableFlavourTagging = True )


        Bd2JpsiKstarDetached = self.createSubSel( InputList = Bd2JpsiKstar,
                                                  OutputList = Bd2JpsiKstar.name() + "Detached" + self.name,
                                                  Cuts = "(CHILD('Beauty -> ^J/psi(1S) X', PFUNA(ADAMASS('J/psi(1S)'))) < %(JpsiMassWindow)s * MeV) & "\
                                                  "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )

        Bd2JpsiKstarDetachedLine  = StrippingLine( self.name + "Bd2JpsiKstarDetachedLine",
                                          algos = [ Bd2JpsiKstarDetached ], EnableFlavourTagging = True  )

        self.registerLine(Bd2JpsiKstarPrescaledLine)
        self.registerLine(Bd2JpsiKstarDetachedLine)

    def makeBd2JpsiKs( self ):
        Bd2JpsiKs = self.createCombinationSel( OutputList = "Bd2JpsiKS" + self.name,
                                  DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                  DaughterLists  = [ self.WideJpsiList, self.KsList ],
                                  PreVertexCuts = "in_range(5000,AM,5650)",
                                  PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                  )

        Bd2JpsiKsPrescaledLine = StrippingLine( self.name + "Bd2JpsiKsPrescaledLine", algos = [ Bd2JpsiKs ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bd2JpsiKsPrescale'], MDSTFlag = True, EnableFlavourTagging = True )

        Bd2JpsiKsDetached = self.createSubSel( InputList = Bd2JpsiKs,
                                               OutputList = Bd2JpsiKs.name() + "Detached" + self.name,
                                               Cuts = "(CHILD('Beauty -> ^J/psi(1S) X', PFUNA(ADAMASS('J/psi(1S)'))) < %(JpsiMassWindow)s * MeV) & "\
                                               "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config)

        Bd2JpsiKsDetachedLine = StrippingLine( self.name + "Bd2JpsiKsDetachedLine",
                                        algos = [ Bd2JpsiKsDetached ], MDSTFlag = True, EnableFlavourTagging = True  )

        self.registerLine(Bd2JpsiKsPrescaledLine)
        self.registerLine(Bd2JpsiKsDetachedLine)

    def makeBd2JpsiKsLD( self ):
        Bd2JpsiKsLD = self.createCombinationSel( OutputList = "Bd2JpsiKSLD" + self.name,
                                  DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                  DaughterLists  = [ self.JpsiList, self.KsLDList ],
                                  PreVertexCuts = "in_range(5000,AM,5650)",
                                  PostVertexCuts = "in_range(5150,M,5550) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                  )
        

        Bd2JpsiKsLDDetached = self.createSubSel( InputList = Bd2JpsiKsLD,
                                    OutputList = Bd2JpsiKsLD.name() + "Detached" + self.name,
                                                 Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config)

        Bd2JpsiKsLDDetachedLine = StrippingLine( self.name + "Bd2JpsiKsLDDetachedLine",
                                                 algos = [ Bd2JpsiKsLDDetached ], MDSTFlag = True, EnableFlavourTagging = True  )

        self.registerLine(Bd2JpsiKsLDDetachedLine)
   
    def makeBs2JpsiKstarWide( self ):
        Bs2JpsiKstarWide      = self.createCombinationSel( OutputList = "Bs2JpsiKstarWide" + self.name,
                                DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc",
                                DaughterLists  = [ self.JpsiList, self.DetachedKstarWideList ],
                                PreVertexCuts = "in_range(5100,AM,5700)",
                                PostVertexCuts = "(VFASPF(VCHI2PDOF) < 10) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" )

        Bs2JpsiKstarWideLine = StrippingLine( self.name + "Bs2JpsiKstarWideLine", algos = [ Bs2JpsiKstarWide ], MDSTFlag = True, EnableFlavourTagging = True )

        self.registerLine(Bs2JpsiKstarWideLine)
        
    def makeLambdab2JpsiLambda( self ):
        Lambdab2JpsiLambda = self.createCombinationSel( OutputList = "Lambdab2JpsiLambda" + self.name,
                                    DecayDescriptor = "[Lambda_b0 -> Lambda0 J/psi(1S) ]cc",
                                    DaughterLists  = [ self.JpsiList, self.LambdaList ],
                                    PreVertexCuts = "in_range(5020,AM,6220)",
                                    PostVertexCuts = "in_range(5120,M,6120) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
                                    )

        Lambdab2JpsiLambdaUnbiasedLine = StrippingLine( self.name + "Lambdab2JpsiLambdaUnbiasedLine", algos = [ Lambdab2JpsiLambda ] )
        #Lambdab2JpsiLambdaUnbiasedLine = StrippingLine( self.name + "Lambdab2JpsiLambdaUnbiasedLine", algos = [ Lambdab2JpsiLambda ], HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')")
    
        self.registerLine(Lambdab2JpsiLambdaUnbiasedLine)

#    def makeBd2JpsiPi0( self ):
#        Bd2JpsiPi0 = self.createCombinationSel( OutputList = "Bd2JpsiPi0" + self.name,
#                                  DecayDescriptor = "B0 -> J/psi(1S) pi0",
#                                  DaughterLists  = [ self.JpsiList, self.Pi0List ],
#                                  PreVertexCuts = "in_range(4500,AM,6000)",
#                                  PostVertexCuts = "in_range(4700,M,5900) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s)" % self.config
#                                  )

#        Bd2JpsiPi0PrescaledLine = StrippingLine( self.name + "Bd2JpsiPi0PrescaledLine", algos = [ Bd2JpsiPi0 ] , HLT = "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')", prescale = self.config['Bs2JpsiPi0Prescale'], MDSTFlag = True, EnableFlavourTagging = True )

#        Bd2JpsiPi0DetachedLine  = StrippingLine( self.name + "Bd2JpsiPi0DetachedLine",
#                                            algos = [ self.createSubSel( InputList = Bd2JpsiPi0,
#                                                                         OutputList = Bd2JpsiPi0.name() + "Detached" + self.name,
#                                                                         Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )], MDSTFlag = True, EnableFlavourTagging = True )

#        self.registerLine(Bd2JpsiPi0PrescaledLine)
#        self.registerLine(Bd2JpsiPi0DetachedLine)

    def makeLambdab2Jpsippi( self ):
        PartCuts = "(PT>500*MeV) & (MIPCHI2DV(PRIMARY)>9)"
        Lambdab2Jpsippi = self.createCombinationSel( OutputList = "Lambdab2Jpsippi" + self.name,
                                  DecayDescriptor = "[Lambda_b0 -> J/psi(1S) p+ pi-]cc",
                                  DaughterCuts = { "p+" : PartCuts , "pi-" : PartCuts },
                                  DaughterLists  = [ self.JpsiList, self.NoPIDPionList, self.ProtonList ],
                                  PreVertexCuts = "in_range(4800,AM,6200)",
                                  PostVertexCuts = "in_range(4900,M,6100) & (VFASPF(VCHI2PDOF) < 5)" % self.config
                                  )

        #Lambdab2JpsippiPrescaledLine = StrippingLine( self.name + "Lambdab2JpsippiPrescaledLine", algos = [ Lambdab2Jpsippi ] , prescale = self.config['Lambdab2JpsippiPrescale'])
        Lambdab2JpsippiDetachedLine  = StrippingLine( self.name + "Lambdab2JpsippiDetachedLine",
                                            algos = [ self.createSubSel( InputList = Lambdab2Jpsippi,
                                                                         OutputList = Lambdab2Jpsippi.name() + "Detached" + self.name,
                                                                         Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )] )

        self.registerLine(Lambdab2JpsippiDetachedLine)

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

