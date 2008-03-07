"""
High level configuration tools for Boole
"""
__version__ = "$Id: Configuration.py,v 1.4 2008-03-07 16:40:47 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from Configurables import ( CondDBAccessSvc, MagneticFieldSvc,
                            MCSTDepositCreator ) 

class Boole(ConfigurableUser):
    __slots__ = {
        "skipEvents":   0     # events to skip
       ,"skipSpill":    0     # spillover events to skip
       ,"useSpillover": True  # set to False to disable spillover
       ,"generateTAE":  False # set to True to simulate time alignment events
       ,"writeRawMDF":  False # set to True to simulate Raw data.
                              # Default is POOL .digi
       ,"writeL0ETC":   False # set to True to write ETC of L0 selected events
       ,"writeL0Only":  False # set to True to write only L0 selected events
       ,"extendedDigi": False # set to True to add MCHits to .digi output file
       ,"expertHistos": False # set to True to write out expert histos
       ,"noWarnings":   False # suppress all messages with MSG::WARNING or
                              # below 
       ,"datasetName":  '00001820_00000001' # string used to build file names
       ,"condDBtag":    "DC06-latest" # conditions database tag
        }

    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def defineDB(self):
        condDBtag = self.getProp("condDBtag")
        # For all DC06 cases, use latest DC06 tag
        if condDBtag.find("DC06") != -1 :
            importOptions( "$DDDBROOT/options/DC06.py" )
            #-- Do not use Geant deposited charge in DC06
            MCSTDepositCreator("MCITDepositCreator").DepChargeTool = "SiDepositedCharge"
            MCSTDepositCreator("MCTTDepositCreator").DepChargeTool = "SiDepositedCharge"

        else:
            CondDBAccessSvc( "DDDB",     DefaultTAG = condDBtag )
            CondDBAccessSvc( "LHCBCOND", DefaultTAG = condDBtag )
            #-- Always DC06 magnetic field for now....
            MagneticFieldSvc().FieldMapFile = os.environ['FIELDMAPROOT']+'/cdf/field047.cdf'

    def defineEvents(self):
        skipEvents = self.getProp("skipEvents")
        if skipEvents > 0 :
            if hasattr(EventSelector(),"FirstEvent"):
                print "EventSelector().FirstEvent already defined, ignoring Boole().skipEvents"
            else:
                EventSelector().FirstEvent = skipEvents + 1
        skipSpill  = self.getProp("skipSpill")
        if skipSpill  > 0 :
            if hasattr(EventSelector("SpilloverSelector"),"FirstEvent"):
                print "EventSelector('SpilloverSelector').FirstEvent already defined, ignoring Boole().skipSpill"
            else:
                EventSelector("SpilloverSelector").FirstEvent = skipSpill + 1

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
        print "WARNING: Spillover is disabled"
        initDataSeq = GaudiSequencer( "InitDataSeq" )
        initDataSeq.Members.remove( "MergeEventAlg/SpilloverAlg" )    

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
        mdf = self.getProp( "writeRawMDF" )
        if mdf:
            importOptions( "$BOOLEOPTS/RawWriter.opts" )
            MyWriter = OutputStream( "RawWriter" )
            # ApplicationMgr().OutStream.append( "RawWriter" ) # Already in RawWriter.opts
        else:
            extended = self.getProp("extendedDigi")
            if ( extended ): importOptions( "$STDOPTS/ExtendedDigi.opts" )
            MyWriter = OutputStream( "DigiWriter" )
            ApplicationMgr().OutStream.append( "DigiWriter" )

        l0yes = self.getProp( "writeL0Only" )
        if l0yes : MyWriter.RequireAlgs.append( "L0Filter" )
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
        self.defineDB()
        self.defineEvents()
        self.defineOptions()
        self.defineHistos()
        self.defineOutput()
