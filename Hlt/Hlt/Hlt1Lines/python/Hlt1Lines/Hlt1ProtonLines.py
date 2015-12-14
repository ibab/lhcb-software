from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *

class Hlt1ProtonLinesConf( HltLinesConfigurableUser ):
    __slots__ = {
          'DiProton_SpdMult'    :   300.   # dimensionless, Spd Multiplicy cut 
        , 'DiProton_PT'         :  1900.   # MeV
        , 'DiProton_P'          : 10000.   # MeV
        , 'DiProton_TrChi2'     :     2.5 
        , 'DiProton_MassMin'    :  2800.   # MeV, after Vtx fit
        , 'DiProton_MassMax'    :  3300.   # MeV, after Vtx fit
        , 'DiProton_VtxDOCA'    :     0.1    
        , 'DiProton_VtxChi2'    :     4.   # dimensionless
        , 'DiProton_JpsiPT'     :  6500.   # MeV 
          
        , 'DiProtonLowMult_SpdMult'    :    10.   # dimensionless, Spd Multiplicy cut 
        , 'DiProtonLowMult_PT'         :   500.   # MeV, same as LooseForward
        , 'DiProtonLowMult_P'          :  6000.   # MeV, same as LooseForward  
        , 'DiProtonLowMult_MassMin'    :  2800.   # MeV, after Vtx fit
        , 'DiProtonLowMult_VtxDOCA'    :     0.3    
        , 'DiProtonLowMult_VtxChi2'    :    25.   # dimensionless          
          }

    
    def DiProton_preambulo( self ):
        from HltTracking.Hlt1Tracking import ( TrackCandidates,
                                               FitTrack )
        
        ## define some "common" preambulo 
        Preambulo = [ TrackCandidates( "DiProton" ),
                      FitTrack,
                      ## helpers
                      "VertexConf = LoKi.Hlt1.VxMakerConf( %(DiProton_VtxDOCA)s * mm, \
                                                           %(DiProton_VtxChi2)s )" % self.getProps(),
                      "MakeDiProtons = TC_VXMAKE4( '', VertexConf )",
                      "from LoKiPhys.decorators import RV_MASS"
                      ]

        Preambulo += [
            "from LoKiPhys.decorators import RV_TrFUN, NTRACKS",
            "PX1 = RV_TrFUN(TrPX,0)",
            "PY1 = RV_TrFUN(TrPY,0)",
            "PX2 = RV_TrFUN(TrPX,1)",
            "PY2 = RV_TrFUN(TrPY,1)",
            "JpsiPTCut = ( sqrt((PX1+PX2)**2 + (PY1+PY2)**2) > %(DiProton_JpsiPT)s * MeV )" % self.getProps()
            ]
        
        return Preambulo


    def DiProtonLowMult_preambulo( self ):
        from HltTracking.Hlt1Tracking import ( TrackCandidates,
                                               FitTrack )
        
        ## define some "common" preambulo 
        Preambulo = [ TrackCandidates( "DiProtonLowMult" ),                      
                      FitTrack,                      
                      ## helpers
                      "VertexConf = LoKi.Hlt1.VxMakerConf( %(DiProtonLowMult_VtxDOCA)s * mm, \
                                                           %(DiProtonLowMult_VtxChi2)s )" % self.getProps(),
                      "MakeDiProtonsLowMult = TC_VXMAKE4( '', VertexConf )",
                      "from LoKiPhys.decorators import RV_MASS"
                      ]
        return Preambulo
    
    
    def DiProton_streamer( self ):
        from Configurables import LoKi__HltUnit as HltUnit
        DiProton_unit = HltUnit(
            'DiProtonStreamer',
            ##OutputLevel = 1 ,
            Preambulo = self.DiProton_preambulo(),
            Code = """
            TrackCandidates
            >>  FitTrack
            >>  ( ( TrPT > %(DiProton_PT)s * MeV ) & ( TrP  > %(DiProton_P)s * MeV ) & ( TrCHI2PDOF < %(DiProton_TrChi2)s ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass PT', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nPT' , LoKi.Monitoring.ContextSvc ) )               
            >>  MakeDiProtons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'p+', 'p~-' ) > %(DiProton_MassMin)s * MeV )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass min', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'massMin' , LoKi.Monitoring.ContextSvc ) )   
            >>  ( RV_MASS ( 'p+', 'p~-' ) < %(DiProton_MassMax)s * MeV )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass max', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'massMax' , LoKi.Monitoring.ContextSvc ) )
            >>  JpsiPTCut
            >>  tee  ( monitor( TC_SIZE > 0, '# pass JpsiPT', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'JpsiPT' , LoKi.Monitoring.ContextSvc ) )           
            >>  SINK( 'Hlt1DiProtonDecision' )
            >> ~TC_EMPTY
            """ % self.getProps()
            )
        return DiProton_unit

    
    def DiProtonLowMult_streamer( self ):
        from Configurables import LoKi__HltUnit as HltUnit
        DiProtonLowMult_unit = HltUnit(
            'DiProtonLowMultStreamer',
            ##OutputLevel = 1 ,
            Preambulo = self.DiProtonLowMult_preambulo(),
            Code = """
            TrackCandidates
            >>  ( ( TrPT > %(DiProtonLowMult_PT)s * MeV ) & ( TrP  > %(DiProtonLowMult_P)s * MeV ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass PT', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nPT' , LoKi.Monitoring.ContextSvc ) )               
            >>  MakeDiProtonsLowMult
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'p+', 'p~-' ) > %(DiProtonLowMult_MassMin)s *MeV)
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass min', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'massMin' , LoKi.Monitoring.ContextSvc ) )   
            >>  SINK( 'Hlt1DiProtonLowMultDecision' )
            >> ~TC_EMPTY
            """ % self.getProps()
            )
        return DiProtonLowMult_unit

    
    
    
    def __apply_configuration__( self ) : 
        from HltLine.HltLine import Hlt1Line
        Hlt1Line(
            'DiProton',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "L0_CHANNEL( 'Hadron' ) & ( L0_DATA('Spd(Mult)') < %(DiProton_SpdMult)s )" % self.getProps(),   
            ##
            algos     = [ self.DiProton_streamer() ] 
            )

        
        Hlt1Line(
            'DiProtonLowMult',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( L0_DATA('Spd(Mult)') < %(DiProtonLowMult_SpdMult)s )" % self.getProps(),   
            ##
            algos     = [ self.DiProtonLowMult_streamer() ] 
            )
