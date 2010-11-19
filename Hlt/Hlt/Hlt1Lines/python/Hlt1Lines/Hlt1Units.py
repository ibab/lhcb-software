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
__all__ = (
    ## upgrade 
    'TMuonConf'      , ## the 'track-upgrade'-configuration for TMuonConf
    'Forward'        , ## the 'track-upgrade'-configuration for Forward
    'FitTrack'       , ## the 'track-upgrade'-configuration for FitTrack
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
    'PtAtOrigin'     , ## track-function configuration for    PtAtOrigin
    ##
    'trMatcher'      , ## get Track Match by name 
    'trUpgrader'     , ## get Track Upgrade by name
    'trFun2s'        , ## get Track Bi-function by name 
    'trFun2s'        , ## get Track function by name
    )

from LoKiTrigger.decorators import LoKi , LHCb 
#from LoKiTrigger.decorators import ( TrINPUT     ,
#                                     TrSELECTION ,
#                                     TTrFUN      ,
#                                     TTrFUN2     ,
#                                     TrUPGRADE   ,
#                                     TrMATCH     ) 

## @todo to be done in LoKiTrigger:
LoKi.Hlt1.UpgradeConf      . __str__  =  LoKi.Hlt1.UpgradeConf     . toString 
LoKi.Hlt1.UpgradeConf      . __repr__ =  LoKi.Hlt1.UpgradeConf     . toString
LoKi.Hlt1.MatchConf        . __str__  =  LoKi.Hlt1.MatchConf       . toString 
LoKi.Hlt1.MatchConf        . __repr__ =  LoKi.Hlt1.MatchConf       . toString
##LoKi.Hlt1.TrackBiFunction  . __str__  =  LoKi.Hlt1.TrackBiFunction . toString 
##LoKi.Hlt1.TrackBiFunction  . __repr__ =  LoKi.Hlt1.TrackBiFunction . toString 
##LoKi.Hlt1.TrackFunction    . __str__  =  LoKi.Hlt1.TrackFunction   . toString 
##LoKi.Hlt1.TrackFunction    . __repr__ =  LoKi.Hlt1.TrackFunction   . toString 


## # ==========================================================================
## ## Create the upgrade functor: 
## def __tr_upgrade__ ( self , output ) :
##     """
##     Create the upgrade streamer :
    
##     >>> upgrader = ...                             ## get the upgrader
##     >>> streamer = upgrader ( 'OutputSelection' )  ## get the streamer 
    
##     """
##     return TrUPGRADE ( output , self )

## LoKi.Hlt1.UpgradeConf. __call__ = __tr_upgrade__

## # =============================================================================
## ## create the matcher streamer 
## def __tr_match__ ( self , output , source , input = False ) :
##     """
##     Create the matcher streamer :
    
##     >>> matcher  = ...                                    ## get the matcher
##     >>> streamer = matcher ( 'OutputSelection' , TrINPUT ( 'second' ) )
    
##     >>> matcher  = ...                                    ## get the matcher
##     >>> streamer = matcher ( 'OutputSelection' , 'second'  )

##     """
##     if str == type ( source ) :
##         source = TrINPUT ( source ) if input else TrSELECTION ( source )
        
##     ## create the actual streamer
##     return TrMATCH  ( output , source , self )

## LoKi.Hlt1.MatchConf. __call__ = __tr_upgrade__


## # ==============================================================================
## ## Create the function/tool from the descriptor
## def __tr_func__ ( self ) :
##     """
##     Create the function/tool from the descriptor

##     >>> fun  = ..                  ## get the function configuration
##     >>> fltr = ( fun() < 1 * GeV ) ## create the streamer
    
##     """
##     return TrFUN ( self )

## LoKi.Hlt1.TrackFunction  . __call__  =  __tr_func__


## # ==============================================================================
## ## Create the function/tool from the descriptor
## def __tr_fun2__ ( self ) :
##     """
##     Create the function/tool from the descriptor

##     >>> fun  = ..                  ## get the function configuration
##     >>> fltr = ( fun() < 1 * GeV ) ## create the streamer
    
##     """
##     return TTrFUN ( self )

## LoKi.Hlt1.TrackBiFunction  . __call__  =  __tr_fun2__


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


_trUpgrader = {}
_trUpgrader [ 'TMuonConf' ] = TMuonConf
_trUpgrader [ 'Forward'   ] = Forward
_trUpgrader [ 'FitTrack'  ] = FitTrack

# =============================================================================
## get Track Upgrade by name 
def trUpgrader ( name ) :
    """
    Get track upgrader  by name :
    
    >>> u = trUpgrader ('TMuonConf')
    
    """
    return _trUpgrader[ name ]


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
    "Hlt1/Track/VeloT"              ,                   ##          "TESOutput"
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

_trMatcher = {}
_trMatcher [ 'VeloT'      ] = VeloT
_trMatcher [ 'VeloL0Muon' ] = VeloL0Muon

# =============================================================================
## get Track Match by name 
def trMatcher ( name ) :
    """
    Get track match by name :

    >>> m = trMatcher ('VeloT')
    
    """
    return _trMatcher[ name ]

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

## _trFun2s = {}
## _trFun2s [ 'Calo3DChi2'   ] = Calo3DChi2
## _trFun2s [ 'Ecal3DChi2'   ] = Ecal3DChi2

# =============================================================================
## get Track Bi-function by name 
def trFun2s ( name ) :
    """
    Get track bi-function by name 
    
    >>> m = trFun2s ( 'Ecal3DChi2' )
    
    """
    return _trFun2s [ name ]


# =============================================================================
## ITrackFunctionTool interface                       @see   ITrackFunctionTool
# =============================================================================
## MuonIDDistance = LoKi.Hlt1.TrackFunction  (
##     "HltMuonIDDistance/MuonIDDistance" ,                      ## tool type-name
##     False                                                     ##   public tool?
##     )
## DeltaP         = LoKi.Hlt1.TrackFunction  (
##     "HltDeltaPTool/DeltaP"             ,                      ## tool type-name
##     False                                                     ##   public tool?
##     )
## IsMuon         = LoKi.Hlt1.TrackFunction  (
##     "HltIsMuonTool/IsMuon"             ,                      ## tool type-name
##     False                                                     ##   public tool?
##     )
## IsPhoton       = LoKi.Hlt1.TrackFunction  (
##     "HltIsPhotonTool/IsPhoton"         ,                      ## tool type-name
##     False                                                     ##   public tool?
##     )
## AntiEleConf    = LoKi.Hlt1.TrackFunction  (
##     "HltAntiEleconf/AntiEleConf"       ,                      ## tool type-name
##     False                                                     ##   public tool?
##     )
## PtAtOrigin     = LoKi.Hlt1.TrackFunction  (
##     "PtTransporter/PtAtOrigin"         ,                      ## tool type-name
##     False                                                     ##   public tool?
##     )


## _trFuns = {}
## _trFuns [ 'MuonIDDistance' ] = MuonIDDistance
## _trFuns [ 'DeltaP'         ] = DeltaP
## _trFuns [ 'IsMuon'         ] = IsMuon
## _trFuns [ 'IsPhoton'       ] = IsPhoton
## _trFuns [ 'AntiEleConf'    ] = AntiEleConf
## _trFuns [ 'PtAtOrigin'     ] = PtAtOrigin

# =============================================================================
## get Track function by name 
def trFuns ( name ) :
    """
    Get track function by name 
    
    >>> m = trFuns ( 'DeltaP' )
    
    """
    return _trFuns [ name ]



# =============================================================================
# High-level embedded stuff
# =============================================================================
## some embedded action
if '__main__' == __name__ :
    print __doc__
    print __version__
    print __author__
    print "\n\tSymbols:\n", dir ()

    print "\nTrack Upgrade      : "
    for key in _trUpgrader :
        print ' %25s  : %s' % ( key , _trUpgrader[key] )
    print "\nTrack Match        : "
    for key in _trMatcher  :
        print ' %25s  : %s' % ( key , _trMatcher[key] )
    #print "\nTrack Functions    : "
    #for key in _trFuns     :
    #    print ' %25s  : %s' % ( key , _trFuns   [key] )
    #print "\nTrack Bi-Functions : "
    #for key in _trFun2s    :
    #    print ' %25s  : %s' % ( key , _trFun2s  [key] )
# =============================================================================
# The END 
# =============================================================================
