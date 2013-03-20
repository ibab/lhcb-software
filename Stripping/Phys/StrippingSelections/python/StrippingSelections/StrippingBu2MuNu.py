# Stripping Line for BMuNu (tight muons with high pT and IP)
# RD Group
#
# Xabier Cid Vidal
#
# Signal:  StdTightMuons, pT > 5GeV/c, IPS>20


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdTightMuons

config_default = { 'Prescale'  : 0.06,
                   'Postscale' : 1.0,
                   'pTmin'     : 5.,
                   'pTmax'     : 40.,
                   'pmax'      : 500.,
                   'IPchi2'    : 400.,
                   'TrChi2'    : 3
                   }

default_name = 'Bu2MuNu'

class Bu2MuNuConf( LineBuilder ) :

    __configuration_keys__ = ( 'Prescale',
                               'Prescale',
                               'pTmin',
                               'pTmax',
                               'pmax',
                               'IPchi2',
                               'TrChi2'
                               )
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts
        _cut = "(MIPCHI2DV(PRIMARY)> %(IPchi2)s)&(TRCHI2DOF < %(TrChi2)s ) & (PT < %(pTmax)s*GeV) & (P < %(pmax)s*GeV) & (PT>%(pTmin)s*GeV)" %config


        # Signal

        self.sel = makeFilter( self._myname + 'Bu2MuNu',
                                  StdTightMuons,
                                  "from LoKiTracks.decorators import *",
                                  _cut
                                  )

        self.line = StrippingLine( self._myname + 'Line',
                                      prescale  = config[ 'Prescale' ],
                                      postscale = config[ 'Postscale' ],
                                      selection = self.sel
                                      )

        self.registerLine( self.line)





def makeFilter( name, _input, _preambulo, _code ) :

    _filter = FilterDesktop( Preambulo = [ _preambulo ],
                             Code      = _code
                             )

    return Selection ( name,
                       Algorithm          = _filter,
                       RequiredSelections = [ _input ]
                       )
