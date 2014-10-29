'''
Module for construction of B+ -> Lambda mu+ +cc stripping selections and lines same sing kaons + opposite kaons


Exported symbols (use python help!):
   - 
'''

__author__ = ['Marcin Chrzaszcz']
__date__ = '14/12/2012'
__version__ = '$Revision: 1.0 $'

__all__ = ('B2LambdaMuLinesConf',
           'config_default',
           'makeB2LambdaMu'
         #  'makeTau2KKMuSS',
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig
from GaudiKernel.PhysicalConstants import c_light

class B2LambdaMuLinesConf(LineBuilder) :
    """
    Builder 

 
    """

    __configuration_keys__ = (    'BPrescale',
                                  'BPostscale',#universal for all lines
                                  'B2LambdaMuPrescale'
                                 # 'Tau2KKMuSSPrescale',
                                  )

    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'BPrescale'         :1.,
        'BPostscale'        :1.,
        'B2LambdaMuPrescale'  :1.
        }                
    
    
    def __init__(self, 
                 name = 'B2LambdaMu',
                 config = None) :

        LineBuilder.__init__(self, name, config)

        B2LambdaMu_name = 'b2LambdaMu'
      
        
        self.selB2LambdaMu = makeB2LambdaMu(B2LambdaMu_name)
     #   self.selTau2KKMuSS = makeTau2KKMuSS(tau2KKMuSS_name)



        self.B2LambdaMuLine = StrippingLine(B2LambdaMu_name+"Line",
                                     prescale = config['BPrescale'],
                                     postscale = config['BPostscale'],
                                     algos = [ self.selB2LambdaMu ]
                                     )
        self.registerLine(self.B2LambdaMuLine)


def makeB2LambdaMu(name):
   


    makeLambda = CombineParticles("Lambda0")
    makeLambda.DecayDescriptor =  "[Lambda0 -> p+ pi-]cc"
    makeLambda.DaughtersCuts = {"pi-": "(TRCHI2DOF < 3.5 ) & (TRGHOSTPROB < 0.5)  & (PT>300*MeV)",
                              "p+" :  " ( PT > 300 * MeV ) & ( TRCHI2DOF < 5.  ) & (PIDp>-3) & ( TRGHOSTPROB < 0.5 )"
                             }
    
    _pions = DataOnDemand(Location='Phys/StdLoosePions/Particles')
    _protons = DataOnDemand(Location='Phys/StdLooseProtons/Particles')   
    
    makeLambda.CombinationCut =  "(ADAMASS('Lambda0')<110*MeV)"
    makeLambda.MotherCut = " ( VFASPF(VCHI2/VDOF) < 20 ) & (ADMASS('Lambda0')<100*MeV)"
    
    SelLambda = Selection( name+"SelLambda",    Algorithm= makeLambda,
                        RequiredSelections=[_pions, _protons] )






    
    B2LambdaMu = CombineParticles("Comine"+name)
    B2LambdaMu.DecayDescriptors = [" [ B+ -> Lambda0 mu+ ]cc", "[B- -> Lambda0 mu- ]cc" ]
    B2LambdaMu.DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3.5) & ( PIDmu > -5 ) & ( (PIDmu - PIDK) > 0 ) & ( TRGHOSTPROB < 0.5 )"
                                 }

    B2LambdaMu.CombinationCut = "( (ADAMASS('B+')<550*MeV) )"

    B2LambdaMu.MotherCut = "(ADMASS('B+')<500*MeV) & (BPVDIRA>0.999) & (MIPCHI2DV(PRIMARY)<25) & (BPVLTCHI2()>9) " 
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    # _stdLooseLambda1 = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")
   # _stdLooseLambda2 = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
    
    return Selection (name,
                      Algorithm = B2LambdaMu,
                      RequiredSelections = [ _stdLooseMuons, SelLambda ])
 


