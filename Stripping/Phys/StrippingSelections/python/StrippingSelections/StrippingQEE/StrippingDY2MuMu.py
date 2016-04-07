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

__all__ = 'DY2MuMuConf', 'default_config'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, SimpleSelection
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
        'DY2MuMu1LineHltPrescale' : 0.1,
        'DY2MuMu2LinePrescale'    : 0.25,
        'DY2MuMu2LineHltPrescale' : 0.5,
        'DY2MuMu3LinePrescale'    : 1.0,  
        'DY2MuMu4LinePrescale'    : 1.0,
        'DY2MuMuLinePostscale'    : 1.0,
        #
        'DY2MuMu1HLT2': "HLT_PASS_RE( 'Hlt2(EW)?DiMuonDY[1-4]Decision' )",
        'DY2MuMu2HLT2': "HLT_PASS_RE( 'Hlt2(EW)?DiMuonDY[1-4]Decision' )",
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

        cut1 = '(P>%(p)s) & (PT>%(pT1)s) & (TRPCHI2>%(trkpchi2)s) & ((PIDmu-PIDpi)>%(pid)s)'%config
        cut2 = '(P>%(p)s) & (PT>%(pT2)s) & (TRPCHI2>%(trkpchi2)s)'%config

        DY1MassCut    = '(MM>%(DY1MinMass)s) & (MM<%(DY1MaxMass)s)'%config
        DY1MassCutHlt = '(MM>%(DY1MinMass)s)'%config
        DY2MassCut    = '(MM>%(DY2MinMass)s) & (MM<%(DY2MaxMass)s)'%config
        DY2MassCutHlt = '(MM>%(DY2MinMass)s)'%config
        DY3MassCut    = '(MM>%(DY3MinMass)s) & (MM<%(DY3MaxMass)s)'%config
        DY4MassCut    = '(MM>%(DY4MinMass)s) & (MM<%(DY4MaxMass)s)'%config

        ## DY2MuMu1

        sel = makeCombination( name + 'DYDecay1', cut1, DY1MassCut )
        self.registerLine(StrippingLine( name + 'Line1',
            prescale  = config['DY2MuMu1LinePrescale'],
            postscale = config['DY2MuMuLinePostscale'],
            RequiredRawEvents = config['RawEvents'],
            selection = sel,
        ))

        ## DY2MuMu1Hlt

        sel = makeCombination( name + 'DYDecay1Hlt', cut1, DY1MassCutHlt )
        self.registerLine(StrippingLine( name + 'Line1Hlt',
           prescale  = config[ 'DY2MuMu1LineHltPrescale' ],
           postscale = config[ 'DY2MuMuLinePostscale' ],
           RequiredRawEvents = config['RawEvents'],
           HLT2      = config[ 'DY2MuMu1HLT2' ],
           selection = sel,
        ))


        ## DY2MuMu2

        sel = makeCombination( name + 'DYDecay2', cut2, DY2MassCut )
        self.registerLine(StrippingLine( name + 'Line2',
            prescale  = config['DY2MuMu2LinePrescale'],
            postscale = config['DY2MuMuLinePostscale'],
            RequiredRawEvents = config['RawEvents'],
            selection = sel,
        ))

        ## DY2MuMu2Hlt

        sel = makeCombination( name + 'DYDecay2Hlt', cut2, DY2MassCutHlt )
        self.registerLine(StrippingLine( name + 'Line2Hlt',
           prescale  = config[ 'DY2MuMu2LineHltPrescale' ],
           postscale = config[ 'DY2MuMuLinePostscale' ],
           RequiredRawEvents = config['RawEvents'],
           HLT2      = config[ 'DY2MuMu1HLT2' ],
           selection = sel,
        ))

        ## DY2MuMu3

        sel = makeCombination( name + 'DY2MuMuDecay3', cut2, DY3MassCut )
        self.registerLine(StrippingLine( name + 'Line3',
            prescale  = config[ 'DY2MuMu3LinePrescale' ],
            postscale = config[ 'DY2MuMuLinePostscale' ],
            RequiredRawEvents = config['RawEvents'],
            selection = sel,
        ))

        ## DY2MuMu4

        sel = makeCombination( name + 'DY2MuMuDecay4', cut2, DY4MassCut )
        self.registerLine(StrippingLine( name + 'Line4',
            prescale  = config[ 'DY2MuMu4LinePrescale' ],
            postscale = config[ 'DY2MuMuLinePostscale' ],
            RequiredRawEvents = config['RawEvents'],
            selection = sel,
        ))


def makeCombination( name, daughters, mother ) :
    return SimpleSelection( name, CombineParticles, [StdAllLooseMuons],
        DecayDescriptor    = 'Z0 -> mu+ mu-',
        DaughtersCuts      = { 'mu+' : daughters, 'mu-' : daughters },
        MotherCut          = mother,
        WriteP2PVRelations = False,
    )
