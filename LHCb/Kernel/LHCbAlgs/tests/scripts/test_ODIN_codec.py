#!/usr/bin/env python

import sys, time

rawEventLoc = "DAQ/RawEvent"
srcOdinLoc = "source/ODIN"
dstOdinLoc = "destination/ODIN"

## Configuation function.
def configure():
    import Gaudi.Configuration
    from Configurables import ODINDecodeTool, ODINEncodeTool
    
    encode = ODINEncodeTool()
    encode.ODINLocation = srcOdinLoc
    encode.RawEventLocation = rawEventLoc
    decode = ODINDecodeTool()
    decode.ODINLocation = dstOdinLoc
    decode.RawEventLocations = [rawEventLoc]
    

## Main function
configure()

import GaudiPython
from GaudiPython import setOwnership

# prepare the application
app = GaudiPython.AppMgr()
obj = [ GaudiPython.gbl.DataObject() for i in range(4) ]
for o in obj:
    setOwnership(o, 0)

app.evtSvc().setRoot("/Event", obj[0])
app.evtSvc().registerObject("source", obj[1])
app.evtSvc().registerObject("destination", obj[2])
app.evtSvc().registerObject("DAQ", obj[3])

# Fill the ODIN object
odin = GaudiPython.gbl.LHCb.ODIN()

# Skip the version number
# odin.setVersion(5)

odin.setRunNumber(0x1973)
odin.setCalibrationStep(0xCAFE)
odin.setEventType(0xACDC)
odin.setOrbitNumber(23)
odin.setEventNumber(0x0000000200000001)
odin.setGpsTime(int(time.time() * 1e6)) # Now
odin.setErrorBits(0xAA)
odin.setDetectorStatus(0xBBBBBB)
odin.setBunchCurrent(0x11)
odin.setBunchCrossingType(odin.BeamCrossing)
odin.setForceBit(False)
# Obsolete: ODIN version < 5
# odin.setReadoutType(odin.NonZeroSuppressed) 
odin.setTriggerType(7)
odin.setBunchId(123)
odin.setTriggerConfigurationKey(0x01234567)
odin.setCalibrationType(odin.C)

setOwnership(odin, 0)
app.evtSvc().registerObject(srcOdinLoc, odin)

# The raw event must be created before calling the encoder
raw = GaudiPython.gbl.LHCb.RawEvent()
setOwnership(raw, 0)
app.evtSvc().registerObject(rawEventLoc, raw)

encoder = app.toolsvc().create("ODINEncodeTool", interface = "IGenericTool")
decoder = app.toolsvc().create("ODINDecodeTool", interface = "IGenericTool")

# Encode
encoder.execute()
# Decode
decoder.execute()

new_odin = app.evtSvc()[dstOdinLoc]

error = False
print ""
print ": ==================================================="
print ": Decoded ODIN version %d" % new_odin.version()
print ": === Comparing original and decoded ODIN objects ==="
# Ignoring "version"
fields = ["runNumber",
          "eventType",
          "orbitNumber",
          "eventNumber",
          "gpsTime",
          "detectorStatus",
          "errorBits",
          "bunchId",
          "triggerType",
          # "readoutType", # obsolete: ODIN version < 5
          "forceBit",
          "bunchCrossingType",
          "bunchCurrent",
          # "version", # not checked, usually overwritten
          "calibrationStep",
          "triggerConfigurationKey",
          "timeAlignmentEventWindow",
          "calibrationType"]

for f in fields:
    o = getattr(odin,f)()
    n = getattr(new_odin,f)()
    if o != n:
        error = True
        print ": Mismatch in %-30r: orig = 0x%X, new = 0x%X" % (f,o,n)
    else:
        print ": OK          %-30r: 0x%X" % (f,n)
print ": ==================================================="
print ""

if error:
    sys.exit(1)
 
