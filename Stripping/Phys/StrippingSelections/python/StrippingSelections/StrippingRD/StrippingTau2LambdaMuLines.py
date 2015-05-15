'''
Module for construction of tau -->KKMu stripping selections and lines same sing kaons + opposite kaons


Exported symbols (use python help!):
   - 
'''

__author__ = ['Marcin Chrzaszcz']
__date__ = '14/12/2012'
__version__ = '$Revision: 1.0 $'

__all__ = ('Tau2LambdaMuLinesConf',
           'default_config',
           'makeTau2LambdaMu'
         #  'makeTau2KKMuSS',
           )

default_config={
    'NAME' : 'Tau2LambdaMu',
    'WGs'     : [ 'RD' ],
    'BUILDERTYPE' : 'Tau2LambdaMuLinesConf',
    'CONFIG' : {
        'TauPrescale'         :1.,
        'TauPostscale'        :1.,
        'Tau2LambdaMuPrescale'  :1.
        },
    'STREAMS' : ['Leptonic']
    }
    


from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig
from GaudiKernel.PhysicalConstants import c_light

class Tau2LambdaMuLinesConf(LineBuilder) :
    """
    Builder 

 
    """

    __configuration_keys__ = (    'TauPrescale',
                                  'TauPostscale',#universal for all lines
                                  'Tau2LambdaMuPrescale'
                                 # 'Tau2KKMuSSPrescale',
                                  )

    
    def __init__(self, 
                 name,
                 config) :

        LineBuilder.__init__(self, name, config)

        tau2LambdaMu_name = 'Tau2LambdaMu'
      
        
        self.selTau2LambdaMu = makeTau2LambdaMu(tau2LambdaMu_name)
     #   self.selTau2KKMuSS = makeTau2KKMuSS(tau2KKMuSS_name)



        self.tau2LambdaMuLine = StrippingLine(tau2LambdaMu_name+"Line",
                                     prescale = config['TauPrescale'],
                                     postscale = config['TauPostscale'],
                                     algos = [ self.selTau2LambdaMu ]
                                     )
        self.registerLine(self.tau2LambdaMuLine)


def makeTau2LambdaMu(name):
   


    makeLambda = CombineParticles("Lambda")
    makeLambda.DecayDescriptor =  "[Lambda0 -> p+ pi-]cc"
    makeLambda.DaughtersCuts = {"pi+": "(ISLONG) & (TRCHI2DOF < 4 ) & (TRGHOSTPROB<0.5) & ( BPVIPCHI2 () >  5 ) "\
                                 "& (PT>250*MeV) & (PIDpi - PIDK  > -5)",
                              "p+" :  " ( PT > 250 * MeV ) & ( TRCHI2DOF < 4  ) & ( BPVIPCHI2 () >  5 ) "\
                                  "& (PIDp>3) & ( TRGHOSTPROB < 0.5 )"
                             }
    
    _pions = DataOnDemand(Location='Phys/StdLoosePions/Particles')
    _protons = DataOnDemand(Location='Phys/StdLooseProtons/Particles')   
    
    makeLambda.CombinationCut =  "(ADAMASS('Lambda0')<100*MeV)"
    makeLambda.MotherCut = " ( VFASPF(VCHI2) < 10 ) & (MIPCHI2DV(PRIMARY)> 16.) & (ADMASS('Lambda0')<90*MeV)"
    
    SelLambda = Selection( name+"SelLambda",    Algorithm= makeLambda,
                        RequiredSelections=[_pions, _protons] )






    
    Tau2LambdaMu = CombineParticles("Comine"+name)
    Tau2LambdaMu.DecayDescriptors = [" [ tau+ -> Lambda0 mu+ ]cc" ]
    Tau2LambdaMu.DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3.  ) & ( BPVIPCHI2 () >  9 ) "\
                                  "& ( PIDmu > -5 ) & ( (PIDmu - PIDK) > 0 ) & ( TRGHOSTPROB < 0.3 )"
                                 }

    Tau2LambdaMu.CombinationCut = "( (ADAMASS('tau+')<150*MeV) )"

    Tau2LambdaMu.MotherCut = """
            ( VFASPF(VCHI2) < 16 ) &
            ( (BPVLTIME () * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 250 )
            """ 
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    # _stdLooseLambda1 = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")
   # _stdLooseLambda2 = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
    
    return Selection (name,
                      Algorithm = Tau2LambdaMu,
                      RequiredSelections = [ _stdLooseMuons, SelLambda ])
 


