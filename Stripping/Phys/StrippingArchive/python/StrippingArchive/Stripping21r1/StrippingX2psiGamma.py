'''
Module for selecting J/psi, Psi(2S) Gamma, both conv and unconv
Cuts taken from the chi_c production ratio paper 
'''

__author__=['Jibo He']
__date__ = '22/02/2012'
__version__= '$Revision: 1.0 $'


__all__ = (
    'X2psiGammaConf'
    )

config_default =  {
    'MuonPT'           :     0.7 , # GeV
    'MuonP'            :     0.  , # GeV
    'MuonPIDmu'        :    -5.  , 
    'MuonTRCHI2DOF'    :     5.  ,
    'JpsiPT'           :     3.  , # GeV
    'JpsiMassCuts'     :  "((in_range(3.04*GeV,MM,3.14*GeV)) | (in_range(3.635*GeV,MM,3.735*GeV)))",
    'JpsiVCHI2PDOF'    :    20.  ,
    'GammaCuts'        :  "(PT>0.6*GeV) & (P>5*GeV) & (CL>0.5)" ,
    'CombMinMass'      :     3.0 , # GeV, no Vtx fit
    'CombMaxMass'      :     5.0   # GeV, no Vtx fit
    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class X2psiGammaConf(LineBuilder):
    
    __configuration_keys__ = (
        'MuonPT',
        'MuonP',
        'MuonPIDmu',
        'MuonTRCHI2DOF',
        'JpsiPT',
        'JpsiMassCuts',
        'JpsiVCHI2PDOF',
        'GammaCuts',
        'CombMinMass',
        'CombMaxMass'
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
        Gamma 
        """
        self.GammaForX = self.createSubSel( OutputList = "GammaForX" + self.name,
                                           InputList =  DataOnDemand( Location = 'Phys/StdLooseAllPhotons/Particles' ), 
                                           Cuts = config['GammaCuts'] 
                                           )
        
        
        self.makeX2psiGamma()

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
                                     ReFitPVs = False)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
    
    
    
    def makeX2psiGamma(self):
        X2psiGamma = self.createCombinationSel( OutputList = "X2psiGamma" + self.name,
                                                DecayDescriptor = "chi_c0(1P) -> J/psi(1S) gamma", 
                                                DaughterLists = [ self.JpsiForX,
                                                                  self.GammaForX ], 
                                                PreVertexCuts = "(in_range( %(CombMinMass)s *GeV, AM, %(CombMaxMass)s *GeV))"%self.config)
        
        X2psiGammaLine = StrippingLine( self.name + "Line",
                                        algos = [ X2psiGamma ] )
        
        self.registerLine(X2psiGammaLine)


