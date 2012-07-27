'''
Module for construction of tau -->MuMuMu stripping selections and lines

Exported symbols (use python help!):
   - 
'''

__author__ = ['Johannes Albrecht', 'Vanya Belyaev']
__date__ = '25/08/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('Tau23MuLinesConf',
           'config_default',
           'makeTau23Mu',
           'makeDs23PiTIS',
           'makeDs23Pi',
           'makeDsPhiPi',
           'makeTau25Mu'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig
from GaudiKernel.PhysicalConstants import c_light

class Tau23MuLinesConf(LineBuilder) :
    """
    Builder 

 
    """

    __configuration_keys__ = (    'TauPrescale',
                                  'TauPostscale',
                                  'Ds23PiTISPrescale',
                                  'Ds23PiTISPostscale',
                                  'Ds23PiPrescale',
                                  'Ds23PiPostscale',
                                  'Ds2PhiPiPrescale',
                                  'Ds2PhiPiPostscale',
                                  'Tau25Prescale',
                                  'Tau25Postscale',
                                  )

    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'TauPrescale'         :1,
        'TauPostscale'        :1,
        'Ds23PiTISPrescale'   :0.04,
        'Ds23PiTISPostscale'  :1,
        'Ds23PiPrescale'      :0.01,
        'Ds23PiPostscale'     :1,
        'Ds2PhiPiPrescale'    :1,
        'Ds2PhiPiPostscale'   :1,  
        'Tau25Prescale'         :1,
        'Tau25Postscale'        :1,      
        }                
    
    
    def __init__(self, 
                 name = 'Tau23Mu',
                 config = None) :

        LineBuilder.__init__(self, name, config)
        #checkConfig(Bs2MuMuLinesConf.__configuration_keys__,config)

        tau_name=name+'Tau23Mu'
        ds23PiTIS_name = name+'Ds23PiTIS'
        ds23Pi_name=name+'Ds23Pi'
        ds2PhiPi_name=name+'Ds2PhiPi'
        tau25_name=name+'Tau25Mu'

        self.selTau23Mu = makeTau23Mu(tau_name)
        self.selDs23PiTIS = makeDs23PiTIS(self,ds23PiTIS_name)
        self.selDs23Pi = makeDs23Pi(ds23Pi_name)
        self.selDs2PhiPi = makeDs2PhiPi(ds2PhiPi_name)
        self.selTau25Mu = makeTau25Mu(tau25_name)

        self.tau23MuLine = StrippingLine(tau_name+"Line",
                                     prescale = config['TauPrescale'],
                                     postscale = config['TauPostscale'],
                                     algos = [ self.selTau23Mu ]
                                     )
        
        self.ds23PiTISLine = StrippingLine(ds23PiTIS_name+"Line",
                                      prescale = config['Ds23PiTISPrescale'],
                                      postscale = config['Ds23PiTISPostscale'],
                                      algos = [ self.selDs23PiTIS ]
                                      )

        self.ds23PiLine = StrippingLine(ds23Pi_name+"Line",
                                      prescale = config['Ds23PiPrescale'],
                                      postscale = config['Ds23PiPostscale'],
                                      algos = [ self.selDs23Pi ]
                                      )
        
        self.ds2PhiPiLine = StrippingLine(ds2PhiPi_name+"Line",
                                      prescale = config['Ds2PhiPiPrescale'],
                                      postscale = config['Ds2PhiPiPostscale'],
                                      algos = [ self.selDs2PhiPi ]
                                      )

        self.tau25MuLine = StrippingLine(tau25_name+"Line",
                                     prescale = config['Tau25Prescale'],
                                     postscale = config['Tau25Postscale'],
                                     algos = [ self.selTau25Mu ]
                                     )
       
        self.registerLine(self.tau23MuLine)
        self.registerLine(self.ds23PiTISLine)
        self.registerLine(self.ds23PiLine)
        self.registerLine(self.ds2PhiPiLine)
        self.registerLine(self.tau25MuLine)


def makeTau23Mu(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Tau2MuMuMu = CombineParticles("Comine"+name)
    Tau2MuMuMu.DecayDescriptor = " [ tau+ -> mu+ mu+ mu- ]cc"
    Tau2MuMuMu.DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 4  ) & ( BPVIPCHI2 () >  9 ) " }
    Tau2MuMuMu.CombinationCut = "(ADAMASS('tau+')<400*MeV)"

    Tau2MuMuMu.MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME () * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = Tau2MuMuMu,
                      RequiredSelections = [ _stdLooseMuons ])

def makeDs23Pi(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Ds2PiPiPi = CombineParticles("Comine"+name)
    Ds2PiPiPi.DecayDescriptor = " [ D_s+  -> pi+ pi+ pi- ]cc " 
    Ds2PiPiPi.DaughtersCuts = { "pi+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 4  ) & ( BPVIPCHI2 () >  9 ) " }
    Ds2PiPiPi.CombinationCut = "(ADAMASS('D_s+')<80*MeV)"

    Ds2PiPiPi.MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME () * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
                             
    _stdLoosePions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")

    return Selection (name,
                      Algorithm = Ds2PiPiPi,
                      RequiredSelections = [ _stdLoosePions ])

def makeDs23PiTIS(self, name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    def makeTISTOS( name, _input, _trigger ) :
            from Configurables import TisTosParticleTagger
            _tisTosFilter = TisTosParticleTagger( name + "Tagger" )
            _tisTosFilter.TisTosSpecs = { _trigger : 0 }
            #_tisTosFilter.ProjectTracksToCalo = False
            #_tisTosFilter.CaloClustForCharged = False
            #_tisTosFilter.CaloClustForNeutral = False
            #_tisTosFilter.TOSFrac = { 4:0.0, 5:0.0 }
            return Selection( name
                              , Algorithm = _tisTosFilter
                              , RequiredSelections = [ _input ]
                              )        

    self.combDs2pipipi=makeDs23Pi(name)

    self.selDs23PiHlt1TIS = makeTISTOS( self.name() + "Ds23PiHlt1TIS"
                                        , self.combDs2pipipi#makeDs23Pi#self.combPiPiPi
                                        , "Hlt1.*Decision%TIS"
                                        )
    self.selDs23PiHlt2TIS = makeTISTOS( self.name() + "Ds23PiHlt2TIS"
                                        , self.selDs23PiHlt1TIS
                                        , "Hlt2.*Decision%TIS"
                                        )
       
    return self.selDs23PiHlt2TIS

#    return Selection (name,
#                      Algorithm = Ds2PiPiPiTIS,
#                      RequiredSelections = [ Ds2PiPiPi ])


def makeDs2PhiPi(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Ds2PhiPi = CombineParticles("Comine"+name)
    Ds2PhiPi.DecayDescriptor =   " [ D_s+  -> pi+  mu+ mu- ]cc "
    Ds2PhiPi.DaughtersCuts = { "pi+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 4  ) & ( BPVIPCHI2 () >  9 ) " ,
                                "mu+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 4  ) & ( BPVIPCHI2 () >  9 ) " }

    Ds2PhiPi.CombinationCut = "(ADAMASS('D_s+')<250*MeV) & in_range ( 970 * MeV , AM23 , 1070 * MeV )"

    Ds2PhiPi.MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME () * c_light)   >100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
                             
    _stdLoosePions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = Ds2PhiPi,
                      RequiredSelections = [ _stdLooseMuons, _stdLoosePions ])



def makeTau25Mu(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Tau2MuMuMuMuMu = CombineParticles("Comine"+name)
    Tau2MuMuMuMuMu.DecayDescriptor = " [ tau+ -> mu+ mu+ mu+ mu- mu-]cc"
    Tau2MuMuMuMuMu.DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 4  ) & ( BPVIPCHI2 () >  9 ) " }
    Tau2MuMuMuMuMu.CombinationCut = "(ADAMASS('tau+')<400*MeV)"

    Tau2MuMuMuMuMu.MotherCut = """
            ( VFASPF(VCHI2) < 30 ) &
            ( (BPVLTIME () * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = Tau2MuMuMuMuMu,
                      RequiredSelections = [ _stdLooseMuons ])
