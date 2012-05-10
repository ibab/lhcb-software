# =============================================================================
## @@file
#  Configuration of Displaced Vertex line
#  @@author Pieter David pieter.david@cern.ch
#  @@date 2012-03-15
# =============================================================================
from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import *

class Hlt1DisplVertexLinesConf( HltLinesConfigurableUser ):
    # steering variables
    __slots__ = {
        # Displaced Vertex line
        "DoTiming"                          : False
        , "VertexDisplVertex_DOCABL"        : "2.0*mm"
        , "VertexDisplVertex_VELO_NSP"      : "3"
        , "VertexDisplVertex_VELO_NCSP"     : "2"
        , "VertexDisplVertex_VELOTrChi2"    : "2.5"
        , "VertexDisplVertex_VX_DOCA"       : "0.3*mm"
        , "VertexDisplVertex_VX_CHI2"       : "1000000000.0"
        , "VertexDisplVertex_VX_RHO"        : "12.0*mm"
        , "VertexDisplVertex_Tr_P"          : "10.0*GeV"
        , "VertexDisplVertex_Tr_PT"         : "1.7*GeV"
        , "VertexDisplVertex_Tr_CHI2"       : "2.5"
        , "VertexDisplVertex_GEC"           : "Loose"
        , "VertexDisplVertex_TracksForVx"   : "50"
        ## Prescaled line without cut on the number of tracks before vertexing
        , "VertexDisplVertexPS_DOCABL"      : "2.0*mm"
        , "VertexDisplVertexPS_VELO_NSP"    : "3"
        , "VertexDisplVertexPS_VELO_NCSP"   : "2"
        , "VertexDisplVertexPS_VELOTrChi2"  : "2.5"
        , "VertexDisplVertexPS_VX_DOCA"     : "0.3*mm"
        , "VertexDisplVertexPS_VX_CHI2"     : "1000000000.0"
        , "VertexDisplVertexPS_VX_RHO"      : "12.0*mm"
        , "VertexDisplVertexPS_Tr_P"        : "10.0*GeV"
        , "VertexDisplVertexPS_Tr_PT"       : "1.7*GeV"
        , "VertexDisplVertexPS_Tr_CHI2"     : "2.5"
        , "VertexDisplVertexPS_GEC"         : "Loose"
        , "Priorities"                    : { "VertexDisplVertex"   : 128
                                            , "VertexDisplVertexPS" : 128 }
        , "Prescale"                      : { "Hlt1VertexDisplVertexPS" : 0.001 }
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

    def vertexDisplVertex_trackPreambulo( self, properties ):
        from HltTracking.Hlt1TrackUpgradeConf import VeloCandidates
        return [ VeloCandidates( properties["name"] ) ]

    def vertexDisplVertex_vertexPreambulo( self, properties ):
        from HltTracking.Hlt1VertexUpgradeConf import ( LooseForward
                                                      , FitTracks
                                                      , VeloOnlyFitTracks
                                                      )

        preambulo = [ LooseForward(allowFail=True)
                    , FitTracks(allowFail=True)
                    , VeloOnlyFitTracks(allowFail=True)
                    , "MakeDisplVertices = TC_VXMAKE4   ( '', LoKi.Hlt1.VxMakerConf( %(VX_DOCA)s, %(VX_CHI2)s ) )" % properties
                    ]

        return preambulo

    def vertexDisplVertex_preambulo( self, properties ):
        from HltTracking.Hlt1TrackUpgradeConf import VeloCandidates
        from HltTracking.Hlt1VertexUpgradeConf import ( LooseForward
                                                      , FitTracks
                                                      , VeloOnlyFitTracks
                                                      )

        preambulo = [ VeloCandidates( properties["name"] )
                    , LooseForward(allowFail=True)
                    , FitTracks(allowFail=True)
                    , VeloOnlyFitTracks(allowFail=True)
                    , "MakeDisplVertices = TC_VXMAKE4   ( '', LoKi.Hlt1.VxMakerConf( %(VX_DOCA)s, %(VX_CHI2)s ) )" % properties
                    ]

        return preambulo

    def vertexDisplVertex_streamer( self, properties ):
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from HltTracking.HltPVs import PV3D
        from Configurables import LoKi__HltUnit as HltUnit
        from HltLine.HltLine import bindMembers

        algos = []

        gec = properties["GEC"]
        algos.append( Hlt1GECUnit( properties["GEC"] ) )

        algos.append( PV3D() )

        trackUnit = HltUnit(
            "Hlt1%(name)sTrackStreamer" % properties
          , Preambulo = self.vertexDisplVertex_trackPreambulo( properties )
          , Code = """
              VeloCandidates
              >>  tee ( monitor( TC_SIZE>0, '# events pass Velo candidates', LoKi.Monitoring.ContextSvc ) )
              >>  tee ( monitor( TC_SIZE  , '# candidates pass Velo candidates', LoKi.Monitoring.ContextSvc ) )
              >>  ( ( TrNCONSECVELOSPACEPOINTS > %(VELO_NCSP)s ) & ( TrNVELOSPACEPOINTS > %(VELO_NSP)s ) )
              >>  tee ( monitor( TC_SIZE>0, '# events pass Velo spacepoints cuts', LoKi.Monitoring.ContextSvc ) )
              >>  tee ( monitor( TC_SIZE  , '# candidates pass Velo spacepoints cuts', LoKi.Monitoring.ContextSvc ) )
              >>  ( Tr_FASTDOCATOBEAMLINE(5.) > %(DOCABL)s )
              >>  tee ( monitor( TC_SIZE>0, '# events pass DOCABL cut', LoKi.Monitoring.ContextSvc ) )
              >>  tee ( monitor( TC_SIZE  , '# candidates pass DOCABL cut', LoKi.Monitoring.ContextSvc ) )
              >>  SINK ( 'Hlt1%(name)sTracks' )
              >> ~TC_EMPTY
              """ % properties
          )
        algos.append( trackUnit )

        if "TracksForVx" in properties:
            combinatoricsGECUnit = HltUnit(
                "Hlt1%(name)sCombinatoricsCut" % properties
              , Code = """
                  SELECTION('Hlt1%(name)sTracks')
                  >> monitor( TC_SIZE , Gaudi.Histo1DDef('# tracks for vertexing', 0., 100., 100 ), 'hNTracksForVertexing%(name)s' ) < %(TracksForVx)s
                  """ % properties
            )
            algos.append( combinatoricsGECUnit )

        vertexUnit = HltUnit(
            "Hlt1%(name)sStreamer" % properties
          , Preambulo = self.vertexDisplVertex_vertexPreambulo( properties )
          , Code = """
              SELECTION ( 'Hlt1%(name)sTracks' )
              >>  MakeDisplVertices
              >>  tee ( monitor( TC_SIZE>0, '# events pass displaced vertex making', LoKi.Monitoring.ContextSvc ) )
              >>  tee ( monitor( TC_SIZE  , '# candidates pass displaced vertex making', LoKi.Monitoring.ContextSvc ) )
              >>  ( VX_BEAMSPOTRHO(5.) > %(VX_RHO)s )
              >>  tee ( monitor( TC_SIZE>0, '# events pass RHO cut', LoKi.Monitoring.ContextSvc ) )
              >>  tee ( monitor( TC_SIZE  , '# candidates pass RHO cut', LoKi.Monitoring.ContextSvc ) )
              >>  LooseForward
              >>  tee ( monitor( TC_SIZE>0, '# events pass forward', LoKi.Monitoring.ContextSvc ) )
              >>  tee ( monitor( TC_SIZE  , '# candidates pass forward', LoKi.Monitoring.ContextSvc ) )
              >>  HLT_TCASRVC_ANY( ( TrPT > %(Tr_PT)s ) & ( TrP > %(Tr_P)s), True )
              >>  tee ( monitor( TC_SIZE>0, '# events pass P, PT cut', LoKi.Monitoring.ContextSvc ) )
              >>  tee ( monitor( TC_SIZE  , '# candidates pass P, PT cut', LoKi.Monitoring.ContextSvc ) )
              >>  FitTracks
              >>  tee ( monitor( TC_SIZE>0, '# events pass track fit', LoKi.Monitoring.ContextSvc ) )
              >>  tee ( monitor( TC_SIZE  , '# candidates pass track fit', LoKi.Monitoring.ContextSvc ) )
              >>  HLT_TCASRVC_ANY( ( TrCHI2PDOF < %(Tr_CHI2)s ), True )
              >>  tee ( monitor( TC_SIZE>0, '# events pass track CHI2 cut', LoKi.Monitoring.ContextSvc ) )
              >>  tee ( monitor( TC_SIZE  , '# candidates pass track CHI2', LoKi.Monitoring.ContextSvc ) )
              >>  SINK ( 'Hlt1%(name)sDecision' )
              >> ~TC_EMPTY
              """ % properties
          )

        algos.append( vertexUnit )

        return bindMembers( "Hlt1%(name)sAlgos" % properties, algos ).setOutputSelection( "Hlt1%(name)sDecision" )

    def build_line( self, name, streamer ):
        from HltLine.HltLine import Hlt1Line
        algos = [ self.do_timing( unit ) if self.getProp("DoTiming") else unit for unit in streamer( self.localise_props( name ) ) ]
        priorities = self.getProp( "Priorities" )
        priority = priorities[ name ] if name in priorities else None
        line = Hlt1Line( name
                       , prescale  = self.prescale
                       , postscale = self.postscale
                       , priority  = priority
                       , L0DU      = "L0_DECISION_PHYSICS"
                       , algos     = algos
                       )

    def __apply_configuration__( self ):
        ## create the lines
        to_build = [ ( "VertexDisplVertex"  , self.vertexDisplVertex_streamer )
                   , ( "VertexDisplVertexPS", self.vertexDisplVertex_streamer ) ]
        for line, streamer in to_build:
            self.build_line( line, streamer )
