## @package Escher
#  High level configuration tools for Escher
#  @author Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.7 2009-04-17 13:04:56 jblouw Exp $"
__author__  = "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, RecSysConf, TrackSys,
                            ProcessPhase, GaudiSequencer, DstConf, TAlignment, VeloAlignment )


## @class Escher
#  Configurable for Escher application
#  @author Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>
#  @date   15/08/2008
class Escher(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ TAlignment, VeloAlignment, TrackSys, RecSysConf, LHCbApp, DstConf ]

    ## Default main sequences for real and simulated data
    DefaultSequence = [   "ProcessPhase/Init"
			, "ProcessPhase/Reco"
			, GaudiSequencer("AlignSequence") ]

    
    # Steering options
    __slots__ = {
         "EvtMax"		: -1       # Maximum number of events to process
       , "SkipEvents"		: 0        # events to skip
       , "PrintFreq"		: 300      # The frequency at which to print event numbers
       , "DataType"   		: "2008"   # Data type, can be ['DC06','2008']
       , "WithMC"		: False    # set to True to use MC truth
       , "Simulation"		: False    # set to True to use SimCond
       , "InputType"		: "DST"    # or "DIGI" or "ETC" or "RDST" or "DST"
       , "OutputType"		: "NONE"   # or "RDST" or "NONE". Also forwarded to RecSys
       , "PackedOutput"		: True     # Flag whether or not to use packed containers
       , "NoWarnings"		: False    # suppress all messages with MSG::WARNING or below 
       , "TrackContainer" 	: "Long"   # Tracktype to be used for alignment (Long, Seed, VeloTT...)
       , "Detectors" 		: ["VELO", "TT", "IT", "OT", "MUON", "Tr", "Vertex"] # detectors to be aligned
       , "AlignmentLevel" 	: "layers" # level of alignment, stations, layers, quarters, modules, sensors...
       , "DatasetName"		: ""       # string used to build file names
       , "DDDBtag"		: ""       # Tag for DDDB. Default as set in DDDBConf for DataType
       , "CondDBtag"		: ""       # Tag for CondDB. Default as set in DDDBConf for DataType
       , "UseOracle"		: False    # if False, use SQLDDDB instead
       , "MainSequence"		: []       # The default main sequence, see self.DefaultSequence
       , "InitSequence"		: ["Reproc", "Escher", "Calo"] # default init sequence
       , "AlignSequence"	: []
       , "Kalman" 		: False    # run the kalman filter type alignment
       , "Millepede"		: False    # run the Millepede type alignment
       , "OutputLevel" 		: 3        # 
       , "Incident"     	:  ""      # for Millepede style alignment, there are two incident handles: GlobalMPedeFit and Converged
                                           # for Kalman style alignment, there is a handle: UpdateConstants.
        # Following are options forwarded to RecSys
       , "RecoSequence"   	: [] # The Sub-detector reconstruction sequencing. See RecSys for default
       , "SpecialData"    	: [] # Various special data processing options. See KnownSpecialData for all options
       , "Context"		: "Offline" # The context within which to run
        }



    def defineGeometry(self):
        # DIGI is always simulation, as is usage of MC truth!
        if self.getProp( "WithMC" ) or self.getProp( "InputType" ).upper() == 'DIGI':
            self.setProp( "Simulation", True )

        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","UseOracle","Simulation"])

    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])

    def defineOptions(self):
        log.info("Defining options!")

        inputType = self.getProp( "InputType" ).upper()
        if inputType not in [ "MDF", "DST", "DIGI", "ETC", "RDST" ]:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "OutputType" ).upper()
        if outputType not in [ "NONE", "DST", "RDST" ]:
            raise TypeError( "Invalid outputType '%s'"%outputType )

        self.configureSequences( )

        self.configureInput( inputType )

        self.configureOutput( outputType )

        # ROOT persistency for histograms
        importOptions('$STDOPTS/RootHist.opts')
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

        # Use a default histogram file name if not already set
        if not hasattr( HistogramPersistencySvc(), "OutputFile" ):
            histosName   = self.getProp("DatasetName")
            if (self.evtMax() > 0): histosName += '-' + str(self.evtMax()) + 'ev'
            histosName += '-histos.root'
            HistogramPersistencySvc().OutputFile = histosName

    def configureSequences(self):
        escherSeq = GaudiSequencer("EscherSequencer")
        ApplicationMgr().TopAlg = [ escherSeq ]
        mainSeq = self.getProp("MainSequence")
        if len( mainSeq ) == 0:
            self.MainSequence = self.DefaultSequence
        mainSeq = self.MainSequence
        escherSeq.Members += mainSeq
        
        ProcessPhase("Init").DetectorList += self.getProp("InitSequence")
        ProcessPhase("Init").Context = self.getProp("Context")
        from Configurables import RecInit, TrackSys, GAlign, Centipede
        log.info("Setting up alignment sequence")
        recInit = RecInit( name = "EscherInit",
                           PrintFreq = self.getProp("PrintFreq"))
        GaudiSequencer("InitEscherSeq").Members += [ recInit ]
        alignSeq = GaudiSequencer("AlignSequence")
        if  self.getProp("Millepede") :
            self.setProp("Kalman", False )
            log.info("Using Millepede type alignment!")
            self.setProp("Incident", "GlobalMPedeFit")
            if "VELO" in self.getProp("Detectors") : # generate the proper tracking sequence depending on which detectors one wants to align            
               TrackSys.TrackPatRecAlgorithms = ["PatSeed"]
               log.info("Aligning VELO")
               va = VeloAlignment()
               va.ElementsToAlign = [ self.getProp( "AlignmentLevel" ) ]
               va.Sequencer = GaudiSequencer("VeloAlignSeq")
               alignSeq.Members.append( va.Sequencer )
            if "OT" in self.getProp("Detectors") or "ot" in self.getProp("Detectors"):
	       log.info("Aligning OT")
               TrackSys.TrackPatRecAlgorithms = ["PatSeed"]
               GaudiSequencer("RecoRICHSeq").Enable = False 
               GaudiSequencer("RecoVELOSeq").Enable = False
	       GaudiSequencer("RecoTTSeq").Enable = False
	       GaudiSequencer("RecoITSeq").Enable = True
               log.info("Escher: initalizing TAlignment!")
               
               ProcessPhase("Align").DetectorList += ["OT"]
               ta = TAlignment()
               ta.Method = "Millepede"
 	       ta.TrackLocation = self.getProp("TrackContainer")
	       ta.Detectors =  self.getProp("Detectors")
               ta.Sequencer = GaudiSequencer("MpedeAlignSeq")
               alignSeq.Members.append(ta.Sequencer)
        if self.getProp("Kalman") :
            TrackSys().ExpertTracking += [ "kalmanSmoother" ]
	    log.info("Using Kalman style alignment!")
            self.setProp("Incident", "UpdateConstants")
            ta = TAlignment()
            ta.Method = "Kalman"
            ta.Sequencer = GaudiSequencer("KalmanAlignSeq")
            alignSeq.Members.append( ta.Sequencer )

		  
    def configureInput(self, inputType):
        """
        Tune initialisation according to input type
        """

        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        # By default, Escher only needs to open one input file at a time
        IODataManager().AgeLimit = 0 
        if  inputType in [ "DST", "RDST", "ETC" ]:
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
            GaudiSequencer("InitEscherSeq").Members.append("ReadStripETC/TagReader")
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


    def configureOutput(self, dstType):
        """
        Set up output stream
        """
        if dstType in [ "DST", "RDST" ]:
            writerName = "DstWriter"
            packedDST  = self.getProp( "PackedOutput" )
            # Do not pack DC06 DSTs, for consistency with existing productions
            if self.getProp("DataType") == "DC06": packedDST = False

            dstWriter = OutputStream( writerName )
            dstWriter.RequireAlgs += ["Reco"] # Write only if Rec phase completed

            # Set a default output file name if not already defined in the user job options
            if not hasattr( dstWriter, "Output" ):
                dstWriter.Output  = "DATAFILE='PFN:" + self.outputName() + "' TYP='POOL_ROOTTREE' OPT='REC'"

            # Define the file content
            DstConf().Writer     = writerName
            DstConf().DstType    = dstType
            DstConf().EnablePack = packedDST

            from Configurables import TrackToDST
            if dstType == "DST":
                # Sequence for altering DST content
                ProcessPhase("Output").DetectorList += [ "DST" ]
                # Filter Track States to be written
                trackFilter = TrackToDST()
            else:
                # Sequence for altering content of rDST compared to DST
                ProcessPhase("Output").DetectorList += [ "L0", "DST" ]
                # Filter Track States to be written
                trackFilter = TrackToDST("TrackToRDST")
                trackFilter.veloStates = ["ClosestToBeam"]
                trackFilter.longStates = ["ClosestToBeam"]
                trackFilter.TTrackStates = ["FirstMeasurement"]
                trackFilter.downstreamStates = ["FirstMeasurement"]
                trackFilter.upstreamStates = ["ClosestToBeam"]
                
            GaudiSequencer("OutputDSTSeq").Members += [ trackFilter ]

            if packedDST:
                # Add the sequence to pack the DST containers
                packSeq = GaudiSequencer("PackDST")
                DstConf().PackSequencer = packSeq
                GaudiSequencer("OutputDSTSeq").Members += [ packSeq ]

        # Always write an ETC if ETC input
        if self.getProp( "InputType" ).upper() == "ETC":
            etcWriter = TagCollectionSvc("EvtTupleSvc")
            ApplicationMgr().ExtSvc.append(etcWriter)
            ApplicationMgr().OutStream.append("GaudiSequencer/SeqTagWriter")
            importOptions( "$ESCHEROPTS/DefineETC.opts" )
            if not hasattr( etcWriter, "Output" ):
               etcWriter.Output = [ "EVTTAGS2 DATAFILE='" + self.getProp("DatasetName") + "-etc.root' TYP='POOL_ROOTTREE' OPT='RECREATE' " ]

        # Do not print event number at every event (done already by Brunel)
        #EventSelector().PrintFreq = -1
        # Modify printout defaults
        if self.getProp( "NoWarnings" ):
            importOptions( "$ESCHEROPTS/SuppressWarnings.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        outputType = self.getProp("OutputType").lower()
        return outputName + '.' + outputType

    def evtMax(self):
        return LHCbApp().evtMax()

    # method to print an algorithm in the sequence
    def printAlgo( self, algName, appMgr, prefix = ' ') :
        #""" print algorithm name, and, if it is a sequencer, recursively those algorithms it calls"""
        print prefix + algName
        alg = appMgr.algorithm( algName.split( "/" )[ -1 ] )
        prop = alg.properties()
        if prop.has_key( "Members" ) :
            subs = prop[ "Members" ].value()
            for i in subs : self.printAlgo( i.strip( '"' ), appMgr, prefix + "     " )
        elif prop.has_key( "DetectorList" ) :
            subs = prop[ "DetectorList" ].value()
            for i in subs : self.printAlgo( algName.split( "/" )[ -1 ] + i.strip( '"' ) + "Seq", appMgr, prefix + "     ")

    # method to print all algorithms        
    def printFlow( self ) :
        from GaudiPython.Bindings import AppMgr
        appMgr = AppMgr()
        mp = appMgr.properties()
        print "\n ****************************** Application Flow ****************************** \n"
        for i in mp["TopAlg"].value(): self.printAlgo( i, appMgr )
        print "\n ****************************************************************************** \n"
        
    ## Apply the configuration
    def __apply_configuration__(self):
        
        GaudiKernel.ProcessJobOptions.PrintOff()
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info("Initializing sequences!")
        self.setOtherProps(RecSysConf(),["SpecialData","RecoSequence","Context","OutputType"])
        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        log.info( self )
        log.info( RecSysConf() )
        log.info( TrackSys() )
        log.info( TAlignment() )
        log.info( RecSysConf() )
        log.info( LHCbApp() )
        GaudiKernel.ProcessJobOptions.PrintOff()

        from GaudiKernel.Configurable import appendPostConfigAction
        appendPostConfigAction(self.printFlow)
