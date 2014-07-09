__author__ = ['Carlos Vazquez Sierra']
__date__ = '10/07/2014'
# -----------------------------------------------------------------------------------------------------------------------------------------
# Stripping line for BsJpsiK* analysis: MC PIDCalib purposes. 
# Lines: Bs2JpsiKstarWideLineNoPID, Bs2JpsiKstarWideLineNoCuts.
# -----------------------------------------------------------------------------------------------------------------------------------------
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions
from StandardParticles import StdNoPIDsKaons
from StandardParticles import StdLoosePions
from StandardParticles import StdLooseKaons
from GaudiKernel.SystemOfUnits import MeV
# -----------------------------------------------------------------------------------------------------------------------------------------
default_config = {} # Empty dictionary, just because of code requirements.
class Bs2JpsiKstNoPIDConf(LineBuilder) :
    __configuration_keys__ = ()
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = name # Not used.
	self.config = config # Not used.
        # Input daughter lists:
        self.WideJpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles")
        self.JpsiList = self.createSubSel( OutputList = "NarrowJpsiListForBsJpsiKstarWideNoPID",
                                           InputList = self.WideJpsiList,
                                           Cuts = "(PFUNA(ADAMASS('J/psi(1S)')) < 80 * MeV)" )
        self.KstarWideListNoPID = self.createCombinationsSel( OutputList = "KstarWideListNoPIDForBsJpsiKstarWideNoPID",
                                                              DaughterLists = [ StdNoPIDsKaons, StdNoPIDsPions ],
                                                              DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc","[K*_0(1430)0 -> K+ pi-]cc" ],
                                                              DaughterCuts = { "pi-" : " (PT > 500 *MeV) & (TRGHOSTPROB < 0.8)",
                                                                               "K+"  : " (PT > 500 *MeV) & (TRGHOSTPROB < 0.8)"},
                                                              PreVertexCuts = "(in_range(750,AM,1900))  & (ADOCACHI2CUT(30, ''))",
                                                              PostVertexCuts = "(VFASPF(VCHI2) < 25)",
                                                              ReFitPVs = False )
        self.KstarWideListNoCuts = self.createCombinationsSel( OutputList = "KstarWideListNoCutsForBsJpsiKstarWideNoCuts",
                                                              DaughterLists = [ StdLooseKaons, StdLoosePions ],
                                                              DecayDescriptors = [ "[K*(892)0 -> K+ pi-]cc","[K*_0(1430)0 -> K+ pi-]cc" ],
                                                              ReFitPVs = False )
        self.makeBs2JpsiKstarWideNoPID() # Making the line.
        self.makeBs2JpsiKstarWideNoCuts() # Making the line.
    # ---------------------------------------------------------------------------------------------------------------------------------
    def createSubSel( self, OutputList, InputList, Cuts ) :
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
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = ReFitPVs)
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
        '''For taking in multiple decay descriptors'''
        combiner = CombineParticles( DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = ReFitPVs)
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
    def makeBs2JpsiKstarWideNoCuts( self ): # Line maker.
        Bs2JpsiKstarWideNoCuts = self.createCombinationSel( OutputList = "Bs2JpsiKstarWideNoCuts",
                                DecayDescriptor = "[B_s~0 -> J/psi(1S) K*(892)0]cc",
                                DaughterLists  = [ self.WideJpsiList, self.KstarWideListNoCuts ],
        Bs2JpsiKstarWideLineNoCuts = StrippingLine( "Bs2JpsiKstarWideLineNoCuts", algos = [Bs2JpsiKstarWideNoCuts])
        self.registerLine(Bs2JpsiKstarWideLineNoCuts)
    # ---------------------------------------------------------------------------------------------------------------------------------
