from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *
import re

class Hlt1MuonLinesConf( HltLinesConfigurableUser ):
    # steering variables
    __slots__ = { 
        #  Muon Lines
        'DoTiming'                  : False
        ,'SingleMuonHighPT_P'       : 8000.
        ,'SingleMuonHighPT_PT'      : 4800.
        ,'SingleMuonHighPT_TrChi2'  :    4.
        ,'SingleMuonHighPT_TrNTHits'   : 0 #OFF
        ,'SingleMuonHighPT_Velo_NHits' : 0 #OFF
        ,'SingleMuonHighPT_Velo_Qcut'  : 999 #OFF
        ,'SingleMuonHighPT_GEC'     : 'Loose'
        ,'SingleMuonHighPT_TrackType' : 'Long'
        ,'SingleMuonNoIP_P'         : 6000.
        ,'SingleMuonNoIP_PT'        : 1300.
        ,'SingleMuonNoIP_TrChi2'    :    4.
        ,'SingleMuonNoIP_TrNTHits'   : 16
        ,'SingleMuonNoIP_Velo_NHits' : 9
        ,'SingleMuonNoIP_Velo_Qcut'  : 3
        ,'SingleMuonNoIP_GEC'       : 'Loose'
        ,'SingleMuonNoIP_TrackType' :'Long'
        ,'SingleMuonVeloHighPT_P'       : 8000.
        ,'SingleMuonVeloHighPT_PT'      : 4800.
        ,'SingleMuonVeloHighPT_TrChi2'  :    4.
        ,'SingleMuonVeloHighPT_TrNTHits'   : 0 #OFF
        ,'SingleMuonVeloHighPT_Velo_NHits' : 0 #OFF
        ,'SingleMuonVeloHighPT_Velo_Qcut'  : 999 #OFF
        ,'SingleMuonVeloHighPT_GEC'     : 'Loose'
        ,'SingleMuonVeloHighPT_TrackType'     : 'Velo'
        ,'SingleMuonVeloNoIP_P'         : 6000.
        ,'SingleMuonVeloNoIP_PT'        : 1300.
        ,'SingleMuonVeloNoIP_TrChi2'    :    4.
        ,'SingleMuonVeloNoIP_TrNTHits'   : 16
        ,'SingleMuonVeloNoIP_Velo_NHits' : 9
        ,'SingleMuonVeloNoIP_Velo_Qcut'  : 3
        ,'SingleMuonVeloNoIP_GEC'       : 'Loose'
        ,'SingleMuonVeloNoIP_TrackType'     : 'Velo'
        ,'SingleMuonVeloTTHighPT_P'       : 8000.
        ,'SingleMuonVeloTTHighPT_PT'      : 4800.
        ,'SingleMuonVeloTTHighPT_TrChi2'  :    4.
        ,'SingleMuonVeloTTHighPT_TrNTHits'   : 0 #OFF
        ,'SingleMuonVeloTTHighPT_Velo_NHits' : 0 #OFF
        ,'SingleMuonVeloTTHighPT_Velo_Qcut'  : 999 #OFF
        ,'SingleMuonVeloTTHighPT_GEC'     : 'Loose'
        ,'SingleMuonVeloTTHighPT_TrackType'     : 'VeloTT'
        ,'SingleMuonVeloTTNoIP_P'         : 6000.
        ,'SingleMuonVeloTTNoIP_PT'        : 1300.
        ,'SingleMuonVeloTTNoIP_TrChi2'    :    4.
        ,'SingleMuonVeloTTNoIP_TrNTHits'   : 16
        ,'SingleMuonVeloTTNoIP_Velo_NHits' : 9
        ,'SingleMuonVeloTTNoIP_Velo_Qcut'  : 3
        ,'SingleMuonVeloTTNoIP_GEC'       : 'Loose'
        ,'SingleMuonVeloTTNoIP_TrackType'     : 'VeloTT'
        ,'DiMuonLowMass_VxDOCA'     :  0.2
        ,'DiMuonLowMass_VxChi2'     :   25.
        ,'DiMuonLowMass_P'          : 6000.
        ,'DiMuonLowMass_PT'         :  500.
        ,'DiMuonLowMass_TrChi2'     :    4.
        ,'DiMuonLowMass_M'          : 1000.
        ,'DiMuonLowMass_IPChi2'     :    9.
        ,'DiMuonLowMass_GEC'        : 'Loose'
        ,'DiMuonLowMass_TrackType' :'Long'
        ,'DiMuonHighMass_VxDOCA'    :  0.2
        ,'DiMuonHighMass_VxChi2'    :   25.
        ,'DiMuonHighMass_P'         : 6000.
        ,'DiMuonHighMass_PT'        :  500.
        ,'DiMuonHighMass_TrChi2'    :    4.
        ,'DiMuonHighMass_M'         : 2900.
        ,'DiMuonHighMass_GEC'       : 'Loose'
        ,'DiMuonHighMass_TrackType' :'Long'
        
        ,'DiMuonVeloLowMass_VxDOCA'     :  0.2
        ,'DiMuonVeloLowMass_VxChi2'     :   25.
        ,'DiMuonVeloLowMass_P'          : 6000.
        ,'DiMuonVeloLowMass_PT'         :  500.
        ,'DiMuonVeloLowMass_TrChi2'     :    4.
        ,'DiMuonVeloLowMass_M'          : 1000.
        ,'DiMuonVeloLowMass_IPChi2'     :    9.
        ,'DiMuonVeloLowMass_GEC'        : 'Loose'
        ,'DiMuonVeloLowMass_TrackType' :'Velo'
        
        ,'DiMuonVeloHighMass_VxDOCA'    :  0.2
        ,'DiMuonVeloHighMass_VxChi2'    :   25.
        ,'DiMuonVeloHighMass_P'         : 6000.
        ,'DiMuonVeloHighMass_PT'        :  500.
        ,'DiMuonVeloHighMass_TrChi2'    :    4.
        ,'DiMuonVeloHighMass_M'         : 2900.
        ,'DiMuonVeloHighMass_GEC'       : 'Loose'
        ,'DiMuonVeloHighMass_TrackType' :'Velo'

        ,'DiMuonVeloTTLowMass_VxDOCA'     :  0.2
        ,'DiMuonVeloTTLowMass_VxChi2'     :   25.
        ,'DiMuonVeloTTLowMass_P'          : 6000.
        ,'DiMuonVeloTTLowMass_PT'         :  500.
        ,'DiMuonVeloTTLowMass_TrChi2'     :    4.
        ,'DiMuonVeloTTLowMass_M'          : 1000.
        ,'DiMuonVeloTTLowMass_IPChi2'     :    9.
        ,'DiMuonVeloTTLowMass_GEC'        : 'Loose'
        ,'DiMuonVeloTTLowMass_TrackType' :'VeloTT'
        
        ,'DiMuonVeloTTHighMass_VxDOCA'    :  0.2
        ,'DiMuonVeloTTHighMass_VxChi2'    :   25.
        ,'DiMuonVeloTTHighMass_P'         : 6000.
        ,'DiMuonVeloTTHighMass_PT'        :  500.
        ,'DiMuonVeloTTHighMass_TrChi2'    :    4.
        ,'DiMuonVeloTTHighMass_M'         : 2900.
        ,'DiMuonVeloTTHighMass_GEC'       : 'Loose'
        ,'DiMuonVeloTTHighMass_TrackType' :'VeloTT'
        ,'MultiMuonNoIP_P'          : 6000.
        ,'MultiMuonNoIP_PT'         :  500.
        ,'MultiMuonNoIP_TrChi2'     :    4.
        ,'MultiMuonNoIP_GT'         :  2.5
        ,'MultiMuonNoIP_GEC'        : 'Loose'
        ,'MultiMuonNoIP_TrackType'  : 'Long'
        ,'MultiMuonVeloNoIP_P'          : 6000.
        ,'MultiMuonVeloNoIP_PT'         :  500.
        ,'MultiMuonVeloNoIP_TrChi2'     :    4.
        ,'MultiMuonVeloNoIP_GT'         :  2.5
        ,'MultiMuonVeloNoIP_GEC'        : 'Loose'
        ,'MultiMuonVeloNoIP_TrackType'  : 'Velo'
        ,'MultiMuonVeloTTNoIP_P'          : 6000.
        ,'MultiMuonVeloTTNoIP_PT'         :  500.
        ,'MultiMuonVeloTTNoIP_TrChi2'     :    4.
        ,'MultiMuonVeloTTNoIP_GT'         :  2.5
        ,'MultiMuonVeloTTNoIP_GEC'        : 'Loose'
        ,'MultiMuonVeloTTNoIP_TrackType'  : 'VeloTT'
        
        ,'L0Channels'               : { 'SingleMuonHighPT' : ( 'Muon', ),
                                        'SingleMuonNoIP'   : ( 'Muon', ),
                                        'SingleMuonVeloHighPT' : ( 'Muon', ),
                                        'SingleMuonVeloNoIP'   : ( 'Muon', ),
                                        'SingleMuonVeloTTHighPT' : ( 'Muon', ),
                                        'SingleMuonVeloTTNoIP'   : ( 'Muon', ),
                                        'DiMuonLowMass'    : ( 'Muon', 'DiMuon' ),
                                        'DiMuonHighMass'   : ( 'Muon', 'DiMuon' ),
                                        'DiMuonVeloLowMass'    : ( 'Muon', 'DiMuon' ),
                                        'DiMuonVeloHighMass'   : ( 'Muon', 'DiMuon' ),
                                        'DiMuonVeloTTLowMass'    : ( 'Muon', 'DiMuon' ),
                                        'DiMuonVeloTTHighMass'   : ( 'Muon', 'DiMuon' ),
                                        'MultiMuonNoIP'    : ( 'Muon', 'DiMuon' ),
                                        'MultiMuonVeloNoIP'    : ( 'Muon', 'DiMuon' ),
                                        'MultiMuonVeloTTNoIP'    : ( 'Muon', 'DiMuon' ) }
        ,'Priorities'               : { 'SingleMuonHighPT' : 7,
                                        'SingleMuonNoIP'   : 6,
                                        'SingleMuonVeloHighPT' : 7,
                                        'SingleMuonVeloNoIP'   : 6,
                                        'SingleMuonVeloTTHighPT' : 7,
                                        'SingleMuonVeloTTNoIP'   : 6,
                                        'DiMuonLowMass'    : 5,
                                        'DiMuonHighMass'   : 4,
                                        'MultiMuonNoIP'    : 8 ,
                                        'MultiMuonNoIP'    : 8 ,
                                        'MultiMuonNoIP'    : 8 }
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

    def singleMuon_preambulo( self, properties ):
        # Common preambulo customized with track type from properties 
        preambulo = []
        print properties
        if "TrackType" in properties : 
            if properties["TrackType"] == "Velo" :
                # Velo tracks 
                from HltTracking.Hlt1Tracking import ( VeloCandidates,
                                                               MatchVeloMuon, IsMuon,
                                                               LooseForward, FitTrack )
                preambulo = [ VeloCandidates( properties[ 'name' ] ),
                              MatchVeloMuon,
                              LooseForward,
                              IsMuon,
                              FitTrack ]
            elif properties["TrackType"] == "VeloTT" :
                # VeloTT tracks 
                from HltTracking.Hlt1Tracking import ( VeloTTCandidates,
                                                               MatchVeloMuon, IsMuon,
                                                               LooseForward, FitTrack )
                preambulo = [ VeloTTCandidates( properties[ 'name' ] ),
                              MatchVeloMuon,
                              LooseForward,
                              IsMuon,
                              FitTrack ]
            else:
                # Long tracks 
                from HltTracking.Hlt1Tracking import ( TrackCandidates, IsMuon,
                                                               FitTrack )
                preambulo = [ TrackCandidates( properties[ 'name' ] ),
                              IsMuon,
                              FitTrack ]
        else:
            # for the moment default for other tracks is velo 
            from HltTracking.Hlt1Tracking import ( VeloCandidates,
                                                           MatchVeloMuon, IsMuon,
                                                           LooseForward, FitTrack )
            preambulo = [ VeloCandidates( properties[ 'name' ] ),
                          MatchVeloMuon,
                          LooseForward,
                          IsMuon,
                          FitTrack ]
            
        return preambulo

    def singleMuon_streamer( self, properties ):
        # SingleMuon with LongTracks (TrackCandidates) 
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleMuon_preambulo( properties ),
            Code = """
            TrackCandidates
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), unit ]

    def singleMuonVelo_streamer( self, properties ):
        # SingleMuon with VeloTracks (VeloCandidates)
        # Mainly for tests
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleMuon_preambulo( properties ),
            Code = """
            VeloCandidates
            >>  MatchVeloMuon
            >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & ( TrNVELOMISS < %(Velo_Qcut)s )  )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  LooseForward
            >>  (TrTNORMIDC > %(TrNTHits)s ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), unit ]

    def singleMuonVeloTT_streamer( self, properties ):
        # SingleMuon with VeloTTTracks (VeloTTCandidates)
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  MatchVeloMuon
            >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & ( TrNVELOMISS < %(Velo_Qcut)s )  )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  LooseForward
            >>  (TrTNORMIDC > %(TrNTHits)s ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), unit ]

    def diMuon_preambulo( self, properties ):
        ## define some "common" preambulo 
        preambulo = self.singleMuon_preambulo( properties ) + \
             [ "VertexConf = LoKi.Hlt1.VxMakerConf( %(VxDOCA)f * mm, %(VxChi2)f )" % properties,
               "MakeDiMuons = TC_VXMAKE4( '', VertexConf )",
               "from LoKiPhys.decorators import RV_MASS" ]
        return preambulo

    def diMuon_streamer( self, properties ):
        # DiMuon with LongTracks (TrackCandidates) 
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            TrackCandidates
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), unit ]

    def diMuonVelo_streamer( self, properties ):
        # DiMuon with VeloTracks (VeloCandidates)
        # Mainly for tests 
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            VeloCandidates
            >>  MatchVeloMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  LooseForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), unit ]

    def diMuonVeloTT_streamer( self, properties ):
        # DiMuon with VeloTTTracks (VeloTTCandidates)
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  MatchVeloMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  LooseForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), unit ]


    def diMuonDetached_streamer( self, properties ):
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            TrackCandidates
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & ( Tr_HLTMIPCHI2( 'PV3D' ) > %(IPChi2)s ) )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), PV3D('Hlt1'), unit ]

    def diMuonVeloDetached_streamer( self, properties ):
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            VeloCandidates
            >>  MatchVeloMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  LooseForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & ( Tr_HLTMIPCHI2( 'PV3D' ) > %(IPChi2)s ) )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), PV3D('Hlt1'), unit ]

    def diMuonVeloTTDetached_streamer( self, properties ):
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.diMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  MatchVeloMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  LooseForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrCHI2PDOF < %(TrChi2)s ) & ( Tr_HLTMIPCHI2( 'PV3D' ) > %(IPChi2)s ) )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > %(M)s * MeV ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), PV3D('Hlt1'), unit ]

    def multiMuon_streamer( self, properties ):
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleMuon_preambulo( properties ),
            Code = """
            TrackCandidates
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >>  TC_SIZE > %(GT)s
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), unit ]
        
    def multiMuonVelo_streamer( self, properties ):
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleMuon_preambulo( properties ),
            Code = """
            VeloCandidates
            >>  MatchVeloMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  LooseForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >>  TC_SIZE > %(GT)s
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), unit ]

    def multiMuonVeloTT_streamer( self, properties ):
        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        unit = HltUnit(
            'Hlt1%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.singleMuon_preambulo( properties ),
            Code = """
            VeloTTCandidates
            >>  MatchVeloMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  LooseForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >>  TC_SIZE > %(GT)s
            """ % properties
            )
        gec = properties[ 'GEC' ]
        return [ Hlt1GECUnit( gec ), unit ]

    def build_line( self, name, streamer ):
        from HltLine.HltLine import Hlt1Line
        algos = [ self.do_timing( unit ) if self.getProp('DoTiming') else unit for unit in streamer( self.localise_props( name ) ) ]
        priorities = self.getProp( "Priorities" )
        priority = priorities[ name ] if name in priorities else None
        line = Hlt1Line(
            name,
            prescale  = self.prescale,
            postscale = self.postscale,
            priority  = priority,
            L0DU = "|".join( [ "L0_CHANNEL('%s')" % l0 for l0 in self.L0Channels[ name ] ] ) ,
            algos = algos
            )

    def __apply_configuration__( self ) : 
         ## Create the lines
        to_build = [ ( 'SingleMuonHighPT', self.singleMuon_streamer ),
                     ( 'SingleMuonNoIP',   self.singleMuon_streamer ),
                     ( 'DiMuonLowMass',    self.diMuonDetached_streamer ),
                     ( 'DiMuonHighMass',   self.diMuon_streamer ),
                     ( 'MultiMuonNoIP',    self.multiMuon_streamer ) ]
        # Velo 
        to_build += [ ( 'SingleMuonVeloHighPT', self.singleMuonVelo_streamer ),
                     ( 'SingleMuonVeloNoIP',   self.singleMuonVelo_streamer ),
                     ( 'DiMuonVeloLowMass',    self.diMuonVeloDetached_streamer ),
                     ( 'DiMuonVeloHighMass',   self.diMuonVelo_streamer ),
                     ( 'MultiMuonVeloNoIP',    self.multiMuonVelo_streamer ) ]
        # VeloTT
        #to_build += [ ( 'SingleMuonVeloTTHighPT', self.singleMuonVeloTT_streamer ),
        #             ( 'SingleMuonVeloTTNoIP',   self.singleMuonVeloTT_streamer ),
        #             ( 'DiMuonVeloTTLowMass',    self.diMuonVeloTTDetached_streamer ),
        #             ( 'DiMuonVeloTTHighMass',   self.diMuonVeloTT_streamer ),
        #             ( 'MultiMuonVeloTTNoIP',    self.multiMuonVeloTT_streamer ) ]
        for line, streamer in to_build:
            self.build_line( line, streamer )

        from HltLine.HltLine import Hlt1Line   as Line
        # The pass-through NoPV line
        Hlt1NoPVMuonPassThrough = Line( 'NoPVPassThrough'
                                        , prescale = self.prescale
                                        , L0DU = "L0_CHANNEL_RE('.*lowMult')"
					, ODIN = "~jbit( ODIN_EVTTYP,0 )"
                                        , postscale = self.postscale)

