#!/usr/bin/env python
import optparse
import sys

# Setup the option parser
usage = "usage: %prog [options] settings"
parser = optparse.OptionParser( usage = usage )
parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                   default="2011", help="DataType to run on.")
parser.add_option( "-n", "--evtmax", type="int", action = "store", dest = "EvtMax",
                   default = 5000, help = "Number of events to run" )
parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                   default='head-20110302', help="DDDBTag to use" )
parser.add_option( "--l0", action="store_true", dest="L0",
                   default=False, help="Rerun L0" )
parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                   default = 'head-20110622', help = "CondDBtag to use" )

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

Moore().Simulation = False
Moore().DataType = options.DataType
Moore().UseDBSnapshot = False
Moore().DDDBtag    = options.DDDBtag
Moore().CondDBtag  = options.CondDBtag

if options.L0:
    Moore().L0 = True
    Moore().ReplaceL0BanksWithEmulated = True

Moore().ForceSingleL0Configuration = True
from Configurables import L0MuonAlg
L0MuonAlg( "L0Muon" ).L0DUConfigProviderType = "L0DUConfigProvider"

EventSelector().Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/user/e/evh/81349/81349_0x002a_MBNB_L0Phys.raw' SVC='LHCb::MDFSelector'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/e/evh/80881/80881_0x002a_MBNB_L0Phys.raw' SVC='LHCb::MDFSelector'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/e/evh/79647/79647_0x002a_MBNB_L0Phys.raw' SVC='LHCb::MDFSelector'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/e/evh/79646/79646_0x002a_MBNB_L0Phys.raw' SVC='LHCb::MDFSelector'"
    ]
EventSelector().PrintFreq = 100

## GaudiPython
from GaudiPython.Bindings import AppMgr
appMgr = AppMgr()
appMgr.initialize()

appMgr.run( Moore().EvtMax )
