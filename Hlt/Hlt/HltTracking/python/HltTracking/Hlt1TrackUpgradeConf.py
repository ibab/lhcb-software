# skip any of the next three lines, and we crash trying to instantiate the AppMgr.... ?!?!!!???
# probably this is needed before Hlt1StreamerConf is imported...
import PyCintex
cpp      = PyCintex.makeNamespace('')
LHCb     = cpp.LHCb
__tmp = LHCb.Track.Long

from Gaudi import Configuration
from Configurables import ( PatSeedingTool,
                            PatForwardTool,
                            PatVeloTTTool,
                            MatchVeloMuon,
                            HltTrackFit )
                            
from HltLine.HltLine import Hlt1Tool

# =============================================================================
## Configuration functions for convenience.
# =============================================================================
__all__ = ( 'MatchVeloL0Muon',
            'TightForward',
            'LooseForward',
            'pET',
            'FitTrack',
            'VeloOnlyFitTrack',
            'MatchVeloMuon',
            'VeloCandidates' )
            
def ConfiguredPatSeeding( minPSeed = 3000):
    # Add the option to define a minimum PT/P 
    # for the tracking to consider
    # Only relevant for the forward upgrade
    return Hlt1Tool( PatSeedingTool, MinMomentum = minPSeed )
        
_minPt =  800  #### MOVE TO 800 (used to be 500 )
_minP  = 8000 #### MOVE TO 8000 (used to be 5000 )
def ConfiguredForward( parent, name = None, minP = _minP, minPt = _minPt, useMomEst = False ) :
    if name == None: name = PatForwardTool.__name__
    from HltTracking.HltReco import CommonForwardTrackingOptions 
    return Hlt1Tool( PatForwardTool
                     , name
                     , SecondLoop = True
                     , UseMomentumEstimate = useMomEst
                     , MaxChi2 = CommonForwardTrackingOptions["MaxChi2"]
                     , MaxChi2Track = CommonForwardTrackingOptions["MaxChi2Track"]
                     , MinHits = CommonForwardTrackingOptions["MinHits"]
                     , MinOTHits = CommonForwardTrackingOptions["MinOTHits"]
                     , MinPt = minPt
                     , MinMomentum = minP ).createConfigurable( parent )

def ConfiguredpET(parent, name = None, minP = _minP) :
    print "%%%%% Running ConfiguredpET %%%%"
    if name == None: name = PatVeloTTTool.__name__
    from HltTracking.HltReco import CommonpETOptions 
    return Hlt1Tool( PatVeloTTTool
                     , name
                     , minMomentum = minP ## it was 2000.
                     , DxGroupFactor = CommonpETOptions["DxGroupFactor"]
                     , maxPseudoChi2 = CommonpETOptions["maxPseudoChi2"]
                     , maxSolutionsPerTrack = CommonpETOptions["maxSolutionsPerTrack"]
                     #, fitTracks = CommonpETOptions["fitTracks"]
                     #, maxChi2 = CommonpETOptions["maxChi2"]
                     ).createConfigurable( parent )

def ConfiguredFastKalman( parent = None, name = None ) :
    if name == None: name = HltTrackFit.__name__
    if parent :
        parent.addTool( HltTrackFit, name )
        parent = getattr( parent, name )
    else :  # make a public instance...
        parent = HltTrackFit()
    from Configurables import TrackMasterFitter
    parent.addTool( TrackMasterFitter, name = 'Fit')
    from TrackFitter.ConfiguredFitters import ConfiguredHltFitter
    fitter = ConfiguredHltFitter( getattr(parent,'Fit') )

def ConfiguredMatchVeloMuon( parent, name = None, minP = _minP ) :
    if name == None: name = MatchVeloMuon.__name__
    from HltTracking.HltReco import CommonMatchVeloMuonOptions
    return Hlt1Tool( MatchVeloMuon
                     , name
                     , MaxChi2DoFX = CommonMatchVeloMuonOptions["MaxChi2DoFX"]
                     , XWindow = CommonMatchVeloMuonOptions["XWindow"]
                     , YWindow = CommonMatchVeloMuonOptions["YWindow"]
                     ## Set this according to PatForward
                     , MinMomentum = minP ).createConfigurable( parent )

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

ConfiguredForward( ToolSvc(), to_name( Conf.TightForward ), 3000, 1250 )
ConfiguredForward( ToolSvc(), to_name( Conf.LooseForward ), 3000,  500 )
ConfiguredForward( ToolSvc(), to_name( Conf.PEstiForward ), 3000, 1250 , useMomEst=True)
ConfiguredpET( ToolSvc(), to_name( Conf.pET ), 800)
## Strings for users
TightForward  = "TightForward  = ( execute(decodeIT) * TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.TightForward  ) )"
LooseForward  = "LooseForward  = ( execute(decodeIT) * TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.LooseForward  ) )"
PEstiForward  = "PEstiForward  = ( execute(decodeIT) * TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.PEstiForward  ) )"
pET           = "pET = (execute(decodeTT) * TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.pET  ) )"
# =============================================================================
## Hlt trackfit upgrade configuration
# =============================================================================
ConfiguredFastKalman( parent = None, name = to_name( Conf.FitTrack ) )
## String for users
FitTrack      = "FitTrack      = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.FitTrack )"

# =============================================================================
## Hlt trackfit upgrade configuration
# =============================================================================
ConfiguredFastKalman( parent = None, name = to_name( Conf.FitVeloTTTrack ) )
## String for users
FitVeloTTTrack      = "FitVeloTTTrack      = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.FitVeloTTTrack )"

# =============================================================================
## Hlt Velo-only trackfit upgrade configuration
# =============================================================================
ConfiguredFastVeloOnlyFit( parent = None, name = to_name( Conf.VeloOnlyFitTrack) )
## String for users
VeloOnlyFitTrack = "VeloOnlyFitTrack = TC_UPGRADE_TR ( '', HltTracking.Hlt1StreamerConf.VeloOnlyFitTrack )"

# =============================================================================
## Match Velo to Muon hits
# =============================================================================
ConfiguredMatchVeloMuon( ToolSvc(), to_name( Conf.MatchVeloMuon ), minP = 6000 )
## Strings for users
MatchVeloMuon = "MatchVeloMuon = ( execute(decodeMUON) * TC_UPGRADE_TR( '', HltTracking.Hlt1StreamerConf.MatchVeloMuon ) )"

# =============================================================================
## IsMuon
# =============================================================================
## Strings for users
IsMuon = "IsMuon = ( execute(decodeMUON) * TC_UPGRADE_TR( '', HltTracking.Hlt1StreamerConf.IsMuon ) )"

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
