#!/usr/bin/env python
# =============================================================================
# $Id: Hlt1StreamerConf.py,v 1.4 2010-09-08 09:23:05 graven Exp $ 
# =============================================================================
## @file Hlt1Lines/Hlt1StreamerConf.py
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
    'TightForward'        , ## the 'track-upgrade'-configuration for Forward
    'LooseForward'        , ## the 'track-upgrade'-configuration for Forward
    'pET'        , ## the 'track-upgrade'-configuration for VeloTT for a momentum estimation to feed the forward algorithm
    'FitTrack'       , ## the 'track-upgrade'-configuration for FitTrack
    'VeloOnlyFitTrack',## the 'track-upgrade'-configuration for a Velo-only (straight line) track fit
    'MatchVeloMuon'  , ## the 'track-upgrade'-configuration for MatchVeloMuon
    'IsMuon'  , ## the 'track-upgrade'-configuration for IsMuon
    ## match 
    #'VeloT'          , ## the 'track-match'-configuration   for VeloT
    'VeloL0Muon'     , ## the 'track-match'-configuration   for VeloL0Muon
    ## bi-functions
    ## track functions
    
    )
# =============================================================================


from LoKiCore.basic import LoKi, LHCb 
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
TightForward = LoKi.Hlt1.UpgradeConf  (
    "PatForwardTool/TightForward",                 ##                   "Tool" 
    "Hlt1/Track/TightForward"    ,                 ##              "TESOutput"
    LHCb.Track.Long              ,                 ##              "TrackType"
    True                         ,                 ##                  "Owner"
    False                        ,                 ##            "TransferIDs"
    False                        ,                 ##       "TransferAncestor"
    True                         ,                 ##      "TransferExtraInfo"
    True                         )                 ##              "OrderByPt
# =============================================================================
LooseForward = LoKi.Hlt1.UpgradeConf(
    "PatForwardTool/LooseForward",                 ##                   "Tool" 
    "Hlt1/Track/LooseForward"    ,                 ##              "TESOutput"
    LHCb.Track.Long              ,                 ##              "TrackType"
    True                         ,                 ##                  "Owner"
    False                        ,                 ##            "TransferIDs"
    False                        ,                 ##       "TransferAncestor"
    True                         ,                 ##      "TransferExtraInfo"
    True                         )                 ##              "OrderByPt
# =============================================================================
PEstiForward = LoKi.Hlt1.UpgradeConf(
    "PatForwardTool/PestiForward",                 ##                   "Tool" 
    "Hlt1/Track/PestiForward"    ,                 ##              "TESOutput"
    LHCb.Track.Long              ,                 ##              "TrackType"
    True                         ,                 ##                  "Owner"
    False                        ,                 ##            "TransferIDs"
    False                        ,                 ##       "TransferAncestor"
    False                         ,                 ##      "TransferExtraInfo"
    True                         )                 ##              "OrderByPt
# =============================================================================
pET = LoKi.Hlt1.UpgradeConf(
    "PatVeloTTTool/pET",                 ##                   "Tool" 
    "Hlt1/Track/VeloTT"    ,                 ##              "TESOutput"
    LHCb.Track.Upstream          ,                 ##              "TrackType"
    True                         ,                 ##                  "Owner"
    False                        ,                 ##            "TransferIDs"
    False                        ,                 ##       "TransferAncestor"
    False                        ,                 ##      "TransferExtraInfo"
    True                         )                 ##              "OrderByPt
# =============================================================================
FitTrack = LoKi.Hlt1.UpgradeConf (
    "HltTrackFit"                ,                     ##               "Tool" 
    "Hlt1/Track/FitTrack"        ,                     ##          "TESOutput"
    LHCb.Track.Long              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
FitVeloTTTrack = LoKi.Hlt1.UpgradeConf (
    "HltTrackFit/VeloTTFit"      ,                     ##               "Tool" 
    "Hlt1/Track/FitVeloTTTrack"  ,                     ##          "TESOutput"
    LHCb.Track.Long              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
VeloOnlyFitTrack = LoKi.Hlt1.UpgradeConf (
    "HltTrackFit/VeloOnlyFit"    ,                     ##               "Tool"
    "Hlt1/Track/VeloOnlyFitTrack",                     ##          "TESOutput"
    LHCb.Track.Velo              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
MatchVeloMuon=LoKi.Hlt1.UpgradeConf(
    "MatchVeloMuon"              ,                     ##               "Tool" 
    "Hlt1/Track/VeloMuon"        ,                     ##          "TESOutput"
    LHCb.Track.Velo              ,                     ##          "TrackType"
    False                        ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    False                        ,                     ##   "TransferAncestor"
    False                        ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
IsMuon=LoKi.Hlt1.UpgradeConf(
    "IsMuonTool"                 ,                     ##               "Tool" 
    "Hlt1/Track/IsMuon"          ,                     ##          "TESOutput"
    LHCb.Track.Long              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"

_trUpgrader = {}
_trUpgrader[ 'TightForward' ] = TightForward
_trUpgrader[ 'LooseForward' ] = LooseForward
_trUpgrader[ 'PEstiForward' ] = PEstiForward
_trUpgrader[ 'pET' ] = pET
_trUpgrader[ 'FitTrack'     ] = FitTrack
_trUpgrader[ 'FitVeloTTTrack'     ] = FitVeloTTTrack
_trUpgrader[ 'VeloOnlyFitTrack'] = VeloOnlyFitTrack
_trUpgrader[ 'MatchVeloMuon' ] = MatchVeloMuon
_trUpgrader[ 'IsMuon' ] = IsMuon

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
#      const bool               moveInfo )   //          transfer Extra Info ? 
# =============================================================================
#                                                      ##           "Old name"
# =============================================================================
# Where is PatMatchTool configured???
#VeloT = LoKi.Hlt1.MatchConf        (
#    "PatMatchTool"                 ,                   ##               "Tool"
#    "Hlt1/Track/VeloT"             ,                   ##          "TESOutput"
#    LHCb.Track.Long                ,                   ##          "TrackType"
#    True                           ,                   ##        "TransferIDs"
#    True                           ,                   ##  "TransferAncestors"
#    True                           )                   ##       "TransferInfo"
# =============================================================================
VeloL0Muon = LoKi.Hlt1.MatchConf   (
    "PatMatchL0MuonTool"           ,                    ##               "Tool"
    "Hlt1/Track/VeloL0Muon"        ,                    ##          "TESOutput"
    LHCb.Track.Long                ,                    ##          "TrackType"
    True                           ,                    ##        "TransferIDs"
    True                           ,                    ##  "TransferAncestors"
    True                           )                    ##       "TransferInfo"
# =============================================================================
VeloL0Calo = LoKi.Hlt1.MatchConf     (
    "Hlt::MatchVeloL0Calo/VeloL0Calo",                  ##               "Tool"
    "Hlt1/Track/VeloL0Calo"          ,                  ##          "TESOutput"
    LHCb.Track.Velo                  ,                  ##          "TrackType"
    False                            ,                  ##        "TransferIDs"
    False                            ,                  ##  "TransferAncestors"
    False                            )                  ##       "TransferInfo"
# =============================================================================

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
