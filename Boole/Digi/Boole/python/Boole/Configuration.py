"""
High level configuration tools for Boole
"""
__version__ = "$Id: Configuration.py,v 1.32 2009-01-28 17:36:05 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp )

class Boole(LHCbConfigurableUser):

    ## Known monitoring sequences, all run by default
    KnownMoniSubdets = [ "VELO", "IT", "TT", "OT", "RICH", "CALO", "MUON", "L0", "MC" ]
    
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
        if not spill :
            if self.getProp("DataType") == "DC06" and not tae:
                log.warning("Spillover is disabled. Should normally be enabled for DC06!")
            from Configurables import ( MuonBackground, MuonDigitization )
            MuonBackground("MuonLowEnergy").OutputLevel = ERROR
            MuonDigitization().OutputLevel = ERROR
        else:
            if self.getProp("DataType") != "DC06" :
                log.warning("Spillover is enabled. Should normally be enabled only for DC06!")
            from Configurables import MergeEventAlg, UnpackMCParticle, UnpackMCVertex
            initDataSeq = GaudiSequencer( "InitDataSeq" )
            spillPaths  = self.getProp("SpilloverPaths")
            spillAlg    = MergeEventAlg( name = "SpilloverAlg", PathList = spillPaths )
            initDataSeq.Members += [ spillAlg ]
            importOptions("$DIGIALGROOT/options/Spillover.opts")
            # Handle the unpacking of pSim containers
            for spill in spillPaths :
                particleUnpacker = UnpackMCParticle( "UnpackMCP" + spill )
                particleUnpacker.RootInTES = spill
                vertexUnpacker = UnpackMCVertex( "UnpackMCV" + spill )
                vertexUnpacker.RootInTES = spill
                DataOnDemandSvc().AlgMap[ spill + "/MC/Particles" ] = particleUnpacker
                DataOnDemandSvc().AlgMap[ spill + "/MC/Vertices" ] = vertexUnpacker

        self.configureMoni()
            
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
            

    def defineMonitors(self):
        # get all defined monitors
        monitors = self.getProp("Monitors") + LHCbApp().getProp("Monitors")
        # Currently no Boole specific monitors, so pass them all to LHCbApp
        LHCbApp().setProp("Monitors", monitors)

    def defineHistos(self):
        """
        Define histograms to save according to Boole.Histograms option
        """
        knownOptions = ["","None","Default","Expert"]
        histOpt = self.getProp("Histograms").capitalize()
        if histOpt not in knownOptions:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)

        if histOpt == "None" or histOpt == "":
            # HistogramPersistency still needed to read in Muon background.
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        if histOpt == "Expert":
            importOptions( "$BOOLEOPTS/ExpertCheck.opts" )

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


    def configureMoni(self):
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
        importOptions( "$BOOLEOPTS/BooleMoni.opts" )


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
        self.defineHistos()
        self.defineOutput()
        self.defineMonitors()
