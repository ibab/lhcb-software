## @package Brunel
#  High level configuration tools for Brunel
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.53 2009-02-09 15:52:51 jonrob Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, RecSysConf, TrackSys,
                            ProcessPhase, GaudiSequencer, RichRecQCConf, DstConf )

## @class Brunel
#  Configurable for Brunel application
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class Brunel(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ TrackSys, RecSysConf, RichRecQCConf, LHCbApp, DstConf ]

    ## Known monitoring sequences, all run by default
    KnownMoniSubdets        = ["CALO","RICH","MUON","VELO","Tr","ST"] 
    KnownExpertMoniSubdets  = KnownMoniSubdets+["TT","IT","OT"]
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
        "EvtMax":          -1 # Maximum number of events to process
       ,"SkipEvents":   0     # events to skip
       ,"PrintFreq":    1     # The frequency at which to print event numbers
       ,"DataType"   : "2008" # Data type, can be ['DC06','2008']
       ,"WithMC":       False # set to True to use MC truth
       ,"Simulation":   False # set to True to use SimCond
       ,"RecL0Only":    False # set to True to reconstruct only L0-yes events
       ,"InputType":    "MDF" # or "DIGI" or "ETC" or "RDST" or "DST"
       ,"OutputType":   "DST" # or "RDST" or "NONE". Also forwarded to RecSys
       ,"PackType":     "TES" # Type of packing for the DST: ['NONE','TES','MDF'] 
       ,"Histograms": "Default" # Type of histograms: ['None','Default','Expert']
       ,"NoWarnings":   False # suppress all messages with MSG::WARNING or below 
       ,"DatasetName":  ""    # string used to build file names
       ,"DDDBtag":      "" # Tag for DDDB. Default as set in DDDBConf for DataType
       ,"CondDBtag":    "" # Tag for CondDB. Default as set in DDDBConf for DataType
       ,"UseOracle": False  # if False, use SQLDDDB instead
       ,"MainSequence": []    # The default main sequence, see self.DefaultSequence
       ,"MCCheckSequence": [] # The default MC Check sequence, see KnownCheckSubdets
       ,"MCLinksSequence": [ "L0", "Unpack", "Tr" ] # The default MC Link sequence
       ,"InitSequence": ["Reproc", "Brunel", "Calo"] # The default init sequence
       ,"MoniSequence": []    # The default Moni sequence, see KnownMoniSubdets
       ,"Monitors": []        # list of monitors to execute, see KnownMonitors
        # Following are options forwarded to RecSys
       ,"RecoSequence"   : [] # The Sub-detector reconstruction sequencing. See RecSys for default
       ,"SpecialData"    : [] # Various special data processing options. See KnownSpecialData for all options
       ,"Context":     "Offline" # The context within which to run
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

        inputType = self.getProp( "InputType" ).upper()
        if inputType not in [ "MDF", "DST", "DIGI", "ETC", "RDST" ]:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "OutputType" ).upper()
        if outputType not in [ "NONE", "DST", "RDST" ]:
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

        self.configureSequences( withMC )

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
            importOptions( "$L0DUROOT/options/L0Sequence.opts" )
            GaudiSequencer("InitL0Seq").Members += [ "GaudiSequencer/L0FilterFromRawSeq" ]

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
        if not hasattr( HistogramPersistencySvc(), "OutputFile" ):
            histosName   = self.getProp("DatasetName")
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
        ProcessPhase("Init").DetectorList += self.getProp("InitSequence")
        ProcessPhase("Init").Context = self.getProp("Context")
        from Configurables import RecInit, MemoryTool
        recInit = RecInit( name = "BrunelInit",
                           PrintFreq = self.getProp("PrintFreq"))
        GaudiSequencer("InitBrunelSeq").Members += [ recInit ]
        recInit.addTool( MemoryTool(), name = "BrunelMemory" )
        recInit.BrunelMemory.HistoTopDir = "Brunel/"
        recInit.BrunelMemory.HistoDir    = "MemoryTool"

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
            writerName = "DstWriter"
            packType  = self.getProp( "PackType" )
            # Do not pack DC06 DSTs, for consistency with existing productions
            if self.getProp("DataType") == "DC06": packType = "NONE"

            dstWriter = OutputStream( writerName )
            dstWriter.RequireAlgs += ["Reco"] # Write only if Rec phase completed

            # Set a default output file name if not already defined in the user job options
            if not hasattr( dstWriter, "Output" ):
                outputFile = self.outputName()
                outputFile = outputFile + '.' + self.getProp("OutputType").lower()
                dstWriter.Output  = "DATAFILE='PFN:" + outputFile + "' TYP='POOL_ROOTTREE' OPT='REC'"

            # Define the file content
            DstConf().Writer     = writerName
            DstConf().DstType    = dstType
            DstConf().PackType   = packType
            DstConf().Simulation = withMC
            DstConf().OutputName = self.outputName()

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

            if packType != "NONE":
                # Add the sequence to pack the DST containers
                packSeq = GaudiSequencer("PackDST")
                DstConf().PackSequencer = packSeq
                GaudiSequencer("OutputDSTSeq").Members += [ packSeq ]

        # Always write an ETC if ETC input
        if self.getProp( "InputType" ).upper() == "ETC":
            etcWriter = TagCollectionSvc("EvtTupleSvc")
            ApplicationMgr().ExtSvc.append(etcWriter)
            ApplicationMgr().OutStream.append("GaudiSequencer/SeqTagWriter")
            importOptions( "$BRUNELOPTS/DefineETC.opts" )
            if not hasattr( etcWriter, "Output" ):
               etcWriter.Output = [ "EVTTAGS2 DATAFILE='" + self.getProp("DatasetName") + "-etc.root' TYP='POOL_ROOTTREE' OPT='RECREATE' " ]

        # Do not print event number at every event (done already by Brunel)
        EventSelector().PrintFreq = -1;
        # Modify printout defaults
        if self.getProp( "NoWarnings" ):
            importOptions( "$BRUNELOPTS/SuppressWarnings.opts" )

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

        # Histograms filled both in real and simulated data cases
        if "CALO" in moniSeq :
            importOptions('$CALOMONIDSTOPTS/CaloMonitor.opts')
        else :
            # Hack for units options, normally included by CaloMonitor.opts
            importOptions('$STDOPTS/PreloadUnits.opts')

        if "VELO" in moniSeq :
            importOptions('$VELORECMONITORSROOT/options/BrunelMoni_Velo.py')

        if "Tr" in moniSeq :
            from TrackMonitors.ConfiguredTrackMonitors import ConfiguredTrackMonitorSequence
            ConfiguredTrackMonitorSequence(Name='MoniTrSeq')

        if "MUON" in moniSeq :
            importOptions("$MUONPIDCHECKERROOT/options/MuonPIDMonitor.py")

        if "ST" in moniSeq :
            from Configurables import STClusterMonitor, GaudiSequencer
            GaudiSequencer( "MoniSTSeq" ).Members += [ STClusterMonitor("TTClusterMonitor"),
                                                       STClusterMonitor("ITClusterMonitor")]
            STClusterMonitor("TTClusterMonitor").DetType = "TT" ## default anyway 
            STClusterMonitor("ITClusterMonitor").DetType = "IT" 

        # Histograms filled only in real data case
        if not withMC:
            if "RICH" in moniSeq :
                from Configurables import GaudiSequencer
                self.setOtherProps(RichRecQCConf(), ["Context","DataType","WithMC"])
                RichRecQCConf().setProp("MoniSequencer", GaudiSequencer("MoniRICHSeq"))

        # Expert histograms
        if expert:
            if "TT" in moniSeq :
                from Configurables import STClusterMonitor
                clusMoni = STClusterMonitor("TTClusterMonitor")
                clusMoni.FullDetail = True
                GaudiSequencer("MoniTTSeq").Members += [clusMoni]
            if "IT" in moniSeq :
                from Configurables import STClusterMonitor
                clusMoni = STClusterMonitor("ITClusterMonitor")
                clusMoni.FullDetail = True
                clusMoni.DetType = "IT"
                GaudiSequencer("MoniITSeq").Members += [clusMoni]
            if "OT" in moniSeq :
                # These histograms should be identical to those already done in Boole.
                GaudiSequencer("MoniOTSeq").Members  += ["OTTimeMonitor"]

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
            self.setOtherProps(RichRecQCConf(),["Context","DataType","WithMC"])
            RichRecQCConf().MoniSequencer = GaudiSequencer("CheckRICHSeq")

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

            if "OT" in checkSeq : GaudiSequencer("CheckOTSeq").Members += ["OTTimeChecker"] # needs MCHits

            if "Tr" in  checkSeq :
                # Checking on the tracks in the "best" container - needs MCHits
                importOptions( "$TRACKSYSROOT/options/TrackChecking.opts" )

            if "CALO" in  checkSeq : 
                importOptions( "$CALOASSOCIATORSROOT/options/CaloAssociators.opts" )
                importOptions( "$CALOMONIDSTOPTS/CaloChecker.opts" )

            if "RICH" in checkSeq :
                RichRecQCConf().setProp( "ExpertHistos", True )

            if "PROTO" in checkSeq :
                from Configurables import ( NTupleSvc, ChargedProtoParticleTupleAlg )
                protoChecker = ChargedProtoParticleTupleAlg("ChargedProtoTuple")
                protoChecker.NTupleLUN = "PROTOTUPLE"
                GaudiSequencer("CheckPROTOSeq").Members += [protoChecker]
                NTupleSvc().Output += ["PROTOTUPLE DATAFILE='protoparticles.tuples.root' TYP='ROOT' OPT='NEW'"]

    ## Apply the configuration
    def __apply_configuration__(self):
        
        GaudiKernel.ProcessJobOptions.PrintOff()
        self.setOtherProps(RecSysConf(),["SpecialData","RecoSequence","Context","OutputType"])
        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        self.defineMonitors()
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info( self )
        log.info( RecSysConf() )
        log.info( TrackSys() )
        GaudiKernel.ProcessJobOptions.PrintOff()
