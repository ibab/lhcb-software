from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *

class Hlt1NewMuonLinesConf( HltLinesConfigurableUser ):
    # steering variables
    __slots__ = { 
        #  Muon Lines
         'DiMuonLowMass_VxDOCA'     :  0.2
        ,'DiMuonLowMass_VxChi2'     :   25
        ,'DiMuonLowMass_P'          : 6000
        ,'DiMuonLowMass_PT'         :  500
        ,'DiMuonLowMass_TrChi2'     :    4
        ,'DiMuonLowMass_M'          : 1000
        ,'DiMuonHighMass_VxDOCA'    :  0.2
        ,'DiMuonHighMass_VxChi2'    :   25
        ,'DiMuonHighMass_P'         : 6000
        ,'DiMuonHighMass_PT'        :  500
        ,'DiMuonHighMass_TrChi2'    :    4
        ,'DiMuonHighMass_M'         : 2900
        ,'SingleMuonHighPT_P'       : 6000
        ,'SingleMuonHighPT_PT'      :  500
        ,'SingleMuonHighPT_TrChi2'  :    4
        ,'SingleMuonNoIP_P'         : 6000
        ,'SingleMuonNoIP_PT'        :  500
        ,'SingleMuonNoIP_TrChi2'    :    4
        ,'channels'                 : { 'DiMuonLowMass'    : ( 'Muon', 'DiMuon' ),
                                        'DiMuonHighMass'   : ( 'Muon', 'DiMuon' ),
                                        'SingleMuonHighPT' : ( 'MuonHigh', ),
                                        'SingleMuonNoIP'   : ( 'Muon', ) }
        }
    
    def localise_props( self, prefix ):
        ps = self.getProps()
        props = dict( [ ( key.split( '_' )[ 1 ], ps[ key ] ) for key in ps if key.find( prefix ) != -1 ] )
        props[ 'name' ] = prefix
        return props

    def preambulo( self, properties ):
        from HltTracking.Hlt1Streamers import ( VeloCandidates,
                                                MatchVeloMuon, IsMuon,
                                                LooseForward, FitTrack )
        ## define some "common" preambulo 
        Preambulo = [ VeloCandidates( properties[ 'name' ] ),
                      MatchVeloMuon,
                      LooseForward,
                      FitTrack,
                      IsMuon ]
        return Preambulo

    def diMuon_streamer( self, properties ):
        from Configurables import LoKi__HltUnit as HltUnit
        extra_pre = [
            "VertexConf = LoKi.Hlt1.VxMakerConf( %(VxDOCA)f * mm, %(VxChi2)f )" % properties,
            "MakeDiMuons = TC_VXMAKE4( '', VertexConf )",
            "from LoKiPhys.decorators import RV_MASS" ]

        diMuon_unit = HltUnit(
            '%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.preambulo( properties ) + extra_pre,
            Code = """
            VeloCandidates
            >>  execute( 'MuonRec' )
            >>  MatchVeloMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  execute( decodeIT )
            >>  LooseForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
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
        return diMuon_unit

    def singleMuon_streamer( self, properties ):
        from Configurables import LoKi__HltUnit as HltUnit
        singleMuon_unit = HltUnit(
            '%(name)sStreamer' % properties,
            ##OutputLevel = 1 ,
            Preambulo = self.preambulo( properties ),
            Code = """
            VeloCandidates
            >>  execute( 'MuonRec' )
            >>  MatchVeloMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass match', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nMatched' , LoKi.Monitoring.ContextSvc ) )
            >>  execute( decodeIT )
            >>  TightForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(PT)s * MeV ) & ( TrP  > %(P)s  * MeV ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(TrChi2)s )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1%(name)sDecision' )
            >> ~TC_EMPTY
            """ % properties
            )
        return singleMuon_unit

    def build_line( self, name, streamer ):
        from HltLine.HltLine import Hlt1Line
        line = Hlt1Line(
            name,
            prescale  = self.prescale,
            postscale = self.prescale,
            L0DU = "|".join( [ "L0_CHANNEL('%s')" % l0 for l0 in self.channels[ name ] ] ) ,
            algos = [ streamer( self.localise_props( name ) ) ] 
            )

    def __apply_configuration__( self ) : 
         ## Create the lines
        to_build = [ ( 'DiMuonLowMass',    self.diMuon_streamer ),
                     ( 'DiMuonHighMass',   self.diMuon_streamer ),
                     ( 'SingleMuonHighPT', self.singleMuon_streamer ),
                     ( 'SingleMuonNoIP',   self.singleMuon_streamer ) ]
        for line, streamer in to_build:
            self.build_line( line, streamer )
