from StrippingConf.Configuration import StrippingConf, StrippingStream
from StrippingSettings.Utils import strippingConfiguration
from StrippingSelections.Utils import buildStreams, cloneLinesFromStream

config = strippingConfiguration('stripping13')
streams = buildStreams(config)
                                             
# Now do some cloning to deal with the full/microDST streams
calibrationFullDSTname = 'Calibration' # we don't clone this
leptonicMicroDSTname = 'Leptonic'            
charmMicroDSTname = 'Charm'
charmCopyDSTname  = 'CharmCompleteEvent'
for stream in streams:
        if stream.name() == charmCopyDSTname: # Need to do this since CharmFull already exists
                charmCopy = stream

for stream in streams:     
        if stream.name() == charmMicroDSTname:
                charmCopy.appendLines( cloneLinesFromStream( stream, 'CompleteEvent', prescale = 0.02 ) )

lines = []
for stream in streams:
	print 'Stream', stream.name()
	for line in stream.lines:
		print '\t', line.name(), '\t', line.outputLocation()
		lines.append(lines)

print "Total number of lines:", len(lines)
