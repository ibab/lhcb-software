from Gaudi import Configuration
from Configurables import ( HltTrackUpgradeTool, L0ConfirmWithT,
                            PatConfirmTool, PatSeedingTool,
                            PatForwardTool, HltGuidedForward,
                            MatchVeloMuon, HltTrackFit )
                            
from HltLine.HltLine import Hlt1Tool

import PyCintex
_global   = PyCintex.makeNamespace('')
cpp      = _global
LHCb     = cpp.LHCb
EarlyDataTracking = False

### create one instance for each output container...

__all__ = ( 'ConfiguredPatSeeding',
            'ConfiguredForward',
            'ConfiguredFastKalman',
            'ConfiguredMatchVeloMuon',
            'Forward',
            'GuidedForward',
            'TMuonConf',
            'THadronConf',
            'TEleConf',
            'Velo',
            'FitTrack',
            'RadCor' )
            
def ConfiguredPatSeeding( minPSeed = 3000):
    # Add the option to define a minimum PT/P 
    # for the tracking to consider
    # Only relevant for the forward upgrade
    return Hlt1Tool( PatSeedingTool, MinMomentum = minPSeed )
        
_minPt =  800  #### MOVE TO 800 (used to be 500 )
_minP  = 8000 #### MOVE TO 8000 (used to be 5000 )
def ConfiguredForward( parent, name = None, minP = _minP, minPt = _minPt ) :
    if name == None: name = PatForwardTool.__name__
    return Hlt1Tool( PatForwardTool
                     , name
                     , SecondLoop = True
                     , MaxChi2 = 40
                     , MaxChi2Track = 40
                     , MinHits = 12
                     , MinOTHits = 14
                     , MinPt = minPt
                     , MinMomentum = minP ).createConfigurable( parent )
            
def ConfiguredFastKalman( parent, name = None ) :
    if name == None: name = HltTrackFit.__name__
    parent.addTool( HltTrackFit, name )
    parent = getattr( parent, name )
    from Configurables import TrackMasterFitter
    parent.addTool( TrackMasterFitter, name = 'Fit')
    from TrackFitter.ConfiguredFitters import ConfiguredHltFitter
    fitter = ConfiguredHltFitter( getattr(parent,'Fit') )
##     fitter.NodeFitter.BiDirectionalFit    = True
##     fitter.NodeFitter.Smooth        = True
##     #fitter.AddDefaultReferenceNodes = True
##     fitter.NumberFitIterations = 1
##     fitter.MaxNumberOutliers = 2
##     fitter.UpdateTransport = False

def ConfiguredMatchVeloMuon( parent, name = None, minP = _minP ) :
    if name == None: name = MatchVeloMuon.__name__
    return Hlt1Tool( MatchVeloMuon
                     , name
                     , MaxChi2DoFX = 10
                     , XWindow = 200
                     , YWindow = 200
                     ## Set this according to PatForward
                     , MinMomentum = minP - 500 ).createConfigurable( parent )

####################################################
TMuonConf = HltTrackUpgradeTool( 'TMuonConfUpgrade' )
TMuonConf.View       = True
TMuonConf.ITrackType = LHCb.Track.Muon
TMuonConf.OTrackType = LHCb.Track.Ttrack
TMuonConf.TESOutput  = "Hlt1/Track/TMuonConf"

# for now, hardwire.... this entire bit of code needs to be a configurable,
# but care needs to be taken in order to a-priori return the right configurables,
# (to get the structure right)
# but then to explicitly configure them during __apply_configure__
# so this needs to be a two-stage process...

Muon_TConfNSigmaX =    5.
Muon_TConfNSigmaY =    5.
Muon_TConfNSigmaTx =    5.
Muon_TConfNSigmaTy =    5.

myTMuonConfTool = Hlt1Tool( L0ConfirmWithT, 'TMuonConf'
                          , particleType = 0
                          , tools = [ Hlt1Tool( PatConfirmTool
                                              , nSigmaX =Muon_TConfNSigmaX
                                              , nSigmaY =Muon_TConfNSigmaY
                                              , nSigmaTx=Muon_TConfNSigmaTx
                                              , nSigmaTy=Muon_TConfNSigmaTy
                                              , restrictSearch = True
                                              , tools = [ConfiguredPatSeeding(10000)]
                                              )]
                          ).createConfigurable( TMuonConf )

TMuonConf.Tool       = myTMuonConfTool.getFullName()
    
####################################################
THadronConf = HltTrackUpgradeTool( "THadronConfUpgrade" )
myHadronConfTool =  Hlt1Tool( type = L0ConfirmWithT , name='THadronConf'
                                                , particleType = 1
                                                , trackingTool='PatConfirmTool'
                                                , tools = [ Hlt1Tool( type = PatConfirmTool
                                                   , name = 'PatConfirmTool'
                                                   , nSigmaX = 4
                                                   , nSigmaY = 4
                                                   , nSigmaTx = 4
                                                   , nSigmaTy = 4
                                                   , restrictSearch = True
                                                   , debugMode = False
                                                   , tools = [ConfiguredPatSeeding()] ) ]).createConfigurable( THadronConf )
THadronConf.Tool = myHadronConfTool.getFullName()
THadronConf.View = True
THadronConf.ITrackType = LHCb.Track.TypeUnknown
THadronConf.OTrackType = LHCb.Track.Ttrack
THadronConf.TESOutput = "Hlt1/Track/THadronConf"



####################################################
TEleConf = HltTrackUpgradeTool( "TEleConfUpgrade" )
myEleConfTool = Hlt1Tool( L0ConfirmWithT 
                        , 'TEleConf' 
                        , particleType = 2 
                        , tools = [ Hlt1Tool( PatConfirmTool
                                            , tools = [ ConfiguredPatSeeding( )]
                                            )
                                  ]
                        ).createConfigurable( TEleConf )
TEleConf.Tool = myEleConfTool.getFullName()
TEleConf.View = True
TEleConf.ITrackType = LHCb.Track.TypeUnknown
TEleConf.OTrackType = LHCb.Track.Ttrack
TEleConf.TESOutput = "Hlt1/Track/TEleConf"

####################################################
Velo = HltTrackUpgradeTool("Velo")
Velo.Tool = "Tf.PatVeloSpaceTool"
Velo.View = False
Velo.ITrackType = LHCb.Track.VeloR
Velo.OTrackType = LHCb.Track.Velo
Velo.TESOutput = "Hlt1/Track/Velo"

####################################################
Forward = HltTrackUpgradeTool("Forward")
Forward.Tool = ConfiguredForward( Forward ).getFullName()  # maybe drop ToolSvc. ???
Forward.View = False
Forward.ITrackType = LHCb.Track.Velo
Forward.OTrackType = LHCb.Track.Long
Forward.TESOutput = "Hlt1/Track/Forward"


####################################################
GuidedForward = HltTrackUpgradeTool("GuidedForward")
GuidedForward.addTool( HltGuidedForward )
myHltGuidedForward =getattr( GuidedForward, 'HltGuidedForward' )  
ConfiguredForward( myHltGuidedForward )
GuidedForward.Tool = myHltGuidedForward.getFullName() # "HltGuidedForward"
GuidedForward.View = False
GuidedForward.ITrackType = LHCb.Track.Velo
GuidedForward.OTrackType = LHCb.Track.Long
GuidedForward.TESOutput = "Hlt1/Track/GuidedForward"

####################################################
FitTrack = HltTrackUpgradeTool("FitTrack")
FitTrack.View = False
FitTrack.ITrackType = LHCb.Track.Long
FitTrack.OTrackType = LHCb.Track.Long
FitTrack.TESOutput = "Hlt1/Track/FitTrack"
FitTrack.Tool = "HltTrackFit"
ConfiguredFastKalman( FitTrack )

####################################################
RadCor = HltTrackUpgradeTool("RadCor")
RadCor.Tool = "HltRadCorTool"
RadCor.View = False
RadCor.ITrackType = LHCb.Track.Long
RadCor.OTrackType = LHCb.Track.Long
RadCor.TESOutput = "Hlt1/Track/RadCor"
