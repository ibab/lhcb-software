#!/usr/bin/env python
import optparse
import sys

# Setup the option parser
usage = "usage: %prog [options] settings"
parser = optparse.OptionParser( usage = usage )
parser.add_option( "-d", "--data", action="store", dest="Data",
                   default="2012_raw_default", help="Data to run on.")
parser.add_option( "-n", "--evtmax", type="int", action = "store", dest = "EvtMax",
                   default = 5000, help = "Number of events to run" )
parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                   default='dddb-20120831', help="DDDBTag to use" )
parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                   default = 'cond-20120831', help = "CondDBtag to use. " )
parser.add_option( "--multil0", action = "store_true", dest = "multil0",
                   default = False, help = "not ForceSingleLevelZeroConfig. " )

# Parse the arguments
(options, args) = parser.parse_args()

# Make sure there is data to run on
if len( args ) != 1:
    print "error, no settings specified"
    sys.exit( 2 )

from Gaudi.Configuration import *
from Moore.Configuration import *

Moore().ThresholdSettings = args[ 0 ]
Moore().EvtMax = options.EvtMax

#Moore().DataType = options.DataType
Moore().DDDBtag    = options.DDDBtag
Moore().CondDBtag  = options.CondDBtag
Moore().ForceSingleL0Configuration=(not options.multil0)
Moore().EnableTimer=args[ 0 ]+".csv"

#take the test data and automatically set with the PRConfig
from PRConfig import TestFileDB
TestFileDB.test_file_db[options.Data].run(withDB=False,configurable=Moore())

EventSelector().PrintFreq = 100

## GaudiPython
from GaudiPython.Bindings import AppMgr
appMgr = AppMgr()
appMgr.initialize()

appMgr.run( Moore().EvtMax )
