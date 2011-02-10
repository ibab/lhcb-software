# =============================================================================
## Symbols for streamer users, this can be moved to Hlt1TrackUpgradeConf as
## soon as the "old" style symbols have become obsolete.
# =============================================================================

__all__ = ( 'MatchVeloL0Muon',
            'TightForward',
            'LooseForward',
            'FitTrack',
            'MatchVeloMuon',
            'VeloCandidates' )

def to_name( conf ):
    return conf.trTool().split( ':' )[ 0 ].split( '/' )[ -1 ]

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
MatchVeloL0Muon = "MatchVeloL0Muon = TC_MATCH2( 'MatchVeloL0Muon' , VeloCandidates, \
                              HltTracking.Hlt1StreamerConf.VeloL0Muon )" % \
                   [ m.getFullName() for m in _L0SingleMuons.Members ]

## Configure tools here, can't do it when the configuration objects are defined
import HltLine.HltDecodeRaw 
from Gaudi.Configuration import ToolSvc
# =============================================================================
## Forward upgrade configuration
# =============================================================================
from HltTracking.Hlt1TrackUpgradeConf import ConfiguredForward
import Hlt1StreamerConf as Conf

ConfiguredForward( ToolSvc(), to_name( Conf.TightForward ), 10000, 1250 )
ConfiguredForward( ToolSvc(), to_name( Conf.LooseForward ), 6000, 600 )

## Strings for users
TightForward  = "TightForward  = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.TightForward  )"
LooseForward  = "LooseForward  = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.LooseForward  )"

# =============================================================================
## Hlt trackfit upgrade configuration
# =============================================================================
from HltTracking.Hlt1TrackUpgradeConf import ConfiguredFastKalman
ConfiguredFastKalman( ToolSvc(), to_name( Conf.FitTrack ) )
## String for users
FitTrack      = "FitTrack      = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.FitTrack )"

# =============================================================================
## Match Velo to Muon hits
# =============================================================================
from HltTracking.Hlt1TrackUpgradeConf import ConfiguredMatchVeloMuon
ConfiguredMatchVeloMuon( ToolSvc(), to_name( Conf.MatchVeloMuon ), minP = 6000 )
## Strings for users
MatchVeloMuon = "MatchVeloMuon = TC_UPGRADE_TR( '', HltTracking.Hlt1StreamerConf.MatchVeloMuon )"

# =============================================================================
## IsMuon
# =============================================================================
## Strings for users
IsMuon = "IsMuon = TC_UPGRADE_TR( '', HltTracking.Hlt1StreamerConf.IsMuon )"

# ==============================================================================
# Velo candidates
# ==============================================================================
from Configurables import Hlt__Track2Candidate
import HltLine.HltDecodeRaw 
from HltReco import MinimalVelo
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
# The END 
# ==============================================================================
