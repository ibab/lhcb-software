'''
Module for selecting Bbbar->PhiPhi, including two lines:
1. Prompt line, with tight PT, PID cuts, requiring Hlt Tis, since there is no lifetime unbiased phi trigger yet.
2. Detached line, with loose PT, PID cuts, but with IPS cuts on Kaons. 

To include it:

from StrippingSelections.StrippingBbbar2PhiPhi import Bbbar2PhiPhiConf
from StrippingSelections.StrippingBbbar2PhiPhi import config_default as config_Bbbar2PhiPhi
confBbbar2PhiPhi = Bbbar2PhiPhiConf( name = 'Bbbar2PhiPhi', config = config_Bbbar2PhiPhi )
stream.appendLines( confBbbar2PhiPhi.lines() )
'''

__author__=['Simone Stracka']
__date__ = '10/03/2015'
__all__ = (
    'Bbbar2PhiPhiConf'
    )

default_config = {
    'NAME'        : 'Bbbar2PhiPhi', 
    'BUILDERTYPE' : 'Bbbar2PhiPhiConf',
    'CONFIG' : {
        'TRCHI2DOF'        :     3.  ,
        'KaonPIDK'         :     0.  ,
        'KaonPT'           :   650.  , # MeV
        'PhiVtxChi2'       :     9.  ,
        'PhiMassW'         :    30.  , 
        'CombMaxMass'      : 10700.  , # MeV, before Vtx fit
        'CombMinMass'      :  8800.  , # MeV, before Vtx fit
        'MaxMass'          : 10600.  , # MeV, after Vtx fit
        'MinMass'          :  8900.  , # MeV, after Vtx fit
        'Phi_TisTosSpecs'  : { "Hlt1Global%TIS" : 0 },
        },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'     : [ 'BandQ' ]
    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdNoPIDsProtons, StdNoPIDsPions, StdNoPIDsDownPions

class Bbbar2PhiPhiConf(LineBuilder):
    
    __configuration_keys__ = (
        'TRCHI2DOF',
        'KaonPIDK',
        'KaonPT',
        'PhiVtxChi2',
        'PhiMassW', 
        'CombMaxMass',
        'CombMinMass',
        'MaxMass',
        'MinMass',
        'Phi_TisTosSpecs',
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config


        """
        Unbiased, require Hlt Tis 
        """
        self.PhiForJpsiList = self.createSubSel( OutputList = "PhiFor" + self.name,
                                                 InputList =  DataOnDemand( Location = 'Phys/StdTightPhi2KK/Particles' ), 
                                                 Cuts = "(VFASPF(VCHI2/VDOF)<%(PhiVtxChi2)s)"\
                                                 " & (ADMASS('phi(1020)')<%(PhiMassW)s*MeV)"\
                                                 " & (INTREE( ('K+'==ID) & (PT> %(KaonPT)s*MeV) & (TRCHI2DOF < %(TRCHI2DOF)s) & (PIDK> %(KaonPIDK)s) ))"\
                                                 " & (INTREE( ('K-'==ID) & (PT> %(KaonPT)s*MeV) & (TRCHI2DOF < %(TRCHI2DOF)s) & (PIDK> %(KaonPIDK)s) ))" % self.config )

        self.TisPhiForJpsiList = self.filterTisTos( "TisPhiFor" + self.name ,
                                                    PhiInput = self.PhiForJpsiList,
                                                    myTisTosSpecs = config['Phi_TisTosSpecs']
                                                    )

        self.makeJpsi2PhiPhi()
        
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
                              PreVertexCuts = "AALL",
                              PostVertexCuts = "ALL" ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = True)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
    
    
    
    def makeJpsi2PhiPhi(self):
        Jpsi2PhiPhi = self.createCombinationSel( OutputList = "Jpsi2PhiPhi" + self.name,
                                                 DecayDescriptor = " J/psi(1S) -> phi(1020) phi(1020)", 
                                                 DaughterLists = [ self.TisPhiForJpsiList ], 
                                                 PreVertexCuts = "(in_range( %(CombMinMass)s *MeV, AM, %(CombMaxMass)s *MeV))" % self.config,
                                                 PostVertexCuts = "(in_range( %(MinMass)s *MeV, MM, %(MaxMass)s *MeV)) & (VFASPF(VCHI2PDOF) < 16 )" %self.config )
        
        Jpsi2PhiPhiLine = StrippingLine( self.name + "Line",
                                         algos = [ Jpsi2PhiPhi ] )
        
        self.registerLine(Jpsi2PhiPhiLine)
        
        
    def filterTisTos(self, name,
                     PhiInput,
                     myTisTosSpecs ) :
        from Configurables import TisTosParticleTagger
        
        myTagger = TisTosParticleTagger(name + "_TisTosTagger")
        myTagger.TisTosSpecs = myTisTosSpecs
        
        # To speed it up, TisTos only with tracking system)
        myTagger.ProjectTracksToCalo = False
        myTagger.CaloClustForCharged = False
        myTagger.CaloClustForNeutral = False
        myTagger.TOSFrac = { 4:0.0, 5:0.0 }
        
        return Selection(name + "_SelTisTos",
                         Algorithm = myTagger,
                         RequiredSelections = [ PhiInput ] )
    
