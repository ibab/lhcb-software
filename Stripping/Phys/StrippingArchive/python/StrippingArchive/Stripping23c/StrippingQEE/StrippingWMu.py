# Stripping Lines for W->MuNu and studies of their background
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani
#
# WMu signal:           StdAllLooseMuons,  pT>20GeV
# WMu control:          StdAllLooseMuons,  pT>15GeV                    (10% PRESCALE)
# WMu background:       StdAllNoPIDsMuons, pT>5GeV  &  Hlt1MBNoBias Dec (RATE LIMITED, 20% PRESCALE)
# SingleMuon control:   StdAllLooseMuons,  pT>10GeV &  Hlt2SingleMuonHighPT Dec     (1% PRESCALE)
# SingleMuon control:   StdAllLooseMuons,  pT>4.8GeV & Hlt2SingleMuonLowPT Dec      (20% PRESCALE)

__all__ = (
  'WMuConf',
  'default_config',
)

from Configurables import FilterDesktop
from PhysSelPython.Wrappers import SimpleSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons, StdAllNoPIDsMuons
from GaudiKernel.SystemOfUnits import GeV


default_config = {
  'NAME'        : 'WMu',
  'BUILDERTYPE' : 'WMuConf',
  'WGs'         : [ 'QEE'],
  'STREAMS'     : [ 'EW' ],
  'CONFIG'      : { 
    'WMu_Prescale'        : 1.0,
    'WMuLow_Prescale'     : 0.1,
    'STNB_Prescale'       : 0.2,
    'WMu_Postscale'       : 1.0,
    'SingMuon10_Prescale' : 0.01,
    'SingMuon48_Prescale' : 0.4,
    'pT'                  : 20. * GeV,
    'pTlow'               : 15. * GeV,
    'pTvlow'              :  5. * GeV,
    'SingMuon10_pT'       : 10. * GeV,
    'SingMuon48_pT'       : 4.8 * GeV,
    #
    'HLT2_Control10'        : "HLT_PASS_RE('Hlt2.*SingleMuonHighPTDecision')",
    'HLT2_Control4800'      : "HLT_PASS_RE('Hlt2.*SingleMuonLowPTDecision')",
    'HLT1_SingleTrackNoBias': "HLT_PASS( 'Hlt1MBNoBiasDecision' )",
  },
}

class WMuConf( LineBuilder ) :

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        # Define the cuts

        _pT     = '(PT>%(pT)s)'%config
        _pTlow  = '(PT>%(pTlow)s)'%config
        _pTvlow = '(PT>%(pTvlow)s)'%config

        _pTSingMuon10    = '(PT>%(SingMuon10_pT)s)'%config
        _pTSingMuon48    = '(PT>%(SingMuon48_pT)s)'%config

        #---------------------------#
        # Single Muon Control Lines #
        #---------------------------#

        sel  = makeFilter(name+'Mu10', StdAllLooseMuons, _pTSingMuon10)

        line = StrippingLine( name + 'Control10Line',
                                       prescale  = config[ 'SingMuon10_Prescale' ],
                                       postscale = config[ 'WMu_Postscale' ],
                                       checkPV   = False,
                                       RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                       HLT2 = config['HLT2_Control10'],
                                       selection = sel
                                       )

        self.registerLine( line )

        #-----------------------------------------------------------------        

        sel  = makeFilter(name+'Mu48', StdAllLooseMuons, _pTSingMuon48)

        line = StrippingLine( name + 'Control4800Line',
                                       prescale  = config[ 'SingMuon48_Prescale' ],
                                       postscale = config[ 'WMu_Postscale' ],
                                       checkPV   = False,
                                       RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                       HLT2 = config['HLT2_Control4800'],
                                       selection = sel
                                       )

        self.registerLine( line )

        #------------#
        # WMu signal #
        #------------#

        sel  = makeFilter(name+'Wmu', StdAllLooseMuons, _pT)

        line = StrippingLine( name + 'Line',
                                       prescale  = config[ 'WMu_Prescale' ],
                                       postscale = config[ 'WMu_Postscale' ],
                                       checkPV   = False,
                                       RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                       selection = sel,
                                       )

        self.registerLine( line )

        #-------------#
        # WMu control #
        #-------------#

        sel  = makeFilter(name+'WMuLow', StdAllLooseMuons, _pTlow)

        line = StrippingLine( name + 'LowLine',
                                          prescale  = config[ 'WMuLow_Prescale' ],
                                          postscale = config[ 'WMu_Postscale' ],
                                          checkPV   = False,
                                          RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                          selection = sel
                                          )

        self.registerLine( line )

        #----------------#
        # WMu background #
        #----------------#

        sel = makeFilter(name+'SingleTrackNoBias', StdAllNoPIDsMuons, _pTlow)

        line = StrippingLine( name + 'SingleTrackNoBiasLine',
                                                     prescale  = config[ 'WMu_Prescale' ],
                                                     postscale = config[ 'WMu_Postscale' ],
                                                     checkPV   = False,
                                                     RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                                     HLT1      = config['HLT1_SingleTrackNoBias'],
                                                     selection = sel,
                                                     )

        self.registerLine( line )

        #---------------------------

        sel = makeFilter(name+'SingleTrackNoBiasPS', StdAllNoPIDsMuons, _pTvlow)

        line = StrippingLine( name + 'SingleTrackNoBiasLinePS',
                                                     prescale  = config[ 'STNB_Prescale' ],
                                                     postscale = config[ 'WMu_Postscale' ],
                                                     checkPV   = False,
                                                     RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                                     HLT1      = config['HLT1_SingleTrackNoBias'],
                                                     selection = sel,
                                                     )

        self.registerLine( line )



def makeFilter(name, single_input, code):
  return SimpleSelection(name, FilterDesktop, [single_input], 'Sel'+name,
    Preambulo = [ "from LoKiTracks.decorators import *" ],    
    Code      = code,
  )

