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
    'JpsiMassCuts'     :  "((in_range(3.04*GeV,MM,3.14*GeV)) | (in_range(3.635*GeV,MM,3.735*GeV)))",
    'JpsiVCHI2PDOF'    :    20.  ,
    'PionCuts'         :  "(PT>0.5*GeV) & (TRCHI2DOF<5)" ,
    'KaonCuts'         :  "(PT>0.5*GeV) & (TRCHI2DOF<5)" ,
    'CombMinMass'      :     3.5 , # GeV, before Vtx fit
    'CombMaxMass'      :     5.1 , # GeV, before Vtx fit
    'XVCHI2PDOF'       :    16.  ,
    'MinMass'          :     3.6 , # GeV, after Vtx fit
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
        'JpsiMassCuts',
        'JpsiVCHI2PDOF',
        'PionCuts',
        'KaonCuts',
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
                                           Cuts = config['JpsiMassCuts'] + " & " + "(VFASPF(VCHI2PDOF)<%(JpsiVCHI2PDOF)s)"\
                                           " & (PT > %(JpsiPT)s *GeV)"\
                                           " & (MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *GeV)"\
                                           " & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *GeV)"\
                                           " & (MINTREE('mu+'==ABSID,PIDmu) > %(MuonPIDmu)s )"\
                                           " & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % self.config )
        
        """
        Pion 
        """
        self.PionForX = self.createSubSel( OutputList = "PionForX" + self.name,
                                            InputList =  DataOnDemand( Location = 'Phys/StdAllNoPIDsPions/Particles' ), 
                                            Cuts = config['PionCuts'] 
                                            )

        """
        Kaon
        """
        self.KaonForX = self.createSubSel( OutputList = "KaonForX" + self.name,
                                           InputList =  DataOnDemand( Location = 'Phys/StdAllLooseKaons/Particles' ), 
                                           Cuts = config['KaonCuts'] 
                                           )

                
        self.makeX2psiPiPi()
        self.makeX2psiPiPiSS()
        self.makeX2psiKK()
        self.makeX2psiKKSS()
        

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
        
        X2psiPiPiLine = StrippingLine( self.name + "PiPiLine",
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
        
        X2psiPiPiSSLine = StrippingLine( self.name + "PiPiSSLine",
                                          algos = [ X2psiPiPiSS ],
                                          prescale = self.config["SSPrescale"] )
        
        self.registerLine(X2psiPiPiSSLine)


    def makeX2psiKK(self):
        X2psiKK = self.createCombinationSel( OutputList = "X2psiKK" + self.name,
                                             DecayDescriptor = " psi(2S) -> J/psi(1S) K+ K-", 
                                             DaughterLists = [ self.JpsiForX,
                                                               self.KaonForX ], 
                                             PreVertexCuts = "(in_range( %(CombMinMass)s *GeV, AM, %(CombMaxMass)s *GeV))" % self.config,
                                             PostVertexCuts = "(in_range( %(MinMass)s *GeV, MM, %(MaxMass)s *GeV))"\
                                             " & (VFASPF(VCHI2PDOF)<%(XVCHI2PDOF)s )" %self.config )
        
        X2psiKKLine = StrippingLine( self.name + "KKLine",
                                        algos = [ X2psiKK ] )
        
        self.registerLine(X2psiKKLine)


    def makeX2psiKKSS(self):
        X2psiKKSS = self.createCombinationSel( OutputList = "X2psiKKSS" + self.name,
                                               DecayDescriptor = "[ psi(2S) -> J/psi(1S) K+ K+ ]cc", 
                                               DaughterLists = [ self.JpsiForX,
                                                                 self.KaonForX ], 
                                               PreVertexCuts = "(in_range( %(CombMinMass)s *GeV, AM, %(CombMaxMass)s *GeV))" % self.config,
                                               PostVertexCuts = "(in_range( %(MinMass)s *GeV, MM, %(MaxMass)s *GeV))"\
                                               " & (VFASPF(VCHI2PDOF)<%(XVCHI2PDOF)s )" %self.config )
        
        X2psiKKSSLine = StrippingLine( self.name + "KKSSLine",
                                       algos = [ X2psiKKSS ],
                                       prescale = self.config["SSPrescale"] )
        
        self.registerLine(X2psiKKSSLine)
