# Stripping Lines for (W->eNu)+Jets
# X. Cid and C. Santamarina
#
# The W->eNu part is taken from the WeNu line written by S.Bifani and D.Ward
#

from Gaudi.Configuration import *
from Configurables       import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdAllLooseMuons, StdNoPIDsPions, StdAllLooseElectrons, StdAllNoPIDsElectrons
from CommonParticles.Utils import *

confdict_WeJets = { 'WeJets_Prescale'    : 1.0,
                    'WeJets_Postscale'   : 1.0,
                    'PrsCalMin'          : 50.,
                    'ECalMin'            : 0.10,
                    'HCalMax'            : 0.05,
                    'min_pT'             : 10.,
                    'max_pT'             : 20.,
                    'TrkSumPt'           : 10.,
                    'TrkMissPt'          : 10.,
                    'min_pT_IP_Particle' : 3.,
                    'min_IPchi2'         : 40.
                  }

default_name = 'WeJets'

class WeJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'WeJets_Prescale',
                               'WeJets_Postscale',
                               'PrsCalMin',
                               'ECalMin',
                               'HCalMax',
                               'min_pT',
                               'max_pT',
                               'TrkSumPt', 
                               'TrkMissPt',
                               'min_pT_IP_Particle',
                               'min_IPchi2'
                               )
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts

        _cut    = "(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s) & (PT>%(min_pT)s*GeV) & (PT<%(max_pT)s*GeV) & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        _sumpt      = "(sumpt>%(TrkSumPt)s*GeV)"%config
        _misspt     = "((sumpx**2 + sumpy**2)>%(TrkMissPt)s*%(TrkMissPt)s*GeV*GeV)"% config

        # WeJets signal

        self.sel_We = makeFilter( self._myname + 'We',
                                  StdAllNoPIDsElectrons,
                                  ["from LoKiTracks.decorators import *",
                                  "trpx    = switch(TrLONG,TrPX,0)",
                                  "trpy    = switch(TrLONG,TrPY,0)",
                                  "trpt    = switch(TrLONG,TrPT,0)",
                                  "trpxsum = RV_TrSUM(trpx,-1)",
                                  "trpysum = RV_TrSUM(trpy,-1)",
                                  "trptsum = RV_TrSUM(trpt,-1)",
                                  "sumpx   = BPV(trpxsum)",
                                  "sumpy   = BPV(trpysum)",
                                  "sumpt   = BPV(trptsum)-PT",],
                                  _cut + '&' + _sumpt + '&' + _misspt
                                  )


        # This requests that at least a pion with no PID, hence a particle, with pt and IPchi2 larger than cuts
        _min_pT_IP_Particle = '(PT>%(min_pT_IP_Particle)s*GeV)'% config
        _min_IP = '(MIPCHI2DV(PRIMARY)>%(min_IPchi2)s)'% config

        self.sel_HighIP = makeFilter( self._myname + 'HighIP',
                                      StdNoPIDsPions,
                                      [],
                                      _min_pT_IP_Particle + '&' + _min_IP
                                      )


        self.sel_WeJets = makeSelection (self._myname + 'WeJets', [self.sel_We,self.sel_HighIP])


        self.line_WeJets = StrippingLine( self._myname + 'Line',
                                      prescale  = config[ 'WeJets_Prescale' ],
                                      postscale = config[ 'WeJets_Postscale' ],
                                      selection = self.sel_WeJets
                                      )

        self.registerLine( self.line_WeJets )


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

