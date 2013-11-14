
# Job options to configure the data on demand service for decoding the RawEvent
print "# WARNING: You have imported DecodeRawEvent.py, this is now replaced with a configurable. See savannah task #19106."

from Configurables import DecodeRawEvent
DecodeRawEvent().DataOnDemand=True

importOptions( "$L0TCK/L0DUConfig.opts" )
