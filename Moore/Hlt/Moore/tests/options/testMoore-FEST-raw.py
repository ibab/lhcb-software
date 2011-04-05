#!/usr/bin/env gaudirun.py
from Gaudi.Configuration import *
from Moore.Configuration import *

Moore().EvtMax = 2000
Moore().Simulation = False
Moore().DataType = '2010'
Moore().UseDBSnapshot = False
Moore().DDDBtag    = 'head-20110302'
Moore().CondDBtag  = 'head-20110318'

Moore().inputFiles  = [ 'castor:/castor/cern.ch/user/e/evh/81349/81349_0x0032_MBNB_L0Phys.raw'
                      , 'castor:/castor/cern.ch/user/e/evh/80881/80881_0x0032_MBNB_L0Phys.raw'
                      , 'castor:/castor/cern.ch/user/e/evh/79646/79646_0x0032_MBNB_L0Phys.raw'
                      , 'castor:/castor/cern.ch/user/e/evh/79647/79647_0x0032_MBNB_L0Phys.raw'
                      ]
#Moore().inputFiles  = [ '/data/bfys/graven/81349_0x0033_MBNB_L0Phys.raw'
#                      , '/data/bfys/graven/80881_0x0033_MBNB_L0Phys.raw'
#                      , '/data/bfys/graven/79646_0x0033_MBNB_L0Phys.raw'
#                      , '/data/bfys/graven/79647_0x0033_MBNB_L0Phys.raw'
#                      ]


Moore().ForceSingleL0Configuration = False


EventSelector().PrintFreq = 100
