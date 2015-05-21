"""
Stripping Lines for W->eNu
Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)

S.Bifani and D.Ward

We signal:  StdAllNoPIDsElectrons, PRS>50Mev & E_ECal/P>0.1 & E_HCal/P<0.05 & pT>20GeV & TTHits
We control: StdAllNoPIDsElectrons, PRS>50Mev & E_ECal/P>0.1 & E_HCal/P<0.05 & pT>15GeV          (10% PRESCALE)
"""

__all__ = (
  'WeConf',
  'default_config',
)

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsElectrons
from GaudiKernel.SystemOfUnits import GeV


default_config = {
    'NAME'        : 'We',
    'BUILDERTYPE' : 'WeConf',
    'STREAMS'     : [ 'EW'  ],
    'WGs'         : [ 'QEE' ],
    'CONFIG'      : { 
        'We_Prescale'   : 1.0,
        'WeLow_Prescale': 0.1,
        'We_Postscale'  : 1.0,
        'PrsCalMin'     : 50.,
        'ECalMin'       :  0.10,
        'HCalMax'       :  0.05,
        'pT'            : 20. * GeV,
        'pTlow'         : 15. * GeV,
    },
}

class WeConf( LineBuilder ) :

    __configuration_keys__ = default_config['CONFIG'].keys()
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )


        # Define the cuts

#        _cut    = "(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s) & (PT>%(pT)s*GeV) & (HASTRACK & TRCUT(0<TrIDC('isTT')))"%config
        _cut    = "(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s) & (PT>%(pT)s)"%config
        _cutLow = '(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s) & (PT>%(pTlow)s)'%config


        # We signal

        sel_We = makeFilter( name + 'We',
                                  StdAllNoPIDsElectrons,
                                  "from LoKiTracks.decorators import *",
                                  _cut
                                  )

        line_We = StrippingLine( name + 'Line',
                                      prescale  = config[ 'We_Prescale' ],
                                      postscale = config[ 'We_Postscale' ],
                                      RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                      selection = sel_We
                                      )

        self.registerLine( line_We )


        # We control

        sel_WeLow = makeFilter( name + 'WeLow',
                                     StdAllNoPIDsElectrons,
                                     "from LoKiTracks.decorators import *",
                                     _cutLow
                                     )

        line_WeLow = StrippingLine( name + 'LowLine',
                                         prescale  = config[ 'WeLow_Prescale' ],
                                         postscale = config[ 'We_Postscale' ],
                                         RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                         selection = sel_WeLow
                                         )
        
        self.registerLine( line_WeLow )




def makeFilter( name, _input, _preambulo, _code ) :

    _filter = FilterDesktop( Preambulo = [ _preambulo ],
                             Code      = _code
                             )

    return Selection ( name,
                       Algorithm          = _filter,
                       RequiredSelections = [ _input ]
                       )
