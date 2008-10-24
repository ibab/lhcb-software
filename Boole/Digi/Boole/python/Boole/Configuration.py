"""
High level configuration tools for Boole
"""
__version__ = "$Id: Configuration.py,v 1.21 2008-10-24 13:09:49 jonrob Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import ( CondDBCnvSvc, MagneticFieldSvc,
                            MCSTDepositCreator, MuonDigitToRawBuffer ) 
import GaudiKernel.ProcessJobOptions

class Boole(LHCbConfigurableUser):
    __slots__ = {
        "EvtMax":          -1 # Maximum number of events to process
       ,"skipEvents":   0     # events to skip
       ,"skipSpill":    0     # spillover events to skip
       ,"useSpillover": True  # set to False to disable spillover
       ,"generateTAE":  False # set to True to simulate time alignment events
       ,"writeRawMDF":  False # set to True to simulate Raw data.
       ,"writeDigi":    True  # set to False to remove POOL .digi output
       ,"writeL0ETC":   False # set to True to write ETC of L0 selected events
       ,"writeL0Only":  False # set to True to write only L0 selected events
       ,"extendedDigi": False # set to True to add MCHits to .digi output file
       ,"expertHistos": False # set to True to write out expert histos
       ,"mainOptions" : '$BOOLEOPTS/Boole.opts' # top level options to import
       ,"noWarnings":   False # suppress all messages with MSG::WARNING or below 
       ,"datasetName":  '00001820_00000001' # string used to build file names
       ,"DDDBtag":      "DC06-default"    # geometry database tag
       ,"condDBtag":    "DC06-default"    # conditions database tag
       ,"useOracleCondDB": False  # if False, use SQLDDDB instead
       ,"monitors": []        # list of monitors to execute
        }

    def defineDB(self):
        # Prefer Boole default over LHCbApp default if not set explicitly
        self.setProp( "condDBtag", self.getProp("condDBtag") )
        self.setProp( "DDDBtag", self.getProp("DDDBtag") )
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["condDBtag","DDDBtag","useOracleCondDB"]) 

        # Special options for DC06 data processing
        if LHCbApp().getProp("DDDBtag").find("DC06") != -1 :
            ApplicationMgr().Dlls += [ "HepMCBack" ]
            MCSTDepositCreator("MCITDepositCreator").DepChargeTool = "SiDepositedCharge"
            MCSTDepositCreator("MCTTDepositCreator").DepChargeTool = "SiDepositedCharge"

        # Special options for 2008 data processing
        if LHCbApp().getProp("DDDBtag").find("-2008") != -1 :
            MuonDigitToRawBuffer().VType = 2 # New RawBank type
          
    def defineEvents(self):
        evtMax = self.getProp("EvtMax")
        if hasattr(LHCbApp(),"EvtMax"):
            print "# LHCbApp().EvtMax already defined, ignoring Boole().EvtMax"
        else:
            LHCbApp().EvtMax = evtMax

        skipEvents = self.getProp("skipEvents")
        if skipEvents > 0 :
            if hasattr(LHCbApp(),"skipEvents"):
                print "# LHCbApp().skipEvents already defined, ignoring Boole().skipEvents"
            else:
                LHCbApp().skipEvents = skipEvents

        skipSpill  = self.getProp("skipSpill")
        if skipSpill  > 0 :
            if hasattr(EventSelector("SpilloverSelector"),"FirstEvent"):
                print "# EventSelector('SpilloverSelector').FirstEvent already defined, ignoring Boole().skipSpill"
            else:
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
        Save histograms. If expert, fill and save also the expert histograms
        """
        expertHistos = self.getProp("expertHistos")
        if expertHistos: importOptions( "$BOOLEOPTS/ExpertCheck.opts" )

    def histosName(self):
        histosName   = self.getProp("datasetName")
        if ( self.evtMax() > 0 ): histosName += '-' + str(self.evtMax()) + 'ev'
        generateTAE  = self.getProp("generateTAE")
        if ( generateTAE )  : histosName += '-TAE'
        expertHistos = self.getProp("expertHistos")
        if expertHistos     : histosName += '-expert'
        histosName += '-histos.root'
        return histosName
    
    def defineOutput(self):
        """
        Set up output stream according to output data type
        """
        l0yes = self.getProp( "writeL0Only" )

        digi = self.getProp( "writeDigi" )
        if digi:
            # Objects to be written to output file
            OutputStream("DigiWriter").Preload = False
            importOptions("$STDOPTS/DigiContent.opts")
            extended = self.getProp("extendedDigi")
            if ( extended ): importOptions( "$STDOPTS/ExtendedDigi.opts" )
            MyWriter = OutputStream( "DigiWriter" )
            if l0yes : MyWriter.RequireAlgs.append( "L0Filter" )
            ApplicationMgr().OutStream.append( "DigiWriter" )

        mdf   = self.getProp( "writeRawMDF" )
        # mdf case must be after digi case, because RawWriter.opts adds EventNodeKiller to OutStream list
        if mdf:
            importOptions( "$BOOLEOPTS/RawWriter.opts" )
            MyWriter = OutputStream( "RawWriter" )
            if l0yes : MyWriter.RequireAlgs.append( "L0Filter" )
            # ApplicationMgr().OutStream.append( "RawWriter" ) # Already in RawWriter.opts

        l0etc = self.getProp( "writeL0ETC" )
        if l0etc : ApplicationMgr().OutStream.append( "Sequencer/SeqWriteTag" )
        nowarn = self.getProp( "noWarnings" )
        if nowarn: importOptions( "$BOOLEOPTS/SuppressWarnings.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("datasetName")
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        generateTAE  = self.getProp("generateTAE")
        if ( generateTAE )  : outputName += '-TAE'
        l0yes = self.getProp( "writeL0Only" )
        if ( l0yes ) : outputName += '-L0Yes'
        extended = self.getProp("extendedDigi")
        if ( extended ): outputName += '-extended'
        return outputName

    def evtMax(self):
        if hasattr(ApplicationMgr(),"EvtMax"):
            return getattr(ApplicationMgr(),"EvtMax")
        else:
            return ApplicationMgr().getDefaultProperties()["EvtMax"]

    def applyConf(self):
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
        LHCbApp().applyConf()
        # Use SIMCOND if not DC06
        if LHCbApp().getProp("condDBtag").find("DC06") == -1:
            CondDBCnvSvc( CondDBReader = allConfigurables["SimulationCondDBReader"] )
