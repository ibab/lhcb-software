import os
import Gaudi.Configuration
from Moore.Configuration import Moore
from GaudiKernel.SystemOfUnits import MeV, mm, m

Moore().ThresholdSettings = 'VanDerMeerScan_2015'
Moore().ForceSingleL0Configuration = True
Moore().OutputLevel = 3
Moore().RemoveInputHltRawBanks = False
Moore().EvtMax = -1
Moore().EnableTimer = True

from PRConfig.TestFileDB import test_file_db
input = test_file_db['2015HLTValidationData_L0filtered_0x0050']
input.run(configurable=Moore())

Moore().CondDBtag = 'cond-20150828'
Moore().DDDBtag = 'dddb-20150724'
Moore().DataType = '2015'

from GaudiKernel.Configurable import applyConfigurableUsers
applyConfigurableUsers()

from Configurables import HltConf
conf = HltConf()
settings = HltConf().settings()

configuredStreams = settings.Streams()
knownStreams = conf.knownStreams()

from Configurables import HltRoutingBitsWriter
rb1 = HltRoutingBitsWriter('Hlt1RoutingBitsWriter')
rb2 = HltRoutingBitsWriter('Hlt2RoutingBitsWriter')

passed = True
for stream, rb in configuredStreams.iteritems():
    try:
        bit, default = knownStreams[stream]
        if bit <= 64:
            rbs = rb1.RoutingBits
        else:
            rbs = rb2.RoutingBits
        assert (bit in rbs), ("Routing bit %d was not set to the writer." % bit)
        assert (rbs[bit] == rb), ("Routing bit set for %d is not the one that was configured." % bit)
    except KeyError:
        print 'FATAL: stream %s is not knows in HltConf'
        passed = False
    except AssertionError as e:
        print e
        passed = False
        
disabled = {k : v for k, v in knownStreams.iteritems() if k not in configuredStreams}
for stream, (bit, default) in disabled.iteritems():
    try:
        if bit <= 64:
            assert (bit not in rb1.RoutingBits), ("HLT1 routing bit %d should have been removed." % bit)
        else:
            assert (bit not in rb2.RoutingBits), ("HLT2 routing bit %d should have been removed." % bit)
    except AssertionError as e:
        print e
        passed = False
    
if passed:
    print 'PASSED'
else:
    print 'FAILED'
