"""
High level configuration tools for Boole
"""
__version__ = "$Id: Configuration.py,v 1.23 2008-11-12 17:43:51 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions

class Boole(LHCbConfigurableUser):
    __slots__ = {
        "EvtMax"         : -1
       ,"SkipEvents"     : 0
       ,"SkipSpill"      : 0
       ,"useSpillover"   : False
       ,"generateTAE"    : False
       ,"Outputs"        : [ "DIGI" ]
       ,"writeL0Only"    : False
       ,"extendedDigi"   : False
       ,"Histograms"     : "Default"
       ,"mainOptions"    : '$BOOLEOPTS/Boole.opts'
       ,"noWarnings"     : False
       ,"DatasetName"    : ''
       ,"DDDBtag"        : "2008-default"
       ,"condDBtag"      : "2008-default"
       ,"UseOracle"      : False
       ,"monitors"       : []
        }

    _propertyDocDct = { 
        'EvtMax'       : """ Maximum number of events to process """
       ,'SkipEvents'   : """ Number of events to skip """
       ,'SkipSpill'    : """ Number of spillover events to skip """
       ,'useSpillover' : """ Flag to enable spillover (default True) """
       ,'generateTAE'  : """ Flag to simulate time alignment events (default False) """
       ,'Outputs'      : """ List of outputs: ['MDF','DIGI','L0ETC'] (default 'DIGI') """
       ,'writeDigi'    : """ Flag to enable POOL .digi output format (default True) """
       ,'writeL0ETC'   : """ Flag to enable writing ETC of L0 selected events (default False) """
       ,'writeL0Only'  : """ Flag to write only L0 selected events (default False) """
       ,'extendedDigi' : """ Flag to add MCHits to .digi output file (default False) """
       ,'Histograms'   : """ Type of histograms: ['None','Default','Expert'] """
       ,'mainOptions'  : """ Top level options to import. Default: $BOOLEOPTS/Boole.opts """
       ,'noWarnings'   : """ Flag to suppress all MSG::WARNING or below (default False) """ 
       ,'DatasetName'  : """ String used to build output file names """
       ,'DDDBtag'      : """ Tag to use for DDDB. Default 'DC06-default' """
       ,'condDBtag'    : """ Tag to use for CondDB. Default 'DC06-default' """
       ,'UseOracle'    : """ Flag to enable Oracle CondDB. Default False (use SQLDDDB) """
       ,'monitors'     : """ List of monitors to execute """
       }
    
    __used_configurables__ = [ LHCbApp ]

    def defineDB(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["condDBtag","DDDBtag","UseOracle"])
        LHCbApp().Simulation = True

        # Special options for DC06 data processing
        if LHCbApp().getProp("DDDBtag").find("DC06") != -1 :
            from Configurables import (MCSTDepositCreator, MuonDigitToRawBuffer)

            ApplicationMgr().Dlls += [ "HepMCBack" ]
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
        tae   = self.getProp("generateTAE")
        spill = self.getProp("useSpillover")
        if tae       : self.enableTAE()
        if not spill : self.disableSpillover()

    def enableTAE(self):
        """
        switch to generate Time Alignment events.
        """
        self.disableSpillover()
        initMUONSeq = GaudiSequencer( "InitMUONSeq" )
        initMUONSeq.Members.remove( "MuonBackground/MuonFlatSpillover" )
        importOptions( "$BOOLEOPTS/TAE-Prev1.opts" ) # add misaligned RawEvent

    def disableSpillover(self):
        """
        Switch to disable spillover. Spillover is on by default
        """
        print "# WARNING: Spillover is disabled"
        initDataSeq = GaudiSequencer( "InitDataSeq" )
        initDataSeq.Members.remove( "MergeEventAlg/SpilloverAlg" )
        from Configurables import ( MuonBackground, MuonDigitization )
        MuonBackground("MuonLowEnergy").OutputLevel = ERROR
        MuonDigitization().OutputLevel = ERROR

    def defineMonitors(self):
        # get all defined monitors
        monitors = self.getProp("monitors") + LHCbApp().getProp("monitors")
        # pass to LHCbApp any monitors not dealt with here
        LHCbApp().setProp("monitors", monitors)

    def defineHistos(self):
        """
        Define histograms to save according to Boole.Histograms option
        """
        knownOptions = ["None","Default","Expert"]
        histOpt = self.getProp("Histograms").capitalize()
        if histOpt not in knownOptions:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)

        if histOpt == "None":
            # HistogramPersistency still needed to read in Muon background.
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        if histOpt == "Expert":
            importOptions( "$BOOLEOPTS/ExpertCheck.opts" )

        # Use a default histogram file name if not already set
        if not hasattr( HistogramPersistencySvc(), "OutputFile" ):
            histosName   = self.getProp("DatasetName")
            if (self.evtMax() > 0): histosName += '-' + str(self.evtMax()) + 'ev'
            generateTAE  = self.getProp("generateTAE")
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

        l0yes = self.getProp( "writeL0Only" )

        if "DIGI" in outputs:
            # Objects to be written to output file
            importOptions("$STDOPTS/DigiContent.opts")
            extended = self.getProp("extendedDigi")
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

        nowarn = self.getProp( "noWarnings" )
        if nowarn: importOptions( "$BOOLEOPTS/SuppressWarnings.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        generateTAE  = self.getProp("generateTAE")
        if ( generateTAE )  : outputName += '-TAE'
        l0yes = self.getProp( "writeL0Only" )
        if ( l0yes ) : outputName += '-L0Yes'
        extended = self.getProp("extendedDigi")
        if ( extended ): outputName += '-extended'
        return outputName

    def evtMax(self):
        return LHCbApp().evtMax()

    def __apply_configuration__(self):
        log.info( self )
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions( self.getProp( "mainOptions" ) )
        
        # CRJ : Rich Digitisation now in python configurables
        from RichDigiSys.Configuration import RichDigiSysConf
        self.setOtherProp(RichDigiSysConf(),"useSpillover")
        RichDigiSysConf().applyConf(GaudiSequencer("DigiRICHSeq"))
        
        self.defineDB()
        self.defineEvents()
        self.defineOptions()
        self.defineHistos()
        self.defineOutput()
        self.defineMonitors()
