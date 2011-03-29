from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *
import re

class Hlt1ElectronLinesConf( HltLinesConfigurableUser ):
    # steering variables
    __slots__ = { 
        #  Electron Lines
        'DoTiming'                    : False
        ,'SingleElectronNoIP_P'       : 8000
        ,'SingleElectronNoIP_PT'      : 4800
        ,'SingleElectronNoIP_TrChi2'  :    4
        ,'SingleElectronNoIP_TrNTHits'   : 0 #OFF
        ,'SingleElectronNoIP_Velo_NHits' : 0 #OFF
        ,'SingleElectronNoIP_Velo_Qcut'  : 999 #OFF
        ,'L0Channels'               : { 'SingleElectronNoIP'   : ( 'Electron', ) }
        }
    
    def localise_props( self, prefix ):
        ps = self.getProps()
        props = dict( [ ( key.split( '_', 1 )[ 1 ], ps[ key ] ) for key in ps if key.find( prefix ) != -1 ] )
        props[ 'name' ] = prefix
        return props

    def do_timing( self, unit ):
        from Configurables import LoKi__HltUnit as HltUnit
        if not isinstance( unit, HltUnit ):
            return unit
        reco = set()
        for entry in unit.Preambulo:
            s = entry.split( '=' )
            if s[ 0 ].find( 'PV3D' ) != -1 or s[ 0 ].find( 'GEC' ) != -1: continue
            if len( s ) > ( 1 ):
                reco.add( s[ 0 ].strip() )
        name = unit.name()[ 4 : unit.name().find( 'Streamer' ) ]
        code = unit.Code
        for step in reco:
            sub = " ( timer( '%s_%s' )" % ( name, step ) + ' % ' +  step + ' ) '
            code = re.sub( '\\s+%s\\s+' % step, sub, code )
        unit.Code = code
        return unit

    def add_gec( self, unit ):
        from Hlt1Lines.Hlt1GECs import Hlt1GECLooseStreamer
        gec = Hlt1GECLooseStreamer().split( '=', 1 )[ 0 ]
        unit.Code = unit.Code.replace( 'VeloCandidates', '%s * VeloCandidates' % gec ) 
        return unit

    def singleElectron_preambulo( self, properties ):
        from HltTracking.Hlt1TrackUpgradeConf import ( VeloCandidates,
                                                       TightForward, FitTrack )
        from HltTracking.Hlt1TrackMatchConf import MatchVeloElectron
        from Hlt1Lines.Hlt1GECs import Hlt1GECLooseStreamer
        ## define some "common" preambulo 
        preambulo = [ Hlt1GECLooseStreamer(),
                      VeloCandidates( properties[ 'name' ] ),
                      MatchVeloElectron,
                      TightForward,
                      FitTrack ]
        return preambulo


    def singleElectron_streamer( self, properties ):
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleElectron_preambulo( properties ),
            Code = """
            VeloCandidates
            >>  MatchVeloElectron
            >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & ( TrNVELOMISS < %(Velo_Qcut)s )  )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  TightForward
            >>  (TrTNORMIDC > %(TrNTHits)s ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        from HltTracking.HltReco import Velo
        return [ Velo, self.add_gec( unit ) ]

    def build_line( self, name, streamer ):
        from HltLine.HltLine import Hlt1Line
        algos = [ self.do_timing( unit ) if self.getProp('DoTiming') else unit for unit in streamer( self.localise_props( name ) ) ]
        L0DU = self.getProp( "L0Channels" )[ name ]
        if type( L0DU ) != type( "" ):
            L0DU = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in L0DU ] )
        line = Hlt1Line(
            name,
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = L0DU,
            algos = algos
            )

    def __apply_configuration__( self ) : 
         ## Create the lines
        to_build = [ ( 'SingleElectronNoIP',   self.singleElectron_streamer ) ]
        for line, streamer in to_build:
            self.build_line( line, streamer )
