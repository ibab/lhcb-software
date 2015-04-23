__all__ = (
  'MBNoBiasConf',
  'default_config',
)

from Gaudi.Configuration import *
from Configurables import FilterDesktop
from PhysSelPython.Wrappers import SimpleSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from LoKiNumbers.decorators import *
from LoKiCore.functions     import *
from LoKiCore.math          import *

default_config = {
  'NAME'        : 'MBNoBias',
  'WGs'         : ['QEE'],
  'BUILDERTYPE' : 'MBNoBiasConf',
  'CONFIG'      : { 
    'NoBias_Prescale'                  : 0.01,
    'NoBias_Postscale'                 : 1.0,
    #'NoBias_ODIN'                      : ["jbit( ODIN_EVTTYP,2 )",['from LoKiCore.math import *']],
    'NoBias_ODIN'                      : None,
    'NoBias_HLT1Line'                  : None,
    #'NoBias_HLT2Line'                  : "(HLT_PASS('Hlt2PassThroughDecision'))",
    'NoBias_HLT2Line'                  : None,
    'HighMult_Prescale'                : 0.05,
    'HighMult_Postscale'               : 1.0,
    'HighMultSinglePV_Prescale'        : 1.0,
    'HighMultSinglePV_Postscale'       : 1.0,
    #'HighMultSinglePV_ODIN'            :  ["jbit( ODIN_EVTTYP,2 )",['from LoKiCore.math import *']],
    'HighMultSinglePV_ODIN'            : None,
    'HighMultSinglePV_HLT1Line'        : None,
    #'HighMultSinglePV_HLT2Line'        : "(HLT_PASS('Hlt2PassThroughDecision'))",
    'HighMultSinglePV_HLT2Line'        : None,
    'HighMultSinglePV_nPVs'            : 1,
    'HighMultSinglePV_MinVeloClusters' : 2000
    },
  'STREAMS'     : [ 'EW' ]
}

class MBNoBiasConf( LineBuilder ) :

    __configuration_keys__ = (  'NoBias_Prescale',
                                'NoBias_Postscale',
                                'NoBias_ODIN',
                                'NoBias_HLT1Line',
                                'NoBias_HLT2Line',
                                'HighMult_Prescale',
                                'HighMult_Postscale',
                                'HighMultSinglePV_Prescale',
                                'HighMultSinglePV_Postscale',
                                'HighMultSinglePV_ODIN',
                                'HighMultSinglePV_HLT1Line',
                                'HighMultSinglePV_HLT2Line',
                                'HighMultSinglePV_nPVs',
                                'HighMultSinglePV_MinVeloClusters'
                                )
    def __init__( self, name, config ) :
        LineBuilder.__init__( self, name, config )

        #---------------------------#
        # NoBias Lines #
        #---------------------------#
        
        line = StrippingLine( name + 'NoBiasLine',
                              prescale  = config[ 'NoBias_Prescale' ],
                              postscale = config[ 'NoBias_Postscale' ],
                              checkPV   = False,
                              RequiredRawEvents = ["Velo","Tracker"],
                              ODIN = config[ 'NoBias_ODIN' ],
                              HLT1 = config[ 'NoBias_HLT1Line' ],
                              HLT2 = config[ 'NoBias_HLT2Line' ],
                              )
        self.registerLine( line )
        
        #---------------------------#
        # High Mult Lines #
        #---------------------------#

        line = StrippingLine( name + 'HighMultLine',
                              prescale  = config[ 'HighMult_Prescale' ],
                              postscale = config[ 'HighMult_Postscale' ],
                              checkPV   = False,
                              RequiredRawEvents = ["Velo","Tracker"],
                              ODIN = config[ 'HighMultSinglePV_ODIN' ],
                              HLT1 = config[ 'HighMultSinglePV_HLT1Line' ],
                              HLT2 = config[ 'HighMultSinglePV_HLT2Line' ],
                              FILTER = "(CONTAINS('Raw/Velo/LiteClusters')> %(MinVeloHits)s)"% {"MinVeloHits":config['HighMultSinglePV_MinVeloClusters']}
                              )
        self.registerLine( line )

        # Cut on single PV

        line = StrippingLine( name + 'HighMultSinglePVLine',
                              prescale  = config[ 'HighMultSinglePV_Prescale' ],
                              postscale = config[ 'HighMultSinglePV_Postscale' ],
                              checkPV   = (1,1),
                              RequiredRawEvents = ["Velo","Tracker"],
                              ODIN = config[ 'HighMultSinglePV_ODIN' ],
                              HLT1 = config[ 'HighMultSinglePV_HLT1Line' ],
                              HLT2 = config[ 'HighMultSinglePV_HLT2Line' ],
                              FILTER = "(CONTAINS('Raw/Velo/LiteClusters')> %(MinVeloHits)s)"% {"MinVeloHits":config['HighMultSinglePV_MinVeloClusters']}
                              )
        self.registerLine( line )

        #-----------------------------------------------------------------        

