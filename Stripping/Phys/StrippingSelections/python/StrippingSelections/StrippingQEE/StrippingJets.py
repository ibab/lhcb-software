"""
Stripping lines which pass through the HLT2 jet decisions.
"""

# Additional line information.
__author__  = ['Philip Ilten', 'Michael Williams']
__date__    = '06/04/2016'
__version__ = 1.0
__all__     = ['default_config', 'JetsConf']

# Default configuration.
default_config = {
    'NAME': 'Jets',
    'BUILDERTYPE': 'JetsConf',
    'WGs': ['QEE'],
    'STREAMS': ['BhadronCompleteEvent'],
    'CONFIG': {
        'DiJet': {
            'Prescale': 1.0,
            'Hlt2Tos': "HLT_PASS_RE('Hlt2JetsDiJetDecision')"},
        'DiJetSV': {
            'Prescale': 1.0,
            'Hlt2Tos': "HLT_PASS_RE('Hlt2JetsDiJetSVDecision')"},
        'DiJetSVSV': {
            'Prescale': 1.0,
            'Hlt2Tos': "HLT_PASS_RE('Hlt2JetsDiJetSVSVDecision')"},
        'DiJetSVMu': {
            'Prescale': 1.0,
            'Hlt2Tos': "HLT_PASS_RE('Hlt2JetsDiJetSVMuDecision')"},
        'DiJetMuMu': {
            'Prescale': 1.0,
            'Hlt2Tos': "HLT_PASS_RE('Hlt2JetsDiJetMuMuDecision')"}
        }
    }

# Define the class for the jet stripping configuration.
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
class JetsConf(LineBuilder):
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        for line, cfg in config.iteritems():
            prescale = cfg['Prescale'] if 'Prescale' in cfg else 1.0
            hlt2tos  = cfg.get('Hlt2Tos')
            if hlt2tos: 
                self.registerLine(StrippingLine(name + line + 'Line'), 
                                  prescale = prescale, checkPV = False,
                                  HLT2 = hlt2tos, RequiredRawEvents = ['Calo'])
