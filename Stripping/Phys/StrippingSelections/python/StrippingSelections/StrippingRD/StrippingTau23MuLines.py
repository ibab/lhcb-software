'''
Module for construction of tau -->MuMuMu stripping selections and lines

Exported symbols (use python help!):
   - 
'''

__author__ = ['Jon Harrison', 'Paul Seyfert', 'Marcin Chrzaszcz']
__date__ = '05/05/2015'
__version__ = '$Revision: 3.0 $'

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
from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays as Combine3Particles
from GaudiConfUtils.ConfigurableGenerators import DaVinci__N5BodyDecays as Combine5Particles
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
       'Tau2PMuMuPrescale'   :1.,
       'TrackGhostProb'           :0.45
       },
    'STREAMS'     : { 'Leptonic' : ['StrippingTau23MuTau23MuLine','StrippingTau23MuDs2PhiPiLine','StrippingTau23MuTau2PMuMuLine','StrippingTau23MuDs23PiLine','StrippingTau23MuTau25MuLine']}
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
                                  'Tau2PMuMuPrescale',
                                  'TrackGhostProb'
                                  )

    
    
    def __init__(self, name, config):
        self.name = name
        self.__confdict__ = config


        LineBuilder.__init__(self, name, config)
        #checkConfig(Bs2MuMuLinesConf.__configuration_keys__,config)

        tau_name=name+'Tau23Mu'
        ds23PiTIS_name = name+'Ds23PiTIS'
        ds23Pi_name=name+'Ds23Pi'
        ds2PhiPi_name=name+'Ds2PhiPi'
        tau25_name=name+'Tau25Mu'
        tau2pmm_name=name+'Tau2PMuMu'

        

        self.selTau23Mu = makeTau23Mu(tau_name,config)
        #self.selDs23PiTIS = makeDs23PiTIS(self,ds23PiTIS_name)
        self.selDs23Pi = makeDs23Pi(ds23Pi_name,config)
        self.selDs2PhiPi = makeDs2PhiPi(ds2PhiPi_name,config)
        self.selTau25Mu = makeTau25Mu(tau25_name,config)
        self.selTau2PMuMu = makeTau2pmm(tau2pmm_name,config)


        self.tau23MuLine = StrippingLine(tau_name+"Line",
                                     prescale = config['TauPrescale'],
                                     postscale = config['TauPostscale'],
                                     MDSTFlag = False,
                                     RequiredRawEvents = ["Calo"],
                                     algos = [ self.selTau23Mu ],
                                     RelatedInfoTools = [{ 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.,
                                                                 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPT', 'CONEPTASYM'],
                                                                 'Location':'ConeIsoInfo',
							         'DaughterLocations':{'[tau+ -> ^mu+ mu+ mu-]CC' : 'MuonConeVarInfo1', '[tau+ -> mu+ ^mu+ mu-]CC' : 'MuonConeVarInfo2', '[tau+ -> mu+ mu+ ^mu-]CC' : 'MuonConeVarInfo3'} 
							       },
                                                               {'Type': 'RelInfoVertexIsolation',
                                                                'Location':'VtxIsoInfo' },
                                                               { 'Type': 'RelInfoTrackIsolationBDT',
                                                                 'Variables' : 0,
                                                                 'Location':'TrackIsoInfo',
							         'DaughterLocations':{'[tau+ -> ^mu+ mu+ mu-]CC' : 'MuonTrackIsoBDTInfo1', '[tau+ -> mu+ ^mu+ mu-]CC' : 'MuonTrackIsoBDTInfo2', '[tau+ -> mu+ mu+ ^mu-]CC' : 'MuonTrackIsoBDTInfo3'} 
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
                                      MDSTFlag = False,
                                      RequiredRawEvents = [ ],
                                      algos = [ self.selDs23Pi ]
                                      )
        
        self.ds2PhiPiLine = StrippingLine(ds2PhiPi_name+"Line",
                                      prescale = config['Ds2PhiPiPrescale'],
                                      postscale = config['TauPostscale'],
                                      MDSTFlag = False,
                                      RequiredRawEvents = ["Calo"],
                                      algos = [ self.selDs2PhiPi ],
                                      RelatedInfoTools = [{ 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.,
                                                                 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPT', 'CONEPTASYM'],
                                                                 'Location':'ConeIsoInfo',
							         'DaughterLocations':{'[D_s+  -> ^pi+  mu+ mu-]CC' : 'PionConeVarInfo', '[D_s+  -> pi+  ^mu+ mu-]CC' : 'MuonConeVarInfo1', '[D_s+  -> pi+  mu+ ^mu-]CC' : 'MuonConeVarInfo2'} 
                                                               },
                                                               {'Type': 'RelInfoVertexIsolation',
                                                                'Location':'VtxIsoInfo' },
                                                               { 'Type': 'RelInfoTrackIsolationBDT',
                                                                 'Variables' : 0,
                                                                 'Location':'TrackIsoInfo',
							         'DaughterLocations':{'[D_s+  -> ^pi+  mu+ mu-]CC' : 'PionTrackIsoBDTInfo', '[D_s+  -> pi+  ^mu+ mu-]CC' : 'MuonTrackIsoBDTInfo1', '[D_s+  -> pi+  mu+ ^mu-]CC' : 'MuonTrackIsoBDT_mu_2'} 

                                                                }
                                                               ]
                                      )

        self.tau25MuLine = StrippingLine(tau25_name+"Line",
                                     prescale = config['Tau25Prescale'],
                                     postscale = config['TauPostscale'],
                                     MDSTFlag = False,
                                     RequiredRawEvents = [ ],
                                     algos = [ self.selTau25Mu ]
                                     )

        self.tau2PMuMuLine = StrippingLine(tau2pmm_name+"Line",
                                           prescale = config['Tau2PMuMuPrescale'],
                                           postscale = config['TauPostscale'],
                                           MDSTFlag = False,
                                           RequiredRawEvents = ["Calo"],
                                           algos = [ self.selTau2PMuMu ] ,
                                           RelatedInfoTools = [{ 'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.,
                                                                 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPT', 'CONEPTASYM'],
                                                                 'Location':'ConeIsoInfo',
							         'DaughterLocations':{"[[ tau+ -> ^p+ mu+ mu- ]CC, [ tau+ -> ^p~- mu+ mu+ ]CC, [ Lambda_c+ -> ^p+ mu+ mu- ]CC, [ Lambda_c+ -> ^p~- mu+ mu+ ]CC]" : 'ProtonConeVarInfo', "[[ tau+ -> p+ ^mu+ mu- ]CC, [ tau+ -> p~- ^mu+ mu+ ]CC, [ Lambda_c+ -> p+ ^mu+ mu- ]CC, [ Lambda_c+ -> p~- ^mu+ mu+ ]CC]" : 'MuonConeVarInfo1', "[[ tau+ -> p+ mu+ ^mu- ]CC, [ tau+ -> p~- mu+ ^mu+ ]CC, [ Lambda_c+ -> p+ mu+ ^mu- ]CC, [ Lambda_c+ -> p~- mu+ ^mu+ ]CC]" : 'MuonConeVarInfo2'} 
                                                               },
							         
							       {'Type': 'RelInfoVertexIsolation',
                                                                'Location':'VtxIsoInfo' },
                                                               { 'Type': 'RelInfoTrackIsolationBDT',
                                                                 'Variables' : 0,
                                                                 'Location':'TrackIsoInfo',
							         'DaughterLocations':{"[[ tau+ -> ^p+ mu+ mu- ]CC, [ tau+ -> ^p~- mu+ mu+ ]CC, [ Lambda_c+ -> ^p+ mu+ mu- ]CC, [ Lambda_c+ -> ^p~- mu+ mu+ ]CC]" : 'ProtonTrackIsoBDTInfo', "[[ tau+ -> p+ ^mu+ mu- ]CC, [ tau+ -> p~- ^mu+ mu+ ]CC, [ Lambda_c+ -> p+ ^mu+ mu- ]CC, [ Lambda_c+ -> p~- ^mu+ mu+ ]CC]" : 'MuonTrackIsoBDTInfo1', "[[ tau+ -> p+ mu+ ^mu- ]CC, [ tau+ -> p~- mu+ ^mu+ ]CC, [ Lambda_c+ -> p+ mu+ ^mu- ]CC, [ Lambda_c+ -> p~- mu+ ^mu+ ]CC]" : 'MuonTrackIsoBDTInfo2'} 
                                                                }
                                                               ]
                                           )
        
        self.registerLine(self.tau23MuLine)
        #self.registerLine(self.ds23PiTISLine)
        self.registerLine(self.ds23PiLine)
        self.registerLine(self.ds2PhiPiLine)
        self.registerLine(self.tau25MuLine)
        self.registerLine(self.tau2PMuMuLine)
        

def makeTau23Mu(name, config):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    Tau2MuMuMu = Combine3Particles(\
               DecayDescriptor = " [ tau+ -> mu+ mu+ mu- ]cc",
               DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < %(TrackGhostProb)s ) & ( TRCHI2DOF < 3  ) "\
                                 "& ( BPVIPCHI2 () >  9 ) " % config},
               Combination12Cut = "AM< 2078*MeV", #"(ADAMASS('tau+')<400*MeV)" -> 1778+400  and then -100 for muon rest mass
               CombinationCut = "(ADAMASS('tau+')<400*MeV)",
               MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME () * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
            )
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = Tau2MuMuMu,
                      RequiredSelections = [ _stdLooseMuons ])

def makeDs23Pi(name, config):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    Ds2PiPiPi = Combine3Particles(\
              DecayDescriptor = " [ D_s+  -> pi+ pi+ pi- ]cc " ,
              DaughtersCuts = { "pi+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < %(TrackGhostProb)s ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) " % config},
              Combination12Cut = "AM('D_s+')<1920*MeV)", #"(ADAMASS('D_s+')<80*MeV)" -> 1970+80 = 2050 and then minus 130 for pion rest mass
              CombinationCut = "(ADAMASS('D_s+')<80*MeV)",
              MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME () * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
              )
                             
    _stdLoosePions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")

    return Selection (name,
                      Algorithm = Ds2PiPiPi,
                      RequiredSelections = [ _stdLoosePions ])

def makeDs23PiTIS(self, name, config):
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

    self.combDs2pipipi=makeDs23Pi(name, config)

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


def makeDs2PhiPi(name, config):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    Ds2PhiPi = Combine3Particles(\
             DecayDescriptor =   " [ D_s+  -> mu+  mu- pi+ ]cc ",
             DaughtersCuts = { "pi+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < %(TrackGhostProb)s ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) " % config,
                               "mu+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < %(TrackGhostProb)s ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) " % config},
             Combination12Cut = " in_range ( 970 * MeV , AM , 1070 * MeV )",
             CombinationCut = "(ADAMASS('D_s+')<250*MeV)", # & in_range ( 970 * MeV , AM12 , 1070 * MeV )"
             MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME () * c_light)   >100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
            )
                             
    _stdLoosePions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = Ds2PhiPi,
                      RequiredSelections = [ _stdLooseMuons, _stdLoosePions ])



def makeTau25Mu(name, config):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    Tau2MuMuMuMuMu = Combine5Particles(\
                   DecayDescriptor = " [ tau+ -> mu+ mu+ mu+ mu- mu-]cc",
                   DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRGHOSTPROB < %(TrackGhostProb)s ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) " % config },
                   Combination12Cut = "AM<1878*MeV",   # 1778 + 400 - 3*100
                   Combination123Cut = "AM<1978*MeV",   # 1778 + 400 - 2*100
                   Combination1234Cut = "AM<2078*MeV",   # 1778 + 400 - 1*100
                   CombinationCut = "(ADAMASS('tau+')<400*MeV)",
                   MotherCut = """
            ( VFASPF(VCHI2) < 30 ) &
            ( (BPVLTIME () * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
                   )
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = Tau2MuMuMuMuMu,
                      RequiredSelections = [ _stdLooseMuons ])


def makeTau2pmm(name, config):
    """
    Please contact Jon Harrison if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    Tau2PMuMu = Combine3Particles(\
              DecayDescriptors = [" [ tau+ -> p+ mu+ mu- ]cc"," [ tau+ -> p~- mu+ mu+ ]cc",
                                  " [ Lambda_c+ -> p+ mu+ mu- ]cc"," [ Lambda_c+ -> p~- mu+ mu+ ]cc" ],
              DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) "\
                                  "& ( PIDmu > -5 ) & ( (PIDmu - PIDK) > 0 ) & ( TRGHOSTPROB < %(TrackGhostProb)s )"% config,
                                  "p+" :  " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3  ) & ( BPVIPCHI2 () >  9 ) "\
                                  "& (PIDp>10) & ( TRGHOSTPROB < %(TrackGhostProb)s )" % config},
              Combination12Cut = "AM<2340*MeV", # lambda_c mass + 150 - muon mass = 2290 + 150 -100 = 
              CombinationCut = "( (ADAMASS('tau+')<150*MeV) | (ADAMASS('Lambda_c+')<150*MeV) )",
              MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME () * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
              )
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")

    return Selection (name,
                      Algorithm = Tau2PMuMu,
                      RequiredSelections = [ _stdLooseMuons, _stdLooseProtons ])
 
