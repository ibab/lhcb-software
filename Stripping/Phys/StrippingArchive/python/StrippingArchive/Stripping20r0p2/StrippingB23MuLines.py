'''
Module for construction of B+ -> mu mu mu lines

Exported symbols (use python help!):
   - 
'''

__author__ = ['Oliver Gruenberg']
__date__ = '19.03.2013'
__version__ = '$Revision: 1.0 $'

#############################################################################

__all__ = ('B23MuLinesConf',
           'config_default',
           'makeB23Mu',
           'makeB2DMu',           
           )

#############################################################################

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig
from GaudiKernel.PhysicalConstants import c_light

#############################################################################

class B23MuLinesConf(LineBuilder) :
    """
    Builder
    
    """

    __configuration_keys__ = (    'Postscale',
                                  'B23MuPrescale',
                                  'B2DMuPrescale',
                                  )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Postscale'           :1,
        'B23MuPrescale'       :1,
        'B2DMuPrescale'       :1,
        }                
    
    
    def __init__(self, name = 'B23Mu', config = None) :

        LineBuilder.__init__(self, name, config)
        
        B23Mu_name=name+'B23Mu'
        B2DMu_name=name+'B2DMu'

        self.selB23Mu = makeB23Mu(B23Mu_name)
        self.selB2DMu = makeB2DMu(B2DMu_name)
        
#############################################################################

        self.B23Mu_Line = StrippingLine(B23Mu_name+"Line",
                                     prescale = config['B23MuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selB23Mu ]
                                     )

        self.B2DMu_Line = StrippingLine(B2DMu_name+"Line",
                                     prescale = config['B2DMuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selB2DMu ]
                                     )


#############################################################################

        self.registerLine(self.B23Mu_Line)
        self.registerLine(self.B2DMu_Line)
       
#############################################################################

BTrackCuts = "(MIPCHI2DV(PRIMARY)>25.0) & (TRCHI2DOF<3.0) & (TRGHP<0.3)"

BCombinationCut = "(ADAMASS('B+') < 2000*MeV)"\
                  " & (AMAXDOCA('') < 0.3*mm)"\

BMotherCut = "(BPVIPCHI2() < 25)"\
             " & (BPVVDCHI2 > 225)"\
             " & (VFASPF(VCHI2/VDOF)<9)"\
             " & (BPVDIRA > 0.0)"\

#############################################################################

DTrackCuts = "(MIPCHI2DV()>1.0) & (TRCHI2DOF<5.0) & (PT>1.0*GeV)"

DCombinationCut = "(ADAMASS('D+') < 110*MeV)"\

DMotherCut = "(ADMASS('D+') < 100*MeV)"\
             " & (VFASPF(VCHI2)<16)"\
             " & (BPVLTIME(16) > 0.1*ps)"\

#############################################################################

BDTrackCuts = "(MIPCHI2DV(PRIMARY)>25.0) & (TRCHI2DOF<3.0) & (TRGHP<0.3)"

BDCombinationCut = ""

BDMotherCut ="(BPVVDCHI2 > 25)"\
             " & (VFASPF(VCHI2/VDOF)<9)"\
             " & (BPVDIRA > 0.0)"\
              
#############################################################################

def makeB23Mu(name):
    
    B23Mu = CombineParticles("Combine"+name)
    B23Mu.DecayDescriptors = [ "[B+ -> mu+ mu+ mu-]cc","[B+ -> mu+ mu+ mu+]cc" ]
    B23Mu.DaughtersCuts = { "mu+" : BTrackCuts }

    B23Mu.CombinationCut = BCombinationCut
     
    B23Mu.MotherCut = BMotherCut

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
    return Selection (name, Algorithm = B23Mu, RequiredSelections = [ _myMuons ])

#############################################################################

def makeB2DMu(name):

    _myMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    
    D2MuMu = CombineParticles("MyD2MuMuCombination")
    D2MuMu.DecayDescriptors = [ "[D0 -> mu+ mu-]cc","[D0 -> mu+ mu+]cc" ]
    D2MuMu.DaughtersCuts = { "mu+" : DTrackCuts }
    D2MuMu.CombinationCut = DCombinationCut
    D2MuMu.MotherCut = DMotherCut
    D2MuMuSelection = Selection ("MyD2MuMuSelection", Algorithm = D2MuMu, RequiredSelections = [ _myMuons ])

    B2DMu = CombineParticles("Combine"+name)
    B2DMu.DecayDescriptors = [ "[B+ -> D0 mu+]cc" ]
    B2DMu.DaughtersCuts = { "mu+" : BDTrackCuts }
    #B2DMu.CombinationCut = BDCombinationCut
    B2DMu.MotherCut = BDMotherCut
    return Selection (name, Algorithm = B2DMu, RequiredSelections = [ D2MuMuSelection,_myMuons ])

#############################################################################
