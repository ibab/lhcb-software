#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
import Gaudi.Configuration
from Moore.Configuration import Moore

# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'Default'
#Moore().ThresholdSettings = 'Commissioning_PassThrough'
#Moore().configLabel = 'ODINRandom acc=0, TELL1Error acc=0'

#Moore().ThresholdSettings = 'Calibration_VeloMicroBias'
#Moore().configLabel = 'Default'
Moore().ThresholdSettings = 'Physics_draft2011'
Moore().configLabel = 'Physics_Feb2011'

from Configurables import ConfigTarFileAccessSvc
ConfigTarFileAccessSvc().CompressOnWrite = True
#Moore().UseTCK = True
#Moore().InitialTCK = "0x0034002a"
#Moore().CheckOdin = False


Moore().Verbose = True
#Moore().EvtMax = 1000

Moore().UseDBSnapshot = False
Moore().DDDBtag    = 'hlt-20100906'
Moore().CondDBtag  = 'hlt-20100906'
Moore().Simulation = False
Moore().DataType   = '2010'
#Moore().inputFiles  = [ 'castor:/castor/cern.ch/user/g/graven/raw/74733_0x0025_MB1.raw' ]
#Moore().inputFiles  = [ '/data/bfys/graven/74733_0x001a002a_MB1.raw' ]
#Moore().inputFiles  = [ '/data/bfys/graven/80881_0x002a_MBNB_L0Phys.raw' ]
Moore().inputFiles  = [ '/usr/tmp/80881_0x002a_MBNB_L0Phys.raw' ]
Moore().outputFile = '/usr/tmp/out.raw'

from Configurables import EventSelector
EventSelector().PrintFreq = 1000

from Configurables import HltSelReportsWriter
HltSelReportsWriter().OutputLevel = 2
#from Configurables import Hlt__Service
#Hlt__Service().Pedantic = False

#from Configurables import NameAuditor
#Moore().EnableAuditor = [ NameAuditor() ]

#import os
#outName = '%s.%s' % ( Moore().ThresholdSettings , os.getpid() )
#from Configurables import HistogramPersistencySvc
#HistogramPersistencySvc().OutputFile = '%s.root' % outName

