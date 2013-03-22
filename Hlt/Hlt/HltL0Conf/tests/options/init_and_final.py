
# ============ Auto-generated init/finalize options file ======
from Gaudi.Configuration import *
import Configurables
from Configurables import LHCbApp
LHCbApp()

ApplicationMgr().EvtSel='NONE'
ApplicationMgr().EvtMax=0

from GaudiKernel.ConfigurableDb import cfgDb, loadConfigurableDb
loadConfigurableDb()
for name,conf in cfgDb.iteritems():
  if conf['package']=='HltL0Conf':
    if hasattr(Configurables,name):
      aConf=getattr(Configurables,name)
      if issubclass(aConf, ConfigurableAlgorithm):
        ApplicationMgr().TopAlg+=[aConf()]

