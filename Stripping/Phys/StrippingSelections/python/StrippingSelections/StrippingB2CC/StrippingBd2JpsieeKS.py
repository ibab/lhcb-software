__author__ = 'Ramon Niet'
__date__ = '2014/10/23'

'''
Bd->JpsieeKS stripping selection

Exports the following stripping lines
- BetaSBd2JpsieeKSPrescaledLine
- BetaSBd2JpsieeKSDetachedLine
'''

__all__ = (
    'Bd2JpsieeKSConf',
    'default_config'
    )

default_config = {
    'NAME'              : 'BetaSBd2JpsieeKS',
    'BUILDERTYPE'       : 'Bd2JpsieeKSConf',
    'CONFIG'    : {
          'BPVLTIME'              :     0.2   # ps
        , 'ElectronPT'            :   500.    # MeV
        , 'ElectronPID'           :     0.    # adimensional
        , 'ElectronTrackCHI2pDOF' :     5.    # adimensional
        , 'JpsiVertexCHI2pDOF'    :    15.    # adimensional
        , 'JpsiMassMin'           :  2500.    # MeV
        , 'JpsiMassMax'           :  3300.    # MeV
        , 'BdVertexCHI2pDOF'      :    10.    # adimensional
        , 'BdMassMin'             :  4500.    # MeV
        , 'BdMassMax'             :  6000.    # MeV
        , 'Prescale'              :     0.1   # adamenssional
        },
    'STREAMS' : [ 'Leptonic' ],
    'WGs'    : [ 'B2CC' ]
    }

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


class Bd2JpsieeKSConf(LineBuilder):

    __configuration_keys__ = (
                  'BPVLTIME'                  # ps
                , 'ElectronPT'                # MeV
                , 'ElectronPID'               # adimensional
                , 'ElectronTrackCHI2pDOF'     # adimensional
                , 'JpsiVertexCHI2pDOF'        # adimensional
                , 'JpsiMassMin'               # MeV
                , 'JpsiMassMax'               # MeV
                , 'BdVertexCHI2pDOF'          # adimensional
                , 'BdMassMin'                 # MeV
                , 'BdMassMax'                 # MeV
                , 'Prescale'                  # adimensional
                )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        # if name not set outside, set it to empty 
        if name == None:
            name = ""        
        
        self.name   = name
        self.config = config

        DiElectrons           = DataOnDemand( Location = "Phys/StdLooseDiElectron/Particles" )

        self.KsListLoose = MergedSelection( "StdLooseKsMergedForBetaS" + self.name,
                                            RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD/Particles"),
                                                                  DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")] )

        self._jpsi = FilterDesktop( Code = "   (MM > %(JpsiMassMin)s *MeV)" \
                                           " & (MM < %(JpsiMassMax)s *MeV)" \
                                           " & (MINTREE('e+'==ABSID,PIDe-PIDpi) > %(ElectronPID)s )" \
                                           " & (MINTREE('e+'==ABSID,PT) > %(ElectronPT)s *MeV)" \
                                           " & (MAXTREE('e+'==ABSID,TRCHI2DOF) < %(ElectronTrackCHI2pDOF)s)" \
                                           " & (VFASPF(VCHI2/VDOF) < %(JpsiVertexCHI2pDOF)s)" % self.config )

        self.Jpsi = Selection( "SelJpsi2eeFor"+name,
                               Algorithm = self._jpsi,
                               RequiredSelections = [DiElectrons] )
    
        self.Bd2JpsieeKSDetachedLine  = self._Bd2JpsieeKSDetachedLine( name+"Detached" )
        self.Bd2JpsieeKSPrescaledLine = self._Bd2JpsieeKSPrescaledLine( name+"Prescaled" )


        # Tests CPU time required for construction of StdLooseDiElectron
        # self.DielectronTestLine      = self._DielectronTestLine( DiElectrons, "DielectronTest", config )
        # self.registerLine( self.DielectronTestLine )

        self.registerLine( self.Bd2JpsieeKSDetachedLine )
        self.registerLine( self.Bd2JpsieeKSPrescaledLine )


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


    def _Bd2JpsieeKSDetachedLine( self, name ) :
        
        KS = self.createSubSel( OutputList = "KsForBetaS" + self.name,
                                InputList = self.KsListLoose,
                                Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)" )

        Bd2JpsieeKs = self.createCombinationSel( OutputList = "Bd2JpsiKS" + name,
                                                 DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                                 DaughterLists  = [ self.Jpsi, KS ],
                                                 PreVertexCuts = "in_range(%(BdMassMin)s,AM,%(BdMassMax)s)" % self.config,
                                                 PostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(BdVertexCHI2pDOF)s)" % self.config )

        Bd2JpsieeKsDetached = self.createSubSel( InputList = Bd2JpsieeKs,
                                                 OutputList = Bd2JpsieeKs.name() + "Detached" + name,
                                                 Cuts = "(BPVLTIME() > %(BPVLTIME)s*ps)" % self.config )
        
        Bd2JpsieeKsDetachedLine = StrippingLine( self.name + "Bd2JpsieeKSDetachedLine",
                                                 prescale = 1.0,
                                                 algos = [ Bd2JpsieeKsDetached ],
                                                 MDSTFlag = True,
                                                 EnableFlavourTagging = True )

        return Bd2JpsieeKsDetachedLine;


    def _Bd2JpsieeKSPrescaledLine( self, name ) :
        
        KS = self.createSubSel( OutputList = "KsForBetaS" + name,
                                InputList = self.KsListLoose,
                                Cuts = "(VFASPF(VCHI2)<20) & (BPVDLS>5)" )

        Bd2JpsieeKsPrescaled = self.createCombinationSel( OutputList = "Bd2JpsiKS" + name,
                                                          DecayDescriptor = "B0 -> J/psi(1S) KS0",
                                                          DaughterLists  = [ self.Jpsi, KS ],
                                                          PreVertexCuts = "in_range(%(BdMassMin)s,AM,%(BdMassMax)s)" % self.config,
                                                          PostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(BdVertexCHI2pDOF)s)" % self.config )

        Bd2JpsieeKsPrescaledLine = StrippingLine( self.name + "Bd2JpsieeKSPrescaledLine",
                                                  prescale = self.config['Prescale'],
                                                  algos = [ Bd2JpsieeKsPrescaled ],
                                                  MDSTFlag = True,
                                                  EnableFlavourTagging = True )

        return Bd2JpsieeKsPrescaledLine;
