from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *

class Hlt1ProtonLinesConf( HltLinesConfigurableUser ):
    __slots__ = {
          'SpdMult'           :   600.   # dimensionless, Spd Multiplicy cut 
        , 'Proton_PT'         :     1.9  # GeV
        , 'Proton_P'          :    10.   # GeV  
        , 'Proton_TrChi2'     :     5. 
        , 'DiProton_MassMin'  :  2900.   # MeV, after Vtx fit
        , 'DiProton_MassMax'  :  4100.   # MeV, after Vtx fit
        , 'DiProton_VtxDOCA'  :     0.2    
        , 'DiProton_VtxChi2'  :     9.   # dimensionless
          }
        
    def preambulo( self ):
        from HltTracking.Hlt1Streamers import ( VeloCandidates,
                                                TightForward,
                                                FitTrack )

        ## define some "common" preambulo 
        Preambulo = [ VeloCandidates( "DiProton" ),                      
                      TightForward,                      
                      FitTrack,                      
                      ## helpers
                      "VertexConf = LoKi.Hlt1.VxMakerConf( %(DiProton_VtxDOCA)s * mm, \
                                                           %(DiProton_VtxChi2)s )" % self.getProps(),
                      "MakeDiProtons = TC_VXMAKE4( '', VertexConf )",
                      "from LoKiPhys.decorators import RV_MASS"
                      ]
        return Preambulo
    
    def DiProton_streamer( self ):
        from Configurables import LoKi__HltUnit as HltUnit
        DiProton_unit = HltUnit(
            'DiProtonStreamer',
            ##OutputLevel = 1 ,
            Preambulo = self.preambulo(),
            Code = """
            VeloCandidates
            >>  TightForward
            >>  ( ( TrPT > %(Proton_PT)s * GeV ) & ( TrP  > %(Proton_P)s * GeV ) )
            >>  FitTrack
            >>  ( TrCHI2PDOF < %(Proton_TrChi2)s )            
            >>  MakeDiProtons
            >>  ( RV_MASS ( 'p+', 'p~-' ) > %(DiProton_MassMin)s )
            >>  ( RV_MASS ( 'p+', 'p~-' ) < %(DiProton_MassMax)s )
            >>  SINK( 'Hlt1DiProtonDecision' )
            >> ~TC_EMPTY
            """ % self.getProps()
            )
        return DiProton_unit
    
    def __apply_configuration__( self ) : 
        from HltLine.HltLine import Hlt1Line
        Hlt1Line(
            'DiProton',
            ##
            prescale  = self.prescale,
            postscale = self.prescale,
            L0DU = "L0_CHANNEL( 'Hadron' )",
            ##
            algos     = [ self.DiProton_streamer() ] 
            )
