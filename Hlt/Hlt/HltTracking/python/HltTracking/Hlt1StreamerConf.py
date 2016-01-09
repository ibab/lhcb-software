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
#  This file instantiates a number of configuration objects used when Hlt1
#  streamers are built, using GaudiPython bindings. The file is therefore as
#  bare as possible and any additional imports should be carefully considered to
#  prevent additional python modules being pulled in by the embedded python
#  interpreter at run time.
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
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = "2008-12-10"
__version__ = "version $Revision: 1.4 $"
# =============================================================================
from HltTracking.Hlt1TrackNames import Hlt1TrackLoc, Hlt1CacheLoc, Hlt1Tools
from LoKiCore.basic import LoKi, LHCb 
# =============================================================================
## define various 'track-upgrade' setups
#  @see LoKi::Hlt1::UpgradeConf
# 
#    UpgradeConf 
#    ( const std::string&        trTool   ,    //   ITrackFromTrack  tool name
#      const std::string&        address  ,    //   TES location of the tracks 
#      const std::string&        cache    ,    //    TES location of the cache 
#      const LHCb::Track::Types  trType   ,    //                   track type 
#      const bool                owner    ,    //                      owner ? 
#      const bool                moveIDs  ,    //               transfer IDs ? 
#      const bool                moveAncs ,    //       transfer anscendents ? 
#      const bool                moveInfo ,    //        transfer extra info ?
#      const bool                ptOrder  ) ;  //                   order pt ?
# =============================================================================
#                                                      ##           "Old name" 
# =============================================================================
ComplementForward = LoKi.Hlt1.UpgradeConf(
    Hlt1Tools["ComplementForward"],                    ##              "Tool" 
    Hlt1TrackLoc["ComplementForward"],                 ##         "TESOutput"
    Hlt1CacheLoc["ComplementForward"],                 ##             "Cache"
    LHCb.Track.Long               ,                    ##         "TrackType"
    True                          ,                    ##             "Owner"
    False                         ,                    ##       "TransferIDs"
    False                         ,                    ##  "TransferAncestor"
    False                         ,                    ## "TransferExtraInfo"
    True                          )                    ##         "OrderByPt"
# =============================================================================
LooseForward = LoKi.Hlt1.UpgradeConf(
    Hlt1Tools["LooseForward"],                         ##               "Tool" 
    Hlt1TrackLoc["LooseForward"] ,                     ##          "TESOutput"
    Hlt1CacheLoc["LooseForward"] ,                     ##              "Cache"
    LHCb.Track.Long              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    False                        ,                     ##   "TransferAncestor"
    False                        ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
FitTrack = LoKi.Hlt1.UpgradeConf (
    Hlt1Tools["FitTrack"]        ,                     ##               "Tool" 
    Hlt1TrackLoc["FitTrack"]     ,                     ##          "TESOutput"
    Hlt1CacheLoc["FitTrack"]     ,                     ##              "Cache"
    LHCb.Track.Long              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    False                        ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
FilterGhostProb = LoKi.Hlt1.UpgradeConf(
    Hlt1Tools["FilterGhostProb"]   ,                     ##               "Tool" 
    Hlt1TrackLoc["FilterGhostProb"],                     ##          "TESOutput"
    Hlt1CacheLoc["FilterGhostProb"],                     ##              "Cache"
    LHCb.Track.Long              ,                     ##          "TrackType"
    False                        ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    False                        ,                     ##   "TransferAncestor"
    False                        ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
VeloOnlyFitTrack = LoKi.Hlt1.UpgradeConf (
    Hlt1Tools["VeloOnlyFitTrack"],                     ##               "Tool" 
    Hlt1TrackLoc["VeloOnlyFitTrack"] ,                 ##          "TESOutput"
    Hlt1CacheLoc["VeloOnlyFitTrack"] ,                 ##              "Cache"
    LHCb.Track.Velo              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
MatchVeloMuon = LoKi.Hlt1.UpgradeConf(
    Hlt1Tools["MatchVeloMuon"]   ,                     ##               "Tool" 
    Hlt1TrackLoc["MatchVeloMuon"],                     ##          "TESOutput"
    Hlt1CacheLoc["MatchVeloMuon"],                     ##              "Cache"
    LHCb.Track.Velo              ,                     ##          "TrackType"
    False                        ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    False                        ,                     ##   "TransferAncestor"
    False                        ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
MatchVeloTTMuon = LoKi.Hlt1.UpgradeConf(
    Hlt1Tools["MatchVeloTTMuon"]   ,                   ##               "Tool" 
    Hlt1TrackLoc["MatchVeloTTMuon"],                   ##          "TESOutput"
    Hlt1CacheLoc["MatchVeloTTMuon"],                   ##              "Cache"
    LHCb.Track.Upstream          ,                     ##          "TrackType"
    False                        ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    False                        ,                     ##   "TransferAncestor"
    False                        ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
IsMuon = LoKi.Hlt1.UpgradeConf(
    Hlt1Tools["IsMuon"]          ,                     ##               "Tool" 
    Hlt1TrackLoc["IsMuon"]       ,                     ##          "TESOutput"
    Hlt1CacheLoc["IsMuon"]       ,                     ##              "Cache"
    LHCb.Track.Long              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    False                        ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"

_trUpgrader = {}
_trUpgrader['ComplementForward'] = ComplementForward
_trUpgrader['FitTrack'         ] = FitTrack
_trUpgrader['FilterGhostProb'     ] = FilterGhostProb
_trUpgrader['MatchVeloMuon'    ] = MatchVeloMuon
_trUpgrader['IsMuon'           ] = IsMuon

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
VeloL0Calo = LoKi.Hlt1.MatchConf     (
    Hlt1Tools["VeloL0Calo"]          ,                  ##               "Tool" 
    Hlt1TrackLoc["VeloL0Calo"]       ,                  ##          "TESOutput"
    LHCb.Track.Velo                  ,                  ##          "TrackType"
    False                            ,                  ##        "TransferIDs"
    False                            ,                  ##  "TransferAncestors"
    False                            )                  ##       "TransferInfo"
# =============================================================================

TrackL0Calo = LoKi.Hlt1.MatchConf    (
    Hlt1Tools["TrackL0Calo"]         ,                  ##               "Tool" 
    Hlt1TrackLoc["TrackL0Calo"]      ,                  ##          "TESOutput"
    LHCb.Track.Long                  ,                  ##          "TrackType"
    False                            ,                  ##        "TransferIDs"
    False                            ,                  ##  "TransferAncestors"
    False                            )                  ##       "TransferInfo"

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
