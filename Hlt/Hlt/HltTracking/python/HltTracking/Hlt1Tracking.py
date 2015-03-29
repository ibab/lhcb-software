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
                            HltTrackFit )
                            
from HltLine.HltLine import Hlt1Tool

# =============================================================================
## Configuration functions for convenience.
# =============================================================================
__all__ = ( 'MatchVeloL0Muon',
            'LooseForward',
            'FitTrack',
            'MatchVeloMuon',
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
    from Configurables import TrackMasterFitter, TrackInitFit , TrackStateInitTool
    from TrackFitter.ConfiguredFitters import ConfiguredHltFitter, ConfiguredMasterFitter
    #TODO: This is a block copy from HltSharedTracking because Fit and Fitter naming inconsistency.
    if not HltRecoConf().getProp("InitFits"):
        # this addTool should not be necessary but unfortunately there is a problem with the toolhandle configuration
        parent.addTool( TrackMasterFitter, name="Fit")
        fitter = parent.Fit
    else:
        parent.FitterName = "TrackInitFit/Fit"
        parent.addTool( TrackInitFit, name = "Fit")
        parent.Fit.Init = "TrackStateInitTool/StateInit"
        parent.Fit.addTool(TrackStateInitTool, name="StateInit")
        parent.Fit.StateInit.VeloFitterName = "FastVeloFitLHCbIDs"
        parent.Fit.Fit = "TrackMasterFitter/Fitter"
        parent.Fit.addTool(TrackMasterFitter, name="Fitter")
        fitter = parent.Fit.Fitter
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
    parent.addTool( TrackMasterFitter, name = "VeloOnlyFitter" )
    from TrackFitter.ConfiguredFitters import ConfiguredForwardStraightLineFitter
    fitter = ConfiguredForwardStraightLineFitter( getattr(parent, "VeloOnlyFitter") )
    parent.FitterName = fitter.getFullName()
    fitter.AddDefaultReferenceNodes = True


def ConfiguredMatchVeloMuon( parent, name , minP ) :
    if name == None: name = MatchVeloMuon.__name__
    from HltTracking.HltRecoConf import CommonMatchVeloMuonOptions
    return Hlt1Tool( MatchVeloMuon
                     , name
                     ## Set this according to PatForward
                     , MinMomentum = minP 
                     , **CommonMatchVeloMuonOptions
                     ).createConfigurable( parent )

# =============================================================================
## Symbols for streamer users
# =============================================================================
def to_name( conf ):
    return conf.trTool().split( '/' )[ -1 ].split( '::' )[ -1 ].split( ':' )[ 0 ]

# Match Velo to L0 Muons
from Configurables import ( GaudiSequencer,
                            Hlt__L0Muon2Candidate,
                            Hlt__L0Muon2Track )
_L0SingleMuons = GaudiSequencer(
    'L0SingleMuonPrep',
    Members = [ 
        ## convert: LHCb::L0MuonCandidate -> Hlt::Candidate
        Hlt__L0Muon2Candidate( 'Hlt1L0MuonCandidates' ),
        ## convert: LHCb::L0MuonCandidate -> Hlt::Candidate
        Hlt__L0Muon2Track(
            'Hlt1L0MuonTracks',
            L0Channel      = 'Muon',
            InputSelection = 'Hlt1L0MuonCandidates',
            )
        ]
    )
MatchVeloL0Muon = "MatchVeloL0Muon = ( execute( %s ) * TC_MATCH2( 'MatchVeloL0Muon' , VeloCandidates, \
                   HltTracking.Hlt1StreamerConf.VeloL0Muon ) )" % \
                   [ m.getFullName() for m in _L0SingleMuons.Members ]

import HltLine.HltDecodeRaw 
from Gaudi.Configuration import ToolSvc
# =============================================================================
## Forward upgrade configuration
# =============================================================================
import Hlt1StreamerConf as Conf
from Configurables import HltRecoConf
ConfiguredForward( ToolSvc(), to_name( Conf.LooseForward ), 
                   minP=HltRecoConf().getProp("Forward_LPT_MinP"),  
                   minPt=HltRecoConf().getProp("Forward_LPT_MinPt"))

# string to use in line code
LooseForward  = "LooseForward  = ( execute(decodeIT) * TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.LooseForward  ) )"

# =============================================================================
## Hlt trackfit upgrade configuration
# =============================================================================
ConfiguredFastKalman( parent = None, name = to_name( Conf.FitTrack ) )
## String for users
FitTrack      = "FitTrack = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.FitTrack )"

# =============================================================================
## Hlt Velo-only trackfit upgrade configuration
# =============================================================================
ConfiguredFastVeloOnlyFit( parent = None, name = to_name( Conf.VeloOnlyFitTrack) )
## String for users
VeloOnlyFitTrack = "VeloOnlyFitTrack = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.VeloOnlyFitTrack )"

# =============================================================================
## Match Velo to Muon hits
# =============================================================================
ConfiguredMatchVeloMuon( ToolSvc(), to_name( Conf.MatchVeloMuon ), 
                         minP = HltRecoConf().getProp("MatchVeloMuon_MinP") )
## Strings for users
MatchVeloMuon = "MatchVeloMuon = ( execute(decodeMUON) * TC_UPGRADE_TR( '', HltTracking.Hlt1StreamerConf.MatchVeloMuon ) )"

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
from Configurables import Hlt__Track2Candidate
import HltLine.HltDecodeRaw 
from HltSharedTracking import VeloTTTracking
from HltLine.HltLine import bindMembers
def VeloTTCandidates( lineName ):
    selection = 'VeloTTCandidates%s' % lineName
    tracks = Hlt__Track2Candidate (
        'VeloTT2Candidates%s' % lineName,
        Code="~TrBACKWARD",
        InputSelection  = VeloTTTracking.outputSelection(),
        OutputSelection = selection,
        #OutputLevel = 1,
        )
    bm = bindMembers ( None , [ VeloTTTracking, tracks ] )
    return "VeloTTCandidates = execute( %s ) * SELECTION( '%s' )" % \
                ( [ m.getFullName() for m in bm.members() ], selection )

# ==============================================================================
# Track candidates
# ==============================================================================
from Configurables import Hlt__Track2Candidate
import HltLine.HltDecodeRaw 
from HltSharedTracking import HltHPTTracking
from HltLine.HltLine import bindMembers
def TrackCandidates( lineName ):
    selection = 'TrackCandidates%s' % lineName
    tracks = Hlt__Track2Candidate (
        'Track2Candidates%s' % lineName,
        Code="~TrBACKWARD",
        InputSelection  = HltHPTTracking.outputSelection(),
        OutputSelection = selection,
        #OutputLevel = 1,
        )
    bm = bindMembers ( None , [ HltHPTTracking, tracks ] )
    return "TrackCandidates = execute( %s ) * SELECTION( '%s' )" % \
                ( [ m.getFullName() for m in bm.members() ], selection )
                

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
