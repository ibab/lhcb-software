from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer,RawEventJuggler
from Configurables import DaVinci
from Configurables import DecodeRawEvent

from Configurables import DstConf
DstConf().Turbo=True

DaVinci()
DaVinci().EvtMax=-1
DaVinci().DataType="2015"

from GaudiConf import IOHelper
IOHelper().inputFiles( ["tesla_2015_TCK.dst"] , clear=True ) 

import GaudiPython
from Gaudi.Configuration import ApplicationMgr
from Configurables import LoKiSvc

from GaudiKernel import ROOT6WorkAroundEnabled
if ROOT6WorkAroundEnabled('ROOT-7492'):
    # trigger autoloading of LHCbKernel dictionary
    GaudiPython.gbl.LHCb.LHCbID
    # trigger autoloading of DigiEvent dictionary
    GaudiPython.gbl.LHCb.CaloDigit

gaudi = GaudiPython.AppMgr()
gaudi.initialize()

TES = gaudi.evtsvc()

tistostool= gaudi.toolsvc().create('TriggerTisTos',interface='ITriggerTisTos')
triggerTisTosTool2 = gaudi.toolsvc().create('Hlt2TriggerTisTos', interface='ITriggerTisTos')

hltline = "Hlt2DiMuonJPsiTurbo"

nTOS = 0
nDEC = 0
while True:
    gaudi.run(1)
    if not TES['DAQ/RawEvent']: break # probably end of file
    #parts = TES["Turbo/Hlt2CharmHadDstp2D0Pip_D02PimPipTurbo/Particles"]
    parts = TES["Turbo/"+hltline+"/Particles"]
    if hasattr(parts,"__iter__"):
        for p in parts:
            if p.particleID().pid()==443:
                #HLT2TISTOS = triggerTisTosTool2.triggerSelectionNames(p,hltline+"Decision",tistostool.kAnything,tistostool.kAnything,tistostool.kAnything,tistostool.kAnything)
                #tos = triggerTisTosTool2.triggerTisTos().tos()
                triggerTisTosTool2.setTriggerInput(hltline+"Decision")
                tistostool.setOfflineInput(p)
                triggerTisTosTool2.setOfflineInput(tistostool.offlineLHCbIDs())
                tos = triggerTisTosTool2.tisTosTobTrigger().tos()
                dec = triggerTisTosTool2.triggerTisTos().decision()
                if dec:
                    nDEC += 1
                if tos:
                    nTOS += 1
print "# decisions = "+str(nDEC)
print "# TOS = "+str(nTOS)
