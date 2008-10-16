
## @package Brunel
#  High level configuration tools for Brunel
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.26 2008-10-16 16:07:13 cattanem Exp $"
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

    # Steering options
    __slots__ = {
        "EvtMax":          -1 # Maximum number of events to process
       ,"skipEvents":   0     # events to skip
       ,"printFreq":    1     # The frequency at which to print event numbers
       ,"withMC":       False # set to True to use MC truth
       ,"recL0Only":    False # set to True to reconstruct only L0-yes events
       ,"inputType":    "MDF" # or "DIGI" or "ETC" or "RDST" or "DST"
       ,"outputType":   "DST" # or "RDST" or "NONE"
       ,"expertHistos": False # set to True to write out expert histos
       ,"noWarnings":   False # suppress all messages with MSG::WARNING or below 
       ,"datasetName":  ""    # string used to build file names
       ,"DDDBtag":      "2008-default" # geometry database tag
       ,"condDBtag":    "2008-default" # conditions database tag
       ,"useOracleCondDB": False  # if False, use SQLDDDB instead
       ,"mainSequence": [ "ProcessPhase/Init",
                          "ProcessPhase/Reco",
                          "ProcessPhase/Moni",
                          "ProcessPhase/MCLinks",
                          "ProcessPhase/Check",
                          "ProcessPhase/Output" ]
       ,"mcCheckSequence": ["Pat","RICH","MUON"] # The default MC Check sequence
       ,"mcLinksSequence": [ "L0", "Unpack", "Tr" ] # The default MC Link sequence
       ,"initSequence": ["Reproc", "Brunel", "Calo"] # The default init sequence
       ,"moniSequence": ["CALO","RICH","MUON","VELO","Track"]    # The default Moni sequence
       ,"monitors": []        # list of monitors to execute, see KnownMonitors
        # Following are options forwarded to RecSys
       ,"recoSequence"   : [] # The Sub-detector reconstruction sequencing. See RecSys for default
       ,"fieldOff":     False # set to True for magnetic field off data
       ,"veloOpen":     False # set to True for Velo open data
        # Following are options forwarded to TrackSys
       ,"expertTracking": []  # list of expert Tracking options, see KnownExpertTracking
        }

    def defineGeometry(self):
        # Prefer Brunel default over LHCbApp default if not set explicitly
        self.setProp( "condDBtag", self.getProp("condDBtag") )
        self.setProp( "DDDBtag", self.getProp("DDDBtag") )
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["condDBtag","DDDBtag","useOracleCondDB"]) 
        if LHCbApp().getProp("DDDBtag").find("DC06") != -1 :
            ApplicationMgr().Dlls += [ "HepMCBack" ]

    def defineEvents(self):
        evtMax = self.getProp("EvtMax")
        if hasattr(LHCbApp(),"EvtMax"):
            print "# LHCbApp().EvtMax already defined, ignoring Brunel().EvtMax"
        else:
            LHCbApp().EvtMax = evtMax

        skipEvents = self.getProp("skipEvents")
        if skipEvents > 0 :
            if hasattr(LHCbApp(),"skipEvents"):
                print "# LHCbApp().skipEvents already defined, ignoring Brunel().skipEvents"
            else:
                LHCbApp().skipEvents = skipEvents

    def defineOptions(self):

        inputType = self.getProp( "inputType" ).upper()
        if inputType not in [ "MDF", "DST", "DIGI", "ETC", "RDST" ]:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "outputType" ).upper()
        if outputType not in [ "NONE", "DST", "RDST" ]:
            raise TypeError( "Invalid outputType '%s'"%outputType )

        withMC = self.getProp("withMC")
        if inputType in [ "MDF", "RDST" ]:
            withMC = False # Force it, MDF and RDST never contain MC truth
        if outputType == "RDST":
            withMC = False # Force it, RDST never contains MC truth

        self.configureInput( inputType )

        self.configureOutput( outputType, withMC )


        # Set up monitoring (i.e. not using MC truth)
        ProcessPhase("Moni").DetectorList += self.getProp("moniSequence")
        importOptions("$BRUNELOPTS/BrunelMoni.py") # Filled in all cases
        if not withMC:
            # Add here histograms to be filled only with real data 
            from RichRecQC.Configuration import RichRecQCConf
            RichRecQCConf().context = "Offline"
            RichRecQCConf().applyConf(GaudiSequencer("MoniRICHSeq"))


        # Setup up MC truth processing and checking
        if withMC:
            ProcessPhase("MCLinks").DetectorList += self.getProp("mcLinksSequence")
            # Unpack Sim data
            GaudiSequencer("MCLinksUnpackSeq").Members += [ "UnpackMCParticle",
                                                            "UnpackMCVertex" ]
            GaudiSequencer("MCLinksTrSeq").Members += [ "TrackAssociator" ]

            # "Check" histograms filled only with simulated data 
            ProcessPhase("Check").DetectorList += self.getProp("mcCheckSequence")
            # Tracking
            importOptions("$TRACKSYSROOT/options/PatChecking.opts")
            if self.getProp( "veloOpen" ):
                GaudiSequencer("CheckPatSeq").Members.remove("TrackAssociator/AssocVeloRZ")
                GaudiSequencer("CheckPatSeq").Members.remove("TrackAssociator/AssocDownstream")
                GaudiSequencer("CheckPatSeq").Members.remove("TrackEffChecker/VeloRZ")
                GaudiSequencer("CheckPatSeq").Members.remove("TrackEffChecker/Downstream")
            # Muon
            importOptions("$MUONPIDCHECKERROOT/options/MuonPIDChecker.opts")
            # RICH
            from RichRecQC.Configuration import RichRecQCConf
            RichRecQCConf().context = "Offline"
            RichRecQCConf().applyConf(GaudiSequencer("CheckRICHSeq"))

        # Setup L0 filtering if requested, runs L0 before Reco
        if self.getProp("recL0Only"):
            ProcessPhase("Init").DetectorList.append("L0")
            importOptions( "$L0DUROOT/options/L0Sequence.opts" )
            GaudiSequencer("InitL0Seq").Members += [ "GaudiSequencer/L0FilterFromRawSeq" ]

    def defineHistos(self):
        """
        Save histograms. If expert, fill and save also the expert histograms
        """
        expertHistos = self.getProp("expertHistos")
        if expertHistos:
            RecSysConf().setProp( "expertHistos", expertHistos )
            importOptions( "$BRUNELOPTS/ExpertCheck.opts" )
            IODataManager().AgeLimit += 1

    def defineMonitors(self):
        # get all defined monitors
        monitors = self.getProp("monitors") + LHCbApp().getProp("monitors")
        # pass to LHCbApp any monitors not dealt with here
        LHCbApp().setProp("monitors", monitors)

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
        if self.getProp( "inputType" ).upper() == "ETC":
            ApplicationMgr().ExtSvc.append("TagCollectionSvc/EvtTupleSvc")
            ApplicationMgr().OutStream.append("GaudiSequencer/SeqTagWriter")
            importOptions( "$BRUNELOPTS/DefineETC.opts" )

        # Modify printout defaults
        importOptions("$BRUNELOPTS/BrunelMessage.opts")
        if self.getProp( "noWarnings" ):
            importOptions( "$BRUNELOPTS/SuppressWarnings.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("datasetName")
        if self.getProp( "recL0Only" ): outputName += '-L0Yes'
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        outputType = self.getProp("outputType").lower()
        return outputName + '.' + outputType

    def histosName(self):
        histosName   = self.getProp("datasetName")
        if self.getProp( "recL0Only" ): histosName += '-L0Yes'
        if ( self.evtMax() > 0 ): histosName += '-' + str(self.evtMax()) + 'ev'
        expertHistos = self.getProp("expertHistos")
        if expertHistos     : histosName += '-expert'
        histosName += '-histos.root'
        return histosName
    
    def evtMax(self):
        if hasattr(ApplicationMgr(),"EvtMax"):
            return getattr(ApplicationMgr(),"EvtMax")
        else:
            return ApplicationMgr().getDefaultProperties()["EvtMax"]

    ## Apply the configuration
    def applyConf(self):
        GaudiKernel.ProcessJobOptions._logFilter.printing_level += 1
        self.setOtherProp(TrackSys(),"expertTracking") 
        self.setOtherProps(RecSysConf(),["fieldOff","veloOpen","recoSequence"])
        brunelSeq = GaudiSequencer("BrunelSequencer")
        ApplicationMgr().TopAlg = [ brunelSeq ]
        brunelSeq.Members += self.getProp("mainSequence")
        ProcessPhase("Init").DetectorList += self.getProp("initSequence")
        GaudiSequencer("InitBrunelSeq").Members += [ "RecInit/BrunelInit" ]
        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        self.defineHistos()
        self.defineMonitors()
        RecSysConf().applyConf()
        LHCbApp().applyConf()
        from Configurables import RecInit
        RecInit("BrunelInit").PrintFreq = self.getProp("printFreq")
        # Use SIMCOND for Simulation, if not DC06
        if self.getProp("withMC") and LHCbApp().getProp("condDBtag").find("DC06") == -1:
            from Configurables import CondDBCnvSvc
            CondDBCnvSvc( CondDBReader = allConfigurables["SimulationCondDBReader"] )
