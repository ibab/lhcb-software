TimeEvent = 1
NetEvent  = 2
UpiEvent  = 3
IocEvent  = 4
QioEvent  = 5
TkEvent   = 6
PVSSEvent = 7

CPP = None
gbl = None
std = None

Interactor  = None
Event       = None
BaseSensor  = None
IocSensor   = None
TimeSensor  = None
Sensor      = None
UpiSensor   = None


def have_cppyy():
  import sys, platform
  try:
    import cppyy as Dict
    if platform.system()=='Linux':
      Dict.loadDictionary('libOnlineKernelDict')
    else:
      Dict.loadDictionary('OnlineKernelDict.dll')

    CPP = Dict.makeNamespace('CPP')
    gbl = Dict.gbl
    std = gbl.std

    Interactor           = CPP.PyInteractor
    Event                = CPP.Event
    BaseSensor           = CPP.Sensor
    IocSensor            = CPP.IocSensor
    TimeSensor           = CPP.TimeSensor
    Sensor               = gbl.UpiSensor
    UpiSensor            = gbl.UpiSensor
    return True
  except Exception,X:
    print 'OnlineKernel: Failed to use cppyy - you need to live without. [%s]'%(str(X),)
    return None
