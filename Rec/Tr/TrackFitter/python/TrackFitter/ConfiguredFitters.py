# ====================================================================
#  Track fitting options
# ====================================================================

from TrackSys.Configuration import TrackSys
from Gaudi.Configuration import allConfigurables

from Configurables import ( TrackEventFitter, TrackMasterFitter, TrackKalmanFilter,
                            TrackProjectorSelector, TrajOTProjector, TrajOTCosmicsProjector,
                            TrackMasterExtrapolator,
                            TrackSimpleExtraSelector, TrackDistanceExtraSelector,
                            TrackHerabExtrapolator,
                            SimplifiedMaterialLocator, DetailedMaterialLocator,
                            MeasurementProvider, StateDetailedBetheBlochEnergyCorrectionTool,
                            StateThickMSCorrectionTool)

def ConfiguredMasterFitter( Name,
                            FieldOff                 = None,
                            SimplifiedGeometry       = None,
                            NoDriftTimes             = None,
                            KalmanSmoother           = None,
                            LiteClusters             = False,
                            ApplyMaterialCorrections = None,
                            StateAtBeamLine          = True,
                            MaxNumberOutliers        = 2 ,
                            FastOutlierIteration     = False,
                            MSRossiAndGreisen        = False):
    # set the mutable default arguments
    if FieldOff is None:                 FieldOff = TrackSys().fieldOff()
    if SimplifiedGeometry is None:       SimplifiedGeometry = TrackSys().simplifiedGeometry()
    if NoDriftTimes is None:             NoDriftTimes = TrackSys().noDrifttimes()
    if KalmanSmoother is None:           KalmanSmoother = TrackSys().kalmanSmoother()
    if ApplyMaterialCorrections is None: ApplyMaterialCorrections = not TrackSys().noMaterialCorrections()
    
    if isinstance(Name,TrackMasterFitter) :
        fitter = Name
    else :
        if allConfigurables.get( Name ) :
            raise ValueError, 'ConfiguredMasterFitter: instance with name '+Name+' already exists'
        fitter = TrackMasterFitter(Name)
        
    # apply material corrections
    if not ApplyMaterialCorrections:
        fitter.ApplyMaterialCorrections = False
        fitter.Extrapolator.ApplyMultScattCorr = False
        fitter.Extrapolator.ApplyEnergyLossCorr = False
        fitter.Extrapolator.ApplyElectronEnergyLossCorr = False

    # provide a state at the beamline
    fitter.StateAtBeamLine = StateAtBeamLine

    # set the maximum number of outliers removed in the fit
    fitter.MaxNumberOutliers = MaxNumberOutliers
    
    # set up the material locator
    # Using 'RossiAndGreisen' will omit the log term in the multiple scattering calculation (which should give better pulls).
    if SimplifiedGeometry:
        fitter.MaterialLocator = SimplifiedMaterialLocator()
        fitter.addTool(SimplifiedMaterialLocator, name = "MaterialLocator")
        fitter.Extrapolator.addTool(SimplifiedMaterialLocator, name="MaterialLocator")
    else:
        fitter.MaterialLocator = DetailedMaterialLocator()
        fitter.addTool(DetailedMaterialLocator, name="MaterialLocator")
        fitter.Extrapolator.addTool(DetailedMaterialLocator, name="MaterialLocator")
        
    fitter.MaterialLocator.StateMSCorrectionToolName = "StateThickMSCorrectionTool/StateMSCorrectionTool"
    fitter.MaterialLocator.addTool( StateThickMSCorrectionTool,  "StateMSCorrectionTool")
    fitter.MaterialLocator.StateMSCorrectionTool.UseRossiAndGreisen = MSRossiAndGreisen
    fitter.Extrapolator.MaterialLocator.StateMSCorrectionToolName = "StateThickMSCorrectionTool/StateMSCorrectionTool"
    fitter.Extrapolator.MaterialLocator.addTool( StateThickMSCorrectionTool,  "StateMSCorrectionTool")
    fitter.Extrapolator.MaterialLocator.StateMSCorrectionTool.UseRossiAndGreisen = MSRossiAndGreisen


        
    # not yet used for DC09 production    
    # else:
    #    fitter.addTool(DetailedMaterialLocator(), name="MaterialLocator")
    #    fitter.MaterialLocator.GeneralDedxToolName="StateDetailedBetheBlochEnergyCorrectionTool"
    #    fitter.Extrapolator.addTool(DetailedMaterialLocator(), name="MaterialLocator")
    #    fitter.Extrapolator.GeneralDedxToolName = "StateDetailedBetheBlochEnergyCorrectionTool"
    #    fitter.Extrapolator.addTool(DetailedMaterialLocator(), name="MaterialLocator")
    #    fitter.Extrapolator.MaterialLocator.GeneralDedxToolName = "StateDetailedBetheBlochEnergyCorrectionTool"
        
    # special settings for field-off
    if FieldOff:
        fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
        fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
        fitter.Extrapolator.ApplyEnergyLossCorr = False
        fitter.Extrapolator.ApplyElectronEnergyLossCorr = False
        fitter.ApplyEnergyLossCorr = False
        fitter.NodeFitter.DoF = 4

    # change the smoother
    if KalmanSmoother:
        fitter.NodeFitter.ForceBiDirectionalFit = False

    if FastOutlierIteration:
        fitter.UpdateReferenceInOutlierIterations = False

    # don't use drift times
    if NoDriftTimes:
        # set up the NoDriftTimeProjector in the toolsvc
        defaultOTNoDriftTimeProjector = TrajOTProjector("OTNoDrifttimesProjector")
        defaultOTNoDriftTimeProjector.UseDrift = False
        fitter.Projector.OT = defaultOTNoDriftTimeProjector

    # use lite clusters for velo and ST
    if LiteClusters:
        from Configurables import (MeasurementProviderT_MeasurementProviderTypes__VeloLiteR_,
                                   MeasurementProviderT_MeasurementProviderTypes__VeloLitePhi_,
                                   MeasurementProviderT_MeasurementProviderTypes__TTLite_,
                                   MeasurementProviderT_MeasurementProviderTypes__ITLite_,
                                   MeasurementProviderT_MeasurementProviderTypes__UTLite_)
        fitter.MeasProvider.VeloRProvider = MeasurementProviderT_MeasurementProviderTypes__VeloLiteR_()
        fitter.MeasProvider.VeloPhiProvider = MeasurementProviderT_MeasurementProviderTypes__VeloLitePhi_()
        fitter.MeasProvider.TTProvider = MeasurementProviderT_MeasurementProviderTypes__TTLite_()
        fitter.MeasProvider.ITProvider = MeasurementProviderT_MeasurementProviderTypes__ITLite_()
        fitter.MeasProvider.UTProvider = MeasurementProviderT_MeasurementProviderTypes__UTLite_()

    if TrackSys().cosmics():
        fitter.MeasProvider.OTProvider.RawBankDecoder = 'OTMultiBXRawBankDecoder'

    # figure out upgrade detectors
    from Configurables import LHCbApp
    if hasattr(LHCbApp(),"Detectors"):
        if LHCbApp().isPropertySet("Detectors"):
            subDets = LHCbApp().upgradeDetectors()
            useUpgrade = bool([det for det in subDets if det in ['VP','UT','FT']])            
            if useUpgrade:
                fitter.MeasProvider.IgnoreVelo = True
                fitter.MeasProvider.IgnoreVP = True
                fitter.MeasProvider.IgnoreTT = True
                fitter.MeasProvider.IgnoreUT = True
                fitter.MeasProvider.IgnoreIT = True
                fitter.MeasProvider.IgnoreOT = True
                fitter.MeasProvider.IgnoreFT = True

                if ("VP" in subDets):
                    fitter.MeasProvider.IgnoreVP = False
                if ("FT" in subDets):
                    fitter.MeasProvider.IgnoreFT = False
                if ("UT" in subDets):
                    fitter.MeasProvider.IgnoreUT = False



    return fitter


def ConfiguredEventFitter( Name,
                           TracksInContainer,
                           FieldOff = None,
                           SimplifiedGeometry = None,
                           NoDriftTimes       = None,
                           KalmanSmoother     = None,
                           LiteClusters = False,
                           ApplyMaterialCorrections = None,
                           StateAtBeamLine = True,
                           MaxNumberOutliers = 2):
    # make sure the name is unique
    if allConfigurables.get( Name ) :
        raise ValueError, 'ConfiguredEventFitter: instance with name '+Name+' already exists'
    # create the event fitter
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer
    # this addTool should not be necessary but unfortunately there is a problem with the toolhandle configuration
    eventfitter.addTool( TrackMasterFitter, name="Fitter")
    # configure the fitter
    ConfiguredMasterFitter( eventfitter.Fitter,
                            FieldOff=FieldOff,
                            SimplifiedGeometry=SimplifiedGeometry,
                            NoDriftTimes=NoDriftTimes,
                            KalmanSmoother=KalmanSmoother,
                            LiteClusters=LiteClusters,
                            ApplyMaterialCorrections=ApplyMaterialCorrections,
                            StateAtBeamLine=StateAtBeamLine,
                            MaxNumberOutliers=MaxNumberOutliers)
    return eventfitter



def ConfiguredHltFitter( Name ):
    fitter = ConfiguredMasterFitter(Name,
                                    FieldOff=None,
                                    SimplifiedGeometry = True,
                                    LiteClusters = True)
    fitter.NodeFitter.ForceBiDirectionalFit    = True
    fitter.NumberFitIterations = 1
    fitter.MaxNumberOutliers = 2
    fitter.UpdateTransport = False
    fitter.AddDefaultReferenceNodes = True   
    fitter.UpdateReferenceInOutlierIterations = False   
    ## Force to use drift time
    from Configurables import TrajOTProjector, TrackProjectorSelector
    otprojector = TrajOTProjector('OTFastFitProjector')
    otprojector.PrefitStrategy = 0
    fitter.Projector.OT = otprojector    
    fitter.MeasProvider.IgnoreMuon = True
    return fitter


def ConfiguredForwardFitter( Name, FieldOff = None, LiteClusters = True,
                          ForceUseDriftTime = True ):
    fitter = ConfiguredMasterFitter(Name,
                                    FieldOff=FieldOff,
                                    SimplifiedGeometry = True,
                                    LiteClusters = LiteClusters)
    fitter.NumberFitIterations = 1
    fitter.MaxNumberOutliers = 0
    fitter.AddDefaultReferenceNodes = False
    fitter.NodeFitter.ForceBiDirectionalFit = False
    fitter.FillExtraInfo = False
    fitter.UpdateReferenceInOutlierIterations = False
    if ForceUseDriftTime:
        from Configurables import TrajOTProjector, TrackProjectorSelector
        otprojector = TrajOTProjector('OTFastFitProjector')
        otprojector.PrefitStrategy = 0
        fitter.Projector.OT = otprojector
    return fitter

def ConfiguredForwardEventFitter( Name, TracksInContainer,
                               ForceUseDriftTime = True ):
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer
    fittername = Name + ".Fitter"
    eventfitter.addTool( ConfiguredForwardFitter( Name = fittername, ForceUseDriftTime = ForceUseDriftTime ), name = "Fitter")
    return eventfitter

def ConfiguredHltEventFitter( Name, TracksInContainer ):
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer
    fittername = Name + ".Fitter"
    eventfitter.addTool( ConfiguredHltFitter( Name = fittername ), name = "Fitter")
    return eventfitter

def ConfiguredVeloOnlyEventFitter( Name, TracksInContainer ):
    eventfitter = ConfiguredFitter( Name, TracksInContainer,FieldOff=True )
    eventfitter.Fitter.MeasProvider.IgnoreIT = True
    eventfitter.Fitter.MeasProvider.IgnoreOT = True
    eventfitter.Fitter.MeasProvider.IgnoreTT = True
    eventfitter.Fitter.MeasProvider.IgnoreMuon = True
    return eventfitter

def ConfiguredStraightLineFitter( Name, TracksInContainer,
                                  NoDriftTimes = None ):
    fitter = ConfiguredFitter(Name,TracksInContainer,
                                   FieldOff=True,
                                   NoDriftTimes=NoDriftTimes,
                                   StateAtBeamLine=False,
                                   ApplyMaterialCorrections=False)
    fitter.Fitter.AddDefaultReferenceNodes = False
    return eventfitter

def ConfiguredForwardStraightLineFitter( Name ):
    fitter =  ConfiguredForwardFitter( Name, FieldOff=True )
    fitter.ApplyMaterialCorrections = False
    return fitter

def ConfiguredForwardStraightLineEventFitter( Name, TracksInContainer ):
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer
    fittername = Name + ".Fitter"
    eventfitter.addTool( ConfiguredForwardStraightLineFitter( Name = fittername ), name = "Fitter")
    return eventfitter

def ConfiguredCosmicsEventFitter( Name, TracksInContainer,
                                  MaxNumberOutliers = 0 ):
    # create the OTCosmicsProjector if it doesn't exist yet
    if not allConfigurables.get( 'OTCosmicsProjector' ) :
        cosmicsOTProjector = TrajOTCosmicsProjector('OTCosmicsProjector')
        cosmicsOTProjector.FitEventT0 = True
        cosmicsOTProjector.UseConstantDriftVelocity = True
    else :
        cosmicsOTProjector = TrajOTCosmicsProjector('OTCosmicsProjector')
    # create the fitter and set it up
    eventfitter = ConfiguredStraightLineFitter(Name, TracksInContainer,  NoDriftTimes = False )
    eventfitter.Fitter.Projector.OT = cosmicsOTProjector
    eventfitter.Fitter.MaxNumberOutliers = MaxNumberOutliers
    eventfitter.Fitter.ErrorQoP = [ 25, 0 ]
    return eventfitter


####################################################################
## Below are configurations for 'Fit' algorithms used in           #
## RecoTracking.py. Don't use these.                               #
####################################################################

def ConfiguredFitVelo( Name = "FitVelo",
                       TracksInContainer = "Rec/Track/PreparedVelo"):
    # note that we ignore curvatue in velo. in the end that seems the
    # most sensible thing to do.
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    return eventfitter

def ConfiguredFitVeloTT( Name = "FitVeloTT",
                         TracksInContainer = "Rec/Track/VeloTT" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    # Make the nodes even though this is a refit, to add StateAtBeamLine
    # (PatVeloTT also fits but does not make nodes)
    eventfitter.Fitter.MakeNodes = True
    return eventfitter

def ConfiguredFitSeed( Name = "FitSeedForMatch",
                       TracksInContainer = "Rec/Track/Seed" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    ## Such a small error is needed for TrackMatching to work properly,
    eventfitter.Fitter.ErrorQoP = [0.04, 5e-08]
    return eventfitter

def ConfiguredFitForward( Name = "FitForward",
                         TracksInContainer = "Rec/Track/Forward" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    return eventfitter

def ConfiguredFitMatch( Name = "FitMatch",
                        TracksInContainer = "Rec/Track/Match" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    return eventfitter
 
def ConfiguredFitDownstream( Name = "FitDownstream",
                             TracksInContainer = "Rec/Track/Downstream" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    return eventfitter

def ConfiguredFit( Name ,
                   TracksInContainer ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    return eventfitter


