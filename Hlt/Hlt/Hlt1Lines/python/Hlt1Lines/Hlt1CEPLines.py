# Central Exclusive Production lines 

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *

class Hlt1CEPLinesConf( HltLinesConfigurableUser ):
    __slots__ = {
          'SpdMult'    :    30.   # dimensionless, Spd Multiplicy cut
        , 'MaxNVelo'   :    15.   # dimensionless, 
        , 'TrChi2'     :     5.   # dimensionless, 
        , 'PT'         :   500.   # MeV
        , 'P'          :  6000.   # MeV 
        }
    
    def preambulo( self ):
        ## define some "common" preambulo 
        from HltTracking.Hlt1Tracking import TrackCandidates
        return [ TrackCandidates( "CEP" ) ]
    
    def streamer( self ):

        props = self.getProps()

        ## VoidFilter to cut on the number of Velo tracks.
        from Configurables import LoKi__VoidFilter as VoidFilter
        from HltTracking.HltSharedTracking import MinimalVelo
        props['Velo'] = MinimalVelo.outputSelection()
        code = "CONTAINS('%(Velo)s') < %(MaxNVelo)s" % props
        veloFilter = VoidFilter('Hlt1CEPNVeloFilter', Code = code)

        ## Streamer
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1CEPStreamer',
            ##OutputLevel = 1 ,
            Preambulo = self.preambulo(),
            Code = """
            TrackCandidates
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass TrackFit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nFit' , LoKi.Monitoring.ContextSvc ) ) 
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s * MeV ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass PT', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nPT' , LoKi.Monitoring.ContextSvc ) )               
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  SINK('Hlt1CEPDecision')
            >> ~TC_EMPTY
            """ % props
            )
        return [veloFilter, unit]
    
    def __apply_configuration__( self ) : 
        from HltLine.HltLine import Hlt1Line
               
        Hlt1Line(
            'CEP',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( L0_DATA('Spd(Mult)') < %(SpdMult)s )" % self.getProps(),   
            ##
            algos     = self.streamer()
            )
