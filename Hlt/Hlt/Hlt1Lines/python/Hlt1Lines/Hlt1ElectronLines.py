from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *
import re

class Hlt1ElectronLinesConf( HltLinesConfigurableUser ):
    # steering variables
    __slots__ = { 
        #  Electron Lines
        'DoTiming'                                 : False
        ,'SingleElectronNoIP_L0ElectronThreshold'  :   254
        ,'SingleElectronNoIP_P'                    :  8000
        ,'SingleElectronNoIP_PT'                   :  4800
        ,'SingleElectronNoIP_TrChi2'               :     4
        ,'SingleElectronNoIP_TrNTHits'             :     0 #OFF
        ,'SingleElectronNoIP_Velo_NHits'           :     0 #OFF
        ,'SingleElectronNoIP_Velo_Qcut'            :   999 #OFF
        ,'SingleElectronNoIP_GEC'                  : 'Loose'
        #ElectronTrack cuts
        ,'ElectronTrack_L0ElectronThreshold'  :   254
        ,'ElectronTrack_TrChi2'               :     4
        ,'ElectronTrack_TrNTHits'             :     0 #OFF
        ,'ElectronTrack_Velo_NHits'           :     0 #OFF
        ,'ElectronTrack_Velo_Qcut'            :   999 #OFF
        ,'ElectronTrack_GEC'                  : 'Loose'
        ,'ElectronTrack_P'                    :  6000
        ,'ElectronTrack_PT'                   :  500
        ,'ElectronTrack_VxDOCA'               :  0.2
        ,'ElectronTrack_VxChi2'               :   25.
        ,'ElectronTrack_M'                    : 100.
        ,'ElectronTrack_IP'                   :  0.1
        ,'ElectronTrack_IPChi2'               :  9

        #TrackElectron cuts
        ,'TrackElectron_L0ElectronThreshold'  :   254
        ,'TrackElectron_TrChi2'               :     4
        ,'TrackElectron_TrNTHits'             :     0 #OFF
        ,'TrackElectron_Velo_NHits'           :     0 #OFF
        ,'TrackElectron_Velo_Qcut'            :   999 #OFF
        ,'TrackElectron_GEC'                  : 'Loose'
        ,'TrackElectron_P'                    :  6000
        ,'TrackElectron_PT'                   :  500
        ,'TrackElectron_IP'                   :  0.1
        ,'TrackElectron_IPChi2'               :  9
        

        ,'L0Channels'                              : { 'SingleElectronNoIP'   : ( 'Electron', ) 
                                                     , 'ElectronTrack'        : ( 'Electron', ) 
                                                     , 'TrackElectron'           : ( 'Electron', ) 
                                                     }
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

    #make into a function
    def getCaloUnit( self, properties ) :
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.Hlt1Tracking import L0CaloCandidates
        caloUnit = HltUnit(
            'Hlt1%(name)sL0CaloStreamer' % properties,
            Preambulo = [ L0CaloCandidates( properties[ 'name' ] ) ],
            Code = """
            L0CaloCandidates
            >>  TC_CUT( LoKi.L0.L0CaloCut( 0, %(L0ElectronThreshold)s ), 0 )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass L0CaloCut', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nCaloCandidates' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( '%(CaloCandidates)s' )
            >> ~TC_EMPTY
            """ % properties
            )
        return caloUnit

    def singleElectron_preambulo( self, properties ):
        from HltTracking.Hlt1Tracking import ( TrackCandidates, FitTrack )
        from HltTracking.Hlt1TrackMatchConf import FilterVeloL0Calo
        from Configurables import Hlt__L0Calo2Candidate

        ## define the preambulo 
        preambulo = [ TrackCandidates( properties[ 'name' ] ),
                      FilterVeloL0Calo( properties[ 'CaloCandidates' ] ),
                      FitTrack ]
        return preambulo

    def singleElectron_streamer( self, properties ):
        from Configurables import LoKi__HltUnit as HltUnit
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        properties[ 'CaloCandidates' ] = 'CaloCandidates'

        trackUnit = HltUnit(
            'Hlt1%(name)sTrackStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleElectron_preambulo( properties ),
            Code = """
            TrackCandidates
            >>  FilterVeloL0Calo
            >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & ( TrNVELOMISS < %(Velo_Qcut)s ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  (TrTNORMIDC > %(TrNTHits)s ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrCHI2PDOF < %(TrChi2)s ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        return [ Hlt1GECUnit( properties[ 'GEC' ] )
                , self.getCaloUnit( properties )
                , trackUnit ]

    def singleTrackElectron_streamer( self, properties ):
        from Configurables import LoKi__HltUnit as HltUnit
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        #properties[ 'CaloCandidates' ] = 'CaloCandidates'
        properties[ 'CaloCandidates' ] = 'TrackElectronCandidates'

        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.Hlt1Tracking import L0CaloCandidates
        caloUnit = HltUnit(
            'Hlt1%(name)sL0CaloStreamer' % properties,
            Preambulo = [ L0CaloCandidates( properties[ 'name' ] ) ],
            Code = """
            L0CaloCandidates
            >>  TC_CUT( LoKi.L0.L0CaloCut( 0, %(L0ElectronThreshold)s ), 0 )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass L0CaloCut', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nCaloCandidates' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( '%(CaloCandidates)s' )
            >> ~TC_EMPTY
            """ % properties
            )

        trackUnit = HltUnit(
            'Hlt1%(name)sTrackStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleElectron_preambulo( properties ),
            Code = """
            TrackCandidates
            >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & ( TrNVELOMISS < %(Velo_Qcut)s )   \
                & ( Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm) )  
            >>  (TrTNORMIDC > %(TrNTHits)s ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrCHI2PDOF < %(TrChi2)s )\
                & ( Tr_HLTMIPCHI2 ( 'PV3D' ) > %(IPChi2)s ) ) 
            >>  FilterVeloL0Calo
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        from HltTracking.HltPVs import PV3D
        return [ Hlt1GECUnit( properties[ 'GEC' ] )
                , self.getCaloUnit( properties )
                , PV3D('Hlt1')
                , trackUnit ]


    def singleElectronPlusTrack_preambulo( self, properties ) :
        from HltTracking.Hlt1Tracking import LooseForward
        from HltTracking.Hlt1TrackMatchConf import FilterVeloL0Calo
        from HltTracking.Hlt1TrackMatchConf import MatchVeloL0CaloCands
        ## define some "common" preambulo 
        preambulo = self.singleElectron_preambulo( properties ) + \
             [ "VertexConf = LoKi.Hlt1.VxMakerConf( %(VxDOCA)f * mm, %(VxChi2)f )" % properties,
               "MakeVertex = TC_VXMAKE4( '', VertexConf )",
               "from LoKiPhys.decorators import RV_MASS" , 
               MatchVeloL0CaloCands( properties[ 'CaloCandidates' ] ),
               LooseForward ]
        return preambulo

    def singleElectronPlusTrack_streamer( self, properties ) :
        from Configurables import LoKi__HltUnit as HltUnit
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        properties[ 'CaloCandidates' ] = 'ElectronTrackCandidates'
        #properties[ 'CaloCandidates' ] = 'CaloCandidates'

        trackUnit = HltUnit(
            'Hlt1%(name)sTrackStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleElectronPlusTrack_preambulo( properties ),
            Code = """
            TrackCandidates
            >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & ( TrNVELOMISS < %(Velo_Qcut)s )   \
                & ( Tr_HLTMIP ( 'PV3D' ) > %(IP)s * mm) )  
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  (TrTNORMIDC > %(TrNTHits)s ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrCHI2PDOF < %(TrChi2)s )\
                & ( Tr_HLTMIPCHI2 ( 'PV3D' ) > %(IPChi2)s ) ) 
            >>  MakeVertex
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'e+' , 'e-' ) < %(M)s * MeV ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nPassedMassCut' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_TrHAS( MatchVeloL0CaloCands ) ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# has electron', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nHasElectron' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        from HltTracking.HltPVs import PV3D
        return [ Hlt1GECUnit( gec )
                , self.getCaloUnit( properties )
                , PV3D('Hlt1')
                    , trackUnit ]




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
        to_build = [ ( 'SingleElectronNoIP', self.singleElectron_streamer ) ]
        to_build += [ ( 'TrackElectron',        self.singleTrackElectron_streamer ) ]
        to_build += [ ( 'ElectronTrack',     self.singleElectronPlusTrack_streamer ) ]
        for line, streamer in to_build:
            self.build_line( line, streamer )
