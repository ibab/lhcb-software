# =============================================================================
# Hlt1 specific tracking algorithms and streamer symbols
# Note that the upfront tracking en'block is defined in HltSharedTracking
# and made aavailable to Hlt1 through TrackCandidates (see end of this file)
# ============================================================================

from Gaudi import Configuration
from Configurables import ( PatSeedingTool,
                            PatForwardTool,
                            PatVeloTTHybridTool,
                            MatchVeloMuon,
                            MatchVeloTTMuon,
                            HltTrackFit )
                            
from HltLine.HltLine import Hlt1Tool

# =============================================================================
## Configuration functions for convenience.
# =============================================================================
__all__ = ( 'MatchVeloL0Muon',
            'LooseForward',
            'FitTrack',
            'MatchVeloMuon',
            'MatchVeloTTMuon',
            'VeloCandidates',
            'VeloTTCandidates',
            'TrackCandidates')
            
def ConfiguredPatSeeding( minPSeed = 3000):
    # Add the option to define a minimum PT/P 
    # for the tracking to consider
    # Only relevant for the forward upgrade
    return Hlt1Tool( PatSeedingTool, MinMomentum = minPSeed )

from GaudiKernel.SystemOfUnits import MeV

# Use HltRecoConf Here!
def ConfiguredForward( parent, name , minP , minPt , useMomEst = False ) :
    if name == None: name = PatForwardTool.__name__
    from HltTracking.HltRecoConf import CommonForwardTrackingOptions
    from HltTracking import HltRecoConf
    opts = CommonForwardTrackingOptions.copy()
    if useMomEst :
        opts.update( UseMomentumEstimate = True
                   , UseWrongSignWindow = True
                   , WrongSignPT = HltRecoConf.getProp("Forward_WrongSignPT")
                   , Preselection = useMomEst
                   )
    return Hlt1Tool( PatForwardTool
                   , name
                   , MinPt = minPt
                   , MinMomentum = minP
                   , **opts 
                   ).createConfigurable( parent )


def ConfiguredFastKalman( parent = None, name = None ) :
    if name == None: name = HltTrackFit.__name__
    if parent :
        parent.addTool( HltTrackFit, name )
        parent = getattr( parent, name )
    else :  # make a public instance...
        parent = HltTrackFit()
    from Configurables import (TrackMasterFitter, TrackInitFit , TrackStateInitTool,
                               TrackMasterExtrapolator)
    from TrackFitter.ConfiguredFitters import ConfiguredHltFitter, ConfiguredMasterFitter
    #TODO: This is a block copy from HltSharedTracking because Fit and Fitter naming inconsistency.
    if not HltRecoConf().getProp("InitFits"):
        # this addTool should not be necessary but unfortunately there is a problem with the toolhandle configuration
        parent.addTool( TrackMasterFitter, "Fit")
        fitter = parent.Fit
    else:
        parent.FitterName = "TrackInitFit/Fit"
        parent.addTool( TrackInitFit, name = "Fit")
        parent.Fit.Init = "TrackStateInitTool/StateInit"
        parent.Fit.addTool(TrackStateInitTool, name="StateInit")
        parent.Fit.StateInit.VeloFitterName = "FastVeloFitLHCbIDs"
        parent.Fit.StateInit.UseFastMomentumEstimate = True 
        parent.Fit.StateInit.addTool(TrackMasterExtrapolator, "Extrapolator")
        parent.Fit.StateInit.Extrapolator.MaterialLocator = "SimplifiedMaterialLocator"
        parent.Fit.addTool(TrackMasterFitter, "Fit")
        fitter = parent.Fit.Fit
    # configure the fitter
    from HltSharedTracking import ConfigureFitter
    ConfigureFitter( fitter )
    #if not HltRecoConf().getProp("MoreOfflineLikeFit"):
    #    ConfiguredHltFitter( fitter )
    #    fitter.NumberFitIterations = HltRecoConf().getProp("FitIterationsInHltFit")
    #else:
    #    ConfiguredMasterFitter( fitter , SimplifiedGeometry = True, LiteClusters = True)
    #    fitter.MeasProvider.IgnoreMuon = True
    

def ConfiguredFastVeloOnlyFit( parent = None, name = None ) :
    if name == None: name = HltTrackFit.__name__ + "VeloOnly"
    if parent :
        parent.addTool( HltTrackFit, name )
        parent = getattr( parent, name )
    else:
        parent = HltTrackFit( name )
    from Configurables import TrackMasterFitter
    parent.addTool( TrackMasterFitter, name = "Fit" )
    from TrackFitter.ConfiguredFitters import ConfiguredForwardStraightLineFitter
    fitter = ConfiguredForwardStraightLineFitter( getattr(parent, "Fit") )
    parent.FitterName = fitter.getFullName()
    fitter.AddDefaultReferenceNodes = True


def ConfiguredMatchVeloMuon( toolType, parent, name, options ) :
    minP = HltRecoConf().getProp("MatchVeloMuon_MinP")
    return Hlt1Tool( toolType
                     , name
                     ## Set this according to PatForward
                     , MinMomentum = minP 
                     , **options
                     ).createConfigurable( parent )

# =============================================================================
## Symbols for streamer users
# =============================================================================
from Hlt1TrackNames import Hlt1Tools
def to_name( key ):
    return Hlt1Tools[key].split('/')[-1]

from Hlt1TrackNames import Hlt1CacheLoc
def cache_loc(trackLoc):
    return Hlt1CacheLoc[trackLoc.split('/')[-1]]

import HltLine.HltDecodeRaw 
from Gaudi.Configuration import ToolSvc
from HltSharedTracking import HltHPTTracking

# =============================================================================
## Forward upgrade configuration
# =============================================================================
from Configurables import HltRecoConf
ConfiguredForward( ToolSvc(), to_name( "ComplementForward" ), 
                   minP=HltRecoConf().getProp("Forward_LPT_MinP"),  
                   minPt=HltRecoConf().getProp("Forward_LPT_MinPt"))
ConfiguredForward( ToolSvc(), to_name( "LooseForward" ), 
                   minP=HltRecoConf().getProp("Forward_LPT_MinP"),  
                   minPt=HltRecoConf().getProp("Forward_LPT_MinPt"))

# string to use in line code
LooseForward      = ("LooseForward       = ( execute(decodeIT) * "
                     "TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.LooseForward ) )")
ComplementForward = ("ComplementForward  = ( execute(decodeIT) * "
                     "TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.ComplementForward,"
                     "'%s' ) )" % cache_loc(HltHPTTracking.outputSelection()))

# =============================================================================
## Hlt trackfit upgrade configuration
# =============================================================================
ConfiguredFastKalman( parent = None, name = to_name( "FitTrack" ) )
## String for users
FitTrack      = "FitTrack = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.FitTrack )"

# =============================================================================
## Hlt Velo-only trackfit upgrade configuration
# =============================================================================
ConfiguredFastVeloOnlyFit( parent = ToolSvc(), name = to_name( "VeloOnlyFitTrack") )
## String for users
VeloOnlyFitTrack = "VeloOnlyFitTrack = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.VeloOnlyFitTrack )"

# =============================================================================
## Match Velo to Muon hits
# =============================================================================
from HltTracking.HltRecoConf import MatchVeloMuonOptions
ConfiguredMatchVeloMuon( MatchVeloMuon, ToolSvc(), to_name( "MatchVeloMuon" ), 
                         MatchVeloMuonOptions )
## Strings for users
MatchVeloMuon = ("MatchVeloMuon = ( execute(decodeMUON) * " +
                 "TC_UPGRADE_TR( '', HltTracking.Hlt1StreamerConf.MatchVeloMuon, " +
                 "'%s' ) )" % cache_loc(HltHPTTracking.outputSelection()))

# =============================================================================
## Match VeloTT to Muon hits
# =============================================================================
from HltTracking.HltRecoConf import MatchVeloTTMuonOptions
ConfiguredMatchVeloMuon( MatchVeloTTMuon, ToolSvc(),
                         to_name( "MatchVeloTTMuon" ), 
                         MatchVeloTTMuonOptions )

## Strings for users
MatchVeloTTMuon = ("MatchVeloTTMuon = ( execute(decodeMUON) * " +
                   "TC_UPGRADE_TR( '', HltTracking.Hlt1StreamerConf.MatchVeloTTMuon, " +
                   "'%s' ) )" % cache_loc(HltHPTTracking.outputSelection()))

# =============================================================================
## IsMuon
# =============================================================================
## Strings for users
IsMuon = "IsMuon = ( execute(decodeMUON) * TC_UPGRADE_TR( '', HltTracking.Hlt1StreamerConf.IsMuon ) )"

# ==============================================================================
# The streamer sources making the output locations of the HltHPTTracking 
# sequence available in the streamer framework :
# ==============================================================================

# ==============================================================================
# Velo candidates
# ==============================================================================
from Configurables import Hlt__Track2Candidate
import HltLine.HltDecodeRaw 
from HltSharedTracking import MinimalVelo
from HltLine.HltLine import bindMembers
def VeloCandidates( lineName ):
    selection = 'VeloCandidates%s' % lineName
    tracks = Hlt__Track2Candidate (
        'Velo2Candidates%s' % lineName,
        Code            = "~TrBACKWARD"    , ## skip backward tracks 
        InputSelection  = MinimalVelo.outputSelection(),
        OutputSelection = selection,
        )
    
    bm = bindMembers ( None , [ MinimalVelo, tracks ] )
    return "VeloCandidates = execute( %s ) * SELECTION( '%s' )" % \
                ( [ m.getFullName() for m in bm.members() ], selection )

# ==============================================================================
# VeloTT candidates
# ==============================================================================
from Configurables import FillHlt1Cache
from Configurables import Hlt__Track2Candidate
import HltLine.HltDecodeRaw 
from HltSharedTracking import VeloTTTracking
from HltLine.HltLine import bindMembers
def VeloTTCandidatesAlgos( lineName ):
    selection = 'VeloTTCandidates%s' % lineName
    output = VeloTTTracking.outputSelection()
    tracks = Hlt__Track2Candidate (
        'VeloTT2Candidates%s' % lineName,
        Code="~TrBACKWARD",
        InputSelection  = output,
        OutputSelection = selection
        )
    # Don't need this one, so don't run it.
    # cache = FillHlt1Cache("FillCache_" + output.split('/')[-1],
    #                       InputTracks = output,
    #                       Cache  = cache_loc(output),
    #                       FromTypes = ['Velo'],
    #                       ToTypes = ['Upstream', 'Velo'])
    return bindMembers ( None , [ VeloTTTracking, tracks ] )
    
def VeloTTCandidates( lineName ):
    bm = VeloTTCandidatesAlgos( lineName )
    return "VeloTTCandidates = execute( %s ) * SELECTION( '%s' )" % \
                ( [ m.getFullName() for m in bm.members() ], bm.outputSelection() )

# ==============================================================================
# Track candidates
# ==============================================================================
def TrackCandidatesAlgos( lineName ):
    veloTTAlgos = VeloTTCandidatesAlgos( lineName )
    veloTTCache = veloTTAlgos.members()[-2]
    
    selection = 'TrackCandidates%s' % lineName
    output = HltHPTTracking.outputSelection()
    tracks = Hlt__Track2Candidate (
        'Track2Candidates%s' % lineName,
        Code="~TrBACKWARD",
        InputSelection  = output,
        OutputSelection = selection,
        )
    fwdCache = FillHlt1Cache("FillCache_" + output.split('/')[-1],
                             ToTracks = output,
                             Cache = cache_loc(output),
                             FromTypes = ['Upstream', 'Velo'],
                             ToTypes = ['Long'])
    return bindMembers ( None , [ HltHPTTracking, veloTTCache,
                                  fwdCache, tracks ] )

def TrackCandidates( lineName ):
    bm = TrackCandidatesAlgos( lineName )
    return "TrackCandidates = execute( %s ) * SELECTION( '%s' )" % \
                ( [ m.getFullName() for m in bm.members() ], bm.outputSelection() )
                

# ==============================================================================
# L0CaloCandidates as Hlt::Candidates
# ==============================================================================
from Configurables import Hlt__L0Calo2Candidate
from HltLine.HltDecodeRaw import DecodeL0FullCALO

def L0CaloCandidates( lineName ):
    ## L0Calo -> Candidate,
    ## This hardcoded location is not pretty...
    selection = 'CaloCandidates%s' % lineName
    caloCandidates = Hlt__L0Calo2Candidate(
        'L0Calo2Candidates%s' % lineName,
        InputSelection  = 'Trig/L0/FullCalo',
        OutputSelection = selection )
    bm = bindMembers ( None , [ DecodeL0FullCALO, caloCandidates ] )
    return "L0CaloCandidates = execute( %s ) * SELECTION( '%s' )" % \
           ( [ m.getFullName() for m in bm.members() ], selection )
