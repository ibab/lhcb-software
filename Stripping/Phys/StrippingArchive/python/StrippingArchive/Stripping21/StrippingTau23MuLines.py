'''
Module for construction of tau -->MuMuMu stripping selections and lines

Exported symbols (use python help!):
   - 
'''

__author__ = ['Johannes Albrecht', 'Jon Harrison']
__date__ = '25/08/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('Tau23MuLinesConf',
           'default_config',
           'makeTau23Mu',
           'makeDs23PiTIS',
           'makeDs23Pi',
           'makeDsPhiPi',
           'makeTau25Mu',
           'makeTau2PMuMu'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.PhysicalConstants import c_light

default_config = {
    'NAME'        : 'Tau23Mu',
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'Tau23MuLinesConf',
    'CONFIG'      : {
       'TauPrescale'         :1.,
       'TauPostscale'        :1.,
       'Ds23PiTISPrescale'   :0.0,
       'Ds23PiPrescale'      :0.0,
       'Ds2PhiPiPrescale'    :1.,
       'Tau25Prescale'       :1.,
       'Tau2PMuMuPrescale' :1.
       },
    'STREAMS'     : { 'Dimuon' : ['StrippingTau23MuTau23MuLine','StrippingTau23MuDs2PhiPiLine','StrippingTau23MuTau2PMuMuLine','StrippingTau23MuDs23PiLine','StrippingTau23MuTau25MuLine']}
    }



class Tau23MuLinesConf(LineBuilder) :
    """
    Builder 

 
    """

    __configuration_keys__ = (    'TauPrescale',
                                  'TauPostscale',#universal for all lines
                                  'Ds23PiTISPrescale',
                                  'Ds23PiPrescale',
                                  'Ds2PhiPiPrescale',
                                  'Tau25Prescale',
                                  'Tau2PMuMuPrescale'
                                  )

    
    
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
        tau2pmm_name=name+'Tau2PMuMu'

        

        self.selTau23Mu = makeTau23Mu(tau_name)
        #self.selDs23PiTIS = makeDs23PiTIS(self,ds23PiTIS_name)
        self.selDs23Pi = makeDs23Pi(ds23Pi_name)
        self.selDs2PhiPi = makeDs2PhiPi(ds2PhiPi_name)
        self.selTau25Mu = makeTau25Mu(tau25_name)
        self.selTau2PMuMu = makeTau2pmm(tau2pmm_name)


        self.tau23MuLine = StrippingLine(tau_name+"Line",
                                     prescale = config['TauPrescale'],
                                     postscale = config['TauPostscale'],
                                     MDSTFlag = True,
                                     RequiredRawEvents = ["Calo"],
                                     algos = [ self.selTau23Mu ],
                                     RelatedInfoTools = [{ 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.,
                                                                 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPT', 'CONEPTASYM'],
                                                                 'RecursionLevel' : 1,
                                                                 'Location':'ConeIsoInfo' },
                                                               {'Type': 'RelInfoVertexIsolation',
                                                                'Location':'VtxIsoInfo' },
                                                               { 'Type': 'RelInfoTrackIsolationBDT',
                                                                 'RecursionLevel' : 2,
                                                                 'Variables' : 0,
                                                                 'Locations': {'Phys/StdAllLooseMuons' : ["MuonTrackIsoBDTInfo1","MuonTrackIsoBDTInfo2","MuonTrackIsoBDTInfo3"]},
                                                               }
                                                               ]
                                     )
        
        #self.ds23PiTISLine = StrippingLine(ds23PiTIS_name+"Line",
        #                              prescale = config['Ds23PiTISPrescale'],
        #                              postscale = config['TauPostscale'],
        #                              algos = [ self.selDs23PiTIS ]
        #                              )

        self.ds23PiLine = StrippingLine(ds23Pi_name+"Line",
                                      prescale = config['Ds23PiPrescale'],
                                      postscale = config['TauPostscale'],
                                      MDSTFlag = True,
                                      RequiredRawEvents = [ ],
                                      algos = [ self.selDs23Pi ]
                                      )
        
        self.ds2PhiPiLine = StrippingLine(ds2PhiPi_name+"Line",
                                      prescale = config['Ds2PhiPiPrescale'],
                                      postscale = config['TauPostscale'],
                                      MDSTFlag = True,
                                      RequiredRawEvents = ["Calo"],
                                      algos = [ self.selDs2PhiPi ],
                                      RelatedInfoTools = [{ 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.,
                                                                 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPT', 'CONEPTASYM'],
                                                                 'RecursionLevel' : 1,
                                                                 'Location':'ConeIsoInfo' },
                                                               {'Type': 'RelInfoVertexIsolation',
                                                                'Location':'VtxIsoInfo' },
                                                               { 'Type': 'RelInfoTrackIsolationBDT',
                                                                 'RecursionLevel' : 2,
                                                                 'Variables' : 0,
                                                                 'Locations': {'Phys/StdAllLooseMuons' : ["MuonTrackIsoBDTInfo1","MuonTrackIsoBDTInfo2"],
                                                                               'Phys/StdAllLoosePions' : ["PionTrackIsoBDTInfo"]},
                                                                }
                                                               ]
                                      )

        self.tau25MuLine = StrippingLine(tau25_name+"Line",
                                     prescale = config['Tau25Prescale'],
                                     postscale = config['TauPostscale'],
                                     MDSTFlag = True,
                                     RequiredRawEvents = [ ],
                                     algos = [ self.selTau25Mu ]
                                     )

        self.tau2PMuMuLine = StrippingLine(tau2pmm_name+"Line",
                                           prescale = config['Tau2PMuMuPrescale'],
                                           postscale = config['TauPostscale'],
                                           MDSTFlag = True,
                                           RequiredRawEvents = ["Calo"],
                                           algos = [ self.selTau2PMuMu ] ,
                                           RelatedInfoTools = [{ 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.,
                                                                 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPT', 'CONEPTASYM'],
                                                                 'RecursionLevel' : 1,
                                                                 'Location':'ConeIsoInfo' },
                                                               {'Type': 'RelInfoVertexIsolation',
                                                                'Location':'VtxIsoInfo' },
                                                               { 'Type': 'RelInfoTrackIsolationBDT',
                                                                 'RecursionLevel' : 2,
                                                                 'Variables' : 0,
                                                                 'Locations': {'Phys/StdAllLooseMuons' : ["MuonTrackIsoBDTInfo1","MuonTrackIsoBDTInfo2"],
                                                                               'Phys/StdAllLooseProtons' : ["ProtonTrackIsoBDTInfo"]},
                                                                }
                                                               ]
                                           )
        
        self.registerLine(self.tau23MuLine)
        #self.registerLine(self.ds23PiTISLine)
        self.registerLine(self.ds23PiLine)
        self.registerLine(self.ds2PhiPiLine)
        self.registerLine(self.tau25MuLine)
        self.registerLine(self.tau2PMuMuLine)
        

def makeTau23Mu(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    Tau2MuMuMu = CombineParticles("Comine"+name)
    Tau2MuMuMu.DecayDescriptor = " [ tau+ -> mu+ mu+ mu- ]cc"
    Tau2MuMuMu.DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < 0.3 ) & ( TRCHI2DOF < 3  ) "\
                                 "& ( BPVIPCHI2 () >  9 ) "}
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
    
    Ds2PiPiPi = CombineParticles("Comine"+name)
    Ds2PiPiPi.DecayDescriptor = " [ D_s+  -> pi+ pi+ pi- ]cc " 
    Ds2PiPiPi.DaughtersCuts = { "pi+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < 0.3 ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) " }
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
    
    Ds2PhiPi = CombineParticles("Comine"+name)
    Ds2PhiPi.DecayDescriptor =   " [ D_s+  -> pi+  mu+ mu- ]cc "
    Ds2PhiPi.DaughtersCuts = { "pi+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < 0.3 ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) " ,
                               "mu+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < 0.3 ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) " }

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
    
    Tau2MuMuMuMuMu = CombineParticles("Comine"+name)
    Tau2MuMuMuMuMu.DecayDescriptor = " [ tau+ -> mu+ mu+ mu+ mu- mu-]cc"
    Tau2MuMuMuMuMu.DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < 0.3 ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) " }
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


def makeTau2pmm(name):
    """
    Please contact Jon Harrison if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    Tau2PMuMu = CombineParticles("Comine"+name)
    Tau2PMuMu.DecayDescriptors = [" [ tau+ -> p+ mu+ mu- ]cc"," [ tau+ -> p~- mu+ mu+ ]cc",
                                  " [ Lambda_c+ -> p+ mu+ mu- ]cc"," [ Lambda_c+ -> p~- mu+ mu+ ]cc" ]
    Tau2PMuMu.DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) "\
                                  "& ( PIDmu > -5 ) & ( (PIDmu - PIDK) > 0 ) & ( TRGHOSTPROB < 0.3 )",
                                  "p+" :  " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) "\
                                  "& (PIDp>10) & ( TRGHOSTPROB < 0.3 )"}

    Tau2PMuMu.CombinationCut = "( (ADAMASS('tau+')<150*MeV) | (ADAMASS('Lambda_c+')<150*MeV) )"

    Tau2PMuMu.MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME () * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")

    return Selection (name,
                      Algorithm = Tau2PMuMu,
                      RequiredSelections = [ _stdLooseMuons, _stdLooseProtons ])
 
