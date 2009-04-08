# Test of Spillover inside Gauss
#
from Gauss.Configuration import *

from tempfile import NamedTemporaryFile

#--Allow to use ns (ie. double) in options of EventClockSvc
ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")
ec.EventTimeDecoder.StartTime = 0
ec.EventTimeDecoder.TimeStep  = 25*SystemOfUnits.ns

gauss = ApplicationMgr()

#-- Define which slots to generate
#########SpillOverSlots = [ '' , 'Prev' , 'Next' , 'PrevPrev' ]
SpillOverSlots = [ '' , 'Prev' ]

#####################################################################################
#-- General settings for Gauss:
# 
rndmSvc = RndmGenSvc()
engine = HepRndm__Engine_CLHEP__RanluxEngine_("RndmGenSvc.Engine")
engine.SetSingleton = True
gauss.EvtSel = 'NONE'
MessageSvc().OutputLevel = 3
# Longer "algorithm" identifier in printout
MessageSvc().Format = "% F%24W%S%7W%R%T %0W%M"
# Transient store setup
EventDataSvc().ForceLeaves = True
# POOL persistency 
EventPersistencySvc().CnvServices  = []
importOptions("$STDOPTS/SimDicts.opts")
# Other outputs
gauss.HistogramPersistency = "NONE"
#
importOptions( "$STDOPTS/RootHist.opts" )
RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True
gauss.ExtSvc += [ AuditorSvc() ]
gauss.AuditAlgorithms = True
AuditorSvc().Auditors += [ TimingAuditor() ]

#####################################################################################
# Define Main sequence
gaussSeq = GaudiSequencer("Gauss")
gauss.TopAlg = [ gaussSeq ]

gaussGeneratorSeq = GaudiSequencer("Generator" , IgnoreFilterPassed = True )
gaussSimulationSeq = GaudiSequencer("Simulation")
gaussSeq.Members += [ gaussGeneratorSeq , gaussSimulationSeq ]

LHCbApp().EvtMax = 1

#######################################################################################
# Define Generation Sequences

EvtGenDecay().DecayFile = "$DECFILESROOT/dkfiles/DECAY.DEC"

for slot in SpillOverSlots:
    sequence = GaudiSequencer("GeneratorSlot" + slot )
    genInit = GenInit("GaussGen" + slot , MCHeader = "/Event/"+slot_(slot)+"Gen/Header" , OutputLevel = 1 )
    genInit.FirstEventNumber = 1
    genInit.RunNumber        = 1082
    generation = Generation("Generation"+slot , 
			     GenHeaderLocation = "/Event/"+slot_(slot)+"Gen/Header" ,
			     HepMCEventLocation = "/Event/"+slot_(slot)+"Gen/HepMCEvents" , 
			     GenCollisionLocation = "/Event/"+slot_(slot)+"Gen/Collisions" )
    sequence.Members += [ genInit , generation ]
    if slot != '':
	generation.PileUpTool = 'FixedLuminosityForSpillOver' 
	
    gaussGeneratorSeq.Members += [ sequence ]
    moniseq = GaudiSequencer("GenMonitor" + slot )
    sequence.Members += [ moniseq ]  
    moniseq.Members += [ 
                         GenMonitorAlg( "GenMonitorAlg" + slot , HistoProduce = True ) 
                         , DumpHepMC( "DumpHepMC" + slot , OutputLevel = 1 , Addresses = ["/Event/"+slot_(slot)+"Gen/HepMCEvents"] ) 
		       ]
  
#######################################################################################
# Define Simulation Sequences
#gaussSimulationSeq.Members += [ SimInit("GaussSim") ]

gauss.ExtSvc += [ "GiGa" ]
EventPersistencySvc().CnvServices += [ "GiGaKine" ]

#geo = GiGaInputStream( "Geo" ) 
#gigaStore = GiGaDataStoreAlgorithm( "GiGaStore" )
#gigaStore.ConversionServices = [ "GiGaKine" ]
#gaussSimulationSeq.Members += [ gigaStore , geo ] 

# Sim geometry
#geo.ExecuteOnce          = True 
#geo.ConversionSvcName    = "GiGaGeo" 
#geo.DataProviderSvcName  = "DetectorDataSvc"

#geo.StreamItems      += ["/dd/Structure/LHCb/BeforeMagnetRegion/Velo"]
#geo.StreamItems      += ["/dd/Structure/LHCb/BeforeMagnetRegion/Velo2Rich1"]
#geo.StreamItems      += ["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1"]
#geo.StreamItems      += ["/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1Surfaces"]
#geo.StreamItems      += ["/dd/Structure/LHCb/BeforeMagnetRegion/TT"]
#geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/PipeInMagnet"]
#geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/PipeSupportsInMagnet"]
#geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/Magnet"]
#geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/BcmDown"]
#geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/T"]
#geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/Rich2"]
#geo.StreamItems      += ["/dd/Geometry/AfterMagnetRegion/Rich2/Rich2Surfaces"]
#geo.StreamItems      += ["/dd/Geometry/BeforeMagnetRegion/Rich1/RichHPDSurfaces"]
#geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/PipeAfterT"]
#geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/PipeSupportsAfterMagnet"]
#geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Spd"]
#geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Converter"]
#geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Prs"]
#geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Ecal"]
#geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Hcal"]
#geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Muon"]
#geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/PipeDownstream"]
#geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/PipeSupportsDownstream"]
#geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/PipeBakeoutDownstream"]

#importOptions( "$GAUSSCALOROOT/options/Calo.opts" )
HistogramDataSvc().Input += [ "GaussCalo DATAFILE='$PARAMFILESROOT/data/gausscalo.root' TYP='ROOT'" ]
GiGaGeo().addTool( SpdPrsSensDet( "Spd" ) , name = "Spd" ) 
GiGaGeo().Spd.StartVolumes = [   "/dd/Geometry/DownstreamRegion/Spd/Modules/InnCell"  ,
                                 "/dd/Geometry/DownstreamRegion/Spd/Modules/MidCell"  ,
                                 "/dd/Geometry/DownstreamRegion/Spd/Modules/OutCell" ]
GiGaGeo().Spd.EndVolume      =  "/dd/Geometry/DownstreamRegion/Spd/Installation/Spd"
GiGaGeo().Spd.CollectionName =  "SpdHits"
GiGaGeo().Spd.Detector       =  "/dd/Structure/LHCb/DownstreamRegion/Spd"
GiGaGeo().Spd.zMin           =  12300. * SystemOfUnits.mm 
GiGaGeo().Spd.zMax           =  15000. * SystemOfUnits.mm 
GiGaGeo().Spd.DetectorPath   =  "/LHCb/Calo"
GiGaGeo().Spd.IntegrationDelays = [ 18.5 * SystemOfUnits.ns , 17.5 * SystemOfUnits.ns , 16.5 * SystemOfUnits.ns ]
GiGaGeo().Spd.Histograms = [ "GaussCalo/SPDTIME/h1" , "GaussCalo/SPDTIME/h2" , "GaussCalo/SPDTIME/h3" ]

GiGaGeo().addTool( SpdPrsSensDet( "Prs" ) , name = "Prs" ) 
GiGaGeo().Prs.StartVolumes = [ "/dd/Geometry/DownstreamRegion/Prs/Modules/InnCell"  ,
			       "/dd/Geometry/DownstreamRegion/Prs/Modules/MidCell"  ,
                               "/dd/Geometry/DownstreamRegion/Prs/Modules/OutCell"  ]
GiGaGeo().Prs.EndVolume      =  "/dd/Geometry/DownstreamRegion/Prs/Installation/Prs"
GiGaGeo().Prs.CollectionName =  "PrsHits"
GiGaGeo().Prs.Detector       =  "/dd/Structure/LHCb/DownstreamRegion/Prs"
GiGaGeo().Prs.zMin           =  12300. * SystemOfUnits.mm
GiGaGeo().Prs.zMax           =  15000. * SystemOfUnits.mm
GiGaGeo().Prs.DetectorPath   =  "/LHCb/Calo"
GiGaGeo().Prs.IntegrationDelays = [ 18.5 * SystemOfUnits.ns , 17.5 * SystemOfUnits.ns , 16.5 * SystemOfUnits.ns ]
GiGaGeo().Prs.Histograms = [ "GaussCalo/PRSTIME/h1" , "GaussCalo/PRSTIME/h2" , "GaussCalo/PRSTIME/h3" ]

GiGaGeo().addTool( EcalSensDet( "Ecal" ) , name = "Ecal" ) 
GiGaGeo().Ecal.StartVolumes = [ "/dd/Geometry/DownstreamRegion/Ecal/Modules/InnCell",
				"/dd/Geometry/DownstreamRegion/Ecal/Modules/MidCell",
				"/dd/Geometry/DownstreamRegion/Ecal/Modules/OutCell" ]
GiGaGeo().Ecal.EndVolume      =  "/dd/Geometry/DownstreamRegion/Ecal/Installation/Ecal"
GiGaGeo().Ecal.CollectionName =  "EcalHits"
GiGaGeo().Ecal.Detector       =  "/dd/Structure/LHCb/DownstreamRegion/Ecal"
GiGaGeo().Ecal.zMin           =  12300. * SystemOfUnits.mm
GiGaGeo().Ecal.zMax           =  15000. * SystemOfUnits.mm
GiGaGeo().Ecal.DetectorPath   =  "/LHCb/Calo"
GiGaGeo().Ecal.Histograms = [ "GaussCalo/ECALTIME/h1" , "GaussCalo/ECALTIME/h2" ]

GiGaGeo().addTool( HcalSensDet( "Hcal" ) , name = "Hcal" ) 
GiGaGeo().Hcal.StartVolumes = [ "/dd/Geometry/DownstreamRegion/Hcal/Cells/lvHcalInnCellUpScTile"  ,
				"/dd/Geometry/DownstreamRegion/Hcal/Cells/lvHcalInnCellLowScTile" ,
				"/dd/Geometry/DownstreamRegion/Hcal/Cells/lvHcalOutCellScTile" ]
GiGaGeo().Hcal.EndVolume      =  "/dd/Geometry/DownstreamRegion/Hcal/Installation/lvHcal"
GiGaGeo().Hcal.CollectionName =  "HcalHits"
GiGaGeo().Hcal.Detector       =  "/dd/Structure/LHCb/DownstreamRegion/Hcal"
GiGaGeo().Hcal.zMin           =  12300. * SystemOfUnits.mm
GiGaGeo().Hcal.zMax           =  15000. * SystemOfUnits.mm
GiGaGeo().Hcal.DetectorPath   =  "/LHCb/Calo"
GiGaGeo().Hcal.Histograms = [ "GaussCalo/HCALTIME/h1" , "GaussCalo/HCALTIME/h2" ]

GiGaGeo().addTool( GaussSensPlaneDet( "CaloSP" ) , name = "CaloSP" ) 
GiGaGeo().CaloSP.DetectorPath   =  "/LHCb/Calo" 
GiGaGeo().CaloSP.CollectionName =  "CaloSPHits" 

GiGaGeo().FieldManager           = "GiGaFieldMgr/FieldMgr"
GiGaGeo().addTool( GiGaFieldMgr("FieldMgr"), name="FieldMgr" )
GiGaGeo().FieldMgr.Stepper       = "ClassicalRK4"
GiGaGeo().FieldMgr.Global        = True
GiGaGeo().FieldMgr.MagneticField = "GiGaMagFieldGlobal/LHCbField"
GiGaGeo().FieldMgr.addTool( GiGaMagFieldGlobal("LHCbField") , name="LHCbField" ) 
GiGaGeo().FieldMgr.LHCbField.MagneticFieldService = "MagneticFieldSvc"

giga = GiGa()
giga.addTool( GiGaPhysListModular("ModularPL") , name="ModularPL" ) 
giga.PhysicsList = "GiGaPhysListModular/ModularPL"
giga.ModularPL.CutForElectron = 10000.0 * SystemOfUnits.m
giga.ModularPL.CutForPositron = 5.0 * SystemOfUnits.mm 
giga.ModularPL.CutForGamma    = 10.0 * SystemOfUnits.mm
importOptions("$GAUSSOPTS/PhysList-LHEP.opts")

giga.addTool( GiGaRunActionSequence("RunSeq") , name="RunSeq" )
giga.RunAction = "GiGaRunActionSequence/RunSeq"
giga.RunSeq.addTool( TrCutsRunAction("TrCuts") , name = "TrCuts" )
giga.RunSeq.addTool( GiGaRunActionCommand("RunCommand") , name = "RunCommand" ) 
giga.RunSeq.Members += [ "TrCutsRunAction/TrCuts" ] 
giga.RunSeq.Members += [ "GiGaRunActionCommand/RunCommand" ]
giga.RunSeq.RunCommand.BeginOfRunCommands  = [ "/tracking/verbose 0", "/tracking/storeTrajectory  1", "/process/eLoss/verbose -1" ]

giga.EventAction = "GiGaEventActionSequence/EventSeq"
giga.addTool( GiGaEventActionSequence("EventSeq") , name="EventSeq" ) 
giga.EventSeq.Members      += [ "GaussEventActionHepMC/HepMCEvent" ]

giga.TrackingAction =   "GiGaTrackActionSequence/TrackSeq" 
giga.addTool( GiGaTrackActionSequence("TrackSeq") , name = "TrackSeq" )
giga.TrackSeq.Members += [ "GaussPreTrackAction/PreTrack" ]

giga.SteppingAction =   "GiGaStepActionSequence/StepSeq"
giga.addTool( GiGaStepActionSequence("StepSeq") , name = "StepSeq" ) 
importOptions("$GAUSSRICHROOT/options/Rich.opts")

giga.TrackSeq.Members += [ "GaussPostTrackAction/PostTrack" ]
giga.TrackSeq.Members += [ "GaussTrackActionHepMC/HepMCTrack" ]
giga.TrackSeq.addTool( GaussPostTrackAction("PostTrack") , name = "PostTrack" ) 
giga.TrackSeq.PostTrack.StoreAll          = False
giga.TrackSeq.PostTrack.StorePrimaries    = True
giga.TrackSeq.PostTrack.StoreMarkedTracks = True
giga.TrackSeq.PostTrack.StoreForcedDecays = True
giga.TrackSeq.PostTrack.StoreByOwnEnergy   = True
giga.TrackSeq.PostTrack.OwnEnergyThreshold = 100.0 * SystemOfUnits.MeV 

giga.TrackSeq.PostTrack.StoreByChildProcess  = True
giga.TrackSeq.PostTrack.StoredChildProcesses = [ "RichG4Cerenkov", "Decay" ]
giga.TrackSeq.PostTrack.StoreByOwnProcess  = True 
giga.TrackSeq.PostTrack.StoredOwnProcesses = [ "Decay" ]
giga.StepSeq.Members += [ "GaussStepAction/GaussStep" ]

SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/Simulation.xml"
giga.addTool( GiGaRunManager("GiGaMgr") , name="GiGaMgr" ) 
giga.GiGaMgr.RunTools += [ "GiGaSetSimAttributes" ]
giga.GiGaMgr.RunTools += [ "GiGaRegionsTool" ]
giga.GiGaMgr.addTool( GiGaSetSimAttributes() , name = "GiGaSetSimAttributes" )
giga.GiGaMgr.GiGaSetSimAttributes.OutputLevel = 4 

# From RichExtendedInfo
giga.ModularPL.addTool( GiGaPhysConstructorOp() , name = "GiGaPhysConstructorOp" ) 
giga.ModularPL.GiGaPhysConstructorOp.RichActivateRichPhysicsProcVerboseTag = True
giga.StepSeq.Members  += [ "RichG4StepAnalysis4/RichStepAgelExit" ]
giga.StepSeq.Members  += [ "RichG4StepAnalysis5/RichStepMirrorRefl" ]
importOptions("$GAUSSRICHROOT/options/RichAnalysis.opts")

gigaStore = GiGaDataStoreAlgorithm( "GiGaStore" , OutputLevel = 1 )
gigaStore.ConversionServices = [ "GiGaKine" ]
gaussSimulationSeq.Members += [ gigaStore ] 

geo = GiGaInputStream( "Geo" , OutputLevel = 1 ) 
gaussSimulationSeq.Members += [ geo ]     
# Sim geometry
geo.ExecuteOnce          = True 
geo.ConversionSvcName    = "GiGaGeo" 
geo.DataProviderSvcName  = "DetectorDataSvc"
    
geo.StreamItems      += ["/dd/Structure/LHCb/BeforeMagnetRegion/Velo"]
geo.StreamItems      += ["/dd/Structure/LHCb/BeforeMagnetRegion/Velo2Rich1"]
geo.StreamItems      += ["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1"]
geo.StreamItems      += ["/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1Surfaces"]
geo.StreamItems      += ["/dd/Structure/LHCb/BeforeMagnetRegion/TT"]
geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/PipeInMagnet"]
geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/PipeSupportsInMagnet"]
geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/Magnet"]
geo.StreamItems      += ["/dd/Structure/LHCb/MagnetRegion/BcmDown"]
geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/T"]
geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/Rich2"]
geo.StreamItems      += ["/dd/Geometry/AfterMagnetRegion/Rich2/Rich2Surfaces"]
geo.StreamItems      += ["/dd/Geometry/BeforeMagnetRegion/Rich1/RichHPDSurfaces"]
geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/PipeAfterT"]
geo.StreamItems      += ["/dd/Structure/LHCb/AfterMagnetRegion/PipeSupportsAfterMagnet"]
geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Spd"]
geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Converter"]
geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Prs"]
geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Ecal"]
geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Hcal"]
geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/Muon"]
geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/PipeDownstream"]
geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/PipeSupportsDownstream"]
geo.StreamItems      += ["/dd/Structure/LHCb/DownstreamRegion/PipeBakeoutDownstream"]
    
for slot in SpillOverSlots:
    sequenceinit = GaudiSequencer( "SimulationInit" + slot ) 
    sequence = GaudiSequencer( "SimulationSlot" + slot , 
                               RequireObjects = [ "/Event/"+slot_(slot)+"Gen/HepMCEvents" ] )
    sequenceinit.Members += [ SimInit("GaussSim" + slot , OutputLevel = 1 , GenHeader = "/Event/"+slot_(slot)+"Gen/Header" ,
                              MCHeader = "/Event/"+slot_(slot)+"MC/Header" ) ,
			      sequence ]
    gaussSimulationSeq.Members += [ sequenceinit ]
    
    genToSim = GenerationToSimulation( "GenerationToSimulation" + slot , LookForUnknownParticles = True , 
                                       HepMCEventLocation = "/Event/"+slot_(slot)+"Gen/HepMCEvents" ,
				       Particles = "/Event/"+slot_(slot)+"MC/Particles" ,
				       Vertices = "/Event/"+slot_(slot)+"MC/Vertices" ,
				       MCHeader = "/Event/"+slot_(slot)+"MC/Header" ) 
    sequence.Members += [ genToSim ]
    sequence.Members += [ GiGaFlushAlgorithm( "GiGaFlushAlgorithm" + slot ) ]
    sequence.Members += [ GiGaCheckEventStatus( "GiGaCheckEventStatus" + slot ) ]
    sequence.Members += [ SimulationToMCTruth( "SimulationToMCTruth" + slot  ,
			  Particles = "/Event/"+slot_(slot)+"MC/Particles",
			  Vertices = "/Event/"+slot_(slot)+"MC/Vertices" ,
			  MCHeader = "/Event/"+slot_(slot)+"MC/Header"  ) ]
    
    detHits = GaudiSequencer( "DetectorsHits" + slot ) 
    sequence.Members += [ detHits ]
    veloHits = GetTrackerHitsAlg( "GetVeloHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    puvetoHits = GetTrackerHitsAlg( "GetPuVetoHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    ttHits = GetTrackerHitsAlg( "GetTTHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    itHits = GetTrackerHitsAlg( "GetITHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    otHits = GetTrackerHitsAlg( "GetOTHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    muonHits = GetTrackerHitsAlg( "GetMuonHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    spdHits = GetCaloHitsAlg( "GetSpdHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    prsHits = GetCaloHitsAlg( "GetPrsHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    ecalHits = GetCaloHitsAlg( "GetEcalHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    hcalHits = GetCaloHitsAlg( "GetHcalHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ) 
    richHits = GetMCRichHitsAlg( "GetRichHits" + slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ,
                                  MCRichHitsLocation = "/Event/"+slot_(slot)+"MC/Rich/Hits") 
    rh = GaudiSequencer( "RichHits" + slot ) 
    rh.Members += [ richHits ] 
    detHits.Members += [ veloHits , puvetoHits , ttHits , itHits , otHits , muonHits , spdHits , prsHits , ecalHits , hcalHits , rh ]
    detHits.MeasureTime = True 
    ##################################
    setTrackersHitsProperties( veloHits , 'Velo' , slot , 'BeforeMagnetRegion/Velo' ) 
    setTrackersHitsProperties( puvetoHits , 'PuVeto' , slot , 'BeforeMagnetRegion/Velo' ) 
    setTrackersHitsProperties( ttHits , 'TT' , slot , 'BeforeMagnetRegion/TT' ) 
    setTrackersHitsProperties( itHits , 'IT' , slot , 'AfterMagnetRegion/T/IT' ) 
    setTrackersHitsProperties( otHits , 'OT' , slot , 'AfterMagnetRegion/T/OT' ) 
    setTrackersHitsProperties( muonHits , 'Muon' , slot , 'DownstreamRegion/Muon' ) 
    ###################################
    rh.Members += [ GetMCRichOpticalPhotonsAlg("GetRichPhotons" + slot , MCRichOpticalPhotonsLocation = "/Event/"+slot_(slot)+"MC/Rich/OpticalPhotons" ,
                    MCRichHits = "/Event/"+slot_(slot)+"MC/Rich/Hits" ) ]
    rh.Members += [ GetMCRichSegmentsAlg("GetRichSegments" + slot , MCRichOpticalPhotonsLocation = "/Event/"+slot_(slot)+"MC/Rich/OpticalPhotons" ,
                                         MCRichHitsLocation = "/Event/"+slot_(slot)+"MC/Rich/Hits" , 
					 MCRichSegmentsLocation = "/Event/"+slot_(slot)+"MC/Rich/Segments" ) ]
    rh.Members += [ GetMCRichTracksAlg("GetRichTracks" + slot , MCRichTracksLocation = "/Event/"+slot_(slot)+"MC/Rich/Tracks" ,
                    MCParticles = "/Event/"+slot_(slot)+"MC/Particles"  , MCRichSegments = "/Event/"+slot_(slot)+"MC/Rich/Segments" ) ]
    rh.Members += [ Rich__MC__MCPartToMCRichTrackAlg("MCPartToMCRichTrack"+slot) ]
    rh.Members += [ Rich__MC__MCRichHitToMCRichOpPhotAlg("MCRichHitToMCRichOpPhot"+slot) ]  
    ####################################
    moniseq = GaudiSequencer( "SimMonitor" + slot ) 
    sequence.Members += [ moniseq ]
    moniseq.Members += [ GiGaGetEventAlg("GiGaGetEventAlg"+slot, Vertices = '' , Particles = "/Event/"+slot_(slot)+"MC/Particles" ) , 
                         MCTruthMonitor( "MCTruthMonitor"+slot , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ,
			 MCVertices = "/Event/"+slot_(slot)+"MC/Vertices" ) ]    
    gethits = GiGaGetHitsAlg( "GiGaGetHitsAlg" + slot )
    moniseq.Members += [ gethits ]
    gethits.VeloHits = '/Event/' + slot_(slot) + 'MC/Velo/Hits'
    gethits.PuVetoHits = '/Event/' + slot_(slot) + 'MC/PuVeto/Hits'
    gethits.TTHits = '/Event/' + slot_(slot) + 'MC/TT/Hits'
    gethits.ITHits = '/Event/' + slot_(slot) + 'MC/IT/Hits'
    gethits.OTHits = '/Event/' + slot_(slot) + 'MC/OT/Hits'
    gethits.CaloHits = [ '/Event/' + slot_(slot) + 'MC/Spd/Hits' , '/Event/' + slot_(slot) + 'MC/Prs/Hits' , 
			 '/Event/' + slot_(slot) + 'MC/Ecal/Hits' , '/Event/' + slot_(slot) + 'MC/Hcal/Hits' ]
    gethits.MuonHits =  '/Event/' + slot_(slot) + 'MC/Muon/Hits' 
    gethits.RichHits =  '/Event/' + slot_(slot) + 'MC/Rich/Hits' 
    gethits.RichOpticalPhotons = '/Event/'+slot_(slot)+'MC/Rich/OpticalPhotons'
    gethits.RichSegments = '/Event/'+slot_(slot)+'MC/Rich/Segments'
    gethits.RichTracks = '/Event/'+slot_(slot)+'MC/Rich/Tracks'
    #####################################
    spdHits.MCHitsLocation = "/Event/" + slot_(slot) + "MC/Spd/Hits"
    spdHits.CollectionName = "SpdHits"
    prsHits.MCHitsLocation = "/Event/" + slot_(slot) + "MC/Prs/Hits"
    prsHits.CollectionName = "PrsHits"
    ecalHits.MCHitsLocation = "/Event/" + slot_(slot) + "MC/Ecal/Hits"
    ecalHits.CollectionName = "EcalHits"
    hcalHits.MCHitsLocation = "/Event/" + slot_(slot) + "MC/Hcal/Hits"
    hcalHits.CollectionName = "HcalHits"
    # 
    detmoniseq = GaudiSequencer( "DetectorsMonitor" + slot ) 
    moniseq.Members += [ detmoniseq ]
    detmoniseq.Members += [ VeloGaussMoni( "VeloGaussMoni" + slot , VeloMCHits = '/Event/' + slot_(slot) + 'MC/Velo/Hits' ,
					   PuVetoMCHits = '/Event/' + slot_(slot) + 'MC/PuVeto/Hits' ) ]
    tthitmoni = MCHitMonitor( "TTHitMonitor" + slot ) 
    ithitmoni = MCHitMonitor( "ITHitMonitor" + slot ) 
    othitmoni = MCHitMonitor( "OTHitMonitor" + slot ) 
    detmoniseq.Members += [ tthitmoni , ithitmoni , othitmoni ]
    tthitmoni.mcPathString = "/Event/" + slot_(slot) + "MC/TT/Hits"
    tthitmoni.zStations = [ 2350.*SystemOfUnits.mm , 2620.*SystemOfUnits.mm ]
    tthitmoni.xMax = 150.*SystemOfUnits.cm 
    tthitmoni.yMax = 150.*SystemOfUnits.cm 
    ithitmoni.mcPathString = "/Event/" + slot_(slot) + "MC/IT/Hits"
    ithitmoni.zStations = [ 7780.0*SystemOfUnits.mm, 8460.0*SystemOfUnits.mm, 9115.0*SystemOfUnits.mm  ]
    ithitmoni.xMax = 100.*SystemOfUnits.cm 
    ithitmoni.yMax = 100.*SystemOfUnits.cm 
    othitmoni.mcPathString = "/Event/" + slot_(slot) + "MC/OT/Hits"
    othitmoni.zStations = [ 7938.0*SystemOfUnits.mm , 8625.0*SystemOfUnits.mm , 9315.0*SystemOfUnits.mm  ]
    #
    spdmoni = MCCaloMonitor( "SpdMonitor" + slot , OutputLevel = 4 , Detector = "Spd" , Regions = True , MaximumEnergy=10.*SystemOfUnits.MeV , Threshold = 1.5*SystemOfUnits.MeV ,
			     MCHits = "/Event/" + slot_(slot) + "MC/Spd/Hits" , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" , 
			     Slot = slot_(slot) ) 
    prsmoni = MCCaloMonitor( "PrsMonitor" + slot , OutputLevel = 4 , Detector = "Prs" , Regions = True , MaximumEnergy=10.*SystemOfUnits.MeV , Threshold = 1.5*SystemOfUnits.MeV , 
			     MCHits = "/Event/" + slot_(slot) + "MC/Prs/Hits" , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" , 
			     Slot = slot_(slot) ) 
    ecalmoni = MCCaloMonitor( "EcalMonitor" + slot , OutputLevel = 4 , Detector = "Ecal" , Regions = True , MaximumEnergy=1000.*SystemOfUnits.MeV , Threshold = 10.*SystemOfUnits.MeV ,
                               MCHits = "/Event/" + slot_(slot) + "MC/Ecal/Hits" , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ,
			       Slot = slot_(slot) ) 
    hcalmoni = MCCaloMonitor( "HcalMonitor" + slot , OutputLevel = 4 , Detector = "Hcal" , Regions = True , MaximumEnergy=1000.*SystemOfUnits.MeV , Threshold = 5.*SystemOfUnits.MeV ,
			      MCHits = "/Event/" + slot_(slot) + "MC/Hcal/Hits" , MCParticles = "/Event/"+slot_(slot)+"MC/Particles" ,
			      Slot = slot_(slot) ) 
    detmoniseq.Members += [ spdmoni , prsmoni , ecalmoni , hcalmoni ]
    #
    detmoniseq.Members += [ MuonHitChecker( "MuonHitChecker" + slot , FullDetail = True , MCHeader = "/Event/"+slot_(slot)+"MC/Header" ,
                            MuonHits = "/Event/" + slot_(slot) + "MC/Muon/Hits" ) ] 
    # Packing
    sequence.Members += [ PackMCParticle("PackMCParticle"+slot , 
                          InputName = "/Event/"+slot_(slot)+"MC/Particles" , OutputName = "/Event/"+slot_(slot)+"pSim/MCParticles") ]
    sequence.Members += [ PackMCVertex("PackMCVertex"+slot , 
                          InputName = "/Event/"+slot_(slot)+"MC/Vertices" , OutputName = "/Event/"+slot_(slot)+"pSim/MCVertices") ]

#--Outputs
setOutputs(SpillOverSlots)

#--Geometry dependent options, use information from SIMCOND for positions
LHCbApp( DataType = "2008", Simulation = True )
LHCbApp().DDDBtag = "head-20090112"
LHCbApp().CondDBtag = "sim-20090112" 

giGaGeo = GiGaGeo()
giGaGeo.UseAlignment      = True
giGaGeo.AlignAllDetectors = True
importOptions('$GAUSSOPTS/SimVeloGeometry.py')  # -- To misalign VELO
