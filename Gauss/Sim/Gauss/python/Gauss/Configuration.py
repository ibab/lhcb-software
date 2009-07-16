"""
High level configuration tools for Gauss
"""
__version__ = "$Id: Configuration.py,v 1.10 2009-07-16 16:25:58 gcorti Exp $"
__author__  = "Gloria Corti <Gloria.Corti@cern.ch>"

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from GaudiKernel import SystemOfUnits
from Configurables import LHCbConfigurableUser, LHCbApp

from Configurables import ( CondDBCnvSvc, EventClockSvc, FakeEventTime,
                            CondDBEntityResolver )
from Configurables import ( GenInit, Generation, MinimumBias, Inclusive,
                            SignalPlain, SignalRepeatedHadronization,
                            SignalForcedFragmentation, StandAloneDecayTool,
                            Special,
                            PythiaProduction, HijingProduction,
                            CollidingBeams, FixedTarget,
                            BeamSpotSmearVertex, FlatZSmearVertex,
                            EvtGenDecay )
from Configurables import ( SimInit, GiGaGeo, GiGaInputStream, GiGa,
                            GiGaDataStoreAlgorithm,
                            GiGaPhysListModular, GiGaRunActionSequence,
                            TrCutsRunAction, GiGaRunActionCommand,
                            GiGaEventActionSequence, GiGaMagFieldGlobal,
                            GiGaTrackActionSequence, GaussPostTrackAction,
                            GiGaStepActionSequence, SimulationSvc,
                            GiGaFieldMgr, GiGaRunManager, GiGaSetSimAttributes,
                            GiGaPhysConstructorOp, 
                            SpdPrsSensDet, EcalSensDet, HcalSensDet,
                            GaussSensPlaneDet )
from Configurables import ( GenerationToSimulation, GiGaFlushAlgorithm,
                            GiGaCheckEventStatus, SimulationToMCTruth,
                            GiGaGetEventAlg, GiGaGetHitsAlg,
                            GetTrackerHitsAlg, GetCaloHitsAlg, 
                            GetMCRichHitsAlg, GetMCRichOpticalPhotonsAlg,
                            GetMCRichSegmentsAlg, GetMCRichTracksAlg,
                            Rich__MC__MCPartToMCRichTrackAlg,
                            Rich__MC__MCRichHitToMCRichOpPhotAlg)
from Configurables import ( GenMonitorAlg, MuonHitChecker, MCTruthMonitor,
                            VeloGaussMoni, MCHitMonitor, MCCaloMonitor,
                            DumpHepMC )
from Configurables import ( PackMCParticle, PackMCVertex,
                            UnpackMCParticle, UnpackMCVertex,
                            CompareMCParticle, CompareMCVertex )

## @class Gauss
#  Configurable for Gauss application
#  @author Gloria Corti <Gloria.Corti@cern.ch>
#  @date   2009-07-13

class Gauss(LHCbConfigurableUser):


    ## Possible used Configurables
    __used_configurables__ = [ LHCbApp ]

    ## Steering options
    __slots__ = {
        "Histograms"        : "Default"
       ,"DatasetName"       : "Gauss"
       ,"SpilloverPaths"    : []
       ,"PhysicsList"       : "LHEP"
       ,"GenStandAlone"     : False
       ,"BeamMomentum"      : 5.0*SystemOfUnits.TeV
       ,"BeamCrossingAngle" : 0.329*SystemOfUnits.mrad
       ,"BeamEmittance"     : 0.704*(10**(-9))*SystemOfUnits.rad*SystemOfUnits.m
       ,"BeamBetaStar"      : 2.0*SystemOfUnits.m
       ,"InteractionSize"   : [ 0.027*SystemOfUnits.mm, 0.027*SystemOfUnits.mm,
                                3.82*SystemOfUnits.cm ]
       ,"BeamSize"          : [ 0.038*SystemOfUnits.mm, 0.038*SystemOfUnits.mm ]
       ,"CrossingRate"      : 11.245*SystemOfUnits.kilohertz
       ,"Luminosity"        : 0.116*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
       ,"TotalCrossSection" : 97.2*SystemOfUnits.millibarn
      }
    
    _propertyDocDct = { 
        'Histograms'     : """ Type of histograms: ['None','Default'] """
       ,'DatasetName'    : """ String used to build output file names """
       ,'SpilloverPaths' : """ Spillover paths to fill: [] means no spillover, otherwise put ['Next', 'Prev', 'PrevPrev'] """
       ,'PhysicsList'    : """ Name of physics list to be passed ['LHEP','QGSP'] """
       ,'GenStandAlone'  : """ Flag to indicate that only generator phase is run"""
       }

    KnownHistOptions = ["","None","Default"]

    ##
    ## Helper functions for spill-over
    def slot_( self, slot ):
        if slot != '':
            return slot + '/'
        return slot

    ##
    def setTrackersHitsProperties( self, alg , det , slot , dd ):
        alg.MCHitsLocation = '/Event/' + self.slot_(slot) + 'MC/' + det + '/Hits'
        if det == 'PuVeto':
            det = 'VeloPu'
        alg.CollectionName = det + 'SDet/Hits'
        alg.Detectors = ['/dd/Structure/LHCb/'+dd]

    ##
    def tapeLocation( self, slot , root , item ):
        return '/Event/' + self.slot_(slot) + root + '/' + item + '#1'

    ##
    def evtMax(self):
        return LHCbApp().evtMax()

    ##
    def eventType(self):
        evtType = ''
        if Generation("Generation").isPropertySet("EventType"):
            evtType = str( Generation("Generation").EventType )
        return evtType

    ##
    ## Function to definewhat to write on output
    ## takes as input list of spill over slots ('' for main event)
    def setOutputContent( self, SpillOverSlots ):

        tape = OutputStream("GaussTape", Preload=False, OutputLevel=3 )

        # output content
        for slot in SpillOverSlots:
            tape.ItemList += [ self.tapeLocation( slot, 'Gen', 'Header' ) ]
            if not self.getProp("GenStandAlone"):
                tape.ItemList += [ self.tapeLocation( slot, 'MC', 'Header' ) ]
                
            generatorList = [ self.tapeLocation( slot, 'Gen', 'Collisions' ),
                              self.tapeLocation( slot, 'Gen', 'HepMCEvents' ) ]
            if slot != '':
                tape.OptItemList += generatorList
            else:
                tape.ItemList += generatorList

            if self.getProp("GenStandAlone"):
                continue
                
            mcTruthList = [ self.tapeLocation( slot, 'MC', 'Particles' ),
                            self.tapeLocation( slot, 'MC', 'Vertices' ) ]
            mcTruthPackedList = [ self.tapeLocation( slot, 'pSim', 'MCParticles' ),
                                 self.tapeLocation( slot, 'pSim', 'MCVertices' ) ]
            simulationList = [ self.tapeLocation( slot, 'MC', 'Velo/Hits' ),
                               self.tapeLocation( slot, 'MC', 'PuVeto/Hits' ), 
                               self.tapeLocation( slot, 'MC', 'TT/Hits' ),
                               self.tapeLocation( slot, 'MC', 'IT/Hits' ),
                               self.tapeLocation( slot, 'MC', 'OT/Hits' ),
                               self.tapeLocation( slot, 'MC', 'Rich/Hits' ),
                               self.tapeLocation( slot, 'MC', 'Spd/Hits' ),
                               self.tapeLocation( slot, 'MC', 'Prs/Hits' ),
                               self.tapeLocation( slot, 'MC', 'Ecal/Hits' ),
                               self.tapeLocation( slot, 'MC', 'Hcal/Hits' ),
                               self.tapeLocation( slot, 'MC', 'Muon/Hits' ) ]

            if slot != '':
                tape.OptItemList += mcTruthList
                tape.OptItemList += simulationList
            else:
                tape.ItemList += mcTruthPackedList
                tape.ItemList += simulationList

            tape.OptItemList += [ self.tapeLocation( slot, 'MC', 'Rich/OpticalPhotons' ),
                                  self.tapeLocation( slot, 'MC', 'Rich/Tracks' ), 
                                  self.tapeLocation( slot, 'MC', 'Rich/Segments' ),
                                  self.tapeLocation( slot, 'Link/MC', 'Particles2MCRichTracks' ),
                                  self.tapeLocation( slot, 'Link/MC', 'Rich/Hits2MCRichOpticalPhotons' ) ]



    ## end of output function


    ##
    ## Functions to configuration various services that are used
    ##
    def configureRndmEngine( self ):

        # Random number service
        from Configurables import HepRndm__Engine_CLHEP__RanluxEngine_    
        rndmSvc = RndmGenSvc()
        engine = HepRndm__Engine_CLHEP__RanluxEngine_("RndmGenSvc.Engine")
        engine.SetSingleton = True


    def configureInput(self):

        # No events are read as input (this is not true if gen phase is
        # switched off
        ApplicationMgr().EvtSel = 'NONE'
        # Transient store setup
        EventDataSvc().ForceLeaves = True
        # May be needed by some options
        importOptions("$STDOPTS/PreloadUnits.opts")

        #self.setOtherProps(LHCbApp(),["EvtMax"])
        
    ##
    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        import time
        outputName = self.getProp("DatasetName")
        if self.eventType() != "":
            if outputName != "": outputName += '-'
            outputName += self.eventType()
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        if outputName == "": outputName = 'Gauss'
        idFile = str(time.localtime().tm_year)
        if time.localtime().tm_mon < 10:
            idFile += '0'
        idFile += str(time.localtime().tm_mon)
        if time.localtime().tm_mday < 10:
            idFile += '0'
        idFile += str(time.localtime().tm_mday)
        outputName += '-' + idFile
        return outputName


    ##
    def defineOutput( self, SpillOverSlots ):
        """
        Set up output stream according to phase processed and spill-over slots
        """
        # and in the future extended or reduced DIGI
        
        # POOL persistency 
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        # Objects to be written on output file
        self.setOutputContent( SpillOverSlots )

        #
        writerName = "GaussTape"
        simWriter = OutputStream( writerName, Preload=False )
        if not simWriter.isPropertySet( "Output" ):
            simWriter.Output = "DATAFILE='PFN:" + self.outputName() + ".sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"
        simWriter.RequireAlgs.append( 'GaussSequencer' )

        ApplicationMgr().OutStream = [ simWriter ]
        FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]



    def defineMonitors( self ):

        # get all defined monitors
        # monitors = self.getProp("Monitors") + LHCbApp().getProp("Monitors")
        # Currently no Gauss specific monitors, so pass them all to LHCbApp
        # LHCbApp().setProp("Monitors", monitors)

        # Use TimingAuditor for timing
        # suppress printout from SequencerTimerTool ??
        from Configurables import ApplicationMgr, AuditorSvc, SequencerTimerTool
        ApplicationMgr().ExtSvc += [ 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ] 
        #SequencerTimerTool().OutputLevel = WARNING

        # Set printout level and longer algorithm" identifier in printout
        MessageSvc().OutputLevel = INFO
        #ToolSvc.EvtGenTool.OutputLevel = 4 is it still necessart to reduce print?
        MessageSvc().setWarning.append( 'XmlGenericCnv' )
        MessageSvc().Format = '% F%24W%S%7W%R%T %0W%M'


    ##
    def saveHistos(self):
        """
        Set up histogram service and file name unless done in job
        """

        # ROOT persistency for histograms
        importOptions('$STDOPTS/RootHist.opts')
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

        histOpt = self.getProp("Histograms").capitalize()
        if histOpt not in self.KnownHistOptions:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)
            # HistogramPersistency needed to read in histogram for calorimeter
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        # Use a default histogram file name if not already set
        #   eventually do what is in example Gauss-Job.py with date and event type
        if not HistogramPersistencySvc().isPropertySet( "OutputFile" ):
            histosName = self.getProp("DatasetName")
            histosName = self.outputName() + '-histos.root'
            HistogramPersistencySvc().OutputFile = histosName
            

    ##
    ##
    def configureGen( self, SpillOverSlots ):
        """
        Set up the generator execution sequence and its sub-phases
        """
        
##         if "Gen" not in self.getProp("MainSequence") :
##             log.warning("No generator phase. Need input file")
##             return

        if self.evtMax() <= 0:
            raise RuntimeError("Generating events but selected '%s' events. Use LHCbApp().EvtMax "%self.evtMax())

        gaussGeneratorSeq = GaudiSequencer( "Generator", IgnoreFilterPassed = True )
        gaussSeq = GaudiSequencer("GaussSequencer")
        gaussSeq.Members += [ gaussGeneratorSeq ]

        EvtGenDecay().DecayFile = "$DECFILESROOT/dkfiles/DECAY.DEC"

        for slot in SpillOverSlots:
            genSequence = GaudiSequencer("GeneratorSlot"+slot )
            gaussGeneratorSeq.Members += [ genSequence ]

            TESNode = "/Event/"+self.slot_(slot)
            genInit = GenInit("GaussGen"+slot,
                              MCHeader = TESNode+"Gen/Header")
            if slot != '':
                genInitT0 = GenInit("GaussGen")
                if genInitT0.isPropertySet("RunNumber"):
                    genInit.RunNumber = genInitT0.RunNumber
                if genInitT0.isPropertySet("FirstEventNumber"):
                    genInit.FirstEventNumber = genInitT0.FirstEventNumber


            genProc = Generation("Generation"+slot, 
                                 GenHeaderLocation = TESNode+"Gen/Header" ,
                                 HepMCEventLocation = TESNode+"Gen/HepMCEvents" , 
                                 GenCollisionLocation = TESNode+"Gen/Collisions" )
            if slot != '':
                genProc.PileUpTool = 'FixedLuminosityForSpillOver' 

            genSequence.Members += [ genInit , genProc ]


    ##
    ##
    def configureMoni( self, SpillOverSlots ):

        histOpt = self.getProp("Histograms").capitalize()
        if ( histOpt == 'None') :
            log.warning("No histograms produced")
            return
        
        # Monitors for the generator:
        for slot in SpillOverSlots:

            genSequence = GaudiSequencer("GeneratorSlot" + slot )
            genMoniSeq = GaudiSequencer("GenMonitor" + slot )
            genSequence.Members += [ genMoniSeq ]  

            TESLocation = "/Event/"+self.slot_(slot)+"Gen/HepMCEvents"
            genMoniSeq.Members += [
                GenMonitorAlg( "GenMonitorAlg"+slot, HistoProduce=True,
                               Input = TESLocation ) ]
            if histOpt == 'Debug':
                genMoniSeq.Members += [ DumpHepMC( "DumpHepMC"+slot,
                                                   OutputLevel=1,
                                                   Addresses = [TESLocation] ) ]


        # Monitors for the MCTruth
        for slot in SpillOverSlots:

            TESNode = "/Event/"+self.slot_(slot)+"MC/"
            simSequence = GaudiSequencer( "SimulationSlot" + slot )
            simMoniSeq = GaudiSequencer( "SimMonitor" + slot, 
                                         RequireObjects = [ TESNode+"Particles"] )
            simSequence.Members += [ simMoniSeq ]

            simMoniSeq.Members += [
                GiGaGetEventAlg( "GiGaGetEventAlg"+slot,
                                 Vertices = "",
                                 Particles = TESNode+"Particles" ), 
                MCTruthMonitor( "MCTruthMonitor"+slot,
                                MCParticles = TESNode+"Particles" ,
                                MCVertices  = TESNode+"Vertices" ) ]


        # Monitors for the various detectors
        for slot in SpillOverSlots:

            simMoniSeq = GaudiSequencer( "SimMonitor" + slot ) 
            # can switch off detectors, or rather switch them on (see options
            # of algorithm)
            checkHits = GiGaGetHitsAlg( "GiGaGetHitsAlg" + slot )
            simMoniSeq.Members += [ checkHits ]

            TESNode = "/Event/"+self.slot_(slot)+"MC/"

            # Monitors for the detectors
            if histOpt == 'Debug':
                checkHits.OutputLevel = DEBUG

            checkHits.VeloHits   = TESNode + 'Velo/Hits'
            checkHits.PuVetoHits = TESNode + 'PuVeto/Hits'
            checkHits.TTHits     = TESNode + 'TT/Hits'
            checkHits.ITHits     = TESNode + 'IT/Hits'
            checkHits.OTHits     = TESNode + 'OT/Hits'
            checkHits.CaloHits   = [ TESNode + 'Spd/Hits',
                                     TESNode + 'Prs/Hits', 
                                     TESNode + 'Ecal/Hits',
                                     TESNode + 'Hcal/Hits' ]
            checkHits.MuonHits   =  TESNode + 'Muon/Hits' 
            checkHits.RichHits   =  TESNode + 'Rich/Hits' 
            checkHits.RichOpticalPhotons = TESNode +'Rich/OpticalPhotons'
            checkHits.RichSegments       = TESNode + 'Rich/Segments'
            checkHits.RichTracks         = TESNode + 'Rich/Tracks'

            detMoniSeq = GaudiSequencer( "DetectorsMonitor" + slot ) 
            simMoniSeq.Members += [ detMoniSeq ]

            detMoniSeq.Members += [
                VeloGaussMoni( "VeloGaussMoni" + slot,
                               VeloMCHits   = TESNode + 'Velo/Hits',
                               PuVetoMCHits = TESNode + 'PuVeto/Hits' ) ]

            ttHitMoni = MCHitMonitor( "TTHitMonitor" + slot )
            detMoniSeq.Members += [ ttHitMoni ]
            ttHitMoni.mcPathString = TESNode + "TT/Hits"
            ttHitMoni.zStations = [ 2350.*SystemOfUnits.mm,
                                    2620.*SystemOfUnits.mm ]
            ttHitMoni.xMax = 150.*SystemOfUnits.cm 
            ttHitMoni.yMax = 150.*SystemOfUnits.cm 

            itHitMoni = MCHitMonitor( "ITHitMonitor" + slot )
            detMoniSeq.Members += [ itHitMoni ]
            itHitMoni.mcPathString = TESNode + "IT/Hits"
            itHitMoni.zStations = [ 7780.0*SystemOfUnits.mm,
                                    8460.0*SystemOfUnits.mm,
                                    9115.0*SystemOfUnits.mm ]
            itHitMoni.xMax = 100.*SystemOfUnits.cm 
            itHitMoni.yMax = 100.*SystemOfUnits.cm 

            otHitMoni = MCHitMonitor( "OTHitMonitor" + slot )
            detMoniSeq.Members += [ otHitMoni ]
            otHitMoni.mcPathString = TESNode + "OT/Hits"
            otHitMoni.zStations = [ 7938.0*SystemOfUnits.mm,
                                    8625.0*SystemOfUnits.mm,
                                    9315.0*SystemOfUnits.mm ]

            spdmoni = MCCaloMonitor( "SpdMonitor" + slot,
                                     OutputLevel = 4,
                                     Detector = "Spd",
                                     Regions = True,
                                     MaximumEnergy = 10.*SystemOfUnits.MeV,
                                     Threshold = 1.5*SystemOfUnits.MeV,
                                     MCHits = TESNode+"Spd/Hits",
                                     MCParticles = TESNode+"Particles", 
                                     Slot = self.slot_(slot) ) 
            prsmoni = MCCaloMonitor( "PrsMonitor" + slot,
                                     OutputLevel = 4,
                                     Detector = "Prs", Regions = True,
                                     MaximumEnergy = 10.*SystemOfUnits.MeV,
                                     Threshold = 1.5*SystemOfUnits.MeV, 
                                     MCHits = TESNode + "Prs/Hits",
                                     MCParticles = TESNode + "Particles", 
                                     Slot = self.slot_(slot) ) 
            ecalmoni = MCCaloMonitor( "EcalMonitor" + slot ,
                                      OutputLevel = 4,
                                      Detector = "Ecal",
                                      Regions = True,
                                      MaximumEnergy = 1000.*SystemOfUnits.MeV,
                                      Threshold = 10.*SystemOfUnits.MeV,
                                      MCHits = TESNode + "Ecal/Hits",
                                      MCParticles = TESNode + "Particles",
                                      Slot = self.slot_(slot) ) 
            hcalmoni = MCCaloMonitor( "HcalMonitor" + slot,
                                      OutputLevel = 4,
                                      Detector = "Hcal",
                                      Regions = True,
                                      MaximumEnergy = 1000.*SystemOfUnits.MeV,
                                      Threshold = 5.*SystemOfUnits.MeV, 
                                      MCHits = TESNode + "Hcal/Hits",
                                      MCParticles = TESNode + "Particles",
                                      Slot = self.slot_(slot) ) 
            detMoniSeq.Members += [ spdmoni , prsmoni , ecalmoni , hcalmoni ]


            detMoniSeq.Members += [
                MuonHitChecker( "MuonHitChecker" + slot,
                                FullDetail = True,
                                MCHeader = TESNode + "Header",
                                MuonHits = TESNode + "Muon/Hits" ) ]
            from Configurables import MuonMultipleScatteringChecker
            detMoniSeq.Members += [
                MuonMultipleScatteringChecker( "MuonMultipleScatteringChecker"+ slot )]

        if histOpt == 'Expert':
            # For the moment do nothing
            log.Warning("Not yet implemented")


        importOptions("$GAUSSRICHROOT/options/RichAnalysis.opts")

        # Check packing and unpacking only for 'main' event
        simMoniSeq = GaudiSequencer( "SimMonitor" )
        testMCV = UnpackMCVertex( "TestMCVertex",
                                  OutputName = "MC/VerticesTest" )
        testMCP = UnpackMCParticle( "TestMCParticle",
                                    OutputName = "MC/ParticlesTest" )
        compareMCV = CompareMCVertex( "CompareMCVertex",
                                      TestName = "MC/VerticesTest" )
        compareMCP = CompareMCParticle( "CompareMCParticle",
                                        TestName = "MC/ParticlesTest" )
        simMoniSeq.Members += [ testMCV, testMCP,
                                compareMCV, compareMCP ]


    ##
    ##
    def defineGeo( self ):
        """
        Set up the geometry to be simulated
        """

        geo = GiGaInputStream( "Geo",
                               ExecuteOnce = True,
                               ConversionSvcName = "GiGaGeo",
                               DataProviderSvcName  = "DetectorDataSvc" )
        gaussSimulationSeq = GaudiSequencer("Simulation")
        gaussSimulationSeq.Members += [ geo ]

    ##     # To set size of World/Universe i.e. the main mother volume (made of Air
    ##     # and in which any LHCb detector is positioned)
    ##     # Now it is half that is inconsistent with "XmlDDDB"
    ##     # GiGaGeo.XsizeOfWorldVolume = 50.0*m;
    ##     # GiGaGeo.YsizeOfWorldVolume = 50.0*m;
    ##     # GiGaGeo.ZsizeOfWorldVolume = 50.0*m; 

        # Detector geometry to simulate
        geo.StreamItems += ["/dd/Structure/LHCb/BeforeMagnetRegion/Velo"]
        geo.StreamItems += ["/dd/Structure/LHCb/BeforeMagnetRegion/Velo2Rich1"]
        geo.StreamItems += ["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1"]
        geo.StreamItems += ["/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1Surfaces"]
        geo.StreamItems += ["/dd/Structure/LHCb/BeforeMagnetRegion/TT"]

        geo.StreamItems += ["/dd/Structure/LHCb/MagnetRegion/PipeInMagnet"]
        geo.StreamItems += ["/dd/Structure/LHCb/MagnetRegion/PipeSupportsInMagnet"]
        geo.StreamItems += ["/dd/Structure/LHCb/MagnetRegion/Magnet"]
        geo.StreamItems += ["/dd/Structure/LHCb/MagnetRegion/BcmDown"]

        geo.StreamItems += ["/dd/Structure/LHCb/AfterMagnetRegion/T"]
        geo.StreamItems += ["/dd/Structure/LHCb/AfterMagnetRegion/Rich2"]
        geo.StreamItems += ["/dd/Geometry/AfterMagnetRegion/Rich2/Rich2Surfaces"]
        geo.StreamItems += ["/dd/Geometry/BeforeMagnetRegion/Rich1/RichHPDSurfaces"]
        geo.StreamItems += ["/dd/Structure/LHCb/AfterMagnetRegion/PipeAfterT"]
        geo.StreamItems += ["/dd/Structure/LHCb/AfterMagnetRegion/PipeSupportsAfterMagnet"]

        geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Spd"]
        geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Converter"]
        geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Prs"]
        geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Ecal"]
        geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Hcal"]
        geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Muon"]
        geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/PipeDownstream"]
        geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/PipeSupportsDownstream"]
        geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/PipeBakeoutDownstream"]

        importOptions("$GAUSSCALOROOT/options/Calo.opts")

        # Use information from SIMCOND and GeometryInfo
        giGaGeo = GiGaGeo()
        giGaGeo.UseAlignment      = True
        giGaGeo.AlignAllDetectors = True
        importOptions('$GAUSSOPTS/SimVeloGeometry.py')  # -- To misalign VELO

    ##     #if "VELO" in geoDets: configureGeoVELO( )
    ##     #if "TT  " in geoDets: configureGeoTT( )
    ##     #if "T" or "IT  " or "OT" in geoDets: configureGeoT( geoDets )
    ##     #if "RICH1" in geoDets: configureGeoRICH( )
    ##     #if "CALO" or "SPD&PRS" or "ECAL" or "HCAL" in geoDets:configureGeoCALO( geoDets )
    ##     #if "MUON" in geoDets: etc.

        ## Set up the magnetic field
        GiGaGeo().FieldManager           = "GiGaFieldMgr/FieldMgr"
        GiGaGeo().addTool( GiGaFieldMgr("FieldMgr"), name="FieldMgr" )
        GiGaGeo().FieldMgr.Stepper       = "ClassicalRK4"
        GiGaGeo().FieldMgr.Global        = True
        GiGaGeo().FieldMgr.MagneticField = "GiGaMagFieldGlobal/LHCbField"
        GiGaGeo().FieldMgr.addTool( GiGaMagFieldGlobal("LHCbField"), name="LHCbField" ) 
        GiGaGeo().FieldMgr.LHCbField.MagneticFieldService = "MagneticFieldSvc"


    ##
    ##
    def configureGiGa(self):
         """
         Set up the configuration for the G4 settings: physics list, cuts and actions
         """

         physList = self.getProp("PhysicsList")
         ## Physics list and cuts
         giga = GiGa()
         giga.addTool( GiGaPhysListModular("ModularPL") , name="ModularPL" ) 
         giga.PhysicsList = "GiGaPhysListModular/ModularPL"
         giga.ModularPL.CutForElectron = 10000.0 * SystemOfUnits.m
         giga.ModularPL.CutForPositron = 5.0 * SystemOfUnits.mm 
         giga.ModularPL.CutForGamma    = 10.0 * SystemOfUnits.mm

         ## Check here that the physics list is allowed
         physListOpts = "$GAUSSOPTS/PhysList-"+physList+".opts"
         importOptions( physListOpts )

         ## Mandatory G4 Run action
         giga.addTool( GiGaRunActionSequence("RunSeq") , name="RunSeq" )
         giga.RunAction = "GiGaRunActionSequence/RunSeq"
         giga.RunSeq.addTool( TrCutsRunAction("TrCuts") , name = "TrCuts" )
         giga.RunSeq.addTool( GiGaRunActionCommand("RunCommand") , name = "RunCommand" ) 
         giga.RunSeq.Members += [ "TrCutsRunAction/TrCuts" ] 
         giga.RunSeq.Members += [ "GiGaRunActionCommand/RunCommand" ]
         giga.RunSeq.RunCommand.BeginOfRunCommands = [
             "/tracking/verbose 0",
             "/tracking/storeTrajectory  1",
             "/process/eLoss/verbose -1" ]

         giga.EventAction = "GiGaEventActionSequence/EventSeq"
         giga.addTool( GiGaEventActionSequence("EventSeq") , name="EventSeq" ) 
         giga.EventSeq.Members += [ "GaussEventActionHepMC/HepMCEvent" ]

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

         # switch off when rich reduced
         giga.ModularPL.addTool( GiGaPhysConstructorOp,
                                 name="GiGaPhysConstructorOp" )
         giga.ModularPL.GiGaPhysConstructorOp.RichActivateRichPhysicsProcVerboseTag = True
         giga.StepSeq.Members += [ "RichG4StepAnalysis4/RichStepAgelExit" ]
         giga.StepSeq.Members += [ "RichG4StepAnalysis5/RichStepMirrorRefl" ]


    ##
    ##
    def configureSim( self, SpillOverSlots ):

        """
        Set up the simulation sequence
        """
        
        if self.getProp("GenStandAlone"):
            log.warning("No simulation phase.")
            return
        
        ApplicationMgr().ExtSvc += [ "GiGa" ]
        EventPersistencySvc().CnvServices += [ "GiGaKine" ]

        gaussSimulationSeq = GaudiSequencer( "Simulation" )
        gaussSeq = GaudiSequencer("GaussSequencer")
        gaussSeq.Members += [ gaussSimulationSeq ]

        gigaStore = GiGaDataStoreAlgorithm( "GiGaStore" )
        gigaStore.ConversionServices = [ "GiGaKine" ]
        gaussSimulationSeq.Members += [ gigaStore ] 

        #geoDets = [ "VELO" ]
        #defineGeo( geoDets )
        self.defineGeo( )

        self.configureGiGa()

        for slot in SpillOverSlots:

            TESNode = "/Event/"+self.slot_(slot)
            simSequence = GaudiSequencer( "SimulationSlot" + slot )
            gaussSimulationSeq.Members += [ simSequence ]

            simSlotSeq = GaudiSequencer( "SimMake" + slot ,
                                         RequireObjects = [ TESNode + "Gen/HepMCEvents" ] )
            simSequence.Members += [ SimInit( "GaussSim" + slot,
                                              GenHeader = TESNode + "Gen/Header" ,
                                              MCHeader = TESNode + "MC/Header" ) ,
                                     simSlotSeq ]

            genToSim = GenerationToSimulation( "GenerationToSimulation" + slot,
                                               LookForUnknownParticles = True, 
                                               HepMCEventLocation = TESNode + "Gen/HepMCEvents" ,
                                               Particles = TESNode + "MC/Particles" ,
                                               Vertices = TESNode+"MC/Vertices" ,
                                               MCHeader = TESNode+"MC/Header" ) 
            simSlotSeq.Members += [ genToSim ]
            simSlotSeq.Members += [ GiGaFlushAlgorithm( "GiGaFlushAlgorithm"+slot ) ]
            simSlotSeq.Members += [ GiGaCheckEventStatus( "GiGaCheckEventStatus"+slot ) ]
            simSlotSeq.Members += [ SimulationToMCTruth( "SimulationToMCTruth"+slot  ,
                                                         Particles = TESNode+"MC/Particles",
                                                         Vertices = TESNode+"MC/Vertices" ,
                                                         MCHeader = TESNode+"MC/Header" ) ]

            ## Detectors hits
            TESNode = TESNode + "MC/"
            detHits = GaudiSequencer( "DetectorsHits" + slot ) 
            detHits.MeasureTime = True 
            simSlotSeq.Members += [ detHits ]

            veloHits   = GetTrackerHitsAlg( "GetVeloHits" + slot,
                                            MCParticles = TESNode + "Particles" )
            puvetoHits = GetTrackerHitsAlg( "GetPuVetoHits" + slot,
                                            MCParticles = TESNode + "Particles" ) 
            self.setTrackersHitsProperties( veloHits, 'Velo', slot, 'BeforeMagnetRegion/Velo' )         
            self.setTrackersHitsProperties( puvetoHits, 'PuVeto', slot, 'BeforeMagnetRegion/Velo' )
            detHits.Members += [ veloHits, puvetoHits ]

            ttHits     = GetTrackerHitsAlg( "GetTTHits" + slot,
                                            MCParticles = TESNode + "Particles" ) 
            self.setTrackersHitsProperties( ttHits, 'TT', slot, 'BeforeMagnetRegion/TT' ) 
            detHits.Members += [ ttHits ]

            itHits     = GetTrackerHitsAlg( "GetITHits" + slot,
                                            MCParticles = TESNode + "Particles" ) 
            self.setTrackersHitsProperties( itHits, 'IT', slot, 'AfterMagnetRegion/T/IT' ) 
            detHits.Members += [ itHits ]

            otHits     = GetTrackerHitsAlg( "GetOTHits" + slot,
                                            MCParticles = TESNode + "Particles" ) 
            self.setTrackersHitsProperties( otHits, 'OT', slot, 'AfterMagnetRegion/T/OT' ) 
            detHits.Members += [ otHits ]

            muonHits   = GetTrackerHitsAlg( "GetMuonHits" + slot,
                                            MCParticles = TESNode + "Particles" ) 
            self.setTrackersHitsProperties( muonHits, 'Muon', slot, 'DownstreamRegion/Muon' ) 
            detHits.Members += [ muonHits ]

            spdHits    = GetCaloHitsAlg( "GetSpdHits" + slot,
                                         MCParticles = TESNode + "Particles" ) 
            spdHits.MCHitsLocation = TESNode + "Spd/Hits"
            spdHits.CollectionName = "SpdHits"
            prsHits    = GetCaloHitsAlg( "GetPrsHits" + slot,
                                         MCParticles = TESNode + "Particles" ) 
            prsHits.MCHitsLocation = TESNode + "Prs/Hits"
            prsHits.CollectionName = "PrsHits"
            ecalHits   = GetCaloHitsAlg( "GetEcalHits" + slot,
                                         MCParticles = TESNode + "Particles" ) 
            ecalHits.MCHitsLocation = TESNode + "Ecal/Hits"
            ecalHits.CollectionName = "EcalHits"
            hcalHits   = GetCaloHitsAlg( "GetHcalHits" + slot,
                                         MCParticles = TESNode + "Particles" ) 
            hcalHits.MCHitsLocation = TESNode + "Hcal/Hits"
            hcalHits.CollectionName = "HcalHits"
            detHits.Members += [ spdHits, prsHits, ecalHits, hcalHits ]

            richHits = GetMCRichHitsAlg( "GetRichHits" + slot,
                                         MCParticles = TESNode + "Particles" ,
                                         MCRichHitsLocation = TESNode + "Rich/Hits") 
            richHitsSeq = GaudiSequencer( "RichHits" + slot )
            detHits.Members += [ richHitsSeq ]
            richHitsSeq.Members += [ richHits ]

            TESRich = TESNode + 'Rich/'
            richPhotons  = GetMCRichOpticalPhotonsAlg("GetRichPhotons" + slot,
                                                      MCRichOpticalPhotonsLocation = TESRich+"OpticalPhotons",
                                                      MCRichHits = TESRich+"Hits" )
            richSegments = GetMCRichSegmentsAlg("GetRichSegments" + slot,
                                                MCRichOpticalPhotonsLocation = TESRich+"OpticalPhotons",
                                                MCRichHitsLocation = TESRich+"Hits", 
                                                MCRichSegmentsLocation = TESRich + "Segments" ) 
            richTracks = GetMCRichTracksAlg("GetRichTracks" + slot,
                                            MCRichTracksLocation = TESRich + "Tracks",
                                            MCParticles = TESNode+"Particles",
                                            MCRichSegments = TESRich+"Segments" )


            richHitsSeq.Members += [ richPhotons, richSegments, richTracks ] 
            richHitsSeq.Members += [ Rich__MC__MCPartToMCRichTrackAlg("MCPartToMCRichTrack"+slot) ]
            richHitsSeq.Members += [ Rich__MC__MCRichHitToMCRichOpPhotAlg("MCRichHitToMCRichOpPhot"+slot) ]  


        # Packing only for main event
        simSlotSeq = GaudiSequencer( "SimMake" )
        packMCP = PackMCParticle( "PackMCParticle" )
        packMCV = PackMCVertex( "PackMCVertex" )
        simSlotSeq.Members += [ packMCP, packMCV ]


    ##########################################################################
    ## Functions to set beam conditions and propagate them
    ##

    #-- Function to set mean number of interactions
    def setInteractionFreq(self, SpillOverSlots):
        
        from Configurables import ( FixedLuminosity,
                                    FixedLuminosityForSpillOver,
                                    FixedLuminosityForRareProcess,
                                    FixedNInteractions )
    
        lumiPerBunch = self.getProp("Luminosity")
        crossingRate = self.getProp("CrossingRate")
        totCrossSection = self.getProp("TotalCrossSection")

        # For standard fixed luminosity in main event
        gen_t0 = Generation("Generation")
    
        gen_t0.addTool(FixedLuminosity,name="FixedLuminosity")
        gen_t0.FixedLuminosity.Luminosity = lumiPerBunch
        gen_t0.FixedLuminosity.CrossingRate = crossingRate
        gen_t0.FixedLuminosity.TotalXSection = totCrossSection

        # the same for rare processes where a different luminosity tool is used
        gen_t0.addTool(FixedLuminosityForRareProcess,
                       name="FixedLuminosityForRareProcess")
        gen_t0.FixedLuminosityForRareProcess.Luminosity = lumiPerBunch
        gen_t0.FixedLuminosityForRareProcess.CrossingRate = crossingRate
        gen_t0.FixedLuminosityForRareProcess.TotalXSection = totCrossSection
        
        # the following is for beam gas events, the values are just to give the
        # nominal beam conditions in the data but 1 single interaction is 
        # forced selecting the appropriate pileup tool in the eventtype
        gen_t0.addTool(FixedNInteractions,name="FixedNInteractions")
        gen_t0.FixedNInteractions.NInteractions = 1
        gen_t0.FixedNInteractions.Luminosity = lumiPerBunch
        gen_t0.FixedNInteractions.CrossingRate = crossingRate
        gen_t0.FixedNInteractions.TotalXSection = totCrossSection
    
        # and for SpillOver where a dedicated tool is used
        for slot in SpillOverSlots:
            gen = Generation("Generation"+slot)
            gen.addTool(FixedLuminosityForSpillOver,
                        name="FixedLuminosityForSpillOver")
            gen.FixedLuminosityForSpillOver.Luminosity = lumiPerBunch
            gen.FixedLuminosityForSpillOver.CrossingRate = crossingRate
            gen.FixedLuminosityForSpillOver.TotalXSection = totCrossSection


    #--Set the luminous region for colliding beams and beam gas and configure
    #--the corresponding vertex smearing tools, the choice of the tools is done
    #--by the event type
    def setInteractionSize( self, CrossingSlots ):

        from Configurables import ( BeamSpotSmearVertex )

        sigmaX, sigmaY, sigmaZ = self.getProp("InteractionSize")

        for slot in CrossingSlots:
            gen = Generation("Generation"+slot)
            gen.addTool(BeamSpotSmearVertex,name="BeamSpotSmearVertex")
            gen.BeamSpotSmearVertex.SigmaX = sigmaX
            gen.BeamSpotSmearVertex.SigmaY = sigmaY
            gen.BeamSpotSmearVertex.SigmaZ = sigmaZ
        
    #--
    def setBeamSize( self, CrossingSlots ):

        from Configurables import ( FlatZSmearVertex )

        sigmaX, sigmaY = self.getProp("BeamSize")
        for slot in CrossingSlots:
            gen = Generation("Generation"+slot)
            gen.addTool(FlatZSmearVertex,name="FlatZSmearVertex")
            gen.FlatZSmearVertex.SigmaX = sigmaX
            gen.FlatZSmearVertex.SigmaY = sigmaY
        
    
    #--Set the energy of the beam,
    #--the half effective crossing angle (in LHCb coordinate system),
    #--beta* and emittance
    #--and configure the colliding beam tool for all type of events in
    #--pp collisions.
    #--For beam gas events (with hijing) only the energy of the beams is set
    def setBeamParameters( self, CrossingSlots ):

        from Configurables import ( MinimumBias, Inclusive, SignalPlain,
                                    SignalRepeatedHadronization,
                                    SignalForcedFragmentation, Special,
                                    StandAloneDecayTool ) 
        from Configurables import ( PythiaProduction, BcVegPyProduction,
                                    HijingProduction ) 
        from Configurables import ( CollidingBeams ) 

        #
        beamMom   = self.getProp("BeamMomentum")
        angle     = self.getProp("BeamCrossingAngle")
        emittance = self.getProp("BeamEmittance")
        betaStar  = self.getProp("BeamBetaStar")

        # for Minimum bias, including those of spill-over events
        for slot in CrossingSlots:
            gen = Generation("Generation"+slot)        
            gen.addTool(MinimumBias,name="MinimumBias")
            gen.MinimumBias.addTool(PythiaProduction,name="PythiaProduction")
            gen.MinimumBias.PythiaProduction.addTool(CollidingBeams,
                                                     name="CollidingBeams")
            gen.MinimumBias.PythiaProduction.CollidingBeams.BeamMomentum = beamMom
            gen.MinimumBias.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
            gen.MinimumBias.PythiaProduction.CollidingBeams.Emittance = emittance
            gen.MinimumBias.PythiaProduction.CollidingBeams.BetaStar = betaStar

    
        # for Inclusive events (only primary bunch)
        gen_t0 = Generation("Generation")
        gen_t0.addTool(Inclusive,name="Inclusive")
        gen_t0.Inclusive.addTool(PythiaProduction,name="PythiaProduction")
        gen_t0.Inclusive.PythiaProduction.addTool(CollidingBeams,
                                                  name="CollidingBeams")
        gen_t0.Inclusive.PythiaProduction.CollidingBeams.BeamMomentum = beamMom
        gen_t0.Inclusive.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
        gen_t0.Inclusive.PythiaProduction.CollidingBeams.Emittance = emittance
        gen_t0.Inclusive.PythiaProduction.CollidingBeams.BetaStar = betaStar
        # Signal plaine (e.g. J/Psi)
        gen_t0.addTool(SignalPlain,name="SignalPlain")
        gen_t0.SignalPlain.addTool(PythiaProduction,name="PythiaProduction")
        gen_t0.SignalPlain.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
        gen_t0.SignalPlain.PythiaProduction.CollidingBeams.BeamMomentum = beamMom
        gen_t0.SignalPlain.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
        gen_t0.SignalPlain.PythiaProduction.CollidingBeams.Emittance = emittance
        gen_t0.SignalPlain.PythiaProduction.CollidingBeams.BetaStar = betaStar
        # Signal repeated hadronization (e.g. Bd, Bu, Bs, Lambda_b)
        gen_t0.addTool(SignalRepeatedHadronization,
                       name="SignalRepeatedHadronization")
        gen_t0.SignalRepeatedHadronization.addTool(PythiaProduction,
                                                   name="PythiaProduction")
        gen_t0.SignalRepeatedHadronization.PythiaProduction.addTool(CollidingBeams,
                                                                    name="CollidingBeams")
        gen_t0.SignalRepeatedHadronization.PythiaProduction.CollidingBeams.BeamMomentum = beamMom
        gen_t0.SignalRepeatedHadronization.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
        gen_t0.SignalRepeatedHadronization.PythiaProduction.CollidingBeams.Emittance = emittance
        gen_t0.SignalRepeatedHadronization.PythiaProduction.CollidingBeams.BetaStar = betaStar
        # Signal forced fragmentation (e.g. Bc)
        gen_t0.addTool(SignalForcedFragmentation,
                       name="SignalForcedFragmentation")
        gen_t0.SignalForcedFragmentation.addTool(PythiaProduction,
                                                 name="PythiaProduction")
        gen_t0.SignalForcedFragmentation.PythiaProduction.addTool(CollidingBeams,
                                                                  name="CollidingBeams")
        gen_t0.SignalForcedFragmentation.PythiaProduction.CollidingBeams.BeamMomentum = beamMom
        gen_t0.SignalForcedFragmentation.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
        gen_t0.SignalForcedFragmentation.PythiaProduction.CollidingBeams.Emittance = emittance
        gen_t0.SignalForcedFragmentation.PythiaProduction.CollidingBeams.BetaStar = betaStar
        # Special signal (Higgs, top and W/Z with Pythia)
        gen_t0.addTool(Special,name="Special")
        gen_t0.Special.addTool(PythiaProduction,name="PythiaProduction")
        gen_t0.Special.PythiaProduction.addTool(CollidingBeams,
                                                name="CollidingBeams")
        gen_t0.Special.PythiaProduction.CollidingBeams.BeamMomentum = beamMom
        gen_t0.Special.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
        gen_t0.Special.PythiaProduction.CollidingBeams.Emittance = emittance
        gen_t0.Special.PythiaProduction.CollidingBeams.BetaStar = betaStar
        gen_t0.Special.addTool(PythiaProduction,name="MinimumBiasPythiaProduction")
        gen_t0.Special.MinimumBiasPythiaProduction.addTool(CollidingBeams,
                                                           name="CollidingBeams")
        gen_t0.Special.MinimumBiasPythiaProduction.CollidingBeams.BeamMomentum = beamMom
        gen_t0.Special.MinimumBiasPythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
        gen_t0.Special.MinimumBiasPythiaProduction.CollidingBeams.Emittance = emittance
        gen_t0.Special.MinimumBiasPythiaProduction.CollidingBeams.BetaStar = betaStar
        # Special signal  (Bc with BcVegPy)
        from Configurables import BcVegPyProduction
        gen_t0.Special.addTool(BcVegPyProduction,name="BcVegPyProduction")
        gen_t0.Special.BcVegPyProduction.addTool(CollidingBeams,
                                                 name="CollidingBeams")
        gen_t0.Special.BcVegPyProduction.CollidingBeams.BeamMomentum = beamMom
        gen_t0.Special.BcVegPyProduction.CollidingBeams.HorizontalCrossingAngle = angle
        gen_t0.Special.BcVegPyProduction.CollidingBeams.Emittance = emittance
        gen_t0.Special.BcVegPyProduction.CollidingBeams.BetaStar = betaStar
        pInGeV   = beamMom*SystemOfUnits.GeV/SystemOfUnits.TeV
        ecmInGeV = 2*pInGeV
        txtECM = "upcom ecm "+str(ecmInGeV)
        gen_t0.Special.BcVegPyProduction.BcVegPyCommands += [ txtECM ]
        # Only signal events
        gen_t0.addTool(StandAloneDecayTool,name="StandAloneDecayTool")
        gen_t0.StandAloneDecayTool.addTool(PythiaProduction,
                                           name="PythiaProduction")
        gen_t0.StandAloneDecayTool.PythiaProduction.addTool(CollidingBeams,
                                                            name="CollidingBeams")
        gen_t0.StandAloneDecayTool.PythiaProduction.CollidingBeams.BeamMomentum = beamMom
        gen_t0.StandAloneDecayTool.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
        gen_t0.StandAloneDecayTool.PythiaProduction.CollidingBeams.Emittance = emittance
        gen_t0.StandAloneDecayTool.PythiaProduction.CollidingBeams.BetaStar = betaStar

        # For beam gas with Pythia
        gen_t0 = Generation("Generation")
        gen_t0.MinimumBias.PythiaProduction.addTool(FixedTarget,
                                                 name="FixedTarget")
        gen_t0.MinimumBias.PythiaProduction.FixedTarget.BeamMomentum = beamMom
        # or with Hijing
        txtP = "hijinginit efrm "+str(pInGeV)
        gen_t0.MinimumBias.addTool(HijingProduction,name="HijingProduction")
        gen_t0.MinimumBias.HijingProduction.Commands += [ txtP ]
    

    ## end of functions to set beam paramters and propagate them
    ##########################################################################

    ##
    ##
    def configurePhases( self, SpillOverSlots ):
        """
        Set up the top level sequence and its phases
        """

        gaussSeq = GaudiSequencer("GaussSequencer")
        ApplicationMgr().TopAlg = [ gaussSeq ]
##         mainSeq = self.getProp("MainSequence")
##         if len( mainSeq ) == 0:
##             mainSeq = self.DefaultSequence

##         mainSeq = map(lambda ph: ph.capitalize(), mainSeq)
##         self.setProp("MainSequence",mainSeq)
##         for phase in mainSeq:
##             raise RuntimeError("Unknown phase '%s'"%phase)
                
        self.configureGen( SpillOverSlots )
        self.configureSim( SpillOverSlots )
        self.configureMoni( SpillOverSlots ) #(expert or default)
        

    ## Apply the configuration
    def __apply_configuration__(self):
        
        
        GaudiKernel.ProcessJobOptions.PrintOff()

        #defineDB() in Boole and
        # defineGeometry() in Brunel, need the same + random seeds
        self.configureRndmEngine()
        self.configureInput()  #defineEvents() in both Boole and Brunel
        LHCbApp( Simulation = True ) # in Boole? where? 

        #--Define sequences: generator, simulation
        #  each with its init, make, moni
        #  in the sim phase define the geometry to simulate and the settings
        crossingList = [ '' ]
        spillOverList = self.getProp("SpilloverPaths")
        if '' in spillOverList : spillOverList.remove('')
        crossingList += spillOverList
        self.setInteractionFreq( spillOverList )
        self.setInteractionSize( crossingList )
        self.setBeamSize( crossingList )
        self.setBeamParameters( crossingList )
        self.configurePhases( crossingList )  # in Boole, defineOptions() in Brunel
        
        #--Configuration of output files and 'default' outputs files that can/should
        #--be overwritten in Gauss-Job.py
        self.defineOutput( crossingList ) # in Boole, in Brunel where?

        self.defineMonitors()
        self.saveHistos()
        
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info( self )
        GaudiKernel.ProcessJobOptions.PrintOff()
