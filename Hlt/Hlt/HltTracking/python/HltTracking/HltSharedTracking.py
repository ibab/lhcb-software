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
from HltTracking.HltRecoConf import VeloTTOptions, VeloTTToolOptions
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
from HltTracking.HltRecoConf import CommonForwardTrackingOptions, ForwardTrackingOptions_MomEstimate
opts = CommonForwardTrackingOptions
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
    if name == None: name = PatForward.__name__
    forward = PatForward( name
                          , InputTracksName  = InputTracksName
                          , OutputTracksName = OutputTracksName
                          )
    
    #Sascha Stahl: We should get rid of GECs in the pattern reco, do it centrally
    from HltTracking.HltRecoConf import CommonForwardOptions
    from Configurables import HltRecoConf
    forward.maxOTHits = HltRecoConf().getProp("Forward_MaxOTHits")
    forward.maxITHits = CommonForwardOptions["MaxITHits"] 
    forward.MaxNVelo = CommonForwardOptions["MaxNVelo"] 
        
    from HltTracking.HltRecoConf import CommonForwardTrackingOptions, ComplementForwardToolOptions, HltRecoConf
    opts = CommonForwardTrackingOptions
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
    if name == None: name = PatForward.__name__
    from Configurables    import PatSeeding
    from Configurables      import PatSeedingTool
    recoSeeding = PatSeeding(name, OutputTracksName = OutputTracksName )
    recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
    #if VetoTrackLocations != None:
    #    recoSeeding.PatSeedingTool.UseForward        = True
    #else:
    #    recoSeeding.PatSeedingTool.UseForward        = True
    # New tool
    if VetoTrackLocations != None:
        from Configurables import TrackUsedLHCbID
        #recoSeeding.PatSeedingTool.UseForward        = True
        recoSeeding.PatSeedingTool.UsedLHCbIDToolName = "TrackUsedLHCbID"
        recoSeeding.PatSeedingTool.addTool( TrackUsedLHCbID, name="TrackUsedLHCbID" )
        recoSeeding.PatSeedingTool.TrackUsedLHCbID.inputContainers = VetoTrackLocations
        

    #recoSeeding.PatSeedingTool.InputTracksName    = VetoTrackLocations
    #recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
    from HltTracking.HltRecoConf import OnlineSeedingToolOptions

    recoSeeding.PatSeedingTool.NDblOTHitsInXSearch = OnlineSeedingToolOptions ["NDblOTHitsInXSearch"]
    recoSeeding.PatSeedingTool.MinMomentum = OnlineSeedingToolOptions ["MinMomentum"]

    from Configurables import HltRecoConf
    from HltTracking.HltRecoConf import CommonForwardOptions
    recoSeeding.PatSeedingTool.MaxOTHits = HltRecoConf().getProp("Forward_MaxOTHits")
    recoSeeding.PatSeedingTool.MaxITHits = CommonForwardOptions["MaxITHits"] #ugly...
    #TODO: put something in like: if(early data):
    #if self.getProp("EarlyDataTracking") :
        #    # Do something special in case of early data
        #    from HltTracking.HltRecoConf import CommonSeedingTrackingOptions_EarlyData
        #    recoSeeding.PatSeedingTool.OTNHitsLowThresh = CommonSeedingTrackingOptions_EarlyData["OTNHitsLowThresh"]
        #    recoSeeding.PatSeedingTool.MinTotalPlanes = CommonSeedingTrackingOptions_EarlyData["MinTotalPlanes"]
        #    recoSeeding.PatSeedingTool.MaxMisses = CommonSeedingTrackingOptions_EarlyData["MaxMisses"]
        #    recoSeeding.PatSeedingTool.MaxTrackChi2LowMult = CommonSeedingTrackingOptions_EarlyData["MaxTrackChi2LowMult"]
        #    recoSeeding.PatSeedingTool.MaxFinalTrackChi2 = CommonSeedingTrackingOptions_EarlyData["MaxFinalTrackChi2"]
        #    recoSeeding.PatSeedingTool.MaxFinalChi2 = CommonSeedingTrackingOptions_EarlyData["MaxFinalChi2"]
        #    recoSeeding.PatSeedingTool.MaxTrackChi2 = CommonSeedingTrackingOptions_EarlyData["MaxTrackChi2"]
        #    recoSeeding.PatSeedingTool.MaxChi2HitIT = CommonSeedingTrackingOptions_EarlyData["MaxChi2HitIT"]
        #    recoSeeding.PatSeedingTool.MaxChi2HitOT = CommonSeedingTrackingOptions_EarlyData["MaxChi2HitOT"]
    


    return recoSeeding

# Move this to TrackFitter package?	
def ConfiguredHltEventFitter( Name,
                              TracksInContainer,
                              DoInit = False):
    # make sure the name is unique
    #from Gaudi.Configuration import allConfigurables
    #if allConfigurables.get( Name ) :
    #    raise ValueError, 'ConfiguredEventFitter: instance with name '+Name+' already exists'
    # create the event fitter
    from Configurables import ( TrackEventFitter, TrackMasterFitter )
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer
    # this addTool should not be necessary but unfortunately there is a problem with the toolhandle configuration
    eventfitter.addTool( TrackMasterFitter, name="Fitter")
    # configure the fitter
    from TrackFitter.ConfiguredFitters import ConfiguredHltFitter
    ConfiguredHltFitter( eventfitter.Fitter )
    return eventfitter

def ConfiguredGoodTrackFilter (Name,
                               InputLocation,
                               OutputLocation = None):
    if OutputLocation == None: OutputLocation = InputLocation + "Filtered"
    from Configurables import TrackListRefiner
    filterTracks = TrackListRefiner( Name,
                                     inputLocation = InputLocation,
                                     outputLocation = OutputLocation )
    from Configurables import  LoKi__Hybrid__TrackSelector as LoKiTrackSelector
    filterTracks.addTool(LoKiTrackSelector,name="LokiTracksSelector")
    filterTracks.Selector = LoKiTrackSelector(name="LokiTracksSelector")
    filterTracks.Selector.Code = "( TrCHI2PDOF < %(TrChi2)s )" % {"TrChi2":  HltRecoConf().getProp("GoodTrCHI2PDOF")}
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

# TODO: put selection revive/redo here (ask Sebastian)
# for now always redo:
bm_members =  DecodeVELO.members() + [recoVelo(), filterVelo] 
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
