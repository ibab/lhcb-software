"""
 script to configure lifetime unbiased two-track lines (for B->hh)
 """
# =============================================================================
__author__  = "Ulrich Kerzel ukerzel@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.10 $"
# =============================================================================

import Gaudi.Configuration



#####################################################################################
#####################################################################################
def histosfilter(name,xlower=0.,xup=100.,nbins=100):
    """ return the dictonary with the booking of the histograms associated to a filter
    @@param filter name i.e 'PT
                """
    histosfs = { name : ( name,xlower,xup,nbins),
                 name+"Best" : (name+"Best",xlower,xup,nbins) }
    return histosfs

#####################################################################################
#####################################################################################
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
    def hlt1DiHadronLTUnbiased_Preambulo( self ) :
        from HltTracking.Hlt1Streamers import LooseForward,TightForward, FitTrack
        from HltTracking.HltPVs import FullPV3D
        Preambulo = [ FullPV3D    ,
                      TightForward,
                      LooseForward,
                      FitTrack    ,
                      "VertexConf    = LoKi.Hlt1.VxMakerConf( %(DiHadron_VxDOCA)f * mm, \
                                                              %(DiHadron_VxChi2)f )" % self.getProps(),
                      "MakeDiHadrons =  SINK( 'DiHadronVx' ) >>  TC_VXMAKE2( '', 'DiHadronVx', VertexConf )",
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
        FullPV3D
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
            Preambulo = self.hlt1DiHadronLTUnbiased_Preambulo(),
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
        from HltLine.HltLine import bindMembers
        from HltLine.HltLine import Hlt1Member as Member
        from Hlt1Lines.Hlt1GECs import Hlt1GECLoose
        from HltTracking.HltReco import Velo
        from HltTracking.HltPVs  import PV3D
        from HltLine.HltDecodeRaw import DecodeIT
        from HltTracking.Hlt1TrackUpgradeConf import Forward, FitTrack
        from Configurables import MuonRec, MuonIDAlg
        from HltTracking.HltTrackNames import HltSharedPIDPrefix, HltMuonTracksName
        from HltTracking.HltTrackNames import HltAllMuonTracksName, HltMuonIDSuffix
        from MuonID import ConfiguredMuonIDs
        from Configurables import ChargedProtoParticleMaker
        from Configurables import ChargedProtoParticleAddMuonInfo
        from Configurables import CombinedParticleMaker, ProtoParticleMUONFilter
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Hlt1Lines.HltL0Candidates import L0Channels
        #
        
        def trackprepare(pt,p):
            return [ Hlt1GECLoose(),
                     Velo,PV3D().ignoreOutputSelection(),
                     Member ( 'TF', 'OTNH'
                              , FilterDescriptor = [ 'NumberOfTrackHits,>,%s'%self.getProp('Velo_NHits')]
                              ),
                     Member ( 'TF', 'OTEXH'
                              , FilterDescriptor = [ 'MissedVeloHits,||<,%s'%self.getProp('Velo_Qcut')]
                              ),
                     DecodeIT,
                     Member ( 'TU', 'Forward'
                              , RecoName = Forward.splitName()[-1]
                              ),
                     Member ( 'TF' , 'OTPT' ,
                              FilterDescriptor = ['PT,>,%s'%pt]
                              , HistogramUpdatePeriod = 1 
                              , HistoDescriptor  = histosfilter('PT',0.,8000.,200)
                              ),
                     Member ( 'TF' , 'OTMom' ,
                              FilterDescriptor = ['P,>,%s'%p]
                              , HistogramUpdatePeriod = 1 
                              , HistoDescriptor  = histosfilter('P',0.,80000.,200)
                              )
                     ]
        
        
        def afterburn(chi2):
            return [ PV3D().ignoreOutputSelection()
                     , Member ( 'TU' , 'FitTrack' , RecoName = FitTrack.splitName()[-1] )
                     , Member ( 'TF' , 'TrkChi2'
                                , FilterDescriptor = ["FitChi2OverNdf,<,%s"%chi2],
                                HistogramUpdatePeriod = 1,
                                HistoDescriptor  = histosfilter('FitChi2OverNdf',0.,100.,100)
                               )
                     ]
        
        Line ( 'DiHadronLTUnbiasedAllL0'
               , prescale = self.prescale
               , postscale = self.postscale
               , L0DU = "L0_DECISION_PHYSICS"
               , algos = self.hlt1DiHadronLTUnbiased_Streamer( "DiHadronLTUnbiasedAllL0" ) 
               )
        
        

#####################################################################################
#####################################################################################


        
