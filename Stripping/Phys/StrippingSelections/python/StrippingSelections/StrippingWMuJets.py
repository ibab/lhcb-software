# Stripping Lines for (W->MuNu)+Jets
# X. Cid and C. Santamarina
#
# The W->MuNu part is taken from the WMuNu line written by S.Bifani
#

from Gaudi.Configuration import *
#from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from Configurables       import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdAllLooseMuons, StdNoPIDsPions, StdAllLooseElectrons
from CommonParticles.Utils import *


confdict_WMuJets = { 'WMuJets_Prescale'    : 1.0,
                     'WMuJets_Postscale'   : 1.0,
                     'min_pT'              : 10.,
                     'max_pT'              : 20.,
                     'pT_isol'             : 3.,
                     'TrkSumPt'            : 20.,
                     'TrkMissPt'           : 10.,
                     'min_pT_IP_Particle'  : 3,
                     'min_IPchi2'          : 40.
                     }

default_name = 'WMuJets'

class WMuJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'WMuJets_Prescale',
                               'WMuJets_Postscale',
                               'min_pT',
                               'max_pT',
                               'pT_isol',
                               'TrkSumPt',
                               'TrkMissPt',
                               'min_pT_IP_Particle',
                               'min_IPchi2'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name

        # Define the muon cuts
        _min_pT     = '(PT>%(min_pT)s*GeV)'%config
        _max_pT     = '(PT<%(max_pT)s*GeV)'%config
        _pT_isol    = '((ptCone-PT)<%(pT_isol)s*GeV)'%config
        _sumpt      = '(sumpt>%(TrkSumPt)s*GeV)'%config
        _misspt     = '((sumpx**2 + sumpy**2)>%(TrkMissPt)s*%(TrkMissPt)s*GeV*GeV)'% config

        self.sel_WMu    = makeFilter( self._myname + 'WMu',
                                       StdAllLooseMuons,
                                       ["from LoKiTracks.decorators import *",
                                       "ptCone  = SUMCONE (   0.5 , PT , '/Event/Phys/StdAllNoPIDsPions/Particles'   )",
                                       "trpx    = switch(TrLONG,TrPX,0)",
                                       "trpy    = switch(TrLONG,TrPY,0)",
                                       "trpt    = switch(TrLONG,TrPT,0)",
                                       "trpxsum = RV_TrSUM(trpx,-1)",
                                       "trpysum = RV_TrSUM(trpy,-1)",
                                       "trptsum = RV_TrSUM(trpt,-1)",
                                       "sumpx   = BPV(trpxsum)",
                                       "sumpy   = BPV(trpysum)",
                                       "sumpt   = BPV(trptsum)-PT",],
                                       #"etCone_ = SUMCONE (   0.25 , PT , '/Event/Phys/StdLooseAllPhotons/Particles' )",
                                       #"ptCone  =   SINFO (  55001 , ptCone_ , True ) ",],
                                       #"etCone  =   SINFO (  55002 , etCone_ , True ) ",],
                                       # The isolation cut is defined here...
                                       _min_pT+ '&' + _max_pT + '&' + _pT_isol + '&' + _sumpt + '&' + _misspt
                                       #_min_pT+ '&' + _pT_isol
                                       )

      # This requests that at least a pion with no PID, hence a particle, with pt larger than 500 MeV has IP above 1.5
        _min_pT_IP_Particle = '(PT>%(min_pT_IP_Particle)s*GeV)'% config
        _min_IP = '(MIPCHI2DV(PRIMARY)>%(min_IPchi2)s)'% config

        self.sel_HighIP = makeFilter( self._myname + 'HighIP',
                                       StdNoPIDsPions,
                                       [],
                                       _min_pT_IP_Particle + '&' + _min_IP
                                       )


        self.sel_WMuJets = makeSelection (self._myname + 'WMuJets', [self.sel_WMu,self.sel_HighIP])

        self.line_WMuJets = StrippingLine( self._myname + 'Line',
                                       prescale  = config[ 'WMuJets_Prescale' ],
                                       postscale = config[ 'WMuJets_Postscale' ],
                                       checkPV   = False,
                                       selection = self.sel_WMuJets
                                       )

        self.registerLine( self.line_WMuJets )

def makeFilter( _name, _input, _preambulo, _code ) :

    _filter = FilterDesktop( _name, Preambulo = _preambulo,
                             Code      = _code
                             )

    return Selection ( "sel"+_name,
                       Algorithm          = _filter,
                       RequiredSelections = [ _input ]
                       )

def makeSelection(_name, _filters):
    
    _Algorithm=FilterDesktop(_name,Code='ALL')

    return Selection ( "sel"+_name,
                       Algorithm=_Algorithm,
                       RequiredSelections = _filters
                       )
