'''
Module for selecting J/psi, Psi(2S) Pi Pi
Cuts taken from the X(3872) paper, relaxed a little bit
Including both Opposite sign and prescaled Same sign line
'''

__author__=['Jibo He']
__date__ = '22/02/2012'
__version__= '$Revision: 1.0 $'


__all__ = (
    'X2psiPiPiConf'
    )

config_default =  {
    'MuonPT'           :     0.9 , # GeV
    'MuonP'            :     8.  , # GeV
    'MuonPIDmu'        :    -5.  , 
    'MuonTRCHI2DOF'    :     5.  ,
    'JpsiPT'           :     3.  , # GeV
    'JpsiMinMass'      :     3.04, # GeV
    'JpsiMaxMass'      :     3.14, # GeV
    'PsiMinMass'       :     3.635,# GeV
    'PsiMaxMass'       :     3.735,# GeV
    'JpsiVCHI2PDOF'    :    20.  ,
    'PionCuts'         :  "(PT>0.5*GeV) & (TRCHI2DOF<5)" ,
    'CombMinMass'      :     3.4 , # GeV, before Vtx fit
    'CombMaxMass'      :     5.1 , # GeV, before Vtx fit
    'XVCHI2PDOF'       :    16.  ,
    'MinMass'          :     3.5 , # GeV, after Vtx fit
    'MaxMass'          :     5.0 , # GeV, after Vtx fit
    'SSPrescale'       :     0.2 
    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class X2psiPiPiConf(LineBuilder):
    
    __configuration_keys__ = (
        'MuonPT',
        'MuonP',
        'MuonPIDmu',
        'MuonTRCHI2DOF',
        'JpsiPT',
        'JpsiMinMass',
        'JpsiMaxMass',
        'PsiMinMass',
        'PsiMaxMass',      
        'JpsiVCHI2PDOF',
        'PionCuts',
        'CombMinMass',
        'CombMaxMass',
        'XVCHI2PDOF',
        'MinMass',
        'MaxMass',
        'SSPrescale'
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config


        """
        Jpsi
        """
        self.JpsiForX = self.createSubSel( OutputList = "JpsiFor" + self.name,
                                           InputList =  DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' ), 
                                           Cuts = "(VFASPF(VCHI2PDOF)<%(JpsiVCHI2PDOF)s)"\
                                           " & ((in_range( %(JpsiMinMass)s *GeV, MM, %(JpsiMaxMass)s*GeV))"\
                                           "  | (in_range( %(PsiMinMass)s *GeV, MM, %(PsiMaxMass)s*GeV)))"\
                                           " & (PT > %(JpsiPT)s *GeV)"\
                                           " & (MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *GeV)"\
                                           " & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *GeV)"\
                                           " & (MINTREE('mu+'==ABSID,PIDmu) > %(MuonPIDmu)s )"\
                                           " & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % self.config )
        
        """
        Pion 
        """
        self.PionForX = self.createSubSel( OutputList = "PionForX" + self.name,
                                           InputList =  DataOnDemand( Location = 'Phys/StdNoPIDsPions/Particles' ), 
                                           Cuts = config['PionCuts'] 
                                           )
        
        
        self.makeX2psiPiPi()
        self.makeX2psiPiPiSS()

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
                              PostVertexCuts = "ALL" ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = False )
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
    
    
    
    def makeX2psiPiPi(self):
        X2psiPiPi = self.createCombinationSel( OutputList = "X2psiPiPi" + self.name,
                                                DecayDescriptor = " psi(2S) -> J/psi(1S) pi+ pi-", 
                                                DaughterLists = [ self.JpsiForX,
                                                                  self.PionForX ], 
                                                PreVertexCuts = "(in_range( %(CombMinMass)s *GeV, AM, %(CombMaxMass)s *GeV))" % self.config,
                                                PostVertexCuts = "(in_range( %(MinMass)s *GeV, MM, %(MaxMass)s *GeV))"\
                                                " & (VFASPF(VCHI2PDOF)<%(XVCHI2PDOF)s )" %self.config )
        
        X2psiPiPiLine = StrippingLine( self.name + "Line",
                                        algos = [ X2psiPiPi ] )
        
        self.registerLine(X2psiPiPiLine)


    def makeX2psiPiPiSS(self):
        X2psiPiPiSS = self.createCombinationSel( OutputList = "X2psiPiPiSS" + self.name,
                                                  DecayDescriptor = "[ psi(2S) -> J/psi(1S) pi+ pi+ ]cc", 
                                                  DaughterLists = [ self.JpsiForX,
                                                                    self.PionForX ], 
                                                  PreVertexCuts = "(in_range( %(CombMinMass)s *GeV, AM, %(CombMaxMass)s *GeV))" % self.config,
                                                  PostVertexCuts = "(in_range( %(MinMass)s *GeV, MM, %(MaxMass)s *GeV))"\
                                                  " & (VFASPF(VCHI2PDOF)<%(XVCHI2PDOF)s )" %self.config )
        
        X2psiPiPiSSLine = StrippingLine( self.name + "SSLine",
                                          algos = [ X2psiPiPiSS ],
                                          prescale = self.config["SSPrescale"] )
        
        self.registerLine(X2psiPiPiSSLine)


