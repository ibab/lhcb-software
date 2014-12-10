# Central Exclusive Production lines 

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *

class Hlt1CEPLinesConf( HltLinesConfigurableUser ):
    __slots__ = {
          'CEP_SpdMult'    :    30.   # dimensionless, Spd Multiplicy cut
        , 'CEP_MaxNVelo'   :    15.   # dimensionless, 
        , 'CEP_PT'         :   500.   # MeV
        , 'CEP_P'          :  6000.   # MeV 
        }

    
    def CEP_preambulo( self ):
        from HltTracking.Hlt1Tracking import ( VeloCandidates,
                                                       LooseForward )
        
        ## define some "common" preambulo 
        Preambulo = [ VeloCandidates( "CEP" ),                      
                      LooseForward ]
        return Preambulo
    
    
    def CEP_streamer( self ):
        from Configurables import LoKi__HltUnit as HltUnit
        CEP_unit = HltUnit(
            'CEPStreamer',
            ##OutputLevel = 1 ,
            Preambulo = self.CEP_preambulo(),
            Code = """
            VeloCandidates
            >>  ( TC_SIZE <= %(CEP_MaxNVelo)s )
            >>  LooseForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass loose forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nLooseForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(CEP_PT)s * MeV ) & ( TrP  > %(CEP_P)s * MeV ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass PT', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nPT' , LoKi.Monitoring.ContextSvc ) )               
            >> ~TC_EMPTY
            """ % self.getProps()
            )
        return CEP_unit

    
    
    
    def __apply_configuration__( self ) : 
        from HltLine.HltLine import Hlt1Line
               
        Hlt1Line(
            'CEP',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( L0_DATA('Spd(Mult)') < %(CEP_SpdMult)s )" % self.getProps(),   
            ##
            algos     = [ self.CEP_streamer() ] 
            )
