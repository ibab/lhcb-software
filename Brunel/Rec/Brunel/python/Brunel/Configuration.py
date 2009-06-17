## @package Brunel
#  High level configuration tools for Brunel
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.81 2009-06-17 08:34:03 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, RecSysConf, TrackSys,
                            ProcessPhase, GaudiSequencer, RichRecQCConf, DstConf, LumiAlgsConf, L0Conf )

## @class Brunel
#  Configurable for Brunel application
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class Brunel(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ TrackSys, RecSysConf, RichRecQCConf, LHCbApp, DstConf, LumiAlgsConf, L0Conf ]

    ## Default init sequences
    DefaultInitSequence     = ["Reproc", "Brunel", "Calo"]
    
    ## Known monitoring sequences, all run by default
    KnownMoniSubdets        = ["CALO","RICH","MUON","VELO","Tr","ST"] 
    KnownExpertMoniSubdets  = KnownMoniSubdets+["TT","IT","PROTO"]
    ## Known checking sequences, all run by default
    KnownCheckSubdets       = ["Pat","RICH","MUON"] 
    KnownExpertCheckSubdets = KnownCheckSubdets+["TT","IT","OT","Tr"]
    ## Default main sequences for real and simulated data
    DefaultSequence = [ "ProcessPhase/Init",
                        "ProcessPhase/Reco",
                        "ProcessPhase/Moni" ]
    DefaultRealSequence = DefaultSequence + [ "ProcessPhase/Output" ]
    DefaultMCSequence   = DefaultSequence + [ "ProcessPhase/MCLinks",
                                              "ProcessPhase/Check",
                                              "ProcessPhase/Output" ]
    
    # Steering options
    __slots__ = {
        "EvtMax"          : -1
       ,"SkipEvents"      : 0
       ,"PrintFreq"       : 1
       ,"DataType"        : "2008"
       ,"WithMC"          : False
       ,"Simulation"      : False
       ,"RecL0Only"       : False
       ,"InputType"       : "MDF"
       ,"DigiType"        : "Default"
       ,"OutputType"      : "DST"
       ,"PackType"        : "TES"
       ,"WriteFSR"        : True
       ,"Histograms"      : "Default"
       ,"NoWarnings"      : False
       ,"DatasetName"     : "Brunel"
       ,"DDDBtag"         : ""
       ,"CondDBtag"       : ""
       ,"MainSequence"    : []
       ,"InitSequence"    : []
       ,"RecoSequence"    : []
       ,"MoniSequence"    : []
       ,"MCCheckSequence" : []
       ,"MCLinksSequence" : ["L0", "Unpack", "Tr"]
       ,"Monitors"        : []
       ,"SpecialData"     : []
       ,"Context"         : "Offline"
        }


    _propertyDocDct = { 
        'EvtMax'       : """ Maximum number of events to process (default -1, all events on input files) """
       ,'SkipEvents'   : """ Number of events to skip (default 0) """
       ,'PrintFreq'    : """ The frequency at which to print event numbers (default 1, prints every event) """
       ,'DataType'     : """ Data type, can be ['DC06','2008']. Default '2008' """
       ,'WithMC'       : """ Flags whether to enable processing with MC truth (default False) """
       ,'Simulation'   : """ Flags whether to use SIMCOND conditions (default False) """
       ,'RecL0Only'    : """ Flags whether to reconstruct and output only events passing L0 (default False) """
       ,'InputType'    : """ Type of input file. Can be one of ['MDF','DIGI','ETC','RDST','DST'] (default 'MDF') """
       ,'DigiType'     : """ Type of digi, can be ['Minimal','Default','Extended'] """
       ,'OutputType'   : """ Type of output file. Can be one of ['RDST','DST','XDST','NONE'] (default 'DST') """
       ,'PackType'     : """ Type of packing for the output file. Can be one of ['TES','MDF','NONE'] (default 'TES') """
       ,'WriteFSR'     : """ Flags whether to write out an FSR """
       ,'Histograms'   : """ Type of histograms. Can be one of ['None','Default','Expert'] """
       ,'NoWarnings'   : """ Flag to suppress all MSG::WARNING or below (default False) """ 
       ,'DatasetName'  : """ String used to build output file names """
       ,'DDDBtag'      : """ Tag for DDDB """
       ,'CondDBtag'    : """ Tag for CondDB """
       ,'MainSequence' : """ List of main sequences, default self.DefaultSequence """
       ,'InitSequence' : """ List of initialisation sequences """
       ,'RecoSequence' : """ List of reconstruction sequences, default defined in RecSys """
       ,'MCLinksSequence' : """ List of MC truth link sequences """
       ,'MCCheckSequence' : """ List of MC Check sequence, default self.KnownCheckSubdets """
       ,'Monitors'     : """ List of monitors to execute, see self.KnownMonitors. Default none """
       ,'SpecialData'  : """ Various special data processing options. See RecSys.KnownSpecialData for all options """
       ,'Context'      : """ The context within which to run (default 'Offline') """
       }

    def defineGeometry(self):
        # DIGI is always simulation, as is usage of MC truth!
        if self.getProp( "WithMC" ) or self.getProp( "InputType" ).upper() == 'DIGI':
            self.setProp( "Simulation", True )

        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","Simulation"])

    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])

    def defineOptions(self):

        inputType = self.getProp( "InputType" ).upper()
        if inputType not in [ "MDF", "DST", "DIGI", "ETC", "RDST", "XDST" ]:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "OutputType" ).upper()
        if outputType not in [ "NONE", "DST", "RDST", "XDST" ]:
            raise TypeError( "Invalid outputType '%s'"%outputType )

        histOpt = self.getProp("Histograms").capitalize()
        if histOpt not in ["","None","Default","Expert"]:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)

        withMC = self.getProp("WithMC")
        if withMC:
            if inputType in [ "MDF", "RDST" ]:
                log.warning( "WithMC = True, but InputType = '%s'. Forcing WithMC = False"%inputType )
                withMC = False # Force it, MDF and RDST never contain MC truth
            if outputType == "RDST":
                log.warning( "WithMC = True, but OutputType = '%s'. Forcing WithMC = False"%inputType )
                withMC = False # Force it, RDST never contains MC truth

        if self.getProp("WriteFSR") and self.getProp("PackType").upper() in ["MDF"]:
            if hasattr( self, "WriteFSR" ): log.warning("Don't know how to write FSR to MDF output file")
            self.setProp("WriteFSR", False)

        self.configureSequences( withMC )

        self.configureInit( inputType )
        
        self.configureInput( inputType )

        self.configureOutput( outputType, withMC )

        # Activate all monitoring (does not use MC truth)
        self.configureMoni( histOpt == "Expert", withMC )

        if withMC:
            # Create associators for checking and for DST
            ProcessPhase("MCLinks").DetectorList += self.getProp("MCLinksSequence")
            ProcessPhase("MCLinks").Context = self.getProp("Context")
            # Unpack Sim data
            GaudiSequencer("MCLinksUnpackSeq").Members += [ "UnpackMCParticle",
                                                            "UnpackMCVertex" ]
            GaudiSequencer("MCLinksTrSeq").Members += [ "TrackAssociator" ]

            # activate all configured checking (uses MC truth)
            self.configureCheck( histOpt == "Expert" )

        # Setup L0 filtering if requested, runs L0 before Reco
        if self.getProp("RecL0Only"):
            ProcessPhase("Init").DetectorList.append("L0")
            L0Conf().L0Sequencer = GaudiSequencer("InitL0Seq")
            L0Conf().FilterL0FromRaw = True
            self.setOtherProps( L0Conf(), ["DataType"] )

        # ROOT persistency for histograms
        importOptions('$STDOPTS/RootHist.opts')
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

        if histOpt == "None" or histOpt == "":
            # HistogramPersistency still needed to read in CaloPID DLLs.
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        # Pass expert checking option to RecSys
        if histOpt == "Expert": RecSysConf().setProp( "ExpertHistos", True )

        # Use a default histogram file name if not already set
        if not HistogramPersistencySvc().isPropertySet( "OutputFile" ):
            histosName   = self.getProp("DatasetName")
            if histosName == "": histosName = "Brunel"
            if self.getProp( "RecL0Only" ): histosName += '-L0Yes'
            if (self.evtMax() > 0): histosName += '-' + str(self.evtMax()) + 'ev'
            if histOpt == "Expert": histosName += '-expert'
            histosName += '-histos.root'
            HistogramPersistencySvc().OutputFile = histosName

    def defineMonitors(self):
        
        # get all defined monitors
        monitors = self.getProp("Monitors") + LHCbApp().getProp("Monitors")
        # Currently no Brunel specific monitors, so pass them all to LHCbApp
        LHCbApp().setProp("Monitors", monitors)

        # Use TimingAuditor for timing, suppress printout from SequencerTimerTool
        from Configurables import (ApplicationMgr,AuditorSvc,SequencerTimerTool)
        ApplicationMgr().ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ] 
        SequencerTimerTool().OutputLevel = 4
        
    def configureSequences(self, withMC):
        brunelSeq = GaudiSequencer("BrunelSequencer")
        ApplicationMgr().TopAlg = [ brunelSeq ]
        mainSeq = self.getProp("MainSequence")
        if len( mainSeq ) == 0:
            if withMC:
                mainSeq = self.DefaultMCSequence
            else:
                mainSeq = self.DefaultRealSequence
            self.MainSequence = mainSeq
        brunelSeq.Members += mainSeq


    def configureInit(self, inputType):
        # Init sequence
        initSeq = self.getProp( "InitSequence" )
        if len( initSeq ) == 0 :
            if inputType in ["MDF"]:
                # add Lumi for MDF input in offline mode
                initSeq = self.DefaultInitSequence + ["Lumi"]
            else:
                initSeq = self.DefaultInitSequence
            self.setProp( "InitSequence", initSeq )

        ProcessPhase("Init").DetectorList += initSeq
        ProcessPhase("Init").Context = self.getProp("Context")

        from Configurables import RecInit, MemoryTool
        recInit = RecInit( "BrunelInit",
                           PrintFreq = self.getProp("PrintFreq"))
        GaudiSequencer("InitBrunelSeq").Members += [ recInit ]
        recInit.addTool( MemoryTool(), name = "BrunelMemory" )
        recInit.BrunelMemory.HistoTopDir = "Brunel/"
        recInit.BrunelMemory.HistoDir    = "MemoryTool"
        # count events
        from Configurables import EventCountHisto
        evtC = EventCountHisto("BrunelEventCount")
        evtC.HistoTopDir = "Brunel/"
        GaudiSequencer("InitBrunelSeq").Members += [ evtC ]
        
        if "Lumi" in initSeq :
            if self.getProp("WriteFSR"):
                self.setOtherProps(LumiAlgsConf(),["Context","DataType","InputType"])
                LumiAlgsConf().LumiSequencer = GaudiSequencer("InitLumiSeq")

        # Convert Calo 'packed' banks to 'short' banks if needed
        if "Calo" in initSeq :
            GaudiSequencer("InitCaloSeq").Members += ["GaudiSequencer/CaloBanksHandler"]
            importOptions("$CALODAQROOT/options/CaloBankHandler.opts")


    def configureInput(self, inputType):
        """
        Tune initialisation according to input type
        """

        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        # By default, Brunel only needs to open one input file at a time
        # Only set to zero if not previously set to something else.
        if not IODataManager().isPropertySet("AgeLimit") : IODataManager().AgeLimit = 0
        
        if inputType in [ "XDST", "DST", "RDST", "ETC" ]:
            # Kill knowledge of any previous Brunel processing
            from Configurables import ( TESCheck, EventNodeKiller )
            InitReprocSeq = GaudiSequencer( "InitReprocSeq" )
            if ( self.getProp("WithMC") and inputType in ["XDST","DST"] ):
                # Load linkers, to kill them (avoid appending to them later)
                InitReprocSeq.Members.append( "TESCheck" )
                TESCheck().Inputs = ["Link/Rec/Track/Best"]
            InitReprocSeq.Members.append( "EventNodeKiller" )
            EventNodeKiller().Nodes = [ "pRec", "Rec", "Raw", "Link/Rec" ]

        # Read ETC selection results into TES for writing to DST
        if inputType == "ETC":
            IODataManager().AgeLimit += 1

        if inputType in [ "MDF", "RDST", "ETC" ]:
            # In case raw data resides in MDF file
            EventPersistencySvc().CnvServices.append("LHCb::RawDataCnvSvc")

        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";

    def configureOutput(self, dstType, withMC):
        """
        Set up output stream
        """
        if dstType in [ "XDST", "DST", "RDST" ]:
            writerName = "DstWriter"
            packType  = self.getProp( "PackType" )
            # Do not pack DC06 DSTs, for consistency with existing productions
            if self.getProp("DataType") == "DC06": packType = "NONE"

            # event output
            dstWriter = OutputStream( writerName )
            dstWriter.RequireAlgs += ["Reco"] # Write only if Rec phase completed
            # Set a default output file name if not already defined in the user job options
            if not dstWriter.isPropertySet( "Output" ):
                outputFile = self.outputName()
                outputFile = outputFile + '.' + self.getProp("OutputType").lower()
                dstWriter.Output = "DATAFILE='PFN:" + outputFile + "' TYP='POOL_ROOTTREE' OPT='REC'"

            # FSR output stream
            if self.getProp("WriteFSR"):
                FSRWriter = RecordStream( "FSRWriter",
                                          ItemList = [ "/FileRecords#999" ],
                                          EvtDataSvc = "FileRecordDataSvc",
                                          EvtConversionSvc = "FileRecordPersistencySvc" )
                # Write the FSRs to the same file as the events
                if FSRWriter.isPropertySet( "Output" ):
                    log.warning("Ignoring FSRWriter.Output option, FSRs will be written to same file as the events")
                FSRWriter.Output = dstWriter.getProp("Output")

                ApplicationMgr().OutStream.append(FSRWriter)
                # Suppress spurious error when reading POOL files without run records
                if self.getProp( "InputType" ).upper() not in [ "MDF" ]:
                    from Configurables import FileRecordDataSvc
                    FileRecordDataSvc().OutputLevel = FATAL

            if dstType == "XDST":
                # Allow multiple files open at once (SIM,DST,DIGI etc.)
                IODataManager().AgeLimit += 1

            from Configurables import TrackToDST
            if dstType == "RDST":
                # Sequence for altering content of rDST compared to DST
                # Filter Track States to be written
                trackFilter = TrackToDST("TrackToRDST")
                trackFilter.veloStates = ["ClosestToBeam"]
                trackFilter.longStates = ["ClosestToBeam"]
                trackFilter.TTrackStates = ["FirstMeasurement"]
                trackFilter.downstreamStates = ["FirstMeasurement"]
                trackFilter.upstreamStates = ["ClosestToBeam"]
            else:
                # Sequence for altering DST content
                # Filter Track States to be written
                trackFilter = TrackToDST()
                
            ProcessPhase("Output").DetectorList += [ "DST" ]
            GaudiSequencer("OutputDSTSeq").Members += [ trackFilter ]

            if packType != "NONE":
                # Add the sequence to pack the DST containers
                packSeq = GaudiSequencer("PackDST")
                DstConf().PackSequencer = packSeq
                GaudiSequencer("OutputDSTSeq").Members += [ packSeq ]

            # Define the file content
            DstConf().Writer     = writerName
            DstConf().DstType    = dstType
            DstConf().PackType   = packType
            if withMC:
                DstConf().SimType = "Full"
            elif self.getProp("DigiType").capitalize() == "Minimal":
                from Configurables import PackMCVertex
                GaudiSequencer("OutputDSTSeq").Members += [PackMCVertex()]
                DstConf().SimType = "Minimal"
            DstConf().OutputName = self.outputName()

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if self.getProp( "RecL0Only" ): outputName += '-L0Yes'
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        return outputName

    def evtMax(self):
        return LHCbApp().evtMax()

    def configureMoni(self,expert,withMC):
        # Set up monitoring (i.e. not using MC truth)
        moniSeq = self.getProp("MoniSequence")
        if len( moniSeq ) == 0:
            if expert:
                moniSeq = self.KnownExpertMoniSubdets
            else:
                moniSeq = self.KnownMoniSubdets
            self.MoniSequence = moniSeq
        else:
            for seq in moniSeq:
                if expert:
                    if seq not in self.KnownExpertMoniSubdets:
                        log.warning("Unknown subdet '%s' in MoniSequence"%seq)
                else:
                    if seq not in self.KnownMoniSubdets:
                        log.warning("Unknown subdet '%s' in MoniSequence"%seq)
        ProcessPhase("Moni").DetectorList += moniSeq
        ProcessPhase('Moni').Context = self.getProp("Context")

        # Do not print event number at every event (done already by Brunel)
        EventSelector().PrintFreq = -1
        # Modify printout defaults
        if self.getProp( "NoWarnings" ):
            LHCbApp().setProp( "Quiet",     True )
            LHCbApp().setProp( "TimeStamp", True )
            # Additional information to be kept
            getConfigurable("BrunelInit").OutputLevel = INFO
            getConfigurable("DstWriter").OutputLevel  = INFO
            # Suppress known warnings
            importOptions( "$BRUNELOPTS/SuppressWarnings.opts" )

        # Units needed in several of the monitoring options
        importOptions('$STDOPTS/PreloadUnits.opts')

        # Histograms filled both in real and simulated data cases
        if "CALO" in moniSeq :
            importOptions('$CALOMONIDSTOPTS/CaloMonitor.opts')

        if "VELO" in moniSeq :
            importOptions('$VELORECMONITORSROOT/options/BrunelMoni_Velo.py')

        if "Tr" in moniSeq :
            from TrackMonitors.ConfiguredTrackMonitors import ConfiguredTrackMonitorSequence
            ConfiguredTrackMonitorSequence(Name='MoniTrSeq')

        if "MUON" in moniSeq :
            importOptions("$MUONPIDCHECKERROOT/options/MuonPIDMonitor.py")

        if "ST" in moniSeq :
            from Configurables import ST__STClusterMonitor, GaudiSequencer
            GaudiSequencer( "MoniSTSeq" ).Members += [ ST__STClusterMonitor("TTClusterMonitor"),
                                                       ST__STClusterMonitor("ITClusterMonitor")]
            ST__STClusterMonitor("TTClusterMonitor").DetType = "TT" ## default anyway 
            ST__STClusterMonitor("ITClusterMonitor").DetType = "IT" 

        # Histograms filled only in real data case
        if not withMC:
            if "RICH" in moniSeq :
                from Configurables import GaudiSequencer
                self.setOtherProps(RichRecQCConf(), ["Context","DataType","WithMC"])
                RichRecQCConf().setProp("MoniSequencer", GaudiSequencer("MoniRICHSeq"))

        # Expert histograms
        if expert:
            if "TT" in moniSeq :
                from Configurables import ST__STClusterMonitor
                clusMoni = ST__STClusterMonitor("TTClusterMonitor")
                clusMoni.FullDetail = True
                GaudiSequencer("MoniTTSeq").Members += [clusMoni]
            if "IT" in moniSeq :
                from Configurables import ST__STClusterMonitor
                clusMoni = ST__STClusterMonitor("ITClusterMonitor")
                clusMoni.FullDetail = True
                clusMoni.DetType = "IT"
                GaudiSequencer("MoniITSeq").Members += [clusMoni]

    def configureCheck(self,expert):
        # "Check" histograms filled only with simulated data
        checkSeq = self.getProp("MCCheckSequence")
        if len( checkSeq ) == 0:
            if expert:
                checkSeq = self.KnownExpertCheckSubdets
            else:
                checkSeq = self.KnownCheckSubdets
            self.MCCheckSequence = checkSeq
        else:
            for seq in checkSeq:
                if expert:
                    if seq not in self.KnownExpertCheckSubdets:
                        log.warning("Unknown subdet '%s' in MCCheckSequence"%seq)
                else:
                    if seq not in self.KnownCheckSubdets:
                        log.warning("Unknown subdet '%s' in MCCheckSequence"%seq)
            
        ProcessPhase("Check").DetectorList += checkSeq
        ProcessPhase("Check").Context = self.getProp("Context")

        # Tracking handled inside TrackSys configurable
        TrackSys().setProp( "WithMC", True )

        if "MUON" in checkSeq :
            importOptions("$MUONPIDCHECKERROOT/options/MuonPIDChecker.py")

        if "RICH" in checkSeq :
            from Configurables import GaudiSequencer
            self.setOtherProps(RichRecQCConf(), ["Context","DataType","WithMC"])
            RichRecQCConf().setProp("MoniSequencer", GaudiSequencer("CheckRICHSeq"))

        if expert:
            # Data on Demand for MCParticle to MCHit association, needed by ST, IT, OT, Tr, Muon
            ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]
            importOptions( "$ASSOCIATORSROOT/options/MCParticleToMCHit.py" )

            # Allow multiple files open at once (SIM,DST,DIGI etc.)
            IODataManager().AgeLimit += 1
        
            if "TT" in checkSeq :
                from Configurables import ( STEffChecker, MCParticleSelector )
                from GaudiKernel.SystemOfUnits import GeV
                effCheck = STEffChecker("TTEffChecker")
                effCheck.FullDetail = True
                effCheck.addTool(MCParticleSelector)
                effCheck.MCParticleSelector.zOrigin = 50.0
                effCheck.MCParticleSelector.pMin = 1.0*GeV
                effCheck.MCParticleSelector.betaGammaMin = 1.0
                GaudiSequencer("CheckTTSeq").Members += [effCheck]

            if "IT" in checkSeq :
                from Configurables import ( STEffChecker, MCParticleSelector )
                from GaudiKernel.SystemOfUnits import GeV
                effCheck = STEffChecker("ITEffChecker")
                effCheck.FullDetail = True
                effCheck.addTool(MCParticleSelector)
                effCheck.MCParticleSelector.zOrigin = 50.0
                effCheck.MCParticleSelector.pMin = 1.0*GeV
                effCheck.MCParticleSelector.betaGammaMin = 1.0
                effCheck.DetType = "IT"
                GaudiSequencer("CheckITSeq").Members += [effCheck]

            if "OT" in checkSeq :
                GaudiSequencer("CheckOTSeq").Members += ["OTTimeCreator", "OTTimeChecker"] # needs MCHits

            if "Tr" in  checkSeq :
                # Checking on the tracks in the "best" container - needs MCHits
                importOptions( "$TRACKSYSROOT/options/TrackChecking.opts" )

            if "CALO" in  checkSeq : 
                importOptions( "$CALOASSOCIATORSROOT/options/CaloAssociators.opts" )
                importOptions( "$CALOMONIDSTOPTS/CaloChecker.opts" )

            if "RICH" in checkSeq :
                RichRecQCConf().setProp( "ExpertHistos", True )

            if "PROTO" in checkSeq :
                from Configurables import ( GaudiSequencer, NTupleSvc, ChargedProtoParticleTupleAlg )
                protoChecker = ChargedProtoParticleTupleAlg("ChargedProtoTuple")
                protoChecker.NTupleLUN = "PROTOTUPLE"
                GaudiSequencer("CheckPROTOSeq").Members += [protoChecker]
                NTupleSvc().Output += ["PROTOTUPLE DATAFILE='protoparticles.tuples.root' TYP='ROOT' OPT='NEW'"]

    ## Apply the configuration
    def __apply_configuration__(self):
        
        GaudiKernel.ProcessJobOptions.PrintOff()
        self.setOtherProps(RecSysConf(),["SpecialData","RecoSequence","Context","OutputType","DataType"])
        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        self.defineMonitors()
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info( self )
        log.info( RecSysConf() )
        log.info( TrackSys() )
        GaudiKernel.ProcessJobOptions.PrintOff()
