"""
TWIki: https://twiki.cern.ch/twiki/bin/view/LHCb/L0App
Runs the L0 trigger - this is expects DAQ/RawEvent to exist and uses ths simulation TCK
Testing this will require running the 
"""
from Gaudi.Configuration import *
#what's up with this configurations?
from Configurables import L0App
L0App().TCK = '0x0045'
L0App().ReplaceL0Banks = True
L0App().outputFile ="L0Processed.dst"
L0App().EvtMax = -1
L0App().DataType = '2012'
L0App().DDDBtag   = 'dddb-20130929-1'  ## latest strip20 tags as of Jan2013
L0App().CondDBtag = 'sim-20131023-vc-md100'
L0App().Simulation = True
from Configurables import EventSelector
EventSelector().PrintFreq = 100


