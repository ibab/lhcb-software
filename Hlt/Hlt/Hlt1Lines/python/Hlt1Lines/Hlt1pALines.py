# =============================================================================
## @file
#  Cosmics 
#  @author Johannes Albrecht
#  @date 2012-08-24
# =============================================================================
"""
 script to configure Hlt lines for p A running
"""
# =============================================================================
__author__  = "Johannes Albrecht"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.5 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt1pALinesConf(HltLinesConfigurableUser):

    from HltTracking.HltReco import MaxOTHits
    
    __slots__ = { 'pA_SpdMult'       : 30 ,
                  'pA_LowMultSpd'    : 30 ,
                  'nVeloLowMultMicroBias' : 99999,
                  'pA_GECPassSPD'    :  800,
                  'pA_GECPassVelo'   : 6000,
                  'pA_GECPassIT'     : 3000,
                  'pA_GECPassOT'     : MaxOTHits
                  }
    
    def __apply_configuration__(self):
        
        from HltLine.HltDecodeRaw import DecodeOT
        from HltLine.HltLine import Hlt1Line as Line
        from Configurables import HltCosmicsOT
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        
        Line('ActivitySPD',
             prescale  = self.prescale,
             postscale = self.postscale,
             L0DU = "( L0_DATA('Spd(Mult)') > %(pA_SpdMult)s )" % self.getProps(), 
             algos     = []   
             )
        
        Line('ActivityTracking',
             prescale  = self.prescale,
             postscale = self.postscale,
             algos     = [ Hlt1GECUnit( 'Activity' ) ]
             )
        
        from HltTracking.HltReco import MinimalVelo as microBiasVelo
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import Hlt1Line as Line
        from HltLine.HltDecodeRaw import DecodeVELO
        from Configurables import LoKi__VoidFilter
        
        fltr =  LoKi__VoidFilter ( 'velohits'
                                   , Preambulo = ['from LoKiPhys.decorators import *','from LoKiCore.functions import *']
                                   , Code = " in_range( 5999 , CONTAINS('Raw/Velo/LiteClusters') , %(nVeloLowMultMicroBias)s ) " % self.getProps())

        Line('ActivityVelo',
             prescale  = self.prescale,
             postscale = self.postscale,
      #       L0DU = "( L0_DATA('Spd(Mult)') < %(pA_LowMultSpd)s )" % self.getProps(), 
             ODIN = 'jbit( ODIN_EVTTYP,2 )',
             algos     = [ DecodeVELO
                          , fltr
                          , microBiasVelo
                          , Member( 'Hlt::TrackFilter','All'
                                    , Code = [ 'TrALL' ]
                                    , InputSelection = 'TES:%s' % microBiasVelo.outputSelection()
                                    , OutputSelection = '%Decision'
                                    )
                          
                          ]   
             )


        """
        SPD pass through
        """
        Line('GECPassSPD',
             prescale  = self.prescale,
             postscale = self.postscale,
             L0DU = "( L0_DATA('Spd(Mult)') > %(pA_GECPassSPD)s )" % self.getProps(), 
             algos     = []   
             )

        
        from HltLine.HltDecodeRaw import DecodeVELO, DecodeIT, DecodeOT
        
        """
        Velo pass through
        """
        Line('GECPassVelo',
             prescale  = self.prescale,
             postscale = self.postscale,
             algos = [ DecodeVELO,
                       LoKi__VoidFilter ( 'FilterVelo'
                                          , Preambulo = ['from LoKiPhys.decorators import *',
                                                         'from LoKiCore.functions import *']
                                          , Code = "CONTAINS('Raw/Velo/LiteClusters')>%(pA_GECPassVelo)s" % self.getProps()
                                          )     
                       ]
             )


        """
        IT pass through
        """
        Line('GECPassIT',
             prescale  = self.prescale,
             postscale = self.postscale,
             algos = [ DecodeIT,
                       LoKi__VoidFilter ( 'FilterIT'
                                          , Preambulo = ['from LoKiPhys.decorators import *',
                                                         'from LoKiCore.functions import *']
                                          , Code = "CONTAINS('Raw/IT/LiteClusters')>%(pA_GECPassIT)s" % self.getProps()
                                          )     
                       ]
             )


      
        """
        OT pass through
        """
        Line('GECPassOT',
             prescale  = self.prescale,
             postscale = self.postscale,
             algos = [ DecodeOT,
                       LoKi__VoidFilter ( 'FilterOT'
                                          , Preambulo = ['from LoKiPhys.decorators import *',
                                                         'from LoKiCore.functions import *']
                                          , Code = "CONTAINS('Raw/OT/Times')>%(pA_GECPassOT)s" % self.getProps()
                                          )     
                       ]
             )        
