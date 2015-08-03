__author__ = 'Andrea Contu'
__date__ = '21/11/2014'
__version__ = '$Revision: 1 $'

"""
Stripping selections for BadEvents.
"""

# Begin StrippingBadEvents.py

default_config = {
    'NAME'        : 'BadEvents',
    'BUILDERTYPE' : 'BadEventsConf',
    'CONFIG'      : { "GEC"       : "((TrSOURCE('/Event/Rec/Track/Best', TrLONG)"\
                                    " >> (sum(TrPT,TrP<1 * TeV))>1 * TeV))",
                      "prescale": 1.0,
                      "postscale": 1.0 },
    'WGs'         : [ 'ALL' ],
    'STREAMS'     : [ 'MiniBias' ]
}

__all__ = ('BadEventsConf' )

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import Selection

class BadEventsConf(LineBuilder) :
    """
    Builder for BadEvent lines
    """

    __configuration_keys__ = ( 
        "GEC",
        "prescale" ,
        "postscale",
       )

    def __init__(self, name, config):
      LineBuilder.__init__(self, name, config)
      
      _filter={'Code': config['GEC'], 'Preambulo' : ["from LoKiTracks.decorators import *" ,
                                                     "from LoKiCore.functions    import * ",
                                                     "from GaudiKernel.SystemOfUnits import *"]}
                                      
      line = StrippingLine(name
                           ,FILTER = _filter
                           ,prescale = config["prescale"]
                           ,postscale = config["postscale"]
                           , RequiredRawEvents = ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"])
      self.registerLine( line )
