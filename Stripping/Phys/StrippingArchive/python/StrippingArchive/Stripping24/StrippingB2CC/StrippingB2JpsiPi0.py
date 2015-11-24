__author__ = ['Maximilien Chefdeville']
__date__ = '15/05/2015'
__version__ = '$Revision: 1.1$'

__all__ = ('B2JpsiPi0Conf','default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from CommonParticles.Utils import updateDoD
from StandardParticles import StdLooseResolvedPi0
from StandardParticles import StdLooseMergedPi0
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV

default_config = {
    'NAME'              : 'BetaSPi0', # The real name of the lines is BetaS, this is a reference this for the Settings.
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2JpsiPi0Conf',
    'CONFIG'            : { 'BPVLTIME'                  :       0.2
                          , 'VCHI2PDOF'                 :       10
                          , 'JpsiMassWindow'            :       80
                          , 'HLTCuts'                   :       "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')"
                          , 'Bs2JpsiPi0Prescale'        :       1.0     # 2011: 0.185, 2012: 0.9
                          },
    'STREAMS'          : { 'Leptonic' : [ 'StrippingBetaSBd2JpsiPi0PrescaledLine' ,
                                          'StrippingBetaSBd2JpsiPi0DetachedLine'  ] }
                 }

### Lines stored in this file:
# StrippingBetaSBd2JpsiPi0PrescaledLine
# StrippingBetaSBd2JpsiPi0DetachedLine

class B2JpsiPi0Conf(LineBuilder) :
    __configuration_keys__ = ('BPVLTIME',
                              'VCHI2PDOF',
                              'JpsiMassWindow',
                              'HLTCuts',
                              'Bs2JpsiPi0Prescale')

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        #self.name = name
        self.name = "BetaS" # To avoid confusion with line names.
        self.config = config

        ### Selections:

        self.WideJpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles")

        self.JpsiList = self.createSubSel( OutputList = 'NarrowJpsiForBetaSPi0' + self.name,
                                           InputList = self.WideJpsiList,
                                           Cuts = "(PFUNA(ADAMASS('J/psi(1S)')) < %(JpsiMassWindow)s * MeV)" % self.config)

        self.Pi0ResolvedList = self.createSubSel( OutputList = "ResolvedPi0ForBetaS" + self.name,
                                                  InputList = DataOnDemand(Location = "Phys/StdLooseResolvedPi0/Particles"),
                                                  Cuts = "(PT > 1000.*MeV) & (MINTREE('gamma'==ABSID, CL) > 0.05 )")
         
        self.Pi0ListMix = MergedSelection("StdLooseCocktailPi0ForBetaS" + self.name,
                                          RequiredSelections = [self.Pi0ResolvedList,
                                                                DataOnDemand(Location = "Phys/StdLooseMergedPi0/Particles")])

        self.Pi0List = self.createSubSel( OutputList = "Pi0ForBetaS" + self.name,
                                          InputList = self.Pi0ListMix,
                                          Cuts = "(PT > 1000.*MeV)")

        self.makeBd2JpsiPi0()

    ### Stripping lines:

    def makeBd2JpsiPi0( self ):
        Bd2JpsiPi0 = self.createCombinationSel( OutputList = "Bd2JpsiPi0" + self.name,
                                                DecayDescriptor = "B0 -> J/psi(1S) pi0",
                                                DaughterLists  = [ self.JpsiList, self.Pi0List ],
                                                PreVertexCuts = "in_range(4500,AM,6000)",
                                                PostVertexCuts = "in_range(4700,M,5900) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s) & (BPVDIRA>0.99755) & (BPVIP()<0.2) & (BPVIPCHI2()<20)" % self.config )

        Bd2JpsiPi0PrescaledLine = StrippingLine( self.name + "Bd2JpsiPi0PrescaledLine",
                                                 algos = [ Bd2JpsiPi0 ] ,
                                                 HLT2 = self.config['HLTCuts'],
                                                 prescale = self.config['Bs2JpsiPi0Prescale'],
                                                 MDSTFlag = True,
                                                 EnableFlavourTagging = True )

        Bd2JpsiPi0DetachedLine  = StrippingLine( self.name + "Bd2JpsiPi0DetachedLine",
                                                 algos = [ self.createSubSel( InputList = Bd2JpsiPi0,
                                                                         OutputList = Bd2JpsiPi0.name() + "Detached" + self.name,
                                                                         Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )],
                                                 MDSTFlag = True,
                                                 EnableFlavourTagging = True )

        self.registerLine(Bd2JpsiPi0PrescaledLine)
        self.registerLine(Bd2JpsiPi0DetachedLine)

    ### Generic tools:

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
                           RequiredSelections = DaughterLists )

