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
                            LumiAlgsConf, L0Conf, GlobalRecoChecks, CondDB,
                            CaloDigitConf, CaloMoniDstConf, CaloProcessor,
                            GlobalRecoConf, RawEventJuggler, DecodeRawEvent,
                            RawEventFormatConf)

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
                               GlobalRecoChecks,CaloDigitConf,CaloMoniDstConf,
                               RawEventJuggler, DecodeRawEvent,
                               RawEventFormatConf
                               ]

    ## Default init sequences
    DefaultInitSequence      = ["Brunel"]
    DefaultReproInitSequence = ["Reproc"] + DefaultInitSequence

    ## Known checking sequences, all run by default
    #KnownCheckSubdets       = ["Pat","RICH","MUON"]
    #KnownExpertCheckSubdets = KnownCheckSubdets+["TT","IT","OT","Tr","CALO","PROTO"]
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
       ,"DataType"        : "2015"
       ,"WithMC"          : False
       ,"Simulation"      : False
       ,"RecL0Only"       : False
       ,"InputType"       : "MDF"
       ,"DigiType"        : "Default"
       ,"OutputType"      : "DST"
       ,"PackType"        : "TES"
       ,"WriteFSR"        : True
       ,"WriteLumi"       : False
       ,"Histograms"      : "OfflineFull"
       ,"OutputLevel"     : INFO
       ,"NoWarnings"      : False
       ,"ProductionMode"  : False
       ,"OnlineMode"      : False
       ,"DatasetName"     : "Brunel"
       ,"DDDBtag"         : ""
       ,"CondDBtag"       : ""
       ,"MainSequence"    : None
       ,"InitSequence"    : None
       ,"RecoSequence"    : None
       ,"MCCheckSequence" : None
       ,"MCLinksSequence" : ["L0", "Unpack", "Tr","Calo"]
       ,"Monitors"        : []
       ,"SpecialData"     : []
       ,"Context"         : "Offline"
       ,"RawBanksToKill"  : None
       ,"VetoHltErrorEvents" : True
       ,"SkipTracking"    : False
       ,"Detectors"       : ['Velo', 'PuVeto', 'Rich1', 'Rich2', 'TT', 'IT', 'OT', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet', 'Tr']
       ,"SplitRawEventInput" : None #Where the raw event sits on the input
       ,"SplitRawEventOutput" : 4.1 #how to split the raw event
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
       ,'WriteFSR'     : """ Flags whether to write out an FSR """
       ,'WriteLumi'    : """ Flags whether to write out Lumi-only events to DST """
       ,'Histograms'   : """ Type of histograms. Can be one of self.KnownHistograms """
       ,'OutputLevel'  : """ The printout level to use (default INFO) """
       ,'NoWarnings'   : """ OBSOLETE, kept for Dirac compatibility. Please use ProductionMode """
       ,'ProductionMode'  : """ Enables special settings for running in production """
       ,"OnlineMode"      : """ Enables special settings for running online """
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
       ,"VetoHltErrorEvents" : """Do not reconstruct events that have been flagged as error by Hlt"""
       ,'SkipTracking' : """ Read the tracks from the input, do not redo them """
       ,"Detectors"    : """List of detectors""" 
       , "SplitRawEventInput" : "How is the even split up in the input? Default 'None' doesn't reset any locations. Other values propagate to RawEventJuggler() and DecodeRawEvent()."
       , "SplitRawEventOutput" : "How to split the raw event up, versions are defined in the RawEventFormat DB, and RawEventCompat/RawEventJuggler is used for the splitting, default 2.0 the split type for Reco14/Stripping20-style DSTs"
        # only use for Online running
        ,"UseDBSnapshot" : """Use a snapshot for velo position and rich pressure"""
        ,"PartitionName" : """Name of the partition when running (needed to find DB: '', 'FEST', or 'LHCb'"""
        ,"DBSnapshotDirectory" : """Local Directory where the snapshot is"""
        }

    KnownInputTypes  = [ "MDF",  "DST", "XDST", "DIGI" ]
    KnownHistograms  = [ "None", "Online", "OfflineExpress", "OfflineFull", "Expert" ]
    Run1DataTypes = [ "2008", "2009", "2010", "2011", "2012", "2013", "MC09" ]

    def defineGeometry(self):
        # DIGI is always simulation, as is usage of MC truth!
        if self.getProp( "WithMC" ) or self.getProp( "InputType" ).upper() == 'DIGI':
            self.setProp( "Simulation", True )

        # Check that detector list does not contain non-upgrade detectors if in upgrade mode (can crash later)
        if( self.getProp("DataType") is "Upgrade"):
            for det in self.getProp("Detectors"):
                if det in ['Velo', 'TT', 'OT', 'IT']:
                    log.warning("You are running with non-upgrade detector %s in upgrade mode."%det)
                    

        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","Simulation","Detectors"])

        # Set list of detectors in DstConf
        dstConfDetList = []
        simConfDetList = []
        for det in self.getProp("Detectors"):
            if det in ['Rich1', 'Rich2', 'Rich1Pmt', 'Rich2Pmt']:
                if "Rich" not in dstConfDetList:
                    dstConfDetList.append("Rich")
                if "Rich" not in simConfDetList:                    
                    simConfDetList.append("Rich")
            elif det in ['Prs', 'Spd', 'Ecal', 'Hcal']:
                simConfDetList.append(det)
                if "Calo" not in dstConfDetList:
                    dstConfDetList.append("Calo")
            else:
                dstConfDetList.append(det)
                simConfDetList.append(det)
        DstConf().setProp("Detectors", dstConfDetList)
        from Configurables import SimConf
        SimConf().setProp("Detectors", simConfDetList)

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

        # HC does not exist in Run 1, so use appropriate split output
        if self.getProp( "DataType" ) in self.Run1DataTypes :
            if not self.isPropertySet( "SplitRawEventOutput" ) :
                self.setProp( "SplitRawEventOutput", 4.0 )

        # Online mode
        if self.getProp( "OnlineMode" ) :
            if not self.isPropertySet("Histograms") :
                self.setProp("Histograms","Online")
            if not CondDB().isPropertySet("Online") :
                CondDB().setProp("Online", True)

        inputType = self.getProp( "InputType" ).upper()
        if inputType not in self.KnownInputTypes:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "OutputType" ).upper()

        histOpt = self.getProp("Histograms")
        if histOpt not in self.KnownHistograms:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)

        withMC = self.getProp("WithMC")
        if withMC:
            if inputType in [ "MDF" ]:
                log.warning( "WithMC = True, but InputType = '%s'. Forcing WithMC = False"%inputType )
                withMC = False # Force it, MDF never contains MC truth
            if outputType in [ "RDST" ]:
                log.warning( "WithMC = True, but OutputType = '%s'. Forcing WithMC = False"%inputType )
                withMC = False # Force it, RDST never contains MC truth

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

        # Top level configuration for skipping tracking
        if self.getProp("SkipTracking"):
            if inputType in ["MDF", "DIGI" ]:
                raise RuntimeError( "Cannot skip tracking if tracks not present on the input file" )
            if withMC:
                raise RuntimeError( "SkipTracking not (yet) supported for simulation input" )
            if( self.getProp("DataType") is "Upgrade"):
                raise RuntimeError( "SkipTracking not (yet) supported for Upgrade configurations" )
            ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ] # to decode the tracks from the DST
            DstConf().setProp("EnableUnpack", ["Tracking"] )

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
            GaudiSequencer("MCLinksUnpackSeq").Members += [ "UnpackMCParticle"]

            # particle gun uses MCVertex to fake a reconstructed one
            # unpacking again would lead to crash
            if "pGun" not in self.getProp("SpecialData"):
                GaudiSequencer("MCLinksUnpackSeq").Members += [ "UnpackMCVertex" ]
            else:
                # Cannot run trigger on pGun events...
                L0Conf().EnsureKnownTCK=False
            
            GaudiSequencer("MCLinksTrSeq").Members += [ "TrackAssociator" ]
            GaudiSequencer("MCLinksCaloSeq").Members += [ "CaloDigit2MCLinks2Table", "CaloClusterMCTruth", "CaloHypoMCTruth" ]

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
            
        #reconfigure decoders to point to default location, if required!
        if DecodeRawEvent().isPropertySet("OverrideInputs") and DecodeRawEvent().getProp("OverrideInputs") is not None:
            #do nothing, it's been configured by someone else!
            pass
        elif self.isPropertySet("SplitRawEventInput") and self.getProp("SplitRawEventInput") is not None:
            #print "WAAAAH Overriding RawEvent Locations"
            DecodeRawEvent().setProp("OverrideInputs",self.getProp("SplitRawEventInput"))
        elif inputType in [ "MDF", "DIGI", "XDIGI" ]:
            #set to the default of what comes out of Moore
            DecodeRawEvent().setProp("OverrideInputs","Moore")
        #remember that the default is a long list of locations,
        #starting with places which only exist _after_ brunel has run!

        # Following needed to build RecSummary, even if tracking is skipped.
        if self.getProp("SkipTracking"):
            from TrackSys import RecoTracking
            RecoTracking.DecodeTracking(["FastVelo"])

        
        # Code  lifted from TrackSys/RecoTracking.py, to be repackaged
#        if self.getProp("SkipTracking"):
#            from DAQSys.Decoders import DecoderDB
#            from DAQSys.DecoderClass import decodersForBank
#            decs=[]
            #clone an existing algorithm, in order to create both the full
            #and the partial clusters
#            vdec=DecoderDB["DecodeVeloRawBuffer/createBothVeloClusters"]
            #set as active to make sure nobody tries to use the DoD service along side...
#            vdec.Active=True
#            globalCuts = TrackSys().getProp("GlobalCuts")
#            if( "Velo" in globalCuts ) :
#                vdec.Properties["MaxVeloClusters"] =  globalCuts["Velo"]
#                decs=decs+[vdec]
#            decs=decs+decodersForBank(DecoderDB,"TT")
#            decs=decs+decodersForBank(DecoderDB,"IT")
#            GaudiSequencer("RecoDecodingSeq").Members += [d.setup() for d in decs ]
    
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
        if not SequencerTimerTool().isPropertySet( "OutputLevel" ):
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
            
        # Hlt decreports decoders
        from DAQSys.Decoders import DecoderDB
        from Configurables import LoKi__HDRFilter, AddToProcStatus
        hltStages = ('Hlt1',) if self.getProp('OnlineMode') else ('Hlt1', 'Hlt2')
        hltDecoders = []
        hltFilters = []
        for stage in hltStages:
            decoder = DecoderDB["HltDecReportsDecoder/%sDecReportsDecoder" % stage].setup()
            filterCode = "HLT_PASS_RE('%s(?!ErrorEvent).*Decision')" % stage
            # identifies events that are not of type ErrorEvent
            hltFilter = LoKi__HDRFilter('%sErrorFilter' % stage, Code = filterCode, Location = decoder.OutputHltDecReportsLocation )   # the filter
            hltDecoders += [decoder]            # decode DecReports
            hltFilters  += [decoder, hltFilter] # and apply filter

        # Do not process events flagged as error in Hlt, but still write procstatus
        if vetoHltErrorEvents:
            """
            By Patrick Koppenburg, 16/6/2011
            """
            # Make a sequence that selects HltErrorEvents
            hltfilterSeq = GaudiSequencer("HltFilterSeq")
            if handleLumi: hltfilterSeq.Members = [ physFilter ]       # protect against lumi (that doesn't have decreports)
            hltfilterSeq.Members += hltFilters

            # Sequence to be executed if HltErrorFilter is failing to set ProcStatus
            hlterrorSeq = GaudiSequencer("HltErrorSeq", ModeOR = True, ShortCircuit = True) # anti-logic
            addToProc = AddToProcStatus("HltErrorProc", Reason = "HltError", Subsystem = "Hlt") # write a procstatus
            hlterrorSeq.Members += [hltfilterSeq, addToProc]           # only run if hltfilterSeq fails
            brunelSeq.Members   += [hlterrorSeq]                       # add this sequece to Brunel _before_ physseq
            physicsSeq.Members  += [hltfilterSeq]                      # take good events in physics seq

        # Convert Calo ReadoutStatus to ProcStatus
        caloBanks=GaudiSequencer("CaloBanksHandler")
        caloDetectors = [det for det in ['Spd','Prs','Ecal','Hcal'] if det in self.getProp("Detectors")]
        CaloDigitConf(ReadoutStatusConvert=True,Sequence=caloBanks,Detectors=caloDetectors)
        physicsSeq.Members += [caloBanks]
        
        # Decode L0 (and HLT if not already done)
        trgSeq = GaudiSequencer("DecodeTriggerSeq")
        l0TrgSeq = GaudiSequencer("L0TriggerSeq")
        if self.getProp( "DataType" ) not in [ "2008", "2009" ]:
            trgSeq.Members += hltDecoders
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
        if not recInit.isPropertySet( "MemoryPurgeLimit" ): recInit.MemoryPurgeLimit = 2000 * 1000 # 2GB

        # count events
        from Configurables import EventCountHisto
        evtC = EventCountHisto("BrunelEventCount")
        evtC.HistoTopDir = "Brunel/"
        GaudiSequencer("InitBrunelSeq").Members += [ evtC ]

        # kill some RAW banks
        banksToKill = []
        if self.isPropertySet( "RawBanksToKill" ):
            banksToKill  = self.getProp( "RawBanksToKill" )
        if ("2009" == self.getProp("DataType")) and (inputType in ["MDF"]) and not self.getProp("Simulation") :
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
        self.setOtherProps(RecSysConf(), ["OutputLevel","Detectors"])
        self.setOtherProps(RecMoniConf(),["OutputLevel","Detectors"])

        # New NoSPDPRS switches
        noSPDPRS = False
        if [det for det in ['Spd', 'Prs'] if det not in self.getProp("Detectors")]:
            noSPDPRS = True
        # only set properties if no use RecoSequence is defined or if it contains 'PROTO'
        # not that 'PROTO' alone is not sufficient to run the Calo reconstruction, but a requirement
        if ( not self.isPropertySet("RecoSequence") or "PROTO" in self.getProp("RecoSequence") ):
            CaloProcessor().setProp("NoSpdPrs", noSPDPRS)
            GlobalRecoConf().setProp("NoSpdPrs", noSPDPRS)


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
            killer = EventNodeKiller()
            killer.Nodes += [ "Raw", "Link/Rec" ]
            if self.getProp("SkipTracking"):
                killer.Nodes += [ "pRec/Rich", "pRec/Muon", "pRec/Calo", "pRec/Track/Muon", "pRec/ProtoP" ]
            else:
                killer.Nodes += [ "pRec", "Rec" ]
            InitReprocSeq.Members.append( killer )
            ### see configureOutput to see how the remainder of the juggler is configured
            
        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";

    def configureOutput(self, dstType, withMC, handleLumi):
        """
        Set up output stream
        """
        
        if dstType in [ "XDST", "DST", "LDST", "RDST" ]:
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

            if dstType in ["DST","XDST","LDST"] and packType not in ["MDF"]:
                jseq=GaudiSequencer("RawEventSplitSeq")
                #################################
                # Split the Raw Event for the DST
                # Use the RawEventJuggler.
                # Not delegated to DSTConf.
                # Some information must be shared with DSTConf
                #################################
                juggler=RawEventJuggler()
                juggler.Sequencer=jseq
                dstseq=GaudiSequencer("OutputDSTSeq")
                dstseq.Members.append(jseq)
                # Set the output version if not already overwritten
                if juggler.isPropertySet("Output") and juggler.getProp("Output") is not None:
                    #it's already set
                    pass
                else:
                    juggler.setProp("Output",self.getProp("SplitRawEventOutput"))
                #set the input version, could come from several places
                if self.isPropertySet("SplitRawEventInput") and self.getProp("SplitRawEventInput") is not None:
                    #if set, take it from Brunel()
                    juggler.setProp("Input",self.getProp("SplitRawEventInput"))
                    #otherwise use the setting of the juggler if it is set
                elif juggler.isPropertySet("Input") and juggler.getProp("Input") is not None:
                    pass
                    #else find it from DecodeRawEvent
                elif DecodeRawEvent().isPropertySet("OverrideInputs") and DecodeRawEvent().getProp("OverrideInputs") is not None:
                    juggler.setProp("Input",DecodeRawEvent().getProp("OverrideInputs"))
                    #else if I'm input with a DST, assume it is a Stripping20 type
                elif self._isReprocessing(self.getProp("InputType")):
                    juggler.setProp("Input",2.0)
                else:
                    #or set the default to whatever comes out of Moore by default
                    juggler.setProp("Input","Moore")
                
                #share information from the Juggler with DSTConf
                #always write out to where the Juggler asked!    
                DstConf().setProp("SplitRawEventOutput", juggler.getProp("Output"))
                #or else the default in the juggler is used, should be 0.0
                #TODO, handle the turned off Calo, shouldn't actually be a problem...
                from RawEventCompat.Configuration import _checkv
                from Configurables import RawEventFormatConf
                RawEventFormatConf().loadIfRequired()
                if juggler.getProp("Input")!=juggler.getProp("Output"):
                    if (juggler.getProp("Input") is None or juggler.getProp("Output")) is None or (_checkv(juggler.getProp("Input")))!=(_checkv(juggler.getProp("Output"))):
                        juggler.KillExtraBanks=True
                        juggler.KillExtraNodes=True
                        #really kill /Event/DAQ to prevent it re-appearing!
                        juggler.KillExtraDirectories=True
            
            from Configurables import TrackToDST

            # Filter Best Track States to be written
            trackFilter = TrackToDST("FilterBestTrackStates")
            from Configurables import ProcessPhase
            ProcessPhase("Output").DetectorList += [ "DST" ]
            GaudiSequencer("OutputDSTSeq").Members += [ trackFilter ]
            
            ### for 2015, filter the fitted Velo tracks for PV
            if( self.getProp("DataType") is "2015" ):
                fittedVeloTracksFilter = TrackToDST("FilterFittedVeloTrackStates", veloStates = [])
                fittedVeloTracksFilter.TracksInContainer = "Rec/Track/FittedHLT1VeloTracks"
                GaudiSequencer("OutputDSTSeq").Members += [ fittedVeloTracksFilter ]
           
            if "Muon" in self.getProp("Detectors"):
                # Filter Muon Track States            
                muonTrackFilter = TrackToDST("FilterMuonTrackStates")
                muonTrackFilter.TracksInContainer = "/Event/Rec/Track/Muon"
                GaudiSequencer("OutputDSTSeq").Members += [ muonTrackFilter ]


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
            #In case we didn't juggle the raw event,
            #We should write out the same as the input type!
            if not DstConf().isPropertySet("SplitRawEventOutput"):
                if self.isPropertySet("SplitRawEventInput") and self.getProp("SplitRawEventInput") is not None:
                    DstConf().setProp("SplitRawEventOutput",self.getProp("SplitRawEventInput"))
                elif DecodeRawEvent().isPropertySet("OverrideInputs") and DecodeRawEvent().getProp("OverrideInputs") is not None:
                    DstConf().setProp("SplitRawEventOutput",DecodeRawEvent().getProp("SplitRawEventInput"))
            
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

        KnownCheckSubdets = ["Pat"]

        # CheckSubdets
        if [det for det in ['Rich1', 'Rich2', 'Rich1Pmt', 'Rich2Pmt'] if det in self.getProp("Detectors")]:
            KnownCheckSubdets.append("RICH")
        if [det for det in ['Muon', 'MuonNoM1'] if det in self.getProp("Detectors")]:
            KnownCheckSubdets.append("MUON")

        # Expert Check Subdets
        KnownExpertCheckSubdets = [] + KnownCheckSubdets
        
        if [det for det in ['TT'] if det in self.getProp("Detectors")]:
            KnownExpertCheckSubdets.append("TT")

        tmpExpertSubdets = [det for det in ['IT', 'OT'] if det in self.getProp("Detectors")]
        if tmpExpertSubdets:
            for det in tmpExpertSubdets:
                KnownExpertCheckSubdets.append(det)
        if [det for det in ['Tr'] if det in self.getProp("Detectors")]:
            KnownExpertCheckSubdets.append('Tr')
        if [det for det in ['Spd', 'Prs', 'Ecal', 'Hcal'] if det in self.getProp("Detectors")]:
            KnownExpertCheckSubdets.append("CALO")

        KnownExpertCheckSubdets.append("PROTO")


        RecMoniConf().setProp( "CheckEnabled", True )

        if not self.isPropertySet("MCCheckSequence"):
            if expert:
                checkSeq = KnownExpertCheckSubdets
            else:
                checkSeq = KnownCheckSubdets
            self.MCCheckSequence = checkSeq
        else:
            for seq in self.getProp("MCCheckSequence"):
                if expert:
                    if seq not in KnownExpertCheckSubdets:
                        log.warning("Unknown subdet '%s' in MCCheckSequence"%seq)
                else:
                    if seq not in KnownCheckSubdets:
                        log.warning("Unknown subdet '%s' in MCCheckSequence"%seq)

        checkSeq = self.getProp("MCCheckSequence")
        from Configurables import ProcessPhase
        ProcessPhase("Check").DetectorList += checkSeq

        # Tracking handled inside TrackSys configurable
        TrackSys().setProp( "WithMC", True )

        if ("MUON" in checkSeq) and ("Muon" in self.getProp("Detectors")):
            from MuonPIDChecker import ConfigureMuonPIDChecker as cmuon
            mydata =  self.getProp("DataType")
            mycheckconf = cmuon.ConfigureMuonPIDChecker(data = mydata)
            mycheckconf.configure(UseMC = True, HistosLevel = self.getProp("Histograms"))

        if ("RICH" in checkSeq) and [det for det in ['Rich1', 'Rich2', 'Rich1Pmt', 'Rich2Pmt'] if det in self.getProp("Detectors")] :
            from Configurables import GaudiSequencer
            richMoniConf = RichRecQCConf(self.richMoniConfName)
            self.setOtherProps(richMoniConf, ["Histograms","Context","OutputLevel","OnlineMode",
                                              "DataType","WithMC","Simulation"] )
            richMoniConf.setProp("MoniSequencer", GaudiSequencer("CheckRICHSeq"))

        if expert:
            # Data on Demand for MCParticle to MCHit association, needed by ST, IT, OT, Tr, Muon
            # Different for upgrade and non-upgrade
            if( self.getProp("DataType") is "Upgrade" ):
                importOptions( "$ASSOCIATORSROOT/options/MCParticleToMCHitUpgrade.py" )
            else:
                importOptions( "$ASSOCIATORSROOT/options/MCParticleToMCHit.py" )
                
            # Allow multiple files open at once (SIM,DST,DIGI etc.)
            IODataManager().AgeLimit += 1

            if (
                ("TT" in checkSeq) and
                (hasattr(self, "Detectors")) and
                ("TT" in self.getProp("Detectors"))
                ):
                from Configurables import ( STEffChecker, MCParticleSelector )
                from GaudiKernel.SystemOfUnits import GeV
                effCheck = STEffChecker("TTEffChecker")
                effCheck.FullDetail = True
                effCheck.addTool(MCParticleSelector)
                effCheck.MCParticleSelector.zOrigin = 50.0
                effCheck.MCParticleSelector.pMin = 1.0*GeV
                effCheck.MCParticleSelector.betaGammaMin = 1.0
                from Configurables import GaudiSequencer
                GaudiSequencer("CheckTTSeq").Members += [effCheck]

            if (
                ("IT" in checkSeq) and
                (hasattr(self, "Detectors")) and
                ("IT" in self.getProp("Detectors"))
                ):
                from Configurables import ( STEffChecker, MCParticleSelector )
                from GaudiKernel.SystemOfUnits import GeV
                effCheck = STEffChecker("ITEffChecker")
                effCheck.FullDetail = True
                effCheck.addTool(MCParticleSelector)
                effCheck.MCParticleSelector.zOrigin = 50.0
                effCheck.MCParticleSelector.pMin = 1.0*GeV
                effCheck.MCParticleSelector.betaGammaMin = 1.0
                effCheck.DetType = "IT"
                from Configurables import GaudiSequencer
                GaudiSequencer("CheckITSeq").Members += [effCheck]

            if (
                ("OT" in checkSeq) and
                (hasattr(self, "Detectors")) and
                ("OT" in self.getProp("Detectors"))
                ):
                GaudiSequencer("CheckOTSeq").Members += ["OTTimeChecker"] # needs MCHits

            if "Tr" in  checkSeq :
                # Checking on the tracks in the "best" container - needs MCHits
                # not active for upgrade for the moment
                if( self.getProp("DataType") is "Upgrade" ):
                    pass
                else:
                    importOptions( "$TRACKSYSROOT/options/TrackChecking.opts" )

            if "CALO" in  checkSeq :
                import GaudiKernel.ProcessJobOptions
                GaudiKernel.ProcessJobOptions.PrintOn()
                from Configurables import GaudiSequencer
                ccseq=GaudiSequencer("CaloCheckers")
                noSPDPRS = False
                if [det for det in ['Spd', 'Prs'] if det in self.getProp("Detectors")]:
                    noSPDPRS = True
                caloMoni = CaloMoniDstConf( CheckerSequence  = ccseq,
                                            OutputLevel = self.getProp('OutputLevel'),
                                            Context = 'Offline',
                                            NoSpdPrs = noSPDPRS)
                caloMoni.printConf()

                GaudiKernel.ProcessJobOptions.PrintOff()
                GaudiSequencer("CheckCALOSeq").Members = [ "CaloDigit2MCLinks2Table", "CaloClusterMCTruth" ,ccseq] 

            if "PROTO" in checkSeq :
                from Configurables import GaudiSequencer
                self.setOtherProps(GlobalRecoChecks(),["OutputLevel"])
                exSeq = GaudiSequencer("ExpertProtoMoni")
                GaudiSequencer("CheckPROTOSeq").Members += [exSeq]
                GlobalRecoChecks().Sequencer = exSeq
                

    def _isReprocessing(self, inputType):
        return inputType in [ "XDST", "DST", "LDST" ]

    def _configureDBSnapshot(self):
        """
        Configure the database to use the online snapshot
        """
        tag = { "DDDB":     self.getProp('DDDBtag')
                , "LHCBCOND": self.getProp('CondDBtag')
                , "SIMCOND" : self.getProp('CondDBtag') 
                , "ONLINE"  : 'fake'
                }

        # https://savannah.cern.ch/bugs/?94454#comment12
        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True

        cdb = CondDB()
        cdb.Tags = tag
        cdb.setProp('IgnoreHeartBeat', True)
        cdb.setProp('EnableRunChangeHandler', True)
        self.setOtherProps( cdb, [ 'UseDBSnapshot',
                                   'DBSnapshotDirectory',
                                   'PartitionName' ])

        if [det for det in ['Rich1', "Rich2", "Rich1Pmt", "Rich2Pmt"] if det in self.getProp("Detectors")]:
            log.warning( "using hack https://savannah.cern.ch/task/index.php?27329#comment45" )
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
                                         "OutputType","DataType","Simulation","OnlineMode","SkipTracking"])
        self.setOtherProps(RecMoniConf(),["Histograms","Context","DataType","Simulation","OnlineMode","SkipTracking"])
        self.setOtherProps(TrackSys(),["DataType","Simulation"])

        if self.isPropertySet("RecoSequence") :
            self.setOtherProp(RecSysConf(),"RecoSequence")
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info( self )
        log.info( RecSysConf() )
        log.info( TrackSys() )
        log.info( RecMoniConf() )
        GaudiKernel.ProcessJobOptions.PrintOff()
