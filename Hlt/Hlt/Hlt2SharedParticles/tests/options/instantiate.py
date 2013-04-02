
# ============ Auto-generated testing python file ======
from Gaudi.Configuration import *
import Configurables
from GaudiKernel.ConfigurableDb import cfgDb, loadConfigurableDb
import sys, traceback

##### Setup Configurable for tracking... #####
from Configurables import Hlt2Tracking, GaudiSequencer

from HltLine.HltLine import Hlt1Line
import HltTracking.Hlt2TrackingConfigurations

for att in dir(HltTracking.Hlt2TrackingConfigurations):
  if "Hlt2BiKalmanFitted" in att:
    iatt=getattr(HltTracking.Hlt2TrackingConfigurations,att)()
    for s in iatt.__slots__:
      if "Seq" in s:
        al=Hlt1Line(s+"dummy")
        al._outputsel="DummySelectionName"
        setattr(iatt,s,al)

#
#for s in Hlt2Tracking.__slots__:
#  if "Seq" in s:
#    setattr(Hlt2Tracking(),s,GaudiSequencer(s+"dummy"))



##### load all configurables of this package #####
loadConfigurableDb()
for name,conf in cfgDb.iteritems():
  if conf['package']=='Hlt2SharedParticles':
    if hasattr(Configurables,name):
      try:
        aConf=getattr(Configurables,name)
        aConf()
      except Exception, e:
        print >> sys.stderr, 'ERROR, cannot import/instantiate configurable', name, '\n-------\n', e.__class__, '\n-------'
        traceback.print_exc()


importOptions("$STDOPTS/DecodeRawEvent.py")

# =========== Auto-generated, import all python modules ====
import glob, os, sys, traceback
modules=glob.glob('../../python/Hlt2SharedParticles/*.py')
for mod in modules:
  try:
    amod=__import__('Hlt2SharedParticles.'+mod.split(os.sep)[-1][:-3])
  except Exception, e:
    print >> sys.stderr, 'ERROR, cannot import module', mod, '\n-------\n', e.__class__, '\n-------'
    traceback.print_exc()

