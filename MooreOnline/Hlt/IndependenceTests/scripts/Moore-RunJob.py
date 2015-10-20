#!/usr/bin/env python
#
# Script to obtain all Hlt Lines for a ThresholdSetting

import os, sys, optparse

# Configuration
from Gaudi.Configuration import *
#from GaudiConf.Configuration import *
from Moore.Configuration import Moore
from Configurables import GaudiSequencer as Sequence
from Configurables import ( EventSelector,
                            CondDB )
import Configurables
from Configurables import TupleHltDecReports

from Config import ConfigLFC
from IndependenceTests.Config import ( Config, ConfigOutputLevel,
                                       ConfigTupleAlg )
# GaudiPython
from GaudiPython import AppMgr

def main():

    # Setup the option parser
    usage = "usage: %prog [options] inputfile <inputfile>"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType",
                       default="2009", help="DataType to run on.")
    parser.add_option( "-n", "--evtmax", type = "int", action = "store",
                       dest = "EvtMax", default = 1e4, help = "Number of events to run" )
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='MC09-20090602', help="DDDBTag to use" )
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'sim-20090402-vc-md100', help = "CondDBtag to use" )
    parser.add_option( "--settings", action = "store", dest="ThresholdSettings",
                       default = 'Physics_10000Vis_1000L0_40Hlt1_Apr09',
                       help = "ThresholdSettings to use")
    parser.add_option( "-s", "--simulation", action = "store_true", dest = "Simulation",
                       default = False, help = "Run on simulated data")
    parser.add_option( "--dbsnapshot", action = "store_true", dest = "UseDBSnapshot",
                       default = False, help = "Use a DB snapshot" )
    parser.add_option( "--snd", action = "store", dest = "SnapshotDirectory",
                       default = '/user/graven/MOORE/conditions', type = "string",
                       help = "DB Snapshot directory" )
    parser.add_option( "--oracle", action = "store_true", dest = "UseOracle",
                       default = False, help = "Use Oracle" )
    parser.add_option( "-v", "--verbose", action = "store_true", dest = "Verbose",
                       default = False, help = "Verbose output" )
    parser.add_option( "--acceptslow", action = "store_true", dest = "AcceptIfSlow",
                       default = False, help = "Accept slow events" )
    parser.add_option( "--hlt1lines", action = "store", dest = "Hlt1Lines",
                       default = "", help = "Colon seperated list of additional hlt1 lines" )
    parser.add_option( "--hlt2lines", action = "store", dest = "Hlt2Lines",
                       default = "", help = "Colon seperated list of additional hlt2 lines" )
    parser.add_option( "--rch", action = "store_true", dest = "RunChangeHandler",
                       default = False, help = "Use the RunChangeHandler" )
    parser.add_option( "--l0", action = "store_true", dest = "L0",
                       default = False, help = "Rerun L0" )
    parser.add_option( "--site", action = "store", type = "string", dest = "Site",
                       default = "", help = "Site at which we run" )
    parser.add_option( "--tempdir", action = "store", type = "string", dest = "Tempdir",
                       default = "/tmpdir", help = "Tempdir for the filestager" )
    parser.add_option( "--tuplefile", action = "store", type = "string", dest = "TupleFile",
                       default = "tuples.root", help = "NTuple filename" )
    parser.add_option( "-f", "--filestager", action = "store_true", dest = "FileStager",
                       default = False, help = "Use the filestager" )
    parser.add_option( "-c", "--verbose_classes", action = "store", type = "string",
                       dest = "VerboseClasses", default = "",
                       help = "Colon seperated list of classes to be made verbose." )

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    # Put the options into the Moore configurable
    Moore().ThresholdSettings = options.ThresholdSettings

    Moore().Verbose = options.Verbose
    Moore().EvtMax = options.EvtMax

    Moore().UseDBSnapshot = options.UseDBSnapshot
    Moore().DBSnapshotDirectory = options.SnapshotDirectory
    Moore().DDDBtag   = options.DDDBtag
    Moore().CondDBtag = options.CondDBtag
    Moore().Simulation = options.Simulation
    Moore().DataType   = options.DataType
    Moore().EnableAcceptIfSlow = options.AcceptIfSlow
    Moore().outputFile = ""
    Moore().ForceSingleL0Configuration = False
    Moore().RequireRoutingBits = [ 0x0, 0x4, 0x0 ]
    Moore().L0 = options.L0
    Moore().ReplaceL0BanksWithEmulated = options.L0

    if options.UseOracle:
        CondDB().UseOracle = True
        site = 'UNKNOWN'
        try:
            site = os.environ[ 'DIRACSITE' ]
        except KeyError:
            if len( options.Site ):
                site = options.Site
                config = ConfigLFC( site )
                appendPostConfigAction( config.setLFCSite )

    # Inputdata is now handled through separate option files, this is for
    # testing/convenience
    if len( args ):
        Moore().inputFiles = args

    try:
        descriptor = EventSelector().Input[ 0 ]
        if descriptor.find( ".raw" ) != -1:
            from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc
            EventPersistencySvc().CnvServices.append( RawDataCnvSvc('RawDataCnvSvc') )
        elif descriptor.find( ".dst" ) != -1:
            importOptions('$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts')
    except IndexError:
        pass

    freq = 0
    if ( len( options.VerboseClasses ) ):
        freq = 1
    else:
        freq = 100
    EventSelector().PrintFreq = freq

    # RunChangeHandler
    if options.RunChangeHandler:
        Moore().EnableRunChangeHandler = True
        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True

    # XMLSummary
    from Configurables import LHCbApp
    LHCbApp().XMLSummary = 'summary.xml'

    # Use the filestager?
    if options.FileStager:
        from FileStager.Configuration import configureFileStager
        configureFileStager()

    # Put the comma separated lists of lines into lists
    hlt1Lines = []
    for line in options.Hlt1Lines.split( ";" ):
        if ( len( line.strip() ) ):
            hlt1Lines.append( line )

    hlt2Lines = []
    for line in options.Hlt2Lines.split( ";" ):
        if ( len( line.strip() ) ):
            hlt2Lines.append( line )

    # parse the specification of the classes to set to verbose
    verboseClasses = []
    for cl in options.VerboseClasses.split( ";" ):
        cl = cl.strip()
        if ( len( cl ) ):
            verboseClasses.append( cl.replace( "::", "__" ) )

    # Instantiate the class to apply the required configuration
    config = Config( hlt1Lines, hlt2Lines )
    appendPostConfigAction( config.postConfigAction )

    # Set the OutputLevel for requested classed
    if len( verboseClasses ):
        configOL = ConfigOutputLevel( verboseClasses, 1 )
        appendPostConfigAction( configOL.setOutputLevel )

    # Add the TupleHltDecReports alg to the sequence
    if options.TupleFile:
        tupleAlg = TupleHltDecReports( "TupleHltDecReports" )
        addTupleAlg = ConfigTupleAlg( filename = options.TupleFile )
        appendPostConfigAction( addTupleAlg.addTupleAlg )

    # Instantiate the AppMgr
    appMgr = AppMgr()

    # Make sure that we have Hlt lines to run
    if not len( Sequence( "Hlt1" ).Members ) or not len( Sequence( "Hlt2" ).Members ):
        print "error, no lines to run\n"
        return 2

    # Run the required number of events
    sc = appMgr.run( Moore().EvtMax )
    if sc.isFailure(): return 2

    # Done
    sc = appMgr.exit()
    if sc.isFailure():
        return 2
    else:
        return 0

if __name__ == "__main__":
    sys.exit( main() )
