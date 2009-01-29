"""
High level configuration tools for Boole
"""
__version__ = "$Id: Configuration.py,v 1.34 2009-01-29 17:28:22 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp )

class Boole(LHCbConfigurableUser):

    ## Known monitoring sequences, all run by default
    KnownMoniSubdets = [ "VELO", "IT", "TT", "OT", "RICH", "CALO", "MUON", "L0", "MC" ]
    KnownHistOptions = ["","None","Default","Expert"]
    
    __slots__ = {
        "EvtMax"         : -1
       ,"SkipEvents"     : 0
       ,"SkipSpill"      : 0
       ,"UseSpillover"   : False
       ,"SpilloverPaths" : ["Prev", "PrevPrev", "Next"]
       ,"GenerateTAE"    : False
       ,"Outputs"        : [ "DIGI" ]
       ,"WriteL0Only"    : False
       ,"ExtendedDigi"   : False
       ,"Histograms"     : "Default"
       ,"MainOptions"    : '$BOOLEOPTS/Boole.opts'
       ,"NoWarnings"     : False
       ,"DatasetName"    : ''
       ,"DataType"       : "2008"
       ,"DDDBtag"        : ""
       ,"CondDBtag"      : ""
       ,"UseOracle"      : False
       ,"Monitors"       : []
       ,"MoniSequence"   : []
        }

    _propertyDocDct = { 
        'EvtMax'       : """ Maximum number of events to process """
       ,'SkipEvents'   : """ Number of events to skip """
       ,'SkipSpill'    : """ Number of spillover events to skip """
       ,'UseSpillover' : """ Flag to enable spillover (default False) """
       ,'SpilloverPaths':""" Paths to fill when spillover is enabled """
       ,'GenerateTAE'  : """ Flag to simulate time alignment events (default False) """
       ,'Outputs'      : """ List of outputs: ['MDF','DIGI','L0ETC'] (default 'DIGI') """
       ,'WriteL0Only'  : """ Flag to write only L0 selected events (default False) """
       ,'ExtendedDigi' : """ Flag to add MCHits to .digi output file (default False) """
       ,'Histograms'   : """ Type of histograms: ['None','Default','Expert'] """
       ,'MainOptions'  : """ Top level options to import. Default: $BOOLEOPTS/Boole.opts """
       ,'NoWarnings'   : """ Flag to suppress all MSG::WARNING or below (default False) """ 
       ,'DatasetName'  : """ String used to build output file names """
       ,'DataType'     : """ Data type, can be ['DC06','2008']. Default '2008' """
       ,'DDDBtag'      : """ Tag for DDDB. Default as set in DDDBConf for DataType """
       ,'CondDBtag'    : """ Tag for CondDB. Default as set in DDDBConf for DataType """
       ,'UseOracle'    : """ Flag to enable Oracle CondDB. Default False (use SQLDDDB) """
       ,'Monitors'     : """ List of monitors to execute """
       ,'MoniSequence' : """ List of subdetectors to monitor, see KnownMoniSubdets """
       }
    
    __used_configurables__ = [ LHCbApp ]

    def defineDB(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["CondDBtag","DDDBtag","UseOracle","DataType"])
        LHCbApp().Simulation = True

        # Special options for DC06 data processing
        if self.getProp("DataType") == "DC06" :
            from Configurables import (MCSTDepositCreator, MuonDigitToRawBuffer)

            MCSTDepositCreator("MCITDepositCreator").DepChargeTool = "SiDepositedCharge"
            MCSTDepositCreator("MCTTDepositCreator").DepChargeTool = "SiDepositedCharge"
            MuonDigitToRawBuffer().VType = 1 # DC06 RawBank type

    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])

        skipSpill  = self.getProp("SkipSpill")
        if skipSpill  > 0 :
            if hasattr(EventSelector("SpilloverSelector"),"FirstEvent"):
                log.warning( "EventSelector('SpilloverSelector').FirstEvent and Boole().SkipSpill both defined, using Boole().SkipSpill")
            EventSelector("SpilloverSelector").FirstEvent = skipSpill + 1

        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

    def defineOptions(self):
        tae   = self.getProp("GenerateTAE")
        spill = self.getProp("UseSpillover")

        if tae :
            if spill :
                log.warning("Disabling spillover, incompatible with TAE")
                spill = False
            self.enableTAE()

        if spill :
            self.enableSpillover()
        else:
            if self.getProp("DataType") == "DC06" and not tae:
                log.warning("Spillover is disabled. Should normally be enabled for DC06!")

        histOpt = self.getProp("Histograms").capitalize()
        if histOpt not in self.KnownHistOptions:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)

        self.configureMoni( histOpt )

        self.saveHistos( histOpt )

            
    def enableTAE(self):
        """
        switch to generate Time Alignment events (only Prev1 for now).
        """
        initMUONSeq = GaudiSequencer( "InitMUONSeq" )
        initMUONSeq.Members.remove( "MuonBackground/MuonFlatSpillover" )
        importOptions( "$BOOLEOPTS/TAE-Prev1.opts" ) # add misaligned RawEvent
        if self.getProp("DataType") == "DC06" :
            from Configurables import MCSTDepositCreator
            MCSTDepositCreator("MCITDepositCreatorPrev1").DepChargeTool = "SiDepositedCharge"
            MCSTDepositCreator("MCTTDepositCreatorPrev1").DepChargeTool = "SiDepositedCharge"
            

    def enableSpillover(self):
        """
        switch to generate spillover events.
        """
        if self.getProp("DataType") != "DC06" :
            log.warning("Spillover is enabled. Should normally be enabled only for DC06!")

        from Configurables import MergeEventAlg, UnpackMCParticle, UnpackMCVertex
        initDataSeq = GaudiSequencer( "InitDataSeq" )
        spillPaths  = self.getProp("SpilloverPaths")
        spillAlg    = MergeEventAlg( name = "SpilloverAlg", PathList = spillPaths )
        initDataSeq.Members += [ spillAlg ]
        importOptions("$DIGIALGROOT/options/Spillover.opts")
        from Configurables import MuonBackground, MuonDigitization
        MuonDigitization().EnableSpillover = True
        MuonBackground("MuonLowEnergy").EnableSpillover = True

        # Handle the unpacking of pSim containers
        for spill in spillPaths :
            particleUnpacker = UnpackMCParticle( "UnpackMCP" + spill )
            particleUnpacker.RootInTES = spill
            vertexUnpacker = UnpackMCVertex( "UnpackMCV" + spill )
            vertexUnpacker.RootInTES = spill
            DataOnDemandSvc().AlgMap[ spill + "/MC/Particles" ] = particleUnpacker
            DataOnDemandSvc().AlgMap[ spill + "/MC/Vertices" ] = vertexUnpacker


    def defineMonitors(self):
        # get all defined monitors
        monitors = self.getProp("Monitors") + LHCbApp().getProp("Monitors")
        # Currently no Boole specific monitors, so pass them all to LHCbApp
        LHCbApp().setProp("Monitors", monitors)

        # Use TimingAuditor for timing, suppress printout from SequencerTimerTool
        from Configurables import ApplicationMgr, AuditorSvc, SequencerTimerTool
        ApplicationMgr().ExtSvc += [ 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ] 
        SequencerTimerTool().OutputLevel = WARNING

    def saveHistos(self, histOpt):

        # ROOT persistency for histograms
        importOptions('$STDOPTS/RootHist.opts')
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

        if histOpt == "None" or histOpt == "":
            # HistogramPersistency still needed to read in Muon background.
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        # Use a default histogram file name if not already set
        if not hasattr( HistogramPersistencySvc(), "OutputFile" ):
            histosName   = self.getProp("DatasetName")
            if (self.evtMax() > 0): histosName += '-' + str(self.evtMax()) + 'ev'
            generateTAE  = self.getProp("GenerateTAE")
            if ( generateTAE )  : histosName += '-TAE'
            if histOpt == "Expert": histosName += '-expert'
            histosName += '-histos.root'
            HistogramPersistencySvc().OutputFile = histosName
    
    def defineOutput(self):
        """
        Set up output stream according to output data type
        """

        knownOptions = ["MDF","DIGI","L0ETC"]
        outputs = []
        for option in self.getProp("Outputs"):
            if option.upper() not in knownOptions:
                raise RuntimeError("Unknown Boole().Outputs value '%s'"%option)
            outputs.append( option.upper() )

        l0yes = self.getProp( "WriteL0Only" )

        if "DIGI" in outputs:
            # Objects to be written to output file
            importOptions("$STDOPTS/DigiContent.opts")
            extended = self.getProp("ExtendedDigi")
            if ( extended ): importOptions( "$STDOPTS/ExtendedDigi.opts" )

            MyWriter = OutputStream( "DigiWriter" )
            MyWriter.Preload = False
            if not hasattr( MyWriter, "Output" ):
                MyWriter.Output  = "DATAFILE='PFN:" + self.outputName() + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"
            if l0yes : MyWriter.RequireAlgs.append( "L0Filter" )
            ApplicationMgr().OutStream.append( "DigiWriter" )

        if "L0ETC" in outputs:
            ApplicationMgr().OutStream.append( "Sequencer/SeqWriteTag" )
            MyWriter = TagCollectionStream( "WR" )
            if not hasattr( MyWriter, "Output" ):
                MyWriter.Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + self.getProp("DatasetName") + "-L0ETC.root' TYP='POOL_ROOTTREE' OPT='RECREATE'"
            else: print MyWriter.getProp("Output")

        if "MDF" in outputs:
            # must be after digi and etc cases, because RawWriter.opts adds EventNodeKiller to OutStream list
            importOptions( "$BOOLEOPTS/RawWriter.opts" )
            MyWriter = OutputStream( "RawWriter" )
            if not hasattr( MyWriter, "Output" ):
                MyWriter.Output = "DATAFILE='PFN:" + self.outputName() + ".mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"
            if l0yes : MyWriter.RequireAlgs.append( "L0Filter" )
            # ApplicationMgr().OutStream.append( "RawWriter" ) # Already in RawWriter.opts

        nowarn = self.getProp( "NoWarnings" )
        if nowarn: importOptions( "$BOOLEOPTS/SuppressWarnings.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        generateTAE  = self.getProp("GenerateTAE")
        if ( generateTAE )  : outputName += '-TAE'
        l0yes = self.getProp( "WriteL0Only" )
        if ( l0yes ) : outputName += '-L0Yes'
        extended = self.getProp("ExtendedDigi")
        if ( extended ): outputName += '-extended'
        return outputName

    def evtMax(self):
        return LHCbApp().evtMax()


    def configureMoni(self, histOpt):
        # Set up monitoring (i.e. not using MC truth)
        from Configurables import ProcessPhase
        moniSeq = self.getProp("MoniSequence")
        if len( moniSeq ) == 0:
            moniSeq = self.KnownMoniSubdets
            self.MoniSequence = moniSeq
        else:
            for seq in moniSeq:
                if seq not in self.KnownMoniSubdets:
                    log.warning("Unknown subdet '%s' in MoniSequence"%seq)
        ProcessPhase("Moni").DetectorList += moniSeq

        from Configurables import BooleInit, MemoryTool
        booleInit = BooleInit("Boole")
        booleInit.addTool( MemoryTool(), name = "BooleMemory" )
        booleInit.BooleMemory.HistoTopDir = "Boole/"
        booleInit.BooleMemory.HistoDir    = "MemoryTool"

        if "VELO" in moniSeq:
            from Configurables import  VeloSimMoni,VeloDigit2MCHitLinker,VeloDigiMoni,VeloRawClustersMoni
            GaudiSequencer("MoniVELOSeq").Members += [ VeloSimMoni(), VeloDigit2MCHitLinker(),
                                                       VeloDigiMoni(), VeloRawClustersMoni()  ]

        if "IT" in moniSeq or "TT" in moniSeq:
            from Configurables import ( MCSTDepositMonitor, MCSTDigitMonitor, STDigitMonitor,
                      STClusterMonitor, STEffChecker, MCParticle2MCHitAlg, MCParticleSelector )
            from GaudiKernel.SystemOfUnits import GeV

        if "IT" in moniSeq:
            mcDepMoni   = MCSTDepositMonitor(  "MCITDepositMonitor", DetType="IT" )
            mcDigitMoni = MCSTDigitMonitor(    "MCITDigitMonitor",   DetType="IT" )
            digitMoni   = STDigitMonitor(      "ITDigitMonitor",     DetType="IT" )
            clusMoni    = STClusterMonitor(    "ITClusterMonitor",   DetType="IT" )
            mcp2MCHit   = MCParticle2MCHitAlg( "MCP2ITMCHitAlg", MCHitPath = "MC/IT/Hits",
                                               OutputData = "/Event/MC/Particles2MCITHits" )
            effCheck    = STEffChecker(        "ITEffChecker",       DetType="IT" )
            effCheck.addTool(MCParticleSelector)
            effCheck.MCParticleSelector.zOrigin = 50.0
            effCheck.MCParticleSelector.pMin = 1.0*GeV
            effCheck.MCParticleSelector.betaGammaMin = 1.0
            GaudiSequencer("MoniITSeq").Members += [ mcDepMoni, mcDigitMoni, digitMoni, clusMoni,
                                                     mcp2MCHit, effCheck ]
            if histOpt == "Expert":
                mcDepMoni.FullDetail   = True
                mcDigitMoni.FullDetail = True
                clusMoni.FullDetail    = True
                effCheck.FullDetail    = True

        if "TT" in moniSeq:
            mcDepMoni   = MCSTDepositMonitor(  "MCTTDepositMonitor" )
            mcDigitMoni = MCSTDigitMonitor(    "MCTTDigitMonitor"   )
            digitMoni   = STDigitMonitor(      "TTDigitMonitor"     )
            clusMoni    = STClusterMonitor(    "TTClusterMonitor"   )
            mcp2MCHit   = MCParticle2MCHitAlg( "MCP2TTMCHitAlg", MCHitPath = "MC/TT/Hits",
                                               OutputData = "/Event/MC/Particles2MCTTHits" )
            effCheck    = STEffChecker(        "TTEffChecker"       )
            effCheck.addTool(MCParticleSelector)
            effCheck.MCParticleSelector.zOrigin = 50.0
            effCheck.MCParticleSelector.pMin = 1.0*GeV
            effCheck.MCParticleSelector.betaGammaMin = 1.0
            GaudiSequencer("MoniTTSeq").Members += [ mcDepMoni, mcDigitMoni, digitMoni, clusMoni,
                                                     mcp2MCHit, effCheck ]
            if histOpt == "Expert":
                mcDepMoni.FullDetail   = True
                mcDigitMoni.FullDetail = True
                clusMoni.FullDetail    = True
                effCheck.FullDetail    = True

        if "OT" in moniSeq:
            from Configurables import MCOTDepositMonitor
            GaudiSequencer("MoniOTSeq").Members += [ MCOTDepositMonitor() ]
            if histOpt == "Expert":
                importOptions("$OTMONITORROOT/options/Boole.opts")

        if "RICH" in moniSeq:
            from Configurables import Rich__MC__Digi__DigitQC
            GaudiSequencer("MoniRICHSeq").Members += [ Rich__MC__Digi__DigitQC("RiDigitQC") ]

        if "CALO" in moniSeq:
            from Configurables import CaloDigitChecker
            importOptions("$CALOMONIDIGIOPTS/CaloDigitChecker.opts")
            GaudiSequencer("MoniCALOSeq").Members += [ CaloDigitChecker("SpdCheck"),
                                                       CaloDigitChecker("PrsCheck"),
                                                       CaloDigitChecker("EcalCheck"),
                                                       CaloDigitChecker("HcalCheck") ]

        if "MUON" in moniSeq:
            from Configurables import MuonDigitChecker
            GaudiSequencer("MoniMUONSeq").Members += [ "MuonDigitChecker" ]

        if "L0" in moniSeq:
            GaudiSequencer("MoniL0Seq").Members += [ GaudiSequencer("L0MoniSeq") ]

        if "MC" in moniSeq:
            from Configurables import UnpackMCVertex, UnpackMCParticle, CompareMCVertex, CompareMCParticle
            # This sequence only makes sense if input data is unpacked. Should be moved to Gauss
            testMCV = UnpackMCVertex(   "TestMCVertex",   OutputName = "MC/VerticesTest" )
            testMCP = UnpackMCParticle( "TestMCParticle", OutputName = "MC/ParticlesTest" )
            GaudiSequencer("MoniMCSeq").Members += [ testMCV, testMCP,
                                                     CompareMCParticle(), CompareMCVertex() ]

    def __apply_configuration__(self):
        log.info( self )
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions( self.getProp( "MainOptions" ) )
        
        # CRJ : Rich Digitisation now in python configurables
        from RichDigiSys.Configuration import RichDigiSysConf
        self.setOtherProp(RichDigiSysConf(),"UseSpillover")
        RichDigiSysConf().applyConf(GaudiSequencer("DigiRICHSeq"))
        
        self.defineDB()
        self.defineEvents()
        self.defineOptions()
        self.defineOutput()
        self.defineMonitors()
