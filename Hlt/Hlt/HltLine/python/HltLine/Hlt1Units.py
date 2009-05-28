#!/usr/bin/env python
# =============================================================================
# $Id: Hlt1Units.py,v 1.1.1.1 2009-05-28 09:57:08 graven Exp $ 
# =============================================================================
## @file HltConf/Hlt1Units.py
#  Collection of utilities for dealing with Hlt1 "units"
#  @see LoKi::IHltUnit
#  @see LoKi::HltUnit
#  @see LoKi::FilterAlg
#  @see LoKi::Hlt1::UpgradeConf 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-10
# =============================================================================
"""

Collection of utilities for dealing with Hlt1 'units'

See:

- LoKi::IHltUnit
- LoKi::HltUnit
- LoKi::FilterAlg
- LoKi::Hlt1::UpgradeConf

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.1.1.1 $"
# =============================================================================
__all__ = (
    'TConf'         , ## the 'track-upgrade'-configuration for TConf
    'TMuonConf'     , ## the 'track-upgrade'-configuration for TMuonConf
    'THadronConf'   , ## the 'track-upgrade'-configuration for THadronConf
    'TEleConf'      , ## the 'track-upgrade'-configuration for TEleConf
    'Velo'          , ## the 'track-upgrade'-configuration for Velo
    'VeloTT'        , ## the 'track-upgrade'-configuration for VeloTT
    'Forward'       , ## the 'track-upgrade'-configuration for Forward
    'GiudedForward' , ## the 'track-upgrade'-configuration for GuidedForward
    'FitTrack'      , ## the 'track-upgrade'-configuration for FitTrack
    'VeloCalo'      , ## the 'track-match'-configuration   for VeloCalo
    'VeloTDist'     , ## the 'track-match'-configuration   for VeloTDist
    'VeloT'         , ## the 'track-match'-configuration   for VeloT
    'trMatch'       , ## get Track Match by name 
    'trUpgrade'     ) ## get Track Upgrade by name 

from LoKiTrigger.decorators import LoKi,LHCb

## @todo to be done in LoKiTrigger:
LoKi.Hlt1.UpgradeConf . __str__  =  LoKi.Hlt1.UpgradeConf . toString 
LoKi.Hlt1.UpgradeConf . __repr__ =  LoKi.Hlt1.UpgradeConf . toString
LoKi.Hlt1.MatchConf   . __str__  =  LoKi.Hlt1.MatchConf   . toString 
LoKi.Hlt1.MatchConf   . __repr__ =  LoKi.Hlt1.MatchConf   . toString

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
TConf = LoKi.Hlt1.UpgradeConf       (
    "L0ConfirmWithT"             ,                     ##               "Tool" 
    "Hlt1/Track/TConf"           ,                     ##          "TESOutput"
    LHCb.Track.Ttrack            ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
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
THadronConf = LoKi.Hlt1.UpgradeConf  (
    "L0ConfirmWithT/THadronConf" ,                     ##               "Tool" 
    "Hlt1/Track/THadronConf"     ,                     ##          "TESOutput"
    LHCb.Track.Ttrack            ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
TEleConf = LoKi.Hlt1.UpgradeConf     (
    "L0ConfirmWithT/TEleConf"    ,                     ##               "Tool" 
    "Hlt1/Track/TEleConf"        ,                     ##          "TESOutput"
    LHCb.Track.Ttrack            ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
Velo = LoKi.Hlt1.UpgradeConf     (
    "Tf::PatVeloSpaceTool"       ,                     ##               "Tool" 
    "Hlt1/Track/Velo"            ,                     ##          "TESOutput"
    LHCb.Track.Velo              ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    False                        ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"
# =============================================================================
VeloTT = LoKi.Hlt1.UpgradeConf     (
    "Tf::PatVeloTTTool"          ,                     ##               "Tool" 
    "Hlt1/Track/VeloTT"          ,                     ##          "TESOutput"
    LHCb.Track.Upstream          ,                     ##          "TrackType"
    False                        ,                     ##              "Owner"
    False                        ,                     ##        "TransferIDs"
    False                        ,                     ##   "TransferAncestor"
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
GuidedForward = LoKi.Hlt1.UpgradeConf    (
    "HltGuidedForward"           ,                     ##               "Tool" 
    "Hlt1/Track/GuidedForward"   ,                     ##          "TESOutput"
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
    LHCb.Track.Ttrack            ,                     ##          "TrackType"
    True                         ,                     ##              "Owner"
    True                         ,                     ##        "TransferIDs"
    True                         ,                     ##   "TransferAncestor"
    True                         ,                     ##  "TransferExtraInfo"
    True                         )                     ##          "OrderByPt"


_trUpgrade = {}

_trUpgrade [ 'TConf'         ] = TConf
_trUpgrade [ 'TMuonConf'     ] = TMuonConf
_trUpgrade [ 'THadronConf'   ] = THadronConf
_trUpgrade [ 'TEleConf'      ] = TEleConf
_trUpgrade [ 'Velo'          ] = Velo
_trUpgrade [ 'VeloTT'        ] = VeloTT
_trUpgrade [ 'Forward'       ] = Forward
_trUpgrade [ 'GuidedForward' ] = GuidedForward
_trUpgrade [ 'FitTrack'      ] = FitTrack
# =============================================================================
## get Track Upgrade by name 
def trUpgrade ( name ) :
    """
    Get track upgrade by name :

    >>> u = trUpgrade ('TMuonConf')
    
    """
    return _trUpgrade[ name ]


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
VeloCalo = LoKi.Hlt1.MatchConf  (
    "HltVeloTCaloMatch"            ,                   ##               "Tool"
    "Hlt/Track/VeloCalo"           ,                   ##          "TESOutput"
    LHCb.Track.Velo                ,                   ##          "TrackType"
    True                           ,                   ##        "TransferIDs"
    True                           ,                   ##  "TransferAncestors"
    False                          ,                   ##       "TransferInfo"
    "VeloCalo3DChi2"               ,                   ##            "Quality"
    ""                             )                   ##           "Quality2"
# =============================================================================
VeloTDist = LoKi.Hlt1.MatchConf  (
    "HltMatchTVeloTracks"          ,                   ##               "Tool"
    "Hlt/Track/VeloTDist"          ,                   ##          "TESOutput"
    LHCb.Track.Long                ,                   ##          "TrackType"
    True                           ,                   ##        "TransferIDs"
    True                           ,                   ##  "TransferAncestors"
    False                          ,                   ##       "TransferInfo"
    "deltaX"                       ,                   ##            "Quality"
    "deltaY"                       )                   ##           "Quality2"
# =============================================================================
VeloT = LoKi.Hlt1.MatchConf  (
    "PatMatchTool"                 ,                   ##               "Tool"
    "Hlt/Track/VeloT"              ,                   ##          "TESOutput"
    LHCb.Track.Long                ,                   ##          "TrackType"
    True                           ,                   ##        "TransferIDs"
    True                           ,                   ##  "TransferAncestors"
    True                           ,                   ##       "TransferInfo"
    "chi2_PatMatch"                ,                   ##            "Quality"
    ""                             )                   ##           "Quality2"

_trMatch = {}
_trMatch [ 'VeloCalo'  ] = VeloCalo
_trMatch [ 'VeloTDist' ] = VeloTDist
_trMatch [ 'VeloT'     ] = VeloT

# =============================================================================
## get Track Match by name 
def trMatch ( name ) :
    """
    Get track match by name :

    >>> m = trMatch ('VeloT')
    
    """
    return _trMatch[ name ]

# =============================================================================
## some embedded action
if '__main__' == __name__ :
    print __doc__
    print __version__
    print __author__
    print "\n\tSymbols:\n", dir ()

    print "Track Upgrade : "
    for key in _trUpgrade :
        print ' %15s  : %s' % ( key , _trUpgrade[key] )
    print "Track Match   : "
    for key in _trMatch :
        print ' %15s  : %s' % ( key , _trMatch[key] )
# =============================================================================
# The END 
# =============================================================================
