"""
Stripping Lines for DY->MuMu
Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)

J.Anderson

DY2MuMu1    (3.2-5GeV): StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>1.5GeV & DLLmu>-3           (5% PRESCALE)
DY2MuMu1Hlt (>3.2GeV):  StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>1.5GeV & Hlt2DiMuonDY* Dec (50% PRESCALE)
DY2MuMu2    (5-10GeV):  StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>3GeV                       (25% PRESCALE)
DY2MuMu2Hlt (>5GeV):    StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>3GeV   & Hlt2DiMuonDY* Dec
DY2MuMu3    (10-20GeV): StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>3GeV
DY2MuMu4    (20-40GeV): StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>3GeV
"""

__all__ = (
  'DY2MuMuConf',
  'default_config',
)

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons
from GaudiKernel.SystemOfUnits import GeV


default_config = {
    'NAME'        : 'DY2MuMu',
    'BUILDERTYPE' : 'DY2MuMuConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : { 
        'DY2MuMu1LinePrescale'    : 0.05,
        'DY2MuMu1LineHltPrescale' : 0.5,
        'DY2MuMu2LinePrescale'    : 0.25,
        'DY2MuMu2LineHltPrescale' : 1.0,
        'DY2MuMu3LinePrescale'    : 1.0,  
        'DY2MuMu4LinePrescale'    : 1.0,
        'DY2MuMuLinePostscale'    : 1.0,
        #
        'DY2MuMu1HLT2': "HLT_PASS_RE( 'Hlt2.*DiMuonDY.*Decision' )",
        'DY2MuMu2HLT2': "HLT_PASS_RE( 'Hlt2.*DiMuonDY.*Decision' )",
        #
        'RawEvents'   : ["Muon","Calo","Rich","Velo","Tracker"],
        #
        'DY1MinMass'  :  3.2 * GeV,
        'DY1MaxMass'  :  5. * GeV,
        'DY2MinMass'  :  5. * GeV,
        'DY2MaxMass'  : 10. * GeV,
        'DY3MinMass'  : 10. * GeV,
        'DY3MaxMass'  : 20. * GeV,
        'DY4MinMass'  : 20. * GeV,
        'DY4MaxMass'  : 40. * GeV,
        'p'           : 10. * GeV,
        'pT1'         :  1.5 * GeV,
        'pT2'         :  3. * GeV,
        'trkpchi2'    :  0.001,
        'pid'         : -3.
    },
}

class DY2MuMuConf( LineBuilder ) :

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        # Define the cuts

        _cut1 = '(P>%(p)s) & (PT>%(pT1)s) & (TRPCHI2>%(trkpchi2)s) & ((PIDmu-PIDpi)>%(pid)s)'%config
        _cut2 = '(P>%(p)s) & (PT>%(pT2)s) & (TRPCHI2>%(trkpchi2)s)'%config

        _DY1MassCut    = '(MM>%(DY1MinMass)s) & (MM<%(DY1MaxMass)s)'%config
        _DY1MassCutHlt = '(MM>%(DY1MinMass)s)'%config
        _DY2MassCut    = '(MM>%(DY2MinMass)s) & (MM<%(DY2MaxMass)s)'%config
        _DY2MassCutHlt = '(MM>%(DY2MinMass)s)'%config
        _DY3MassCut    = '(MM>%(DY3MinMass)s) & (MM<%(DY3MaxMass)s)'%config
        _DY4MassCut    = '(MM>%(DY4MinMass)s) & (MM<%(DY4MaxMass)s)'%config


        # DY2MuMu1

        sel_DY2MuMu1 = makeCombination( name + 'DYDecay1',
                                             StdAllLooseMuons,
                                             _cut1,
                                             _DY1MassCut
                                             )

        line_DY2MuMu1 = StrippingLine( name + 'Line1',
                                            prescale  = config['DY2MuMu1LinePrescale'],
                                            postscale = config['DY2MuMuLinePostscale'],
                                            RequiredRawEvents = config['RawEvents'],
                                            selection = sel_DY2MuMu1,
#                                            RelatedInfoTools = [
#                                                               {'Type' : 'RelInfoConeVariables',
#                                                                'ConeAngle' : 0.5,
#                                                                'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
#                                                                'TopSelection' : sel_DY2MuMu1,
#                                                                'Location' : 'P2CVD'
#                                                               },
#                                                               ]
                                            )

        self.registerLine( line_DY2MuMu1 )


        # DY2MuMu1Hlt

        sel_DY2MuMu1Hlt = makeCombination( name + 'DYDecay1Hlt',
                                                StdAllLooseMuons,
                                                _cut1,
                                                _DY1MassCutHlt
                                                )

        line_DY2MuMu1Hlt = StrippingLine( name + 'Line1Hlt',
                                               prescale  = config[ 'DY2MuMu1LineHltPrescale' ],
                                               postscale = config[ 'DY2MuMuLinePostscale' ],
                                               RequiredRawEvents = config['RawEvents'],
                                               HLT2      = config[ 'DY2MuMu1HLT2' ],
                                               selection = sel_DY2MuMu1Hlt,
                                               )

        self.registerLine( line_DY2MuMu1Hlt )


        # DY2MuMu2

        sel_DY2MuMu2 = makeCombination( name + 'DYDecay2',
                                             StdAllLooseMuons,
                                             _cut2,
                                             _DY2MassCut
                                             )

        line_DY2MuMu2 = StrippingLine( name + 'Line2',
                                            prescale  = config['DY2MuMu2LinePrescale'],
                                            postscale = config['DY2MuMuLinePostscale'],
                                            RequiredRawEvents = config['RawEvents'],
                                            selection = sel_DY2MuMu2,
                                            )


        self.registerLine( line_DY2MuMu2 )


        # DY2MuMu2Hlt

        sel_DY2MuMu2Hlt = makeCombination( name + 'DYDecay2Hlt',
                                                StdAllLooseMuons,
                                                _cut2,
                                                _DY2MassCutHlt
                                                )

        line_DY2MuMu2Hlt = StrippingLine( name + 'Line2Hlt',
                                               prescale  = config[ 'DY2MuMu2LineHltPrescale' ],
                                               postscale = config[ 'DY2MuMuLinePostscale' ],
                                               RequiredRawEvents = config['RawEvents'],
                                               HLT2      = config[ 'DY2MuMu1HLT2' ],
                                               selection = sel_DY2MuMu2Hlt,
                                               )

        self.registerLine( line_DY2MuMu2Hlt )


        # DY2MuMu3

        sel_DY2MuMu3 = makeCombination( name + 'DY2MuMuDecay3',
                                             StdAllLooseMuons,
                                             _cut2,
                                             _DY3MassCut
                                             )

        line_DY2MuMu3 = StrippingLine( name + 'Line3',
                                            prescale  = config[ 'DY2MuMu3LinePrescale' ],
                                            postscale = config[ 'DY2MuMuLinePostscale' ],
                                            RequiredRawEvents = config['RawEvents'],
                                            selection = sel_DY2MuMu3,
                                            )

        self.registerLine( line_DY2MuMu3 )


        # DY2MuMu4

        sel_DY2MuMu4 = makeCombination( name + 'DY2MuMuDecay4',
                                             StdAllLooseMuons,
                                             _cut2,
                                             _DY4MassCut
                                             )

        line_DY2MuMu4 = StrippingLine( name + 'Line4',
                                            prescale  = config[ 'DY2MuMu4LinePrescale' ],
                                            postscale = config[ 'DY2MuMuLinePostscale' ],
                                            RequiredRawEvents = config['RawEvents'],
                                            selection = sel_DY2MuMu4,
                                            )

        self.registerLine( line_DY2MuMu4 )


def makeCombination( name, _input, _daughters, _mother ) :

    _combination = CombineParticles( DecayDescriptor    = 'Z0 -> mu+ mu-',
                                     DaughtersCuts      = { 'mu+' : _daughters, 
                                                            'mu-' : _daughters },
                                     MotherCut          = _mother,
                                     WriteP2PVRelations = False
                                     )

    return Selection ( name,
                       Algorithm          = _combination,
                       RequiredSelections = [ _input ]
                       )
