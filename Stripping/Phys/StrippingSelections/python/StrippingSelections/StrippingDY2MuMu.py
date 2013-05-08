# Stripping Lines for DY->MuMu
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# J.Anderson
#
# DY2MuMu1    (3.2-5GeV): StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>1.5GeV & DLLmu>-3           (5% PRESCALE)
# DY2MuMu1Hlt (>3.2GeV):  StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>1.5GeV & Hlt2DiMuonDY* Dec (50% PRESCALE)
# DY2MuMu2    (5-10GeV):  StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>3GeV                       (25% PRESCALE)
# DY2MuMu2Hlt (>5GeV):    StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>3GeV   & Hlt2DiMuonDY* Dec
# DY2MuMu3    (10-20GeV): StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>3GeV
# DY2MuMu4    (20-40GeV): StdAllLooseMuons & TrkPChi2>0.1% & P>10GeV & pT>3GeV

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons

confdict_DY2MuMu = { 'DY2MuMu1LinePrescale'    : 1.0, ##changed from 0.05
                     'DY2MuMu1LineHltPrescale' : 1.0, ##changed from 0.5
                     'DY2MuMu2LinePrescale'    : 1.0, ##changed from 0.25
                     'DY2MuMu2LineHltPrescale' : 1.0,
                     'DY2MuMu3LinePrescale'    : 1.0,
                     'DY2MuMu4LinePrescale'    : 1.0,
                     'DY2MuMuLinePostscale'    : 1.0,
                     'DY1MinMass' :  3.2,
                     'DY1MaxMass' :  5.,
                     'DY2MinMass' :  5.,
                     'DY2MaxMass' : 10.,
                     'DY3MinMass' : 10.,
                     'DY3MaxMass' : 20.,
                     'DY4MinMass' : 20.,
                     'DY4MaxMass' : 40.,
                     'p'             : 10.,
                     'pT1'           :  1.5,
                     'pT2'           :  3.,
                     'trkpchi2'      :  0.001,
                     'pid'           : -3.
                     }

default_name = 'DY2MuMu'

class DY2MuMuConf( LineBuilder ) :

    __configuration_keys__ = ( 'DY2MuMu1LinePrescale',
                               'DY2MuMu1LineHltPrescale',
                               'DY2MuMu2LinePrescale',
                               'DY2MuMu2LineHltPrescale',
                               'DY2MuMu3LinePrescale',
                               'DY2MuMu4LinePrescale',
                               'DY2MuMuLinePostscale',
                               'DY1MinMass',
                               'DY1MaxMass',
                               'DY2MinMass',
                               'DY2MaxMass',
                               'DY3MinMass',
                               'DY3MaxMass',
                               'DY4MinMass',
                               'DY4MaxMass',
                               'p',
                               'pT1',
                               'pT2',
                               'trkpchi2',
                               'pid'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts

        _cut1 = '(P>%(p)s*GeV) & (PT>%(pT1)s*GeV) & (TRPCHI2>%(trkpchi2)s) & ((PIDmu-PIDpi)>%(pid)s)'%config
        _cut2 = '(P>%(p)s*GeV) & (PT>%(pT2)s*GeV) & (TRPCHI2>%(trkpchi2)s)'%config

        _DY1MassCut    = '(MM>%(DY1MinMass)s*GeV) & (MM<%(DY1MaxMass)s*GeV)'%config
        _DY1MassCutHlt = '(MM>%(DY1MinMass)s*GeV)'%config
        _DY2MassCut    = '(MM>%(DY2MinMass)s*GeV) & (MM<%(DY2MaxMass)s*GeV)'%config
        _DY2MassCutHlt = '(MM>%(DY2MinMass)s*GeV)'%config
        _DY3MassCut    = '(MM>%(DY3MinMass)s*GeV) & (MM<%(DY3MaxMass)s*GeV)'%config
        _DY4MassCut    = '(MM>%(DY4MinMass)s*GeV) & (MM<%(DY4MaxMass)s*GeV)'%config


        # DY2MuMu1

        self.sel_DY2MuMu1 = makeCombination( self._myname + 'DYDecay1',
                                             StdAllLooseMuons,
                                             _cut1,
                                             _DY1MassCut
                                             )

        self.line_DY2MuMu1 = StrippingLine( self._myname + 'Line1',
                                            prescale  = config['DY2MuMu1LinePrescale'],
                                            postscale = config['DY2MuMuLinePostscale'],
                                            selection = self.sel_DY2MuMu1
                                            )

        self.registerLine( self.line_DY2MuMu1 )


        # DY2MuMu1Hlt

        self.sel_DY2MuMu1Hlt = makeCombination( self._myname + 'DYDecay1Hlt',
                                                StdAllLooseMuons,
                                                _cut1,
                                                _DY1MassCutHlt
                                                )

        self.line_DY2MuMu1Hlt = StrippingLine( self._myname + 'Line1Hlt',
                                               prescale  = config[ 'DY2MuMu1LineHltPrescale' ],
                                               postscale = config[ 'DY2MuMuLinePostscale' ],
                                               HLT       = "HLT_PASS_RE( 'Hlt2DiMuonDY.*Decision' )",
                                               selection = self.sel_DY2MuMu1Hlt
                                               )

        self.registerLine( self.line_DY2MuMu1Hlt )


        # DY2MuMu2

        self.sel_DY2MuMu2 = makeCombination( self._myname + 'DYDecay2',
                                             StdAllLooseMuons,
                                             _cut2,
                                             _DY2MassCut
                                             )

        self.line_DY2MuMu2 = StrippingLine( self._myname + 'Line2',
                                            prescale  = config['DY2MuMu2LinePrescale'],
                                            postscale = config['DY2MuMuLinePostscale'],
                                            selection = self.sel_DY2MuMu2
                                            )


        self.registerLine( self.line_DY2MuMu2 )


        # DY2MuMu2Hlt

        self.sel_DY2MuMu2Hlt = makeCombination( self._myname + 'DYDecay2Hlt',
                                                StdAllLooseMuons,
                                                _cut2,
                                                _DY2MassCutHlt
                                                )

        self.line_DY2MuMu2Hlt = StrippingLine( self._myname + 'Line2Hlt',
                                               prescale  = config[ 'DY2MuMu2LineHltPrescale' ],
                                               postscale = config[ 'DY2MuMuLinePostscale' ],
                                               HLT       = "HLT_PASS_RE( 'Hlt2DiMuonDY.*Decision' )",
                                               selection = self.sel_DY2MuMu2Hlt
                                               )

        self.registerLine( self.line_DY2MuMu2Hlt )


        # DY2MuMu3

        self.sel_DY2MuMu3 = makeCombination( self._myname + 'DY2MuMuDecay3',
                                             StdAllLooseMuons,
                                             _cut2,
                                             _DY3MassCut
                                             )

        self.line_DY2MuMu3 = StrippingLine( self._myname + 'Line3',
                                            prescale  = config[ 'DY2MuMu3LinePrescale' ],
                                            postscale = config[ 'DY2MuMuLinePostscale' ],
                                            selection = self.sel_DY2MuMu3
                                            )

        self.registerLine( self.line_DY2MuMu3 )


        # DY2MuMu4

        self.sel_DY2MuMu4 = makeCombination( self._myname + 'DY2MuMuDecay4',
                                             StdAllLooseMuons,
                                             _cut2,
                                             _DY4MassCut
                                             )

        self.line_DY2MuMu4 = StrippingLine( self._myname + 'Line4',
                                            prescale  = config[ 'DY2MuMu4LinePrescale' ],
                                            postscale = config[ 'DY2MuMuLinePostscale' ],
                                            selection = self.sel_DY2MuMu4
                                            )

        self.registerLine( self.line_DY2MuMu4 )


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
