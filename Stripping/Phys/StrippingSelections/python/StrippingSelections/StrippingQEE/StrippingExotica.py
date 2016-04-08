#!/usr/bin/env python

"""

StrippingLine making exotic candidates based on Hlt2Exotica.

### Exotica Lines written by Phil Ilten and Mike Williams (mwill@mit.edu)
Hlt2ExoticaDisplPhiPhi
Hlt2ExoticaSharedDiMuonNoIP
Hlt2ExoticaQuadMuonNoIP
Hlt2ExoticaDisplDiMuon
Hlt2ExoticaDisplDiMuonNoPoint
Hlt2ExoticaPrmptDiMuonTurbo
Hlt2ExoticaPrmptDiMuonSSTurbo
Hlt2ExoticaPrmptDiMuonHighMass


"""

from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, SimpleSelection, MergedSelection, AutomaticData
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine


__author__  = 'Chitsanu Khurewathanakul', 'Philip Ilten', 'Michael Williams'
__date__    = '2016-04-08'
__version__ = 1.1
__all__     = 'ExoticaConf', 'default_config'

default_config = {
  'NAME'        : 'Exotica',
  'BUILDERTYPE' : 'ExoticaConf',
  'STREAMS'     : [ 'EW'  ],
  'WGs'         : [ 'QEE' ],
  'CONFIG'      : {
    'Common': {
      'GhostProb': 0.3,
    },
    'Prescales': {
      'DisplPhiPhi'         : 1.0,
      'QuadMuonNoIP'        : 1.0,
      'DisplDiMuon'         : 1.0,
      'DisplDiMuonNoPoint'  : 1.0,
      'PrmptDiMuonHighMass' : 1.0,
    },
    'DisplPhiPhi': {
      # 'TOS_HLT2'      : 'Hlt2ExoticaDisplPhiPhiDecision', # Comment me in when needed
      'input'         : 'Phys/StdLoosePhi2KK/Particles',
      'TisTosSpec'    : "Hlt1IncPhi.*Decision",
      'KPT'           : 500*MeV,
      'KIPChi2'       : 16,
      'KProbNNk'      : 0.1,
      'PhiPT'         : 1000*MeV,
      'PhiMassWindow' : 20*MeV,
      'VChi2'         : 10,
      'FDChi2'        : 45,
    },
    'SharedDiMuonNoIP': {
      'input'       : 'Phys/StdAllLooseMuons/Particles',
      'MuPT'        : 500*MeV,
      'MuP'         : 10000*MeV,
      'MuProbNNmu'  : 0.2,
      'DOCA'        : 0.2*mm,
      'VChi2'       : 10,
    },
    'QuadMuonNoIP': {
      # 'TOS_HLT2'  : 'Hlt2ExoticaQuadMuonNoIPDecision', # Comment me in when needed
      'PT'        : 0,
      'VChi2'     : 10,
    },
    'DisplDiMuon': {
      # 'TOS_HLT2'  : 'Hlt2ExoticaDisplDiMuonDecision', # Comment me in when needed
      'MuProbNNmu': 0.2,
      'MuIPChi2'  : 16,
      'PT'        : 1000*MeV,
      'IPChi2'    : 16,
      'FDChi2'    : 30,
    },
    'DisplDiMuonNoPoint': {
      # 'TOS_HLT2'  : 'Hlt2ExoticaDisplDiMuonNoPointDecision', # Comment me in when needed
      'MuProbNNmu'  : 0.5,
      'MuIPChi2'    : 16,
      'PT'          : 1000*MeV,
      'FDChi2'      : 30,
    },
    'PrmptDiMuonHighMass': {
      # 'TOS_HLT2'  : 'Hlt2ExoticaPrmptDiMuonHighMassDecision', # Comment me in when needed
      'MuPT'      : 500*MeV,
      'MuP'       : 10000*MeV,                                                  
      'M'         : 5000*MeV,
      'MuProbNNmu': 0.99,
      'MuIPChi2'  : 6,
      'PT'        : 1000*MeV,                                  
      'FDChi2'    : 45,
    },
  },
}

#===============================================================================

def DisplPhiPhi( conf ):

  inputs = [ AutomaticData(Location=conf['input'])  ]

  dc = ("TOS('%(TisTosSpec)s','Hlt1TriggerTisTos')"
        "& (PT > %(PhiPT)s) "
        "& (MINTREE('K+'==ABSID,PT) > %(KPT)s) "
        "& (MINTREE('K+'==ABSID,BPVIPCHI2()) > %(KIPChi2)s) "
        "& (MAXTREE('K+'==ABSID,TRGHOSTPROB) < %(GhostProb)s) "
        "& (MINTREE('K+'==ABSID,PROBNNK) > %(KProbNNk)s) "
        "& (VFASPF(VCHI2PDOF) < %(VChi2)s) "
        "& (in_range( PDGM('phi(1020)') - %(PhiMassWindow)s , M , PDGM('phi(1020)') + %(PhiMassWindow)s ) )")%conf

  mc =  ("(HASVERTEX)"
         "& (VFASPF(VCHI2) < %(VChi2)s) "
         "& (BPVVDCHI2 > %(FDChi2)s)")%conf

  ## TOS on HLT2 on-demand
  if 'TOS_HLT2' in conf:
    mc += '& (TOS("%(TOS_HLT2)s", "Hlt2TriggerTisTos"))'%conf

  return SimpleSelection( 'DisplPhiPhi', CombineParticles, inputs,
    DecayDescriptor = 'B0 -> phi(1020) phi(1020)',
    DaughtersCuts   = {'phi(1020)': dc},
    MotherCut       = mc,
  )

#-------------------------------------------------------------------------------

def SharedDiMuonNoIP( conf ):

  inputs = [ AutomaticData(Location=conf['input'])  ]

  dc = ("(PT > %(MuPT)s) "
        "& (P > %(MuP)s) "
        "& (TRGHOSTPROB < %(GhostProb)s) "
        "& (PROBNNmu > %(MuProbNNmu)s) ")%conf 
  cc = "(AMAXDOCA('') < %(DOCA)s)"%conf
  mc = "(VFASPF(VCHI2PDOF) < %(VChi2)s) "%conf

  return SimpleSelection( 'SharedDiMuonNoIP', CombineParticles, inputs,
    DecayDescriptor = "KS0 -> mu+ mu-",
    DaughtersCuts   = {'mu+':dc, 'mu-':dc},
    CombinationCut  = cc,
    MotherCut       = mc,
  )


def QuadMuonNoIP( conf, sharedDiMuon ):
  cc = "APT > %(PT)s"%conf 
  mc = "(HASVERTEX) & (VFASPF(VCHI2) < %(VChi2)s)"%conf

  ## TOS on HLT2 on-demand
  if 'TOS_HLT2' in conf:
    mc += '& (TOS("%(TOS_HLT2)s", "Hlt2TriggerTisTos"))'%conf

  return SimpleSelection( 'QuadMuonNoIP', CombineParticles, sharedDiMuon,
    DecayDescriptor = 'B0 -> KS0 KS0',
    CombinationCut  = cc,
    MotherCut       = mc,
  )


def DisplDiMuon( conf, sharedDiMuon ):
  code = ("(MINTREE('mu+'==ABSID,BPVIPCHI2()) > %(MuIPChi2)s)"
            "& (MINTREE('mu+'==ABSID,PROBNNmu) > %(MuProbNNmu)s)"
            "& (PT > %(PT)s)"
            "& (HASVERTEX)"
            "& (BPVIPCHI2() < %(IPChi2)s)"
            "& (BPVVDCHI2 > %(FDChi2)s)")%conf

  ## TOS on HLT2 on-demand
  if 'TOS_HLT2' in conf:
    code += '& (TOS("%(TOS_HLT2)s", "Hlt2TriggerTisTos"))'%conf

  return SimpleSelection( 'DisplDiMuon', FilterDesktop, sharedDiMuon, Code=code)


def DisplDiMuonNoPoint( conf, sharedDiMuon ):
  code = ("(MINTREE('mu+'==ABSID,BPVIPCHI2()) > %(MuIPChi2)s)"
          "& (MINTREE('mu+'==ABSID,PROBNNmu) > %(MuProbNNmu)s)"
          "& (PT > %(PT)s)"
          "& (HASVERTEX)"
          "& (BPVVDCHI2 > %(FDChi2)s)")%conf

  ## TOS on HLT2 on-demand
  if 'TOS_HLT2' in conf:
    code += '& (TOS("%(TOS_HLT2)s", "Hlt2TriggerTisTos"))'%conf

  return SimpleSelection( 'DisplDiMuonNoPoint', FilterDesktop, sharedDiMuon, Code=code)


def PrmptDiMuonHighMass( conf, sharedDiMuon ):
  code = ('(M > %(M)s) &'
          "(MINTREE('mu+'==ABSID,PT) > %(MuPT)s) & (MINTREE('mu+'==ABSID,P) > %(MuP)s)"
          "& (MINTREE('mu+'==ABSID,BPVIPCHI2()) < %(MuIPChi2)s)"
          "& (MINTREE('mu+'==ABSID,PROBNNmu) > %(MuProbNNmu)s)"
          "& (PT > %(PT)s)"  
          "& (HASVERTEX)"
          "& (BPVVDCHI2 < %(FDChi2)s)")%conf

  ## TOS on HLT2 on-demand
  if 'TOS_HLT2' in conf:
    code += '& (TOS("%(TOS_HLT2)s", "Hlt2TriggerTisTos"))'%conf

  return SimpleSelection( 'PrmptDiMuonHighMass', FilterDesktop, sharedDiMuon, Code=code)


#===============================================================================

class ExoticaConf(LineBuilder):

  __configuration_keys__ = default_config['CONFIG'].keys()  # Legacy field

  def __init__(self, name, config):
    LineBuilder.__init__(self, name, config)

    ## Inject 'Common' into all other confs
    d = config.pop('Common')
    for key in config:
      config[key].update(d)

    ## sharedDiMuon, used in common in many lines
    sharedDiMuon = [SharedDiMuonNoIP(config['SharedDiMuonNoIP'])]

    ## Register lines
    prescales = config['Prescales']

    self.registerLine(StrippingLine( name+'DisplPhiPhiLine',
      selection = DisplPhiPhi(config['DisplPhiPhi']),
      prescale  = prescales['DisplPhiPhi'],
    ))

    self.registerLine(StrippingLine( name+'QuadMuonNoIPLine',
      selection = QuadMuonNoIP(config['QuadMuonNoIP'], sharedDiMuon),
      prescale  = prescales['QuadMuonNoIP'],
    ))

    self.registerLine(StrippingLine( name+'DisplDiMuonLine',
      selection = DisplDiMuon(config['DisplDiMuon'], sharedDiMuon),
      prescale  = prescales['DisplDiMuon'],
    ))

    self.registerLine(StrippingLine( name+'DisplDiMuonNoPointLine',
      selection = DisplDiMuonNoPoint(config['DisplDiMuonNoPoint'], sharedDiMuon),
      prescale  = prescales['DisplDiMuonNoPoint'],
    ))

    self.registerLine(StrippingLine( name+'PrmptDiMuonHighMassLine',
      selection = PrmptDiMuonHighMass(config['PrmptDiMuonHighMass'], sharedDiMuon),
      prescale  = prescales['PrmptDiMuonHighMass'],
    ))

