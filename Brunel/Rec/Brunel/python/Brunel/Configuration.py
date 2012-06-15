# @package Brunel
#  High level configuration tools for Brunel
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "v41r2"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, RecSysConf, TrackSys,
                            RecMoniConf,GaudiSequencer, RichRecQCConf, DstConf,
                            LumiAlgsConf, L0Conf, GlobalRecoChecks, CondDB )

## @class Brunel
#  Configurable for Brunel application
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class Brunel(LHCbConfigurableUser):

    ## Name used for RICH Monitoring configurable
    richMoniConfName = "OfflineRichMoni"

    ## Possible used Configurables
    __used_configurables__ = [ TrackSys, RecSysConf, RecMoniConf,
                               (RichRecQCConf,richMoniConfName),
                               LHCbApp, DstConf, LumiAlgsConf, L0Conf,
                               GlobalRecoChecks ]

    ## Default init sequences
    DefaultInitSequence      = ["Brunel"]
    DefaultReproInitSequence = ["Reproc"] + DefaultInitSequence

    ## Known checking sequences, all run by default
    KnownCheckSubdets       = ["Pat","RICH","MUON"]
    KnownExpertCheckSubdets = KnownCheckSubdets+["TT","IT","OT","Tr","CALO","PROTO"]
    ## Default main sequences for real and simulated data
    DefaultSequence = [ "ProcessPhase/Reco",
                        "ProcessPhase/Moni" ]
    DefaultMCSequence   = DefaultSequence + [ "ProcessPhase/MCLinks",
                                              "ProcessPhase/Check" ]

    # Steering options
    __slots__ = {
        "EvtMax"          : -1
       ,"SkipEvents"      : 0
       ,"PrintFreq"       : 1
       ,"DataType"        : "2012"
       ,"WithMC"          : False
       ,"Simulation"      : False
       ,"RecL0Only"       : False
       ,"InputType"       : "MDF"
       ,"DigiType"        : "Default"
       ,"OutputType"      : "DST"
       ,"PackType"        : "TES"
       ,"Persistency"     : None
       ,"WriteFSR"        : True
       ,"WriteLumi"       : False
       ,"Histograms"      : "OfflineFull"
       ,"OutputLevel"     : INFO
       ,"NoWarnings"      : False
       ,"ProductionMode"  : False
       ,"DatasetName"     : "Brunel"
       ,"DDDBtag"         : ""
       ,"CondDBtag"       : ""
       ,"MainSequence"    : None
       ,"InitSequence"    : None
       ,"RecoSequence"    : None
       ,"MCCheckSequence" : None
       ,"MCLinksSequence" : ["L0", "Unpack", "Tr"]
       ,"Monitors"        : []
       ,"SpecialData"     : []
       ,"Context"         : "Offline"
       ,"RawBanksToKill"  : None
       ,"CaloPhotonChecker" : False
       ,"VetoHltErrorEvents" : True
        # only use for Online running
       ,"UseDBSnapshot" : False
       ,"PartitionName" : "LHCb"
       ,"DBSnapshotDirectory" : "/group/online/hlt/conditions"
        }


    _propertyDocDct = {
        'EvtMax'       : """ Maximum number of events to process (default -1, all events on input files) """
       ,'SkipEvents'   : """ Number of events to skip (default 0) """
       ,'PrintFreq'    : """ The frequency at which to print event numbers (default 1, prints every event) """
       ,'DataType'     : """ Data type, possible values defined in DDDBConf """
       ,'WithMC'       : """ Flags whether to enable processing with MC truth (default False) """
       ,'Simulation'   : """ Flags whether to use SIMCOND conditions (default False) """
       ,'RecL0Only'    : """ Flags whether to reconstruct and output only events passing L0 (default False) """
       ,'InputType'    : """ Type of input file. Can be one of self.KnownInputTypes (default 'MDF') """
       ,'DigiType'     : """ Type of digi, can be ['Minimal','Default','Extended'] """
       ,'OutputType'   : """ Type of output file. Can be one of DstConf().KnownDstTypes (default 'DST') """
       ,'PackType'     : """ Type of packing for the output file. Can be one of ['TES','MDF','NONE'] (default 'TES') """
       ,'Persistency'  : """ Overwrite the default persistency with something else. """
       ,'WriteFSR'     : """ Flags whether to write out an FSR """
       ,'WriteLumi'    : """ Flags whether to write out Lumi-only events to DST """
       ,'Histograms'   : """ Type of histograms. Can be one of self.KnownHistograms """
       ,'OutputLevel'  : """ The printout level to use (default INFO) """
       ,'NoWarnings'   : """ OBSOLETE, kept for Dirac compatibility. Please use ProductionMode """
       ,'ProductionMode'  : """ Enables special settings for running in production """
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
       ,'RawBanksToKill':""" Raw banks to remove from RawEvent before processing. Removed also from DST copy of RawEvent """
       ,'CaloPhotonChecker':""" Temporary workaround to bug #73392 """
       ,"VetoHltErrorEvents" : """Do not reconstruct events that have been flagged as error by Hlt"""
        # only use for Online running
        ,"UseDBSnapshot" : """Use a snapshot for velo position and rich pressure"""
        ,"PartitionName" : """Name of the partition when running (needed to find DB: '', 'FEST', or 'LHCb'"""
        ,"DBSnapshotDirectory" : """Local Directory where the snapshot is"""
        }

    KnownInputTypes  = [ "MDF",  "DST", "SDST", "XDST", "DIGI" ]
    KnownHistograms  = [ "None", "Online", "OfflineExpress", "OfflineFull", "Expert" ]

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

        # Kept for Dirac backward compatibility
        if self.getProp( "NoWarnings" ) :
            log.warning("Brunel().NoWarnings=True property is obsolete and maintained for Dirac compatibility. Please use Brunel().ProductionMode=True instead")
            self.setProp( "ProductionMode", True )

        # Special settings for production
        if self.getProp( "ProductionMode" ) :
            if not self.isPropertySet( "OutputLevel" ) :
                self.setProp("OutputLevel", ERROR)
            if not LHCbApp().isPropertySet( "TimeStamp" ) :
                LHCbApp().setProp( "TimeStamp", True )
            if not self.isPropertySet( "PrintFreq" ) :
                self.setProp("PrintFreq", 1000)

        inputType = self.getProp( "InputType" ).upper()
        if inputType not in self.KnownInputTypes:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "OutputType" ).upper()

        histOpt = self.getProp("Histograms")
        if histOpt not in self.KnownHistograms:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)

        withMC = self.getProp("WithMC")
        if withMC:
            if inputType in [ "MDF", "SDST" ]:
                log.warning( "WithMC = True, but InputType = '%s'. Forcing WithMC = False"%inputType )
                withMC = False # Force it, MDF, SDST never contain MC truth
            if outputType in [ "SDST" ]:
                log.warning( "WithMC = True, but OutputType = '%s'. Forcing WithMC = False"%inputType )
                withMC = False # Force it, SDST never contains MC truth

        if self.getProp("WriteFSR") and self.getProp("PackType").upper() in ["MDF"]:
            if hasattr( self, "WriteFSR" ): log.warning("Don't know how to write FSR to MDF output file")
            self.setProp("WriteFSR", False)

        # Do not look for Hlt errors in data without HltDecReports bank
        if self.getProp( "DataType" ) in [ "2008", "2009" ]:
            self.setProp( "VetoHltErrorEvents", False )
        # For simulation, change the default but allow to override
        if self.getProp( "Simulation" ) and not hasattr( self, "VetoHltErrorEvents" ):
            self.setProp( "VetoHltErrorEvents", False )


        # Flag to handle or not LumiEvents
        handleLumi = inputType in ["MDF"] and not withMC and not self.getProp('UseDBSnapshot')

        # veto Hlt Error Events
        vetoHltErrorEvents = self.getProp("VetoHltErrorEvents")

        self.configureSequences( withMC, handleLumi, vetoHltErrorEvents )

        self.configureInit( inputType )

        self.configureInput( inputType )

        self.configureOutput( outputType, withMC, handleLumi )

        if withMC:
            # Create associators for checking and for DST
            from Configurables import ProcessPhase
            ProcessPhase("MCLinks").DetectorList += self.getProp("MCLinksSequence")
            # Unpack Sim data
            GaudiSequencer("MCLinksUnpackSeq").Members += [ "UnpackMCParticle",
                                                            "UnpackMCVertex" ]
            GaudiSequencer("MCLinksTrSeq").Members += [ "TrackAssociator" ]

            # activate all configured checking (uses MC truth)
            self.configureCheck( histOpt == "Expert" )

            # data on demand needed to pack RichDigitSummary for DST, when reading unpacked DIGI
            # Also needed to unpack MCHit containers when expert checking enabled
            ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]

        # ROOT persistency for histograms
        ApplicationMgr().HistogramPersistency = "ROOT"
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

        if histOpt == "None" or histOpt == "":
            # HistogramPersistency still needed to read in CaloPID DLLs.
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        # Pass expert checking option to RecSys and RecMoni
        if histOpt == "Expert":
            DstConf().EnablePackingChecks = True

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

    def configureSequences(self, withMC, handleLumi, vetoHltErrorEvents):
        brunelSeq = GaudiSequencer("BrunelSequencer")
        brunelSeq.Context = self.getProp("Context")
        ApplicationMgr().TopAlg += [ brunelSeq ]
        brunelSeq.Members += [ "ProcessPhase/Init" ]
        physicsSeq = GaudiSequencer( "PhysicsSeq" )

        # Treatment of luminosity events
        if handleLumi:
            lumiSeq = GaudiSequencer("LumiSeq")

            # Prepare the FSR
            if self.getProp("WriteFSR"):
                self.setOtherProps(LumiAlgsConf(),["Context","DataType","InputType","Simulation"])
                lumiCounters = GaudiSequencer("LumiCounters")
                lumiSeq.Members += [ lumiCounters ]
                LumiAlgsConf().LumiSequencer = lumiCounters

            # Filter out Lumi only triggers from further processing, but still write to output
            from Configurables import HltRoutingBitsFilter
            physFilter = HltRoutingBitsFilter( "PhysFilter", RequireMask = [ 0x0, 0x4, 0x0 ] )
            physicsSeq.Members += [ physFilter ]
            lumiFilter = HltRoutingBitsFilter( "LumiFilter", RequireMask = [ 0x0, 0x2, 0x0 ] )
            lumiSeq.Members += [ lumiFilter, physFilter ]
            lumiSeq.ModeOR = True

            # Sequence to be executed if physics sequence not called (nano events)
            notPhysSeq = GaudiSequencer("NotPhysicsSeq")
            notPhysSeq.ModeOR = True
            notPhysSeq.Members = [ physFilter ]

            brunelSeq.Members += [ lumiSeq, notPhysSeq ]
            #brunelSeq.Members += [ lumiSeq ]

        # Do not process events flagged as error in Hlt, but still write procstatus
        from Configurables import HltDecReportsDecoder
        if vetoHltErrorEvents:
            """
            By Patrick Koppenburg, 16/6/2011
            """
            from Configurables import LoKi__HDRFilter, AddToProcStatus
            # identifies events that are not of type Hlt1ErrorEvent or Hlt2ErrorEvent
            filterCode = "HLT_PASS_RE('Hlt1(?!ErrorEvent).*Decision') & HLT_PASS_RE('Hlt2(?!ErrorEvent).*Decision')"  # from Gerhard
            hltErrorFilter = LoKi__HDRFilter('HltErrorFilter', Code = filterCode )   # the filter
            # Make a sequence that selects these events
            hltfilterSeq = GaudiSequencer( "HltFilterSeq" )
            if handleLumi: hltfilterSeq.Members = [ physFilter ]         # protect against lumi (that doesn't have decreports)
            hltfilterSeq.Members += [ HltDecReportsDecoder(),            # decode DecReports
                                      hltErrorFilter ]                   # apply filter
            # Sequence to be executed if hltErrorFilter is failing to set ProcStatus
            hlterrorSeq = GaudiSequencer("HltErrorSeq", ModeOR = True, ShortCircuit = True) # anti-logic
            addToProc = AddToProcStatus("HltErrorProc",Reason="HltError",Subsystem="Hlt")   # write a procstatus
            hlterrorSeq.Members += [ hltfilterSeq, addToProc ]           # only run if hltfilterSeq fails
            brunelSeq.Members += [ hlterrorSeq ]                         # add this sequece to Brunel _before_ physseq
            physicsSeq.Members += [ hltfilterSeq ]                       # take good events in physics seq

        # Convert Calo 'packed' banks to 'short' banks if needed
        physicsSeq.Members += ["GaudiSequencer/CaloBanksHandler"]
        importOptions("$CALODAQROOT/options/CaloBankHandler.opts")

        # Decode L0 (and HLT if not already done)
        trgSeq = GaudiSequencer("DecodeTriggerSeq")
        l0TrgSeq = GaudiSequencer("L0TriggerSeq")
        if self.getProp( "DataType" ) not in [ "2008", "2009" ]:
            trgSeq.Members += [ HltDecReportsDecoder() ]
        trgSeq.Members += [ l0TrgSeq ]
        physicsSeq.Members += [ trgSeq ]
        L0Conf().L0Sequencer = l0TrgSeq
        if self.getProp("RecL0Only"):
            # Setup L0 filtering if requested, runs L0 before Reco
            L0Conf().FilterL0FromRaw = True
            self.setOtherProps( L0Conf(), ["DataType"] )
        else:
            L0Conf().DecodeL0DU = True

        if not self.isPropertySet("MainSequence"):
            if withMC:
                mainSeq = self.DefaultMCSequence
            else:
                mainSeq = self.DefaultSequence
            self.MainSequence = mainSeq
        physicsSeq.Members += self.getProp("MainSequence")
        from Configurables import ProcessPhase
        outputPhase = ProcessPhase("Output")
        brunelSeq.Members  += [ physicsSeq ]
        brunelSeq.Members  += [ outputPhase ]

    def configureInit(self, inputType):
        # Init sequence
        if not self.isPropertySet("InitSequence"):
            if self._isReprocessing(inputType):
                self.setProp( "InitSequence", self.DefaultReproInitSequence )
            else:
                self.setProp( "InitSequence", self.DefaultInitSequence )
        from Configurables import ProcessPhase
        ProcessPhase("Init").DetectorList += self.getProp("InitSequence")

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

        # kill some RAW banks
        banksToKill = []
        if self.isPropertySet( "RawBanksToKill" ):
            banksToKill  = self.getProp( "RawBanksToKill" )
        if ("2009" == self.getProp("DataType")) and (inputType in ["MDF","SDST"]) and not self.getProp("Simulation") :
            banksToKill += ["VeloFull", "L0PUFull"]
        if len(banksToKill) > 0 :
            from Configurables import bankKiller
            bkKill = bankKiller("BrunelBankKiller")
            bkKill.BankTypes = banksToKill
            GaudiSequencer("InitBrunelSeq").Members += [ bkKill ]

        # Do not print event number at every event (done already by BrunelInit)
        EventSelector().PrintFreq = -1

        # OutputLevel
        self.setOtherProp(LHCbApp(),"OutputLevel")
        if self.isPropertySet( "OutputLevel" ) :
            level = self.getProp("OutputLevel")
            if level == ERROR or level == WARNING :
                # Suppress known warnings
                importOptions( "$BRUNELOPTS/SuppressWarnings.opts" )
                if not recInit.isPropertySet( "OutputLevel" ): recInit.OutputLevel = INFO
        self.setOtherProps(RecSysConf(), ["OutputLevel"])
        self.setOtherProps(RecMoniConf(),["OutputLevel"])

        # Always print Magnetic Field used
        from Configurables import MagneticFieldSvc
        magSvc = MagneticFieldSvc()
        if not magSvc.isPropertySet("OutputLevel") :
            magSvc.setProp("OutputLevel", INFO)

        # Switch off LoKi banner
        from Configurables import LoKiSvc
        LoKiSvc().Welcome = False


    def configureInput(self, inputType):
        """
        Tune initialisation according to input type
        """

        # By default, Brunel only needs to open one input file at a time
        # Only set to zero if not previously set to something else.
        if not IODataManager().isPropertySet("AgeLimit") : IODataManager().AgeLimit = 0

        if self._isReprocessing(inputType):
            # Kill knowledge of any previous Brunel processing
            from Configurables import ( TESCheck, EventNodeKiller )
            InitReprocSeq = GaudiSequencer( "InitReprocSeq" )
            if ( self.getProp("WithMC") and inputType in ["XDST","DST"] ):
                # Load linkers, to kill them (avoid appending to them later)
                InitReprocSeq.Members.append( "TESCheck" )
                TESCheck().Inputs = ["Link/Rec/Track/Best"]
            InitReprocSeq.Members.append( "EventNodeKiller" )
            EventNodeKiller().Nodes += [ "pRec", "Rec", "Raw", "Link/Rec" ]

        if inputType in [ "SDST" ]:
            # Allow navigation to ancestor file
            IODataManager().AgeLimit += 1

        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";

    def configureOutput(self, dstType, withMC, handleLumi):
        """
        Set up output stream
        """
        if hasattr( self, "Persistency" ):
            self.setOtherProps(LHCbApp(),["Persistency"])
            self.setOtherProps(DstConf(),["Persistency"])

        if dstType in [ "XDST", "DST", "SDST" ]:
            writerName = "DstWriter"
            packType  = self.getProp( "PackType" )

            # event output
            dstWriter = OutputStream( writerName )
            dstWriter.AcceptAlgs += ["Reco"] # Write only if Rec phase completed
            if handleLumi and self.getProp( "WriteLumi" ):
                dstWriter.AcceptAlgs += ["LumiSeq"] # Write also if Lumi sequence completed
            # set verbosity
            if self.getProp( "ProductionMode" ):
                if not dstWriter.isPropertySet( "OutputLevel" ):
                    dstWriter.OutputLevel = INFO
                if self.getProp("WriteFSR"):
                    FSRWriter = RecordStream( "FSROutputStreamDstWriter")
                    if not FSRWriter.isPropertySet( "OutputLevel" ):
                        FSRWriter.OutputLevel = INFO
            # Suppress spurious error when reading POOL files without run records
            if self.getProp("WriteFSR"):
                if self.getProp( "InputType" ).upper() not in [ "MDF" ]:
                    from Configurables import FileRecordDataSvc
                    FileRecordDataSvc().OutputLevel = FATAL

            if dstType == "XDST":
                # Allow multiple files open at once (SIM,DST,DIGI etc.)
                IODataManager().AgeLimit += 1

            if dstType in ["SDST","DST","XDST"] and packType not in ["MDF"]:
                # Split the Raw Event for the DST

                # first the Trigger Raw Event
                from Configurables import RawEventSelectiveCopy
                trigRawBankCopy = RawEventSelectiveCopy('TriggerRawBank')
                trigBanks = [ 'ODIN',
                              'HltSelReports',
                              'HltDecReports',
                              'HltRoutingBits',
                              'HltVertexReports',
                              'L0Calo',
                              'L0CaloFull',
                              'L0DU',
                              'L0Muon',
                              'L0MuonProcCand',
                              'L0PU'
                              ]
                trigRawBankCopy.RawBanksToCopy = trigBanks
                trigRawBankCopy.OutputRawEventLocation = "Trigger/RawEvent"
                GaudiSequencer("OutputDSTSeq").Members += [trigRawBankCopy]
                    
                # then the Calo Raw Event
                caloRawBankCopy = RawEventSelectiveCopy('CaloRawBank')
                caloBanks = [ 'PrsE',
                              'EcalE',
                              'HcalE',
                              'PrsTrig',
                              'EcalTrig',
                              'HcalTrig',
                              'EcalPacked',
                              'HcalPacked',
                              'PrsPacked',
                              'EcalPackedError',
                              'HcalPackedError',
                              'PrsPackedError'
                              ]
                caloRawBankCopy.RawBanksToCopy = caloBanks
                caloRawBankCopy.OutputRawEventLocation = "Calo/RawEvent"
                GaudiSequencer("OutputDSTSeq").Members += [caloRawBankCopy]
                    
                # then the Muon Raw Event
                muonRawBankCopy = RawEventSelectiveCopy('MuonRawBank')
                muonRawBankCopy.RawBanksToCopy = [ 'Muon' ]
                muonRawBankCopy.OutputRawEventLocation = "Muon/RawEvent"
                GaudiSequencer("OutputDSTSeq").Members += [muonRawBankCopy]

                # then the Rich Raw Event
                richRawBankCopy = RawEventSelectiveCopy('RichRawBank')
                richRawBankCopy.RawBanksToCopy = [ 'Rich' ]
                richRawBankCopy.OutputRawEventLocation = "Rich/RawEvent"
                GaudiSequencer("OutputDSTSeq").Members += [richRawBankCopy]
                    
                # and the rest
                allOtherRawBankCopy = RawEventSelectiveCopy('OtherRawEvent')
                allOtherRawBankCopy.RawBanksToRemove = trigBanks + ['Muon','Rich']
                allOtherRawBankCopy.OutputRawEventLocation = "Other/RawEvent"
                GaudiSequencer("OutputDSTSeq").Members += [allOtherRawBankCopy]
                    
                # Finally, remove knowledge of original RawEvent
                from Configurables import EventNodeKiller
                daqKiller = EventNodeKiller("DAQKiller")
                daqKiller.Nodes += [ "DAQ" ]
                GaudiSequencer("OutputDSTSeq").Members += [daqKiller]

            from Configurables import TrackToDST

            # Filter Best Track States to be written
            trackFilter = TrackToDST("FilterBestTrackStates")

            # Filter Muon Track States
            muonTrackFilter = TrackToDST("FilterMuonTrackStates")
            muonTrackFilter.TracksInContainer = "/Event/Rec/Track/Muon"

            from Configurables import ProcessPhase
            ProcessPhase("Output").DetectorList += [ "DST" ]
            GaudiSequencer("OutputDSTSeq").Members += [ trackFilter, muonTrackFilter ]

            if packType != "NONE":
                # Add the sequence to pack the DST containers
                packSeq = GaudiSequencer("PackDST")
                DstConf().PackSequencer = packSeq
                DstConf().AlwaysCreate  = True
                GaudiSequencer("OutputDSTSeq").Members += [ packSeq ]
                # Run the packers also on Lumi only events to write empty containers
                if handleLumi and self.getProp( "WriteLumi" ):
                    notPhysSeq = GaudiSequencer("NotPhysicsSeq")
                    notPhysSeq.Members += [ packSeq ]

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
            self.setOtherProps(DstConf(),["DataType","WriteFSR"])

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

    def configureCheck(self,expert):
        # "Check" histograms filled only with simulated data

        RecMoniConf().setProp( "CheckEnabled", True )

        if not self.isPropertySet("MCCheckSequence"):
            if expert:
                checkSeq = self.KnownExpertCheckSubdets
            else:
                checkSeq = self.KnownCheckSubdets
            self.MCCheckSequence = checkSeq
        else:
            for seq in self.getProp("MCCheckSequence"):
                if expert:
                    if seq not in self.KnownExpertCheckSubdets:
                        log.warning("Unknown subdet '%s' in MCCheckSequence"%seq)
                else:
                    if seq not in self.KnownCheckSubdets:
                        log.warning("Unknown subdet '%s' in MCCheckSequence"%seq)

        checkSeq = self.getProp("MCCheckSequence")
        from Configurables import ProcessPhase
        ProcessPhase("Check").DetectorList += checkSeq

        # Tracking handled inside TrackSys configurable
        TrackSys().setProp( "WithMC", True )

        if "MUON" in checkSeq :
            from MuonPIDChecker import ConfigureMuonPIDChecker as cmuon
            mydata =  self.getProp("DataType")
            mycheckconf = cmuon.ConfigureMuonPIDChecker(data = mydata)
            mycheckconf.configure(UseMC = True, HistosLevel = self.getProp("Histograms"))

        if "RICH" in checkSeq :
            # Unpacking RICH summaries
            from Configurables import DataPacking__Unpack_LHCb__MCRichDigitSummaryPacker_
            unp = DataPacking__Unpack_LHCb__MCRichDigitSummaryPacker_("MCRichDigitSummaryUnpack")
            from Configurables import GaudiSequencer
            GaudiSequencer("MCLinksUnpackSeq").Members += [unp]
            richMoniConf = RichRecQCConf(self.richMoniConfName)
            self.setOtherProps(richMoniConf, ["Histograms","Context","OutputLevel",
                                              "DataType","WithMC","Simulation"] )
            richMoniConf.setProp("MoniSequencer", GaudiSequencer("CheckRICHSeq"))

        if expert:
            # Data on Demand for MCParticle to MCHit association, needed by ST, IT, OT, Tr, Muon
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
                GaudiSequencer("CheckOTSeq").Members += ["OTTimeChecker"] # needs MCHits

            if "Tr" in  checkSeq :
                # Checking on the tracks in the "best" container - needs MCHits
                importOptions( "$TRACKSYSROOT/options/TrackChecking.opts" )

            if "CALO" in  checkSeq :
                GaudiSequencer("CheckCALOSeq").Members += [ "CaloDigit2MCLinks2Table", "CaloClusterMCTruth" ]
                importOptions( "$STDOPTS/PreloadUnits.opts" )
                importOptions( "$CALOMONIDSTOPTS/CaloChecker.opts" )
                # temporary workaround to bug #73392
                if not self.getProp("CaloPhotonChecker"):
                    caloPIDsChecker = GaudiSequencer("CaloPIDsChecker")
                    caloPIDsChecker.Members.remove("CaloPhotonChecker")

            if "PROTO" in checkSeq :
                from Configurables import GaudiSequencer
                self.setOtherProps(GlobalRecoChecks(),["OutputType"])
                GlobalRecoChecks().Sequencer = GaudiSequencer("CheckPROTOSeq")

    def _isReprocessing(self, inputType):
        return inputType in [ "XDST", "DST", "SDST" ]

    def _configureDBSnapshot(self):
        """
        Configure the database to use the online snapshot
        """
        tag = { "DDDB":     self.getProp('DDDBtag')
                , "LHCBCOND": self.getProp('CondDBtag')
                , "SIMCOND" : self.getProp('CondDBtag') 
                , "ONLINE"  : 'fake'
                }

        cdb = CondDB()
        cdb.Tags = tag
        cdb.setProp('IgnoreHeartBeat', True)
        cdb.setProp('EnableRunChangeHandler', True)
        self.setOtherProps( cdb, [ 'UseDBSnapshot',
                                   'DBSnapshotDirectory',
                                   'PartitionName' ])

        print "# WARNING using hack https://savannah.cern.ch/task/index.php?27329#comment45"
        from Configurables import RichRecSysConf
        rConf = RichRecSysConf("RichOfflineRec")
        rConf.Context = "HLT"
        from Configurables import RichRecQCConf
        rMoni = RichRecQCConf("OfflineRichMoni")
        rMoni.Context = "HLT"

    ## Apply the configuration
    def __apply_configuration__(self):

        GaudiKernel.ProcessJobOptions.PrintOff()

        # database hacking for online
        if self.getProp('UseDBSnapshot') : self._configureDBSnapshot()

        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        self.defineMonitors()
        self.setOtherProps(RecSysConf(),["Histograms","SpecialData","Context",
                                         "OutputType","DataType","Simulation"])
        self.setOtherProps(RecMoniConf(),["Histograms","Context","DataType","Simulation"])
        self.setOtherProps(TrackSys(),["DataType","Simulation"])

        if self.isPropertySet("RecoSequence") :
            self.setOtherProp(RecSysConf(),"RecoSequence")
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info( self )
        log.info( RecSysConf() )
        log.info( TrackSys() )
        log.info( RecMoniConf() )
        GaudiKernel.ProcessJobOptions.PrintOff()
