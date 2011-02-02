#!/usr/bin/env python
# =============================================================================
# $Id: Hlt1Units.py,v 1.4 2010-09-08 09:23:05 graven Exp $ 
# =============================================================================
## @file Hlt1Lines/Hlt1Units.py
#  Collection of utilities for dealing with Hlt1 "units"
#
#  @see LoKi::IHltUnit
#  @see LoKi::HltUnit
#  @see LoKi::FilterAlg
#  @see LoKi::Hlt1::UpgradeConf
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-10
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""

Collection of utilities for dealing with Hlt1 'units'

See:

- LoKi::IHltUnit
- LoKi::HltUnit
- LoKi::FilterAlg
- LoKi::Hlt1::UpgradeConf
- LoKi::Hlt1::MatchConf


                   $Revision$
 Last modification $Date$
                by $Author$

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = "2008-12-10"
__version__ = "version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    ## upgrade 
    'TMuonConf'      , ## the 'track-upgrade'-configuration for TMuonConf
    'Forward'        , ## the 'track-upgrade'-configuration for Forward
    'FitTrack'       , ## the 'track-upgrade'-configuration for FitTrack
    'MatchVeloMuon'  , ## the 'track-upgrade'-configuration for MatchVeloMuon
    ## match 
    'VeloT'          , ## the 'track-match'-configuration   for VeloT
    'VeloL0Muon'     , ## the 'track-match'-configuration   for VeloL0Muon
    ## bi-functions
    'Calo3DChi2'     , ## track-bi-function configuration for Calo3DChi2 
    'Ecal3DChi2'     , ## track-bi-function configuration for Ecal3DChi2
    ## track functions
    'MuonIDDistance' , ## track-function configuration for    MuonIDDistance
    'DeltaP'         , ## track-function configuration for    DeltaP
    'IsMuon'         , ## track-function configuration for    IsMuon
    'IsPhoton'       , ## track-function configuration for    IsPhoton
    'AntiEleConf'    , ## track-function configuration for    AntiEleConf
    'PtAtOrigin'       ## track-function configuration for    PtAtOrigin
    )
# =============================================================================


from LoKiCore.basic import LoKi , LHCb 

# =============================================================================
## define various 'track-upgrade' setups
#  @see LoKi::Hlt1::UpgradeConf
# 
#    UpgradeConf 
#    ( const std::string&        trTool   ,    //   ITrackFromTrack  tool name
#      const std::string&        address  ,    //   TES location of the tracks 
#      const LHCb::Track::Types  trType   ,    //                   track type 
#      const bool                owner    ,    //                      owner ? 
#      const bool                moveIDs  ,    //               transfer IDs ? 
#      const bool                moveAncs ,    //       transfer anscendents ? 
#      const bool                moveInfo ,    //        transfer extra info ?
#      const bool                ptOrder  ) ;  //                   order pt ?
# =============================================================================
#                                                      ##           "Old name" 
# =============================================================================
TMuonConf = LoKi.Hlt1.UpgradeConf   (
    "L0ConfirmWithT/TMuonConf"   ,                     ##               "Tool" 
    "Hlt1/Track/TMuonConf"       ,                     ##          "TESOutput"
    LHCb.Track.Ttrack            ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
Forward = LoKi.Hlt1.UpgradeConf    (
    "PatForwardTool"             ,                     ##               "Tool" 
    "Hlt1/Track/Forward"         ,                     ##          "TESOutput"
    LHCb.Track.Long              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    False                        ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
FitTrack = LoKi.Hlt1.UpgradeConf        (
    "HltTrackFit"                ,                     ##               "Tool" 
    "Hlt1/Track/FitTrack"        ,                     ##          "TESOutput"
    LHCb.Track.Long              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
MatchVeloMuon = LoKi.Hlt1.UpgradeConf (
    "MatchVeloMuon"              ,                     ##               "Tool" 
    "Hlt1/Track/VeloMuon"        ,                     ##          "TESOutput"
    LHCb.Track.Velo              ,                     ##          "TrackType"
    False                        ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"

_trUpgrader = {}
_trUpgrader [ 'TMuonConf' ] = TMuonConf
_trUpgrader [ 'Forward'   ] = Forward
_trUpgrader [ 'FitTrack'  ] = FitTrack
_trUpgrader [ 'MatchVeloMuon' ] = MatchVeloMuon

# =============================================================================


# =============================================================================
## define various 'track-match' setups
#  @see LoKi::Hlt1::MatchConf
# 
#  MatchConf 
#    ( const std::string&       mTool    ,   //          ITrackMatch tool name  
#      const std::string&       address  ,   // TES location of matched tracks
#      const LHCb::Track::Types trType   ,   //                     Track type
#      const bool               moveIDs  ,   //                 transfer IDs ? 
#      const bool               moveAncs ,   //           transfer ancestors ? 
#      const bool               moveInfo ,   //          transfer Extra Info ? 
#      const std::string&       quality  ,   //               matching quality 
#      const std::string&       quality2 ,   //              matching quality2 
#      const double             maxQCut  = 1.e6 ,  // max matching quality cut 
#      const double             maxQ2Cut = 1.e6 ) ;//max matching quality2 cut  
# =============================================================================
#                                                      ##           "Old name"
# =============================================================================
VeloT = LoKi.Hlt1.MatchConf  (
    "PatMatchTool"                 ,                   ##               "Tool"
    "Hlt1/Track/VeloT"             ,                   ##          "TESOutput"
    LHCb.Track.Long                ,                   ##          "TrackType"
    True                           ,                   ##        "TransferIDs"
    True                           ,                   ##  "TransferAncestors"
    True                           ,                   ##       "TransferInfo"
    "chi2_PatMatch"                ,                   ##            "Quality"
    ""                             )                   ##           "Quality2"
# =============================================================================
VeloL0Muon = LoKi.Hlt1.MatchConf  (
    "PatMatchL0MuonTool"           ,                   ##               "Tool"
    "Hlt1/Track/VeloL0Muon"        ,                   ##          "TESOutput"
    LHCb.Track.Long                ,                   ##          "TrackType"
    True                           ,                   ##        "TransferIDs"
    True                           ,                   ##  "TransferAncestors"
    True                           ,                   ##       "TransferInfo"
    "chi2_PatMatchL0Muon"          ,                   ##            "Quality"
    ""                             )                   ##           "Quality2"
# =============================================================================

# =============================================================================
# Other wrapper tools 
# =============================================================================
## ITrackBiFunctionTool interface                     @see ITrackBiFunctionTool
# =============================================================================
## Calo3DChi2    = LoKi.Hlt1.TrackBiFunction (
##     "HltVeloTCaloMatch/Calo3DChi2"    ,                       ## tool type-name
##     False                                                     ##   public tool?
##     )
## Ecal3DChi2    = LoKi.Hlt1.TrackBiFunction (
##     "HltVeloEcalMatch/Ecal3DChi2"     ,                       ## tool type-name
##     False                                                     ##   public tool?
##     )


# =============================================================================
## ITrackFunctionTool interface                       @see   ITrackFunctionTool
# =============================================================================
MuonIDDistance = LoKi.Hlt1.TrackFunction  (
    "HltMuonIDDistance/MuonIDDistance" ,                      ## tool type-name
    False                                                     ##   public tool?
    )
DeltaP         = LoKi.Hlt1.TrackFunction  (
    "HltDeltaPTool/DeltaP"             ,                      ## tool type-name
    False                                                     ##   public tool?
    )
IsMuon         = LoKi.Hlt1.TrackFunction  (
    "HltIsMuonTool/IsMuon"             ,                      ## tool type-name
    False                                                     ##   public tool?
    )
IsPhoton       = LoKi.Hlt1.TrackFunction  (
    "HltIsPhotonTool/IsPhoton"         ,                      ## tool type-name
    False                                                     ##   public tool?
    )
AntiEleConf    = LoKi.Hlt1.TrackFunction  (
    "HltAntiEleconf/AntiEleConf"       ,                      ## tool type-name
    False                                                     ##   public tool?
    )
PtAtOrigin     = LoKi.Hlt1.TrackFunction  (
    "PtTransporter/PtAtOrigin"         ,                      ## tool type-name
    False                                                     ##   public tool?
    )

# =============================================================================
# High-level embedded stuff
# =============================================================================
## some embedded action
if '__main__' == __name__ :
    print __doc__
    print __version__
    print __author__
    print "\n\tSymbols:\n", dir ()

# =============================================================================
# The END 
# =============================================================================
