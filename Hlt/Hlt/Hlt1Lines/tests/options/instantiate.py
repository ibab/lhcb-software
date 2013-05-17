
# ============ Auto-generated testing python file ======
from Gaudi.Configuration import *
import Configurables
from GaudiKernel.ConfigurableDb import cfgDb, loadConfigurableDb
import sys, traceback

##### load all configurables of this package #####
loadConfigurableDb()
for name,conf in cfgDb.iteritems():
  if conf['package']=='Hlt1Lines':
    if hasattr(Configurables,name):
      try:
        aConf=getattr(Configurables,name)
        aConf()
      except Exception, e:
        print >> sys.stderr, 'ERROR, cannot import/instantiate configurable', name, '\n-------\n', e.__class__, '\n-------'
        traceback.print_exc()


# =========== Auto-generated, import all python modules ====
import glob, os, sys, traceback
modules=glob.glob('../../python/Hlt1Lines/*.py')
for mod in modules:
  try:
    amod=__import__('Hlt1Lines.'+mod.split(os.sep)[-1][:-3])
  except Exception, e:
    print >> sys.stderr, 'ERROR, cannot import module', mod, '\n-------\n', e.__class__, '\n-------',
    traceback.print_exc()

