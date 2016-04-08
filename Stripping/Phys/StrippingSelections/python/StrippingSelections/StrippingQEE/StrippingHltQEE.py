"""

Stripping lines which pass through the HLT decisions.

Hlt2Jets by P.Ilten, M.Williams
Hlt2Exotica by P.Ilten, M.Williams

The stripping line doesn't create any candidate, just filter event passing given 
HLT decision. This is suitable in case the reconstruction strategy is not yet
mature and triggered-event is suitable as a starting point.
Only the HLT with small rate are allowed to be placed here.

"""

__author__  = 'Chitsanu Khurewathanakul'
__date__    = '08/04/2016'
__version__ = 1.0
__all__     = 'default_config', 'HltQEEConf'

from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine

default_config = {
  'HltQEEJets': {
    'BUILDERTYPE' : 'HltQEEConf',
    'WGs'         : ['QEE'],
    'STREAMS'     : ['BhadronCompleteEvent'],
    'CONFIG'      : {
      'common': {
        'checkPV'           : False,
        'RequiredRawEvents' : ['Calo'],
      },
      'lines': {
        'DiJet': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2JetsDiJetDecision')",
        },
        'DiJetSV': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2JetsDiJetSVDecision')",
        },
        'DiJetSVSV': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2JetsDiJetSVSVDecision')",
        },
        'DiJetSVMu': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2JetsDiJetSVMuDecision')",
        },
        'DiJetMuMu': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2JetsDiJetMuMuDecision')",
        },
      },
    },
  },
  'HltQEEExotica': {
    'BUILDERTYPE' : 'HltQEEConf',
    'WGs'         : ['QEE'],
    'STREAMS'     : ['EW'],
    'CONFIG'      : {
      'common': {
        'checkPV': False,
      },
      'lines': {
        'DisplPhiPhi': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2ExoticaDisplPhiPhiDecision')",
        },
        'QuadMuonNoIP': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2ExoticaQuadMuonNoIPDecision')",
        },
        'DisplDiMuon': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2ExoticaDisplDiMuonDecision')",
        },
        'DisplDiMuonNoPoint': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2ExoticaDisplDiMuonNoPointDecision')",
        },
        'PrmptDiMuonHighMass': {
          'prescale': 1.0,
          'HLT2'    : "HLT_PASS('Hlt2ExoticaPrmptDiMuonHighMassDecision')",
        },
      },
    },
  },
}


class HltQEEConf(LineBuilder):
  __configuration_keys__ = ('common','lines')

  def __init__(self, name, config) :
    LineBuilder.__init__(self, name, config)
    conf_common = config['common']
    for linename, conf_override in config['lines'].iteritems():
      ## Make a copy, based on common, then update with override on top.
      conf = dict(conf_common)
      conf.update(conf_override)
      self.registerLine(StrippingLine( name+linename+'Line', **conf ))

