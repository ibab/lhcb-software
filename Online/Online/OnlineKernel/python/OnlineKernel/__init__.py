TimeEvent = 1
NetEvent  = 2
UpiEvent  = 3
IocEvent  = 4
QioEvent  = 5
TkEvent   = 6
PVSSEvent = 7

import sys, platform, PyCintex as Dict

if platform.system()=='Linux':
  Dict.loadDictionary('libOnlineKernelDict.so')
else:
  Dict.loadDictionary('OnlineKernelDict.dll')

CPP = Dict.makeNamespace('CPP')
gbl = Dict.makeNamespace('')

Interactor           = CPP.PyInteractor
Event                = gbl.Event
BaseSensor           = gbl.Sensor
Sensor               = gbl.UpiSensor
IocSensor            = gbl.IocSensor
TimeSensor           = gbl.TimeSensor


