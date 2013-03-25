
# ============ Auto-generated testing python file ======
from Gaudi.Configuration import *
import Configurables
from GaudiKernel.ConfigurableDb import cfgDb, loadConfigurableDb
import sys, traceback

##### load all configurables of this package #####
loadConfigurableDb()
for name,conf in cfgDb.iteritems():
  if conf['package']=='HltDisplVertices':
    if hasattr(Configurables,name):
      try:
        aConf=getattr(Configurables,name)
        aConf()
      except Exception, e:
        print >> sys.stderr, 'ERROR, cannot import/instantiate configurable', name, '\n-------\n', e.__class__, '\n-------'
        traceback.print_exc()

