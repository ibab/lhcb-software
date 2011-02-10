from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *

class Hlt1NewMuonLinesConf( HltLinesConfigurableUser ):
    # steering variables
    __slots__ = { 
        #  Muon Lines
         'L0SingleMuon'             :"Muon"
        ,'L0SingleMuonGEC'          :"Muon"
        ,'DiMuon_VxDOCA'            :  0.2
        ,'DiMuon_VxChi2'            :   25
        ,'DiMuon_P'                 : 6000
        ,'DiMuon_PT'                :  500
        ,'DiMuon_TrChi2'            :    4
        }

    def preambulo( self ):
        from HltTracking.Hlt1Streamers import ( VeloCandidates,
                                                MatchVeloMuon, IsMuon,
                                                LooseForward, FitTrack )
        ## define some "common" preambulo 
        Preambulo = [ VeloCandidates( "DiMuon" ),
                      MatchVeloMuon,
                      LooseForward,
                      FitTrack,
                      IsMuon,
                      ## helpers
                      "VertexConf = LoKi.Hlt1.VxMakerConf( %(DiMuon_VxDOCA)f * mm, \
                                      %(DiMuon_VxChi2)f )" % self.getProps(),
                      "MakeDiMuons = TC_VXMAKE4( '', VertexConf )",
                      "from LoKiPhys.decorators import RV_MASS"
                      ]
        return Preambulo

    def dimuon_streamer( self ):
        from Configurables import LoKi__HltUnit as HltUnit
        dimuon_unit = HltUnit(
            'DiMuonStreamer',
            ##OutputLevel = 1 ,
            Preambulo = self.preambulo(),
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
            >>  ( ( TrPT > %(DiMuon_PT)s * MeV ) & ( TrP  > %(DiMuon_P)s  * MeV ) )
            >>  FitTrack
            >>  tee  ( monitor( TC_SIZE > 0, '# pass fit', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nFitted' , LoKi.Monitoring.ContextSvc ) )
            >>  ( TrCHI2PDOF < %(DiMuon_TrChi2)s )
            >>  IsMuon
            >>  tee  ( monitor( TC_SIZE > 0, '# pass IsMuon', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nIsMuon' , LoKi.Monitoring.ContextSvc ) )
            >>  MakeDiMuons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'mu+' , 'mu-' ) > 1000 ) 
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nDiMuons' , LoKi.Monitoring.ContextSvc ) )
            >>  SINK( 'Hlt1NewDiMuonDecision' )
            >> ~TC_EMPTY
            """ % self.getProps()
            )
        return dimuon_unit
    
    def __apply_configuration__( self ) : 
         ## Create the DiMuon line
        from HltLine.HltLine import Hlt1Line
        Hlt1Line(
            'NewDiMuon',
            ##
            prescale  = self.prescale,
            postscale = self.prescale,
            L0DU = "L0_CHANNEL( 'Muon' ) | L0_CHANNEL( 'DiMuon' )",
            ##
            algos     = [ self.dimuon_streamer() ] 
            )
