__author__ = ['Carlos Vazquez Sierra']
__date__ = '04/06/2014'
__version__ = '$Revision: 1.0 $'
# -----------------------------------------------------------------------------------------------------------------------------------------
# Stripping line for BsJpsiK* analysis: MC PIDCalib purposes. 
# Line name: Bs2JpsiKstarWideLineNoPID
# -----------------------------------------------------------------------------------------------------------------------------------------
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions
from StandardParticles import StdNoPIDsKaons
from GaudiKernel.SystemOfUnits import MeV
# -----------------------------------------------------------------------------------------------------------------------------------------
__all__ = ('Bs2JpsiKstNoPIDConf','default_config')
# -----------------------------------------------------------------------------------------------------------------------------------------
default_config = {'NAME' : 'Bs2JpsiKstNoPID',
                  'BUILDERTYPE' : 'Bs2JpsiKstNoPIDConf',
                  'CONFIG' : {'JpsiMassWindow' : 80,
                              'PostVertexCuts' : 25},
                  'STREAMS' : ['Leptonic'],
                  'WGs' : ['B2CC']}
# -----------------------------------------------------------------------------------------------------------------------------------------
class Bs2JpsiKstNoPIDConf(LineBuilder) :
    __configuration_keys__ = ('JpsiMassWindow', 'PostVertexCuts')
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
	self.config = config
        # Input daughter lists:
        self.WideJpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles")

        self.JpsiList = self.createSubSel( OutputList = "NarrowJpsiListForBsJpsiKstarWideNoPID",
                                           InputList = self.WideJpsiList,
                                           Cuts = "(PFUNA(ADAMASS('J/psi(1S)')) < %(JpsiMassWindow)s * MeV)" % self.config )

        self.KstarWideListNoPID = self.createCombinationsSel( OutputList = "KstarWideListNoPIDForBsJpsiKstarWideNoPID",
                                                              DaughterLists = [ StdNoPIDsKaons, StdNoPIDsPions ],
                                                              DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc","[K*_0(1430)0 -> K+ pi-]cc" ],
                                                              DaughterCuts = { "pi-" : " (PT > 500 *MeV) & (TRGHOSTPROB < 0.8)",
                                                                               "K+"  : " (PT > 500 *MeV) & (TRGHOSTPROB < 0.8)"},
                                                              PreVertexCuts = "(in_range(750,AM,1900))  & (ADOCACHI2CUT(30, ''))",
                                                              PostVertexCuts = "(VFASPF(VCHI2) < %(PostVertexCuts)s)" % self.config,
                                                              ReFitPVs = False )
        # Excluding K*_0(1430) resonance in mcMatch.
        self.KstarWideListTruth = self.createCombinationsSelTruth( OutputList = "KstarWideListNoPIDForBsJpsiKstarWideTruth",
                                       DaughterLists = [ StdNoPIDsKaons, StdNoPIDsPions ],
                                       DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc","[K*_0(1430)0 -> K+ pi-]cc" ],
                                       DaughterCuts = { "pi-" : " mcMatch('[K*(892)0 ==> K+ ^pi-]CC','Relations/Rec/ProtoP/Charged') ",
                                                        "K+"  : " mcMatch('[K*(892)0 ==> ^K+ pi-]CC','Relations/Rec/ProtoP/Charged') " }, 
                                       #DaughterCuts = { "pi-" : " mcMatch('[K*(892)0 ==> K+ ^pi-]CC||[K*_0(1430)0 ==> K+ ^pi-]CC','Relations/Rec/ProtoP/Charged') ",
                                       #                 "K+"  : " mcMatch('[K*(892)0 ==> ^K+ pi-]CC||[K*_0(1430)0 ==> K+ ^pi-]CC','Relations/Rec/ProtoP/Charged') " }, 
                                       ReFitPVs = False )

        self.KstarWideListFull = self.createCombinationsSelFull( OutputList = "KstarWideListNoPIDForBsJpsiKstarWideFull",
                                       DaughterLists = [ StdNoPIDsKaons, StdNoPIDsPions ],
                                       DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc","[K*_0(1430)0 -> K+ pi-]cc" ],
                                       ReFitPVs = False )

        self.makeBs2JpsiKstarWideNoPID() # Making the line.
        self.makeBs2JpsiKstarWideTruth() # Making the line.
        self.makeBs2JpsiKstarWideFull() # Making the line.
        self.makeBs2JpsiKstarWideFullNoPV() # Making the line.
    # ---------------------------------------------------------------------------------------------------------------------------------
    def createSubSel( self, OutputList, InputList, Cuts ="ALL" ) :
        '''create a selection using a FilterDesktop'''
        filter = FilterDesktop(Code = Cuts)
        return Selection( OutputList,
                      Algorithm = filter,
                      RequiredSelections = [ InputList ] )
    # ---------------------------------------------------------------------------------------------------------------------------------
    def createCombinationSel( self, OutputList,
                          DecayDescriptor,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL",
                          ReFitPVs = True ) :
                          #,Preambulo = [ "from LoKiPhysMC.decorators import *" , "from LoKiPhysMC.functions import mcMatch" ] ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = ReFitPVs )
                                     #,Preambulo = Preambulo )
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
   # ---------------------------------------------------------------------------------------------------------------------------------
    def createCombinationSelTruth( self, OutputList,
                          DecayDescriptor,
                          DaughterLists,
                          DaughterCuts = {} ,
                          ReFitPVs = True,
                          Preambulo = [ "from LoKiPhysMC.decorators import *" , "from LoKiPhysMC.functions import mcMatch" ] ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = "ALL",
                                     ReFitPVs = ReFitPVs,
                                     Preambulo = Preambulo )
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
    # ---------------------------------------------------------------------------------------------------------------------------------
    def createCombinationSelFull( self, OutputList,
                          DecayDescriptor,
                          DaughterLists,
                          DaughterCuts = {} ,
                          ReFitPVs = True ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = "ALL",
                                     ReFitPVs = ReFitPVs )
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
    # ---------------------------------------------------------------------------------------------------------------------------------
    def createCombinationsSel( self, OutputList,
	                  DecayDescriptors,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL",
                          ReFitPVs = True ) :
                          #,Preambulo = [ "from LoKiPhysMC.decorators import *" , "from LoKiPhysMC.functions import mcMatch" ] ) :
        '''For taking in multiple decay descriptors'''
        combiner = CombineParticles( DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = ReFitPVs )
                                 #,Preambulo = Preambulo )
        return Selection ( OutputList,
                            Algorithm = combiner,
                            RequiredSelections = DaughterLists)
    # ---------------------------------------------------------------------------------------------------------------------------------
    def createCombinationsSelTruth( self, OutputList,
	                  DecayDescriptors,
                          DaughterLists,
                          DaughterCuts = {} ,
                          ReFitPVs = True,
                          Preambulo = [ "from LoKiPhysMC.decorators import *" , "from LoKiPhysMC.functions import mcMatch" ] ) :
        '''For taking in multiple decay descriptors'''
        combiner = CombineParticles( DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = "ALL",
                                 ReFitPVs = ReFitPVs,
                                 Preambulo = Preambulo )
        return Selection ( OutputList,
                            Algorithm = combiner,
                            RequiredSelections = DaughterLists)
    # ---------------------------------------------------------------------------------------------------------------------------------
    def createCombinationsSelFull( self, OutputList,
                          DecayDescriptors,
                          DaughterLists,
                          DaughterCuts = {} ,
                          ReFitPVs = True ) :
        '''For taking in multiple decay descriptors'''
        combiner = CombineParticles( DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = "ALL",
                                 ReFitPVs = ReFitPVs )
        return Selection ( OutputList,
                            Algorithm = combiner,
                            RequiredSelections = DaughterLists)
    # ---------------------------------------------------------------------------------------------------------------------------------

    def makeBs2JpsiKstarWideNoPID( self ): # Line maker.
        Bs2JpsiKstarWideNoPID = self.createCombinationSel( OutputList = "Bs2JpsiKstarWideNoPID",
                                DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc",
                                DaughterLists  = [ self.JpsiList, self.KstarWideListNoPID ],
                                PreVertexCuts = "in_range(5100,AM,5700)",
                                PostVertexCuts = "(VFASPF(VCHI2PDOF) < 10) & (BPVDIRA >0.999) & (BPVVD > 1.5 *mm)" )
        Bs2JpsiKstarWideLineNoPID = StrippingLine( "Bs2JpsiKstarWideLineNoPID", algos = [Bs2JpsiKstarWideNoPID])
        self.registerLine(Bs2JpsiKstarWideLineNoPID)
    # ---------------------------------------------------------------------------------------------------------------------------------
    def makeBs2JpsiKstarWideTruth( self ): # Line maker.
        Bs2JpsiKstarWideTruth = self.createCombinationSelTruth( OutputList = "Bs2JpsiKstarWideTruth",
                                DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc",
                                DaughterLists  = [ self.WideJpsiList, self.KstarWideListTruth ] )
                                #,ReFitPVs = False )
                                #,DaughtersCuts = {mcMatch here for J/psi(1S) and K*(892)0})
        Bs2JpsiKstarWideLineTruth = StrippingLine( "Bs2JpsiKstarWideLineTruth", algos = [Bs2JpsiKstarWideTruth])
        self.registerLine(Bs2JpsiKstarWideLineTruth)
    # ---------------------------------------------------------------------------------------------------------------------------------
    def makeBs2JpsiKstarWideFull( self ): # Line maker.
        Bs2JpsiKstarWideFull = self.createCombinationSelFull( OutputList = "Bs2JpsiKstarWideFull",
                                DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc",
                                DaughterLists  = [ self.WideJpsiList, self.KstarWideListFull ])
                                #,DaughtersCuts = {mcMatch here for J/psi(1S) and K*(892)0})
        Bs2JpsiKstarWideLineFull = StrippingLine( "Bs2JpsiKstarWideLineFull", algos = [Bs2JpsiKstarWideFull])
        self.registerLine(Bs2JpsiKstarWideLineFull)
    # ---------------------------------------------------------------------------------------------------------------------------------
    def makeBs2JpsiKstarWideFullNoPV( self ): # Line maker.
        Bs2JpsiKstarWideFullNoPV = self.createCombinationSelFull( OutputList = "Bs2JpsiKstarWideFullNoPV",
                                DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc",
                                DaughterLists  = [ self.WideJpsiList, self.KstarWideListFull ],
                                ReFitPVs = False )
                                #,DaughtersCuts = {mcMatch here for J/psi(1S) and K*(892)0})
        Bs2JpsiKstarWideLineFullNoPV = StrippingLine( "Bs2JpsiKstarWideLineFullNoPV", algos = [Bs2JpsiKstarWideFullNoPV])
        self.registerLine(Bs2JpsiKstarWideLineFullNoPV)
    # ---------------------------------------------------------------------------------------------------------------------------------


