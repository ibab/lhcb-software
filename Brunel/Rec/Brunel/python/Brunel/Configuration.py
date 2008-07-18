"""
High level configuration tools for Brunel
"""
__version__ = "$Id: Configuration.py,v 1.8 2008-07-18 12:39:25 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( ProcessPhase, CondDBCnvSvc, MagneticFieldSvc, ReadStripETC )



class Brunel(ConfigurableUser):

    # Steering options
    __slots__ = {
        "EvtMax":          -1 # Maximum number of events to process
       ,"skipEvents":   0     # events to skip
       ,"withMC":       True  # set to False for real data or to ignore MC truth
       ,"recL0Only":    False # set to True to reconstruct only L0-yes events
       ,"inputType":    "DIGI"# or "MDF" or "ETC" or "RDST" or "DST"
       ,"outputType":   "DST" # or "RDST" or "NONE"
       ,"expertHistos": False # set to True to write out expert histos
       ,"noWarnings":   False # suppress all messages with MSG::WARNING or below 
       ,"datasetName":  ""    # string used to build file names
       ,"mainOptions" : '$BRUNELOPTS/Brunel.opts' # top level options to import
       ,"DDDBtag":  "DEFAULT" # geometry database tag
       ,"condDBtag":"DEFAULT" # conditions database tag
       ,"fieldOff":     False # set to True for magnetic field off data
       ,"veloOpen":     False # set to True for Velo open data
       ,"monitors": []        # list of monitors to execute, see KnownMonitors
       ,"expertTracking": []  # list of expert Tracking options, see KnownExpertTracking
        }

    KnownMonitors       = ["SC", "FPE"]
    KnownExpertTracking = ["usePatSeeding", "noDrifttimes", "simplifiedGeometry"]

    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def setProp(self,name,value):
        return setattr(self,name,value)

    def defineGeometry(self):
        condDBtag = self.getProp("condDBtag")
        if condDBtag != "DEFAULT":
            if hasattr(LHCbApp(),"condDBtag"):
                print "LHCbApp().condDBtag already defined, ignoring Brunel().condDBtag"
            else:
                LHCbApp().condDBtag = condDBtag
        else:
            condDBtag =  LHCbApp().getDefaultProperties()["condDBtag"]

        DDDBtag = self.getProp("DDDBtag")
        if DDDBtag != "DEFAULT":
            if hasattr(LHCbApp(),"DDDBtag"):
                print "LHCbApp().DDDBtag already defined, ignoring Brunel().DDDBtag"
            else:
                LHCbApp().DDDBtag = DDDBtag
        else:
            DDDBtag = LHCbApp().getDefaultProperties()["DDDBtag"]

    def defineEvents(self):
        evtMax = self.getProp("EvtMax")
        if hasattr(LHCbApp(),"EvtMax"):
            print "LHCbApp().EvtMax already defined, ignoring Brunel().EvtMax"
        else:
            LHCbApp().EvtMax = evtMax

        skipEvents = self.getProp("skipEvents")
        if skipEvents > 0 :
            if hasattr(LHCbApp(),"skipEvents"):
                print "LHCbApp().skipEvents already defined, ignoring Brunel().skipEvents"
            else:
                LHCbApp().skipEvents = skipEvents

    def defineOptions(self):
        for prop in Brunel().getProp("expertTracking"):
            if prop not in self.KnownExpertTracking:
                raise RuntimeError("Unknown expertTracking option '%s'"%prop)

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
            importOptions( "$BRUNELOPTS/BrunelMC.opts" )
        elif inputType not in [ "DIGI", "DST" ]:
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
            importOptions( "$BRUNELOPTS/ExpertCheck.opts" )
            IODataManager().AgeLimit += 1

    def defineMonitors(self):
        # get all defined monitors
        monitors = self.getProp("monitors") + LHCbApp().getProp("monitors")
        for prop in monitors:
            if prop not in self.KnownMonitors:
                raise RuntimeError("Unknown monitor '%s'"%prop)
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

    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1
        importOptions( "$DDDBROOT/options/DDDB.py" )
        importOptions( self.getProp( "mainOptions" ) )
        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        self.defineHistos()
        self.defineOutput()
        self.defineMonitors()
        LHCbApp().applyConf()
        # Use SIMCOND for Simulation, if not DC06
        if self.getProp("withMC") and LHCbApp().getProp("condDBtag").find("DC06") == -1:
            CondDBCnvSvc( CondDBReader = allConfigurables["SimulationCondDBReader"] )
