"""
 script to configure lifetime unbiased two-track lines (for B->hh)
 """
# =============================================================================
__author__  = "Ulrich Kerzel ukerzel@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.10 $"
# =============================================================================

import Gaudi.Configuration
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt1HadronLTUnbiasedLinesConf( HltLinesConfigurableUser ) :
    

    #
    #
    #
    __slots__ = {
        'PT'              :  1200.   ,
        'P'               : 10000.   ,
        'TrChi2'          :     3    ,
        'Velo_NHits'      :     9    , # Minimum number of hits on a Velo track - 1
        'Velo_Qcut'       :     3    , # This - 1 is the maximum allowed difference between
                                       # the number of hits on a velo track and the
                                       # expected number of hits from the track's angle
                                       # and first measured point on the track
        'DiHadron_VxDOCA' :     0.1  ,
        'DiHadron_VxChi2' :    25.0  ,
        'DiHadron_VxmMin' :  5000.0  ,
        'DiHadron_VxmMax' :  5800.0 
        } 

    #
    #
    #
    def localise_props( self, prefix ):
        ps = self.getProps()
        gp = set( ( "Velo_NHits", "Velo_Qcut" ) )
        lp = set( ( "PT", "P", "TrChi2"  ) )
        return dict( [ ( key, ps[ key ] ) for key in gp ] + \
                     [ ( key, ps[ prefix + "_" + key ] ) for key in lp ] )
                                    
    #
    #
    #
    def hlt1DiHadronLTUnbiased_Preambulo( self, prefix ) :
        from HltTracking.Hlt1Streamers import ( VeloCandidates, LooseForward,
                                                TightForward, FitTrack )
        from HltTracking.HltPVs import RecoPV3D
        Preambulo = [ VeloCandidates( prefix ),
                      RecoPV3D    ,
                      TightForward,
                      LooseForward,
                      FitTrack    ,
                      "VertexConf    = LoKi.Hlt1.VxMakerConf( %(DiHadron_VxDOCA)f * mm, \
                                                              %(DiHadron_VxChi2)f )" % self.getProps(),
                      "MakeDiHadrons = TC_VXMAKE4( '', VertexConf )",
                      "from LoKiPhys.decorators import RV_MASS"
                      ]
        return Preambulo
                                            
    #
    #
    #
    def hlt1DiHadronLTUnbiased_Streamer( self, name, props ) :
        from Hlt1Lines.Hlt1GECs import Hlt1GECLoose
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name
        props['forward'] = 'TightForward'
        lineCode = """
        VeloCandidates
        >> RecoPV3D
        >>  ( ( TrIDC('isVelo') > %(Velo_NHits)s ) & \
        ( TrNVELOMISS < %(Velo_Qcut)s )) 
        >>  execute( decodeIT )
        >> %(forward)s
        >>  ( ( TrPT > %(PT)s * MeV ) & \
        ( TrP  > %(P)s  * MeV ) )
        >>  FitTrack
        >>  ( ( TrCHI2PDOF < %(TrChi2)s ))
        >>  MakeDiHadrons
        >>  ( RV_MASS ( 'K+' , 'K-' ) > %(DiHadron_VxmMin)s )
        >>  ( RV_MASS ( 'K+' , 'K-' ) < %(DiHadron_VxmMax)s )
        >> SINK( 'Hlt1%(name)sDecision' )
        >> ~TC_EMPTY
        """ % props
        hlt1DiHadronLTUnbiased_Unit = HltUnit(
            name+'Unit',
            ##OutputLevel = 1 ,
            Preambulo = self.hlt1DiHadronLTUnbiased_Preambulo( name ),
            Code = lineCode
            )
        return [ Hlt1GECLoose() ,hlt1DiHadronLTUnbiased_Unit ]
    
    #
    # 
    #
    def __apply_configuration__(self) :
        props = self.getProps()
        #
        # Begin imports
        #
        from HltLine.HltLine import Hlt1Line   as Line
        
        Line ( 'DiHadronLTUnbiasedAllL0'
               , prescale = self.prescale
               , postscale = self.postscale
               , L0DU = "L0_DECISION_PHYSICS"
               , algos = self.hlt1DiHadronLTUnbiased_Streamer( "DiHadronLTUnbiasedAllL0" ) 
               )
        
        

#####################################################################################
#####################################################################################


        
