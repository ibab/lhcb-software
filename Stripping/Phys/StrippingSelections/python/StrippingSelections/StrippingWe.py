# Stripping Lines for W->eNu
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani and D.Ward
#
# We signal:  StdAllNoPIDsElectrons, PRS>50Mev & E_ECal/P>0.1 & E_HCal/P<0.05 & pT>20GeV & TTHits
# We control: StdAllNoPIDsElectrons, PRS>50Mev & E_ECal/P>0.1 & E_HCal/P<0.05 & pT>15GeV          (10% PRESCALE)

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsElectrons

confdict_We = { 'We_Prescale'    : 1.0,
                'WeLow_Prescale' : 0.1,
                'We_Postscale'   : 1.0,
                'PrsCalMin' : 50.,
                'ECalMin'   :  0.10,
                'HCalMax'   :  0.05,
                'pT'        : 20.,
                'pTlow'     : 15.
                }

default_name = 'We'

class WeConf( LineBuilder ) :

    __configuration_keys__ = ( 'We_Prescale',
                               'WeLow_Prescale',
                               'We_Postscale',
                               'PrsCalMin',
                               'ECalMin',
                               'HCalMax',
                               'pT',
                               'pTlow'
                               )
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts

        _cut    = "(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s) & (PT>%(pT)s*GeV) & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        _cutLow = '(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s) & (PT>%(pTlow)s*GeV)'%config


        # We signal

        self.sel_We = makeFilter( self._myname + 'We',
                                  StdAllNoPIDsElectrons,
                                  "from LoKiTracks.decorators import *",
                                  _cut
                                  )

        self.line_We = StrippingLine( self._myname + 'Line',
                                      prescale  = config[ 'We_Prescale' ],
                                      postscale = config[ 'We_Postscale' ],
                                      selection = self.sel_We
                                      )

        self.registerLine( self.line_We )


        # We control

        self.sel_WeLow = makeFilter( self._myname + 'WeLow',
                                     StdAllNoPIDsElectrons,
                                     "from LoKiTracks.decorators import *",
                                     _cutLow
                                     )

        self.line_WeLow = StrippingLine( self._myname + 'LowLine',
                                         prescale  = config[ 'WeLow_Prescale' ],
                                         postscale = config[ 'We_Postscale' ],
                                         selection = self.sel_WeLow
                                         )
        
        self.registerLine( self.line_WeLow )




def makeFilter( name, _input, _preambulo, _code ) :

    _filter = FilterDesktop( Preambulo = [ _preambulo ],
                             Code      = _code
                             )

    return Selection ( name,
                       Algorithm          = _filter,
                       RequiredSelections = [ _input ]
                       )
