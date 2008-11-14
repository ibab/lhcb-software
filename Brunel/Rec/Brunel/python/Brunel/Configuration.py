## @package Brunel
#  High level configuration tools for Brunel
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.32 2008-11-14 17:10:25 jonrob Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration  import *
from RecSys.Configuration     import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( ProcessPhase, GaudiSequencer )

## @class Brunel
#  Configurable for Brunel application
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class Brunel(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ LHCbApp, TrackSys, RecSysConf ]
    
    # Steering options
    __slots__ = {
        "EvtMax":          -1 # Maximum number of events to process
       ,"SkipEvents":   0     # events to skip
       ,"PrintFreq":    1     # The frequency at which to print event numbers
       ,"WithMC":       False # set to True to use MC truth
       ,"UseSimCond":   False # set to True to use SimCond
       ,"RecL0Only":    False # set to True to reconstruct only L0-yes events
       ,"InputType":    "MDF" # or "DIGI" or "ETC" or "RDST" or "DST"
       ,"OutputType":   "DST" # or "RDST" or "NONE"
       ,"ExpertHistos": False # set to True to write out expert histos
       ,"NoWarnings":   False # suppress all messages with MSG::WARNING or below 
       ,"DatasetName":  ""    # string used to build file names
       ,"DDDBtag":      "2008-default" # geometry database tag
       ,"CondDBtag":    "2008-default" # conditions database tag
       ,"UseOracle": False  # if False, use SQLDDDB instead
       ,"MainSequence": [ "ProcessPhase/Init",
                          "ProcessPhase/Reco",
                          "ProcessPhase/Moni",
                          "ProcessPhase/MCLinks",
                          "ProcessPhase/Check",
                          "ProcessPhase/Output" ]
       ,"McCheckSequence": ["Pat","RICH","MUON"] # The default MC Check sequence
       ,"McLinksSequence": [ "L0", "Unpack", "Tr" ] # The default MC Link sequence
       ,"InitSequence": ["Reproc", "Brunel", "Calo"] # The default init sequence
       ,"MoniSequence": ["CALO","RICH","MUON","VELO","Track","ST"]    # The default Moni sequence
       ,"Monitors": []        # list of monitors to execute, see KnownMonitors
        # Following are options forwarded to RecSys
       ,"RecoSequence"   : [] # The Sub-detector reconstruction sequencing. See RecSys for default
       ,"SpecialData"    : [] # Various special data processing options. See KnownSpecialData for all options
        # Following are options forwarded to TrackSys
       ,"ExpertTracking": []  # list of expert Tracking options, see KnownExpertTracking
       ,"Context":     "Offline" # The context within which to run
        }

    def defineGeometry(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["CondDBtag","DDDBtag","UseOracle"]) 
        if LHCbApp().getProp("DDDBtag").find("DC06") != -1 :
            ApplicationMgr().Dlls += [ "HepMCBack" ]

    def defineEvents(self):
        
        evtMax = self.getProp("EvtMax")
        if hasattr(LHCbApp(),"EvtMax"):
            print "# LHCbApp().EvtMax already defined, ignoring Brunel().EvtMax"
        else:
            LHCbApp().EvtMax = evtMax

        skipEvents = self.getProp("SkipEvents")
        if skipEvents > 0 :
            if hasattr(LHCbApp(),"SkipEvents"):
                print "# LHCbApp().skipEvents already defined, ignoring Brunel().skipEvents"
            else:
                LHCbApp().skipEvents = skipEvents

    def defineOptions(self):

        inputType = self.getProp( "InputType" ).upper()
        if inputType not in [ "MDF", "DST", "DIGI", "ETC", "RDST" ]:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "OutputType" ).upper()
        if outputType not in [ "NONE", "DST", "RDST" ]:
            raise TypeError( "Invalid outputType '%s'"%outputType )

        withMC = self.getProp("WithMC")
        if inputType in [ "MDF", "RDST" ]:
            withMC = False # Force it, MDF and RDST never contain MC truth
        if outputType == "RDST":
            withMC = False # Force it, RDST never contains MC truth

        useSimCond = self.getProp("UseSimCond")
        if outputType == "DIGI":
            useSimCond = True # Force it, DIGI always from MC

        self.configureInput( inputType )

        self.configureOutput( outputType, withMC )

        # Set up monitoring (i.e. not using MC truth)
        ProcessPhase("Moni").DetectorList += self.getProp("MoniSequence")
        importOptions("$BRUNELOPTS/BrunelMoni.py") # Filled in all cases
        if not withMC:
            # Add here histograms to be filled only with real data 
            from RichRecQC.Configuration import RichRecQCConf
            self.setOtherProps(RichRecQCConf(),["ExpertHistos","Context"])
            RichRecQCConf().MoniSequencer = GaudiSequencer("MoniRICHSeq")

        # Setup up MC truth processing and checking
        if withMC:
            ProcessPhase("MCLinks").DetectorList += self.getProp("McLinksSequence")
            # Unpack Sim data
            GaudiSequencer("MCLinksUnpackSeq").Members += [ "UnpackMCParticle",
                                                            "UnpackMCVertex" ]
            GaudiSequencer("MCLinksTrSeq").Members += [ "TrackAssociator" ]

            # "Check" histograms filled only with simulated data 
            ProcessPhase("Check").DetectorList += self.getProp("McCheckSequence")
            # Tracking
            importOptions("$TRACKSYSROOT/options/PatChecking.opts")
            if "veloOpen" in self.getProp( "SpecialData" ) :
                GaudiSequencer("CheckPatSeq").Members.remove("TrackAssociator/AssocVeloRZ")
                GaudiSequencer("CheckPatSeq").Members.remove("TrackAssociator/AssocDownstream")
                GaudiSequencer("CheckPatSeq").Members.remove("TrackEffChecker/VeloRZ")
                GaudiSequencer("CheckPatSeq").Members.remove("TrackEffChecker/Downstream")
            # Muon
            importOptions("$MUONPIDCHECKERROOT/options/MuonPIDChecker.opts")
            # RICH
            from RichRecQC.Configuration import RichRecQCConf
            self.setOtherProps(RichRecQCConf(),["ExpertHistos","Context"])
            RichRecQCConf().MoniSequencer = GaudiSequencer("CheckRICHSeq")

        # Setup L0 filtering if requested, runs L0 before Reco
        if self.getProp("RecL0Only"):
            ProcessPhase("Init").DetectorList.append("L0")
            importOptions( "$L0DUROOT/options/L0Sequence.opts" )
            GaudiSequencer("InitL0Seq").Members += [ "GaudiSequencer/L0FilterFromRawSeq" ]

    def defineHistos(self):
        """
        Save histograms. If expert, fill and save also the expert histograms
        """
        ExpertHistos = self.getProp("ExpertHistos")
        if ExpertHistos:
            RecSysConf().setProp( "ExpertHistos", ExpertHistos )
            importOptions( "$BRUNELOPTS/ExpertCheck.opts" )
            IODataManager().AgeLimit += 1

    def defineMonitors(self):
        
        # get all defined monitors
        monitors = self.getProp("Monitors") + LHCbApp().getProp("Monitors")
        # pass to LHCbApp any monitors not dealt with here
        LHCbApp().setProp("Monitors", monitors)

    def configureInput(self, inputType):
        """
        Tune initialisation according to input type
        """

        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        # By default, Brunel only needs to open one input file at a time
        IODataManager().AgeLimit = 0
        
        if inputType in [ "DST", "RDST", "ETC" ]:
            # Kill knowledge of any previous Brunel processing
            from Configurables import ( TESCheck, EventNodeKiller )
            InitReprocSeq = GaudiSequencer( "InitReprocSeq" )
            InitReprocSeq.Members.append( "TESCheck" )
            TESCheck().Inputs = ["Link/Rec/Track/Best"]
            # in case above container is not on input (e.g. RDST)
            TESCheck().Stop = False
            TESCheck().OutputLevel = ERROR
            InitReprocSeq.Members.append( "EventNodeKiller" )
            EventNodeKiller().Nodes = [ "Rec", "Raw", "Link/Rec" ]

        # Read ETC selection results into TES for writing to DST
        if inputType == "ETC":
            from Configurables import ReadStripETC
            GaudiSequencer("InitBrunelSeq").Members.append("ReadStripETC/TagReader")
            ReadStripETC("TagReader").CollectionName = "TagCreator"
            IODataManager().AgeLimit += 1

        if inputType not in [ "DIGI", "DST" ]:
            # In case raw data resides in MDF file
            EventPersistencySvc().CnvServices.append("LHCb::RawDataCnvSvc")

        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";

        # Convert Calo 'packed' banks to 'short' banks if needed
        GaudiSequencer("InitCaloSeq").Members += ["GaudiSequencer/CaloBanksHandler"]
        importOptions("$CALODAQROOT/options/CaloBankHandler.opts")


    def configureOutput(self, dstType, withMC):
        """
        Set up output stream
        """
        if dstType in [ "DST", "RDST" ]:
            if( dstType == "DST" ):
                importOptions( "$BRUNELOPTS/DstContent.opts" )
            else:
                importOptions( "$BRUNELOPTS/rDstContent.opts" )
            dstWriter = OutputStream( "DstWriter" )
            ApplicationMgr().OutStream.append( dstWriter )
            dstWriter.Preload = False
            dstWriter.RequireAlgs += ["Reco"] # Write only if Rec phase completed

        # Add MC truth to DST
        if withMC and dstType == "DST":
            importOptions("$STDOPTS/MCDstContent.opts")
            
        # Always write an ETC if ETC input
        if self.getProp( "InputType" ).upper() == "ETC":
            ApplicationMgr().ExtSvc.append("TagCollectionSvc/EvtTupleSvc")
            ApplicationMgr().OutStream.append("GaudiSequencer/SeqTagWriter")
            importOptions( "$BRUNELOPTS/DefineETC.opts" )

        # Modify printout defaults
        importOptions("$BRUNELOPTS/BrunelMessage.opts")
        if self.getProp( "NoWarnings" ):
            importOptions( "$BRUNELOPTS/SuppressWarnings.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if self.getProp( "RecL0Only" ): outputName += '-L0Yes'
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        outputType = self.getProp("OutputType").lower()
        return outputName + '.' + outputType

    def histosName(self):
        
        histosName   = self.getProp("DatasetName")
        if self.getProp( "RecL0Only" ): histosName += '-L0Yes'
        if ( self.evtMax() > 0 ): histosName += '-' + str(self.evtMax()) + 'ev'
        ExpertHistos = self.getProp("ExpertHistos")
        if ExpertHistos     : histosName += '-expert'
        histosName += '-histos.root'
        return histosName
    
    def evtMax(self):
        
        if hasattr(ApplicationMgr(),"EvtMax"):
            return getattr(ApplicationMgr(),"EvtMax")
        else:
            return ApplicationMgr().getDefaultProperties()["EvtMax"]

    ## Apply the configuration
    def applyConf(self):
        
        GaudiKernel.ProcessJobOptions.PrintOff()
        self.setOtherProp(TrackSys(),"ExpertTracking")
        self.setOtherProps(RecSysConf(),["SpecialData","RecoSequence"])
        brunelSeq = GaudiSequencer("BrunelSequencer")
        ApplicationMgr().TopAlg = [ brunelSeq ]
        brunelSeq.Members += self.getProp("MainSequence")
        ProcessPhase("Init").DetectorList += self.getProp("InitSequence")
        GaudiSequencer("InitBrunelSeq").Members += [ "RecInit/BrunelInit" ]
        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        self.defineHistos()
        self.defineMonitors()
        from Configurables import RecInit
        RecInit("BrunelInit").PrintFreq = self.getProp("PrintFreq")
        # Use SIMCOND for Simulation, if not DC06
        #if self.getProp("UseSimCond") and LHCbApp().getProp("CondDBtag").find("DC06") == -1:
        #    from Configurables import CondDBCnvSvc
        #    CondDBCnvSvc( CondDBReader = allConfigurables["SimulationCondDBReader"] )
