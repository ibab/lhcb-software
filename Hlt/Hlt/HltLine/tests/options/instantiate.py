
# ============ Auto-generated testing python file ======
from Gaudi.Configuration import *
import Configurables
from GaudiKernel.ConfigurableDb import cfgDb, loadConfigurableDb

##### load all configurables of this package #####
loadConfigurableDb()
for name,conf in cfgDb.iteritems():
  if conf['package']=='HltLine':
    if hasattr(Configurables,name):
      try:
        aConf=getattr(Configurables,name)
        aConf()
      except:
        print 'ERROR, cannot import/instantiate', name


# =========== Auto-generated, import all python modules ====
import glob, os
modules=glob.glob('../../python/HltLine/*.py')
for mod in modules:
  try:
    amod=__import__('HltLine.'+mod.split(os.sep)[-1][:-3])
  except ImportError:
    print 'ERROR, cannot import', mod

