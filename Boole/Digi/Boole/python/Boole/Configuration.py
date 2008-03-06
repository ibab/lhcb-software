"""
High level configuration tools for Boole
"""
__version__ = "$Id: Configuration.py,v 1.2 2008-03-06 07:01:50 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from Configurables import ( CondDBAccessSvc, MagneticFieldSvc,
                            MCSTDepositCreator ) 

# Exported symbols
__all__ = [ "BooleSetDB", "BooleSetEvents", "BooleSetOptions",
            "BooleGenerateTAE", "BooleDisableSpillover",
            "BooleSaveHistos", "BooleHistosName",
            "BooleSetOutput", "BooleOutputName" ]

def BooleSetDB( condDBtag ):

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

def BooleSetEvents( numEvents, skipEvents, skipSpill ):
    if numEvents  > 0 : ApplicationMgr().EvtMax = numEvents
    if skipEvents > 0 : EventSelector().FirstEvent = skipEvents + 1
    if skipSpill  > 0 : EventSelector("SpilloverSelector").FirstEvent = skipSpill + 1

def BooleSetOptions( generateTAE, useSpillover ):
    if generateTAE : BooleGenerateTAE()
    if not useSpillover : BooleDisableSpillover()

def BooleGenerateTAE():
    """
    switch to generate Time Alignment events.
    """
    BooleDisableSpillover()
    initMUONSeq = GaudiSequencer( "InitMUONSeq" )
    initMUONSeq.Members.remove( "MuonBackground/MuonFlatSpillover" )
    importOptions( "$BOOLEOPTS/TAE-Prev1.opts" ) # add misaligned RawEvent


def BooleDisableSpillover():
    """
    Switch to disable spillover. Spillover is on by default
    """
    print "WARNING: Spillover is disabled"
    initDataSeq = GaudiSequencer( "InitDataSeq" )
    initDataSeq.Members.remove( "MergeEventAlg/SpilloverAlg" )


def BooleSaveHistos( histosName, expertHistos ):
    """
    Save histograms. If expert, fill and save also the expert histograms
    """
    if expertHistos: importOptions( "$BOOLEOPTS/ExpertCheck.opts" )
    HistogramPersistencySvc().OutputFile = histosName

def BooleHistosName( dataset, numEvents, expertHistos, generateTAE ):
    histosName   = dataset
    if ( numEvents > 0 ): histosName += '-' + str(numEvents) + 'ev'
    if ( generateTAE )  : histosName += '-TAE'
    if expertHistos     : histosName += '-expert'
    histosName += '-histos.root'
    return histosName
    
def BooleSetOutput( mdf, extended, l0yes, l0etc, nowarn ):
    """
    Set up output stream according to output data type
    """
    if mdf:
        importOptions( "$BOOLEOPTS/RawWriter.opts" )
        MyWriter = OutputStream( "RawWriter" )
        ApplicationMgr().OutStream.append( "RawWriter" )
    else:
        if ( extended ): importOptions( "$STDOPTS/ExtendedDigi.opts" )
        MyWriter = OutputStream( "DigiWriter" )
        ApplicationMgr().OutStream.append( "DigiWriter" )

    if l0yes : MyWriter.RequireAlgs.append( "L0Filter" )
    if l0etc : ApplicationMgr().OutStream.append( "Sequencer/SeqWriteTag" )
    if nowarn: importOptions( "$BOOLEOPTS/SuppressWarnings.opts" )

def BooleOutputName( dataset, numEvents, generateTAE, writeL0Only, extendedDigi ):
    """
    Build a name for the output file, based in input options
    """
    outputName = dataset
    if ( numEvents > 0 ): outputName += '-' + str(numEvents) + 'ev'
    if ( generateTAE )  : outputName += '-TAE'
    if ( writeL0Only ) : outputName += '-L0Yes'
    if ( extendedDigi ): outputName += '-extended'
    return outputName
