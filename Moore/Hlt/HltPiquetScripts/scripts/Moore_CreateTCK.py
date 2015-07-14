import os
import Gaudi.Configuration
from Moore.Configuration import Moore
from GaudiKernel.SystemOfUnits import MeV, mm, m

## Adapt these to your local environment
Moore().generateConfig = True
Moore().ForceSingleL0Configuration = True
Moore().OutputLevel = 4
Moore().Split = ''
Moore().EvtMax = 500
Moore().DataType = '2015'
Moore().CondDBtag = 'cond-20150617'
Moore().DDDBtag = 'dddb-20150526'

from Configurables import EventSelector
EventSelector().PrintFreq = 100

from Configurables import HltConf
HltConf().Verbose = True

## EM

Moore().configLabel = 'Early measurements test June 2015; split RICH mirror lines'
Moore().ThresholdSettings = 'Physics_draftEM2015'

Moore().inputFiles = ['StripHLT_153759.mdf']

## SMOG

# Moore().configLabel = 'SMOG May 2015'
# Moore().ThresholdSettings = 'SMOG_April2015'


# from Configurables import HltRecoConf
# HltRecoConf().Forward_HPT_MinPt =   80 * MeV
# HltRecoConf().Forward_HPT_MinP  = 1000 * MeV

## vdM

# Moore().configLabel = "vdM Scan 2015, *FullZ prescale = 1.00, N_tr > 9"
# Moore().ThresholdSettings = 'VanDerMeerScan_2015'

# Moore().inputFiles = ['RedoL0_0x0141.mdf']

## Velo Microbias

# Moore().configLabel = 'Prescale = 0.01'
# Moore().ThresholdSettings = 'Calibration_VeloMicroBias'

# Moore().inputFiles = ['RedoL0_0x1715.mdf']

## Moore PassThrough

# Moore().configLabel = 'PassThrough with MicroBias'
# Moore().ThresholdSettings = 'Commissioning_MB_PassThrough'
# Moore().ForceSingleL0Configuration = False

# Moore().inputFiles = ['StripHLT_153759.mdf']

## Input

# from PRConfig.TestFileDB import test_file_db
# input = test_file_db['2012_raw_default']
# input.run(configurable=Moore())

from Configurables import CondDB
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().EnableRunChangeHandler = True
CondDB().Tags["ONLINE"] = 'fake'
CondDB().setProp("IgnoreHeartBeat", True)

import sys
try:
    import AllHlt1
except ImportError:
    rd = '/group/online/hlt/conditions/RunChangeHandler'
    sys.path.append(rd)
    import AllHlt1

CondDB().RunChangeHandlerConditions = AllHlt1.ConditionMap

# manual post-postConfigAction, very dirty....
# from Configurables import LHCbConfigurableUser
# from GaudiKernel.Configurable import applyConfigurableUsers
# applyConfigurableUsers()


# from Gaudi.Configuration import allConfigurables
# from Configurables import HltGenConfig

# overrules = {}
# to_replace = ['$PARAMFILESROOT']

# import re
# re_env = re.compile(r"(\$[\w\d_]+)")

# from collections import defaultdict
# overrules = defaultdict(list)
# for aot, rules in HltGenConfig().Overrule.iteritems():
#     overrules[aot] += rules

# for name, conf in allConfigurables.iteritems():
#    for prop_name, prop_val in conf.properties().iteritems():
#       m = re_env.search(str(prop_val))
#       if (m) and m.group(1) in to_replace and not isinstance(conf, LHCbConfigurableUser):
#          overrules[name] += ["%s:%s" % (prop_name, repr(prop_val))]

# HltGenConfig().Overrule = dict(overrules)

# from GaudiPython.Bindings import AppMgr
# gaudi = AppMgr()

# gaudi.run(1000)

# gaudi.finalize()
# gaudi.exit()
