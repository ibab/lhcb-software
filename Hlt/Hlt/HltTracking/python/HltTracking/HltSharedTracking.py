#!/usr/bin/env python
# =============================================================================
## @file HltTracking/HltSharedTracking.py
#  was HltReco.py
#  Collection of predefined algorithms to perform reconstruction
#  @author Sebastian Neubert sebastian.neubert@cern.ch, Gerhard Raven Gerhard.Raven@nikhef.nl
# =============================================================================
#

__all__ = ( 'MinimalVelo'
          , 'RevivedVelo'
          , 'RevivedForward'
          , 'Hlt1Seeding'
          , 'HltHPTTracking'
          , 'VeloTTTracking'  
          )


############################################################################################
# Option to decide which pattern to use
############################################################################################
#############################################################################################
# Import Configurables
#############################################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import Tf__PatVeloGeneric, Tf__PatVeloRTracking, FastVeloTracking
from Configurables import PVOfflineTool
from HltLine.HltLine import bindMembers
from Configurables import PatSeeding, PatSeedingTool

# New tune JvT 2010-07-14
# globally set the resolutions for ST
#when syntax errors there are fixed take from 
#importOptions( "$STTOOLSROOT/options/Brunel_EarlyData.opts" )
from Configurables import STOfflinePosition, STOnlinePosition

STOnlinePosition().ErrorVec=[0.257, 0.245, 0.277]
STOnlinePosition().APE = 0.197

ITOnl = STOnlinePosition("ITLiteClusterPosition")
ITOnl.ErrorVec=[0.253,0.236,0.273]
ITOnl.APE=0.0758
#ITOnl.DetType = "IT"

STPos = STOfflinePosition("ITClusterPosition")
STPos.LinSharingCorr2 = -0.0152  
STPos.CubicSharingCorr2 = 12.33 
STPos.CubicSharingCorr3 = 4.369 
STPos.LinSharingCorr4 = 0.530 
STPos.DetType = "IT"
STPos.ErrorVec =[ 0.253, 0.236, 0.273, 0.185 ]
STPos.APE = 0.0758

STOfflinePosition().ErrorVec = [0.257, 0.245, 0.277, 0.208]
STOfflinePosition().APE = 0.197

from Configurables import OTRawBankDecoder
OTRawBankDecoder().TimeWindow = ( -8.0, 56.0 ) # add units: ns!!


#############################################################################################
# Configure pattern recognition algorithms
#############################################################################################

from HltTrackNames import Hlt1TrackLoc, HltSharedTrackLoc, Hlt2TrackLoc

from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool
from Configurables import FastVeloHitManager, DecodeVeloRawBuffer
from Configurables import TrackStateInitAlg, TrackStateInitTool

#### Velo Tracking

# the full Velo reconstruction
def recoVelo(OutputTracksName=HltSharedTrackLoc["Velo"]):
    recoVelo = FastVeloTracking( 'FastVeloHlt', OutputTracksName = OutputTracksName) 
    # TODO: Move this to HltRecoConf
    recoVelo.HLT1Only = False 
    recoVelo.HLT2Complement = False 
    recoVelo.StatPrint = True
    recoVelo.VetoObjects = [ OutputTracksName ]
    return recoVelo
 

#### filter the Velo output
from Configurables import TrackListRefiner, HltRecoConf
filterVelo = TrackListRefiner('VeloFilter',
                              inputLocation = HltSharedTrackLoc["Velo"],
                              outputLocation = HltSharedTrackLoc["VeloSelection"])
from Configurables import  LoKi__Hybrid__TrackSelector as LoKiTrackSelector
filterVelo.addTool(LoKiTrackSelector,name="VeloSelector")
filterVelo.Selector = LoKiTrackSelector(name="VeloSelector")
veloSelector = filterVelo.Selector
veloSelector.Code = HltRecoConf().getProp("VeloSelectionCut")
veloSelector.StatPrint = True

#### VeloTT Tracking
from HltRecoConf import VeloTTOptions, VeloTTToolOptions
from Configurables import PatVeloTTHybrid, PatVeloTTHybridTool
recoVeloTT = PatVeloTTHybrid( 'PatVeloTTHlt', 
                        InputTracksName = HltSharedTrackLoc["VeloSelection"],
                        OutputTracksName = HltSharedTrackLoc["VeloTTHPT"],
                        **VeloTTOptions )
recoVeloTT.addTool(PatVeloTTHybridTool(**VeloTTToolOptions), name="PatVeloTTTool")
recoVeloTT.PatVeloTTTool.StatPrint = True
recoVeloTT.VetoObjects = [ recoVeloTT.OutputTracksName ]

#### Forward Tracking
from Configurables import PatForward, PatForwardTool
from Configurables import HltRecoConf
recoForwardHPT = PatForward( 'Hlt1ForwardHPT'
                          , InputTracksName  = recoVeloTT.OutputTracksName 
                          , OutputTracksName = HltSharedTrackLoc["ForwardHPT"]
                          , maxOTHits=HltRecoConf().getProp("Forward_MaxOTHits") )
recoForwardHPT.VetoObjects = [ recoForwardHPT.OutputTracksName ]
# apply modifications on top
from HltRecoConf import CommonForwardTrackingOptions, ForwardTrackingOptions_MomEstimate
opts = CommonForwardTrackingOptions.copy()
opts.update(ForwardTrackingOptions_MomEstimate)
recoForwardHPT.addTool(PatForwardTool(**opts), name='PatForwardTool')

# update configuration with HLT specific tweaks
recoForwardHPT.PatForwardTool.MinMomentum = HltRecoConf().getProp("Forward_HPT_MinP")
recoForwardHPT.PatForwardTool.MinPt = HltRecoConf().getProp("Forward_HPT_MinPt")
recoForwardHPT.PatForwardTool.StatPrint = True
recoForwardHPT.StatPrint = True


##### Hlt selections
from Configurables import Hlt__TrackFilter as HltTrackFilter
prepare3DVelo = HltTrackFilter( 'Hlt1Prepare3DVelo'
                              , InputSelection   = "TES:" + HltSharedTrackLoc["Velo"]
                              , RequirePositiveInputs = False
                              , Code = [ '~TrBACKWARD' ] 
                              , OutputSelection     = "Velo" )

def fittedVelo(inputTracks, outputTracks):
    from Configurables import TrackEventFitter, TrackInitFit
    from Configurables import TrackStateInitTool, TrackMasterFitter
    fa = TrackEventFitter('VeloOnlyFitterAlg')
    fa.TracksInContainer = inputTracks
    fa.TracksOutContainer = outputTracks
    fa.Fitter = "TrackInitFit/VeloInitFit"
    fa.addTool(TrackInitFit, "VeloInitFit")
    fa.VeloInitFit.Init = "TrackStateInitTool/VeloOnlyStateInit"
    fa.VeloInitFit.addTool(TrackStateInitTool, "VeloOnlyStateInit")
    fa.VeloInitFit.VeloOnlyStateInit.VeloFitterName = "FastVeloFitLHCbIDs"
    fa.VeloInitFit.Fit = "TrackMasterFitter/VeloOnlyFitter"
    fa.VeloInitFit.addTool(TrackMasterFitter, name = "VeloOnlyFitter")
    from TrackFitter.ConfiguredFitters import ConfiguredForwardFitter
    fitter = ConfiguredForwardFitter(fa.VeloInitFit.VeloOnlyFitter)
    return fa

#############################################################################################
# HLT2 tracking codes
#############################################################################################
def ConfiguredForwardComplement(name 
                                , InputTracksName #= HltSharedTrackLoc["Velo"]
                                , OutputTracksName #= Hlt2TrackLoc["ForwardComp"]
                                , VetoSeedLocations #= [ HltSharedTrackLoc["ForwardHPT"] ]
                                , MinMomentum #= HltRecoConf().getProp("Forward_LPT_MinP")
                                , MinPt #= HltRecoConf().getProp("Forward_LPT_MinPt")
                                ):
    if name == None:
        log.fatal( '##################################################################################')
        log.fatal( '## FATAL You did not specify the name of the PatForward instance.' )
        log.fatal( '## FATAL I will now die, you need to make Hlt2Tracking aware of the name!  ')
        log.fatal( '##################################################################################')
    forward = PatForward( name
                          , InputTracksName  = InputTracksName
                          , OutputTracksName = OutputTracksName
                          )
    
    #Sascha Stahl: We should get rid of GECs in the pattern reco, do it centrally
    from HltRecoConf import CommonForwardOptions
    from Configurables import HltRecoConf
    forward.maxOTHits = HltRecoConf().getProp("Forward_MaxOTHits")
    forward.maxITHits = CommonForwardOptions["MaxITHits"] 
    forward.MaxNVelo = CommonForwardOptions["MaxNVelo"] 
        
    from HltRecoConf import CommonForwardTrackingOptions, ComplementForwardToolOptions
    opts = CommonForwardTrackingOptions.copy()
    opts.update(ComplementForwardToolOptions)
    opts.update({"MinMomentum" : MinMomentum
                 ,"MinPt" : MinPt })
    forward.addTool(PatForwardTool(**opts), name='PatForwardTool')

    from Configurables import TrackUsedLHCbID
    forward.PatForwardTool.addTool(TrackUsedLHCbID, name='TrackUsedLHCbID')
    forward.PatForwardTool.UsedLHCbIDToolName="TrackUsedLHCbID"
    forward.PatForwardTool.TrackUsedLHCbID.inputContainers= VetoSeedLocations
    forward.PatForwardTool.VeloVetoTracksNames =  VetoSeedLocations
    # make them a bit more verbose
    forward.StatPrint = True
    forward.PatForwardTool.StatPrint = True
    return forward

def ConfiguredPatSeeding(name
                         , OutputTracksName 
                         , VetoTrackLocations = None):
    if name == None:
        log.fatal( '##################################################################################')
        log.fatal( '## FATAL You did not specify the name of the PatSeeding instance.' )
        log.fatal( '## FATAL I will now die, you need to make Hlt2Tracking aware of the name!  ')
        log.fatal( '##################################################################################')
    from Configurables    import PatSeeding
    from Configurables      import PatSeedingTool
    recoSeeding = PatSeeding(name, OutputTracksName = OutputTracksName )
    recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
    # New tool
    if VetoTrackLocations != None:
        from Configurables import TrackUsedLHCbID
        #recoSeeding.PatSeedingTool.UseForward        = True
        recoSeeding.PatSeedingTool.UsedLHCbIDToolName = "TrackUsedLHCbID"
        recoSeeding.PatSeedingTool.addTool( TrackUsedLHCbID, name="TrackUsedLHCbID" )
        recoSeeding.PatSeedingTool.TrackUsedLHCbID.inputContainers = VetoTrackLocations
  
    from HltRecoConf import OnlineSeedingToolOptions
    recoSeeding.PatSeedingTool.NDblOTHitsInXSearch = OnlineSeedingToolOptions ["NDblOTHitsInXSearch"]
    recoSeeding.PatSeedingTool.MinMomentum = HltRecoConf().getProp("Forward_LPT_MinP")
    from Configurables import HltRecoConf
    from HltTracking.HltRecoConf import CommonForwardOptions
    recoSeeding.PatSeedingTool.MaxOTHits = HltRecoConf().getProp("Forward_MaxOTHits")
    recoSeeding.PatSeedingTool.MaxITHits = CommonForwardOptions["MaxITHits"] #ugly...
    return recoSeeding

#TODO: Move this to TrackFitter package?
def ConfiguredHltInitFitter( parent ):
    from Configurables import ( TrackEventFitter, TrackMasterFitter, TrackInitFit, TrackStateInitTool )
    from TrackFitter.ConfiguredFitters import ConfiguredHltFitter, ConfiguredMasterFitter
    if not HltRecoConf().getProp("InitFits"):
        parent.addTool( TrackMasterFitter, name="Fitter")
        fitter = parent.Fitter
    else:
        parent.addTool( TrackInitFit, name = "Fitter")
        parent.Fitter.Init = "TrackStateInitTool/StateInit"
        parent.Fitter.addTool(TrackStateInitTool, name="StateInit")
        parent.Fitter.StateInit.VeloFitterName = "FastVeloFitLHCbIDs"
        parent.Fitter.Fit = "TrackMasterFitter/Fitter"
        parent.Fitter.addTool(TrackMasterFitter, name="Fitter")
        fitter = parent.Fitter.Fitter
    # configure the fitter
    if not HltRecoConf().getProp("MoreOfflineLikeFit"):
        ConfiguredHltFitter( fitter )
        fitter.NumberFitIterations = HltRecoConf().getProp("FitIterationsInHltFit")
    else:
        ConfiguredMasterFitter( fitter , SimplifiedGeometry = True, LiteClusters = True)
        fitter.MeasProvider.IgnoreMuon = True
    return fitter

def ConfiguredHltEventFitter( name,
                              TracksInContainer ):
    # create the event fitter
    from Configurables import ( TrackEventFitter, TrackMasterFitter, TrackInitFit, TrackStateInitTool )
    
    eventfitter = TrackEventFitter(name)
    eventfitter.TracksInContainer = TracksInContainer
    ConfiguredHltInitFitter(eventfitter)
    return eventfitter

def ConfiguredGoodTrackFilter (name,
                               InputLocation,
                               OutputLocation = None):
    if name == None:
        log.fatal( '##################################################################################')
        log.fatal( '## FATAL You did not specify the name of the Filter instance.' )
        log.fatal( '## FATAL I will now die, you need to make Hlt2Tracking aware of the name!  ')
        log.fatal( '##################################################################################')
    if OutputLocation == None: OutputLocation = InputLocation + "Filtered"
    from Configurables import TrackListRefiner
    filterTracks = TrackListRefiner( name,
                                     inputLocation = InputLocation,
                                     outputLocation = OutputLocation)
    from Configurables import  LoKi__Hybrid__TrackSelector as LoKiTrackSelector
    filterTracks.addTool(LoKiTrackSelector,name="LokiTracksSelector")
    filterTracks.Selector = LoKiTrackSelector(name="LokiTracksSelector")
    filterTracks.Selector.Code = "(~TrINVALID) & ( TrCHI2PDOF < %(TrChi2)s )" % {"TrChi2":  HltRecoConf().getProp("GoodTrCHI2PDOF")}
    filterTracks.Selector.StatPrint = True
    return filterTracks

#############################################################################################
# Define modules for the reconstruction sequence 
#############################################################################################
from HltLine.HltDecodeRaw import DecodeVELO, DecodeTRACK, DecodeTT, DecodeIT
from Configurables import DecodeVeloRawBuffer, Hlt2Conf

### define exported symbols (i.e. these are externally visible, the rest is NOT)
#This is the part which is shared between Hlt1 and Hlt2
MinimalVelo = bindMembers( None, [DecodeVELO, recoVelo( OutputTracksName=HltSharedTrackLoc["Velo"] ) ] ).setOutputSelection( HltSharedTrackLoc["Velo"] )
#DecodeTRACK.VetoObjects += [ HltSharedTrackLoc["Velo"], recoForwardHPT.OutputTracksName ]
#for d in DecodeTRACK.members() :
#    d.VetoObjects += [ HltSharedTrackLoc["Velo"], recoForwardHPT.OutputTracksName ]
RevivedVelo = bindMembers( None, [DecodeVELO, DecodeTRACK, recoVelo( OutputTracksName=HltSharedTrackLoc["Velo"] ) ] ).setOutputSelection( HltSharedTrackLoc["Velo"] )
FittedVelo  = bindMembers( None, RevivedVelo.members() + [fittedVelo(RevivedVelo.outputSelection(), Hlt1TrackLoc["FittedVelo"])]).setOutputSelection(Hlt1TrackLoc["FittedVelo"])

# TODO: put selection revive/redo here (ask Sebastian)
# for now always redo:
bm_members =  DecodeVELO.members() + [recoVelo(), filterVelo ] 
bm_members += DecodeTT.members() + [recoVeloTT] 
bm_members += DecodeIT.members() + [recoForwardHPT]

HltHPTTracking = bindMembers(None, bm_members).setOutputSelection( recoForwardHPT.OutputTracksName )

RevivedForward = bindMembers(None,DecodeTT.members() + DecodeIT.members() + [ DecodeTRACK ] + HltHPTTracking.members() ).setOutputSelection( recoForwardHPT.OutputTracksName )

#VeloTT tracking
vt_members = DecodeVELO.members() + [ recoVelo(), filterVelo ]
vt_members += DecodeTT.members()  + [ recoVeloTT ]
VeloTTTracking = bindMembers(None, vt_members).setOutputSelection( recoVeloTT.OutputTracksName ) 

# ==============================================================================
# Hlt1Seeding, used by MicroBias
# ==============================================================================
from Configurables import PatSeeding, PatSeedingTool
from HltLine.HltDecodeRaw import DecodeIT
ps = PatSeeding('Hlt1MBSeeding' , OutputTracksName = Hlt1TrackLoc["Seeding"])
ps.addTool(PatSeedingTool, name="PatSeedingTool")
ps.PatSeedingTool.MaxOTHits = HltRecoConf().getProp("Forward_MaxOTHits")

Hlt1Seeding = bindMembers( None, [ DecodeIT, ps ] )
