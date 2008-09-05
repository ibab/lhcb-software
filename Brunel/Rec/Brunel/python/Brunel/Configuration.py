
## @package Brunel
#  High level configuration tools for Brunel
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.15 2008-09-05 16:26:54 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration  import *
from RecSys.Configuration     import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( ProcessPhase, CondDBCnvSvc, MagneticFieldSvc,
                            ReadStripETC, RecInit, GaudiSequencer )

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
       ,"withMC":       True  # set to False for real data or to ignore MC truth
       ,"recL0Only":    False # set to True to reconstruct only L0-yes events
       ,"inputType":    "DIGI"# or "MDF" or "ETC" or "RDST" or "DST"
       ,"outputType":   "DST" # or "RDST" or "NONE"
       ,"expertHistos": False # set to True to write out expert histos
       ,"noWarnings":   False # suppress all messages with MSG::WARNING or below 
       ,"datasetName":  ""    # string used to build file names
       ,"mainOptions" : '$BRUNELOPTS/Brunel.opts' # top level options to import
       ,"DDDBtag":      ""    # geometry database tag
       ,"condDBtag":    ""    # conditions database tag
       ,"useOracleCondDB": False  # if False, use SQLDDDB instead
       ,"mainSequence": [ "ProcessPhase/Init",
                          "ProcessPhase/Reco",
                          "ProcessPhase/Moni",
                          "ProcessPhase/MCLinks",
                          "ProcessPhase/Check",
                          "ProcessPhase/Output" ]
       ,"mcCheckSequence": ["Pat","RICH","MUON"] # The default MC Check sequence
       ,"initSequence": ["Reproc", "Brunel", "Calo"] # The default init sequence
       ,"moniSequence": ["CALO","RICH"]    # The default Moni sequence
       ,"monitors": []        # list of monitors to execute, see KnownMonitors
        # Following are options forwarded to RecSys
       ,"recoSequence"   : [] # The Sub-detector reconstruction sequencing. See RecSys for default
       ,"fieldOff":     False # set to True for magnetic field off data
       ,"veloOpen":     False # set to True for Velo open data
        # Following are options forwarded to TrackSys
       ,"expertTracking": []  # list of expert Tracking options, see KnownExpertTracking
        }

    def defineGeometry(self):
        self.setOtherProp(LHCbApp(),"condDBtag") 
        self.setOtherProp(LHCbApp(),"DDDBtag") 
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
            raise TypeError( "Invalid outputType '%s'"%inputType )

        withMC = self.getProp("withMC")
        if inputType in [ "DST", "RDST", "ETC" ]:
            # Kill knowledge of any previous Brunel processing
            InitReprocSeq = GaudiSequencer( "InitReprocSeq" )
            InitReprocSeq.Members.append( "TESCheck" )
            InitReprocSeq.Members.append( "EventNodeKiller" )
            if inputType == "ETC":
                # Read ETC selection results into TES for writing to DST
                GaudiSequencer("InitBrunelSeq").Members.append("ReadStripETC/TagReader")
                ReadStripETC("TagReader").CollectionName = "TagCreator"
                IODataManager().AgeLimit += 1
                
        elif ( inputType == "MDF" ):
            withMC = False # Force it, MDF never contains MC truth

        if self.getProp("outputType").upper() == "RDST":
            withMC = False # Force it, RDST never contains MC truth

        if withMC:
            #importOptions( "$BRUNELOPTS/BrunelMC.opts" )
            # CRJ : explicitly include the options from above here
            importOptions("$STDOPTS/MCDstContent.opts")
            ProcessPhase("MCLinks").DetectorList += [ "L0", "Unpack", "Tr" ]
            # Unpack Sim data
            GaudiSequencer("MCLinksUnpackSeq").Members += [ "UnpackMCParticle",
                                                            "UnpackMCVertex" ]
            GaudiSequencer("MCLinksTrSeq").Members += [ "TrackAssociator" ]
            # MC checks
            ProcessPhase("Check").DetectorList += self.getProp("mcCheckSequence")
            # Tracking
            GaudiSequencer("CheckPatSeq").Members += [ "PatLHCbID2MCParticle"
                                                       ,"PatTrack2MCParticle"
                                                       ,"PatChecker" ]
            # Muon
            importOptions("$MUONPIDCHECKERROOT/options/MuonPIDChecker.opts")
            # RICH
            from RichRecQC.Configuration import RichRecQCConf
            RichRecQCConf().context = "Offline"
            RichRecQCConf().applyConf(GaudiSequencer("CheckRICHSeq"))
        else:
            # Add the RICH monitors to the Moni sequence
            from RichRecQC.Configuration import RichRecQCConf
            RichRecQCConf().context = "Offline"
            RichRecQCConf().applyConf(GaudiSequencer("MoniRICHSeq"))

        if inputType not in [ "DIGI", "DST" ]:
            # In case raw data resides in MDF file
            EventPersistencySvc().CnvServices.append("LHCb::RawDataCnvSvc")

        if self.getProp("recL0Only"):
            ProcessPhase("Init").DetectorList.append("L0")

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

    def histosName(self):
        histosName   = self.getProp("datasetName")
        if self.getProp( "recL0Only" ): histosName += '-L0Yes'
        if ( self.evtMax() > 0 ): histosName += '-' + str(self.evtMax()) + 'ev'
        expertHistos = self.getProp("expertHistos")
        if expertHistos     : histosName += '-expert'
        histosName += '-histos.root'
        return histosName
    
    def defineOutput(self):
        """
        Set up output stream
        """
        dstType = self.getProp( "outputType" ).upper()
        if dstType not in [ "RDST", "DST", "NONE" ]:
            raise TypeError( "Invalid outputType '%s'"%dstType )
        if dstType in [ "DST", "RDST" ]:
            if( dstType == "DST" ):
                importOptions( "$BRUNELOPTS/DstContent.opts" )
            else:
                importOptions( "$BRUNELOPTS/rDstContent.opts" )
            OutputStream( "DstWriter" )
            ApplicationMgr().OutStream.append( "DstWriter" )
            
        nowarn = self.getProp( "noWarnings" )
        if nowarn: importOptions( "$BRUNELOPTS/SuppressWarnings.opts" )

        # Always write an ETC if ETC input
        if self.getProp( "inputType" ).upper() == "ETC":
            ApplicationMgr().ExtSvc.append("TagCollectionSvc/EvtTupleSvc")
            ApplicationMgr().OutStream.append("GaudiSequencer/SeqTagWriter")
            importOptions( "$BRUNELOPTS/DefineETC.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("datasetName")
        if self.getProp( "recL0Only" ): outputName += '-L0Yes'
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        outputType = self.getProp("outputType").lower()
        return outputName + '.' + outputType

    def evtMax(self):
        if hasattr(ApplicationMgr(),"EvtMax"):
            return getattr(ApplicationMgr(),"EvtMax")
        else:
            return ApplicationMgr().getDefaultProperties()["EvtMax"]

    ## Apply the configuration
    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1
        # Propagate the useOracleCondDB property to LHCbApp before it is used by DDDB.py
        self.setOtherProp(LHCbApp(),"useOracleCondDB")
        # Next line has to be before mainOptions - TODO: why?
        importOptions( "$DDDBROOT/options/DDDB.py" )
        self.setOtherProp(TrackSys(),"expertTracking") 
        self.setOtherProps(RecSysConf(),["fieldOff","veloOpen","recoSequence"])
        # Set main sequence
        brunelSeq = GaudiSequencer("BrunelSequencer")
        ApplicationMgr().TopAlg = [ brunelSeq ]
        brunelSeq.Members += self.getProp("mainSequence")
        importOptions( self.getProp( "mainOptions" ) )
        ProcessPhase("Init").DetectorList += self.getProp("initSequence")
        ProcessPhase("Moni").DetectorList += self.getProp("moniSequence")
        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        self.defineHistos()
        self.defineOutput()
        self.defineMonitors()
        RecSysConf().applyConf()
        LHCbApp().applyConf()
        RecInit("BrunelInit").PrintFreq = self.getProp("printFreq")
        # Use SIMCOND for Simulation, if not DC06
        if self.getProp("withMC") and LHCbApp().getProp("condDBtag").find("DC06") == -1:
            CondDBCnvSvc( CondDBReader = allConfigurables["SimulationCondDBReader"] )
