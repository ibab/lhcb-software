#!/usr/bin/env python
import sys 

#from array import array

# Parse the command line options.
from optparse import OptionParser
parser = OptionParser()
parser.add_option("-a", "--analysisType", type="string",
                  help="Type of analysis (DelayScan, Pedestals)",
                  dest="analysisType")

parser.add_option("-r", "--runNumber", type="string",
                  help="Run number to be analysed",
                  dest="runNumber")

parser.add_option("-n", "--NumberOfEvents", type="int",
                  help="Number of events to be analysed",
                  dest="NumberOfEvents" , default = -1)

parser.add_option( "--minBx", type="int",
                  help="Minimum bxID to consider",
                  dest="minBx" , default = 0)

parser.add_option( "--maxBx", type="int",
                  help="Maximum bxID to consider",
                  dest="maxBx" , default = 10000)

parser.add_option("-d", "--DataDirectory", type="string",
                  help="Top directory for data",
                  dest="DataDirectory", default = "/daqarea/lhcb/data/2015/RAW/FULL/HRC/TEST")

parser.add_option("-f", "--filename", type="string",
                  help="RAW file name",
                  dest="FileName", default = "")

parser.add_option("-o", "--OutputDirectory", type="string",
                  help="Ouptut Directory",
                  dest="OutputDirectory", default = "/home/herschel/AnalysedRuns")

parser.add_option("-t", "--TAE", type="int",
                  help="TAE mode, number of prev and next",
                  dest="TAE", default = -1 )

options, arguments = parser.parse_args()

# Get the input file names.
import os
import commands
listOfFiles = []
if options.FileName != "":
  listOfFiles.append(options.FileName)
for runNumber in  options.runNumber.split(','):
  dirname = options.DataDirectory+'/'+runNumber
  listOfAllFiles = []
  if "castor" in options.DataDirectory:
    status, output = commands.getstatusoutput('nsls ' + dirname)
    if status != 0:
      print output
      print "Cannot list content of directory ", dirname
      exit()
    listOfAllFiles = output.splitlines()
    dirname = "root://castorlhcb.cern.ch//" + dirname
  else:     
    listOfAllFiles = os.listdir(dirname)
  for f in listOfAllFiles:
    if f.count('.raw') == 1:
      listOfFiles.append("DATAFILE='PFN:mdf:" + dirname+ '/'+f + "' SVC='LHCb::MDFSelector'")

if len(options.runNumber.split(','))>1:
   options.runNumber = options.runNumber.split(',')[0]+'_'+options.runNumber.split(',')[-1]

from Gaudi.Configuration import *

from Configurables import LHCbApp
lhcbApp = LHCbApp()

EventSelector().PrintFreq = 100000
EventSelector().Input = listOfFiles

# Set up the raw bank decoder(s)
from DAQSys.DecoderClass import Decoder
DecoderDB = {}

if options.TAE > 0:
  for k in ['Prev', 'Next']:
    for i in range(options.TAE):
      location = k + repr(options.TAE - i)
      decoderName = "HCRawBankDecoder/HCRawBankDecoder" + location
      dec = Decoder(decoderName,
              active = True, banks = ["HC"],
              outputs = {"DigitLocation" : location + "/Raw/HC/Digits",
                         "L0DigitLocation" : location + "/Raw/HC/L0Digits"},
              inputs = {"RawEventLocations" : ['/Event/' + location + '/DAQ/RawEvent']},
              properties = {"DigitLocation" : location + "/Raw/HC/Digits",
                            "L0DigitLocation" : location + "/Raw/HC/L0Digits"},
              conf = DecoderDB)
      conf = dec.setup()

decoderName = "HCRawBankDecoder/HCRawBankDecoder"
dec = Decoder(decoderName,
              active = True, banks = ["HC"],
              outputs = {"DigitLocation" : "Raw/HC/Digits",
                         "L0DigitLocation" : "Raw/HC/L0Digits"},
              inputs = {"RawEventLocations" : ['/Event/DAQ/RawEvent']},
              properties = {"DigitLocation" :  "Raw/HC/Digits",
                            "L0DigitLocation" : "Raw/HC/L0Digits"},
              conf = DecoderDB)
conf = dec.setup()

from GaudiPython.Bindings import AppMgr
appMgr = AppMgr()

# Set the output histogram and tuple files. 
appMgr.HistogramPersistency = "ROOT"
hpSvc = appMgr.service('HistogramPersistencySvc')
hpSvc.OutputFile = 'scan_'+options.runNumber+'.root'

ntSvc = appMgr.ntupleSvc()
ntSvc.Output = ["FILE1 DATAFILE='" + options.OutputDirectory + '/' + options.runNumber + "/HC_Digits_run" + options.runNumber + ".root'  TYP='ROOT' OPT='NEW'"]

eventTimeDecoder = appMgr.toolsvc().create("OdinTimeDecoder", interface = "IEventTimeDecoder")

# Add the required algorithms to the sequence.
appMgr.addAlgorithm('LbAppInit')

from Configurables import HCRawBankDecoder
if options.TAE > 0:
  for k in ['Prev','Next']:
    for i in range(options.TAE):
      location = k + repr(options.TAE - i)
      decoderName = "HCRawBankDecoder/HCRawBankDecoder" + location
      appMgr.addAlgorithm(decoderName)

appMgr.addAlgorithm("HCRawBankDecoder/HCRawBankDecoder")

channelsB0 = [47, 46, 45, 44]
channelsB1 = [23, 22, 21, 20]
channelsB2 = [11, 10,  9,  8]
channelsF1 = [23, 22, 21, 46]
channelsF2 = [11, 10,  9,  8]
#channelsB0 = [47, 46, 45, 44, 43]
#channelsB1 = [23, 22, 21, 20, 51]
#channelsB2 = [11, 10,  9,  8, 12]
#channelsF1 = [23, 22, 21, 20, 51]
#channelsF2 = [11, 10,  9,  8, 12]
if options.analysisType == 'DelayScan':
  from Configurables import HCClockScan
  appMgr.addAlgorithm("HCClockScan")
  appMgr.algorithm('HCClockScan').CrateB = 0
  appMgr.algorithm('HCClockScan').CrateF = 1
  appMgr.algorithm('HCClockScan').ChannelsB0 = channelsB0
  appMgr.algorithm('HCClockScan').ChannelsB1 = channelsB1
  appMgr.algorithm('HCClockScan').ChannelsB2 = channelsB2
  appMgr.algorithm('HCClockScan').ChannelsF1 = channelsF1
  appMgr.algorithm('HCClockScan').ChannelsF2 = channelsF2
  appMgr.algorithm('HCClockScan').MinimumStepNr = 0
  appMgr.algorithm('HCClockScan').MaximumStepNr = 512
  appMgr.algorithm('HCClockScan').VFEClkPitch = 1
  appMgr.algorithm('HCClockScan').ADCClkPitch = 2
  #appMgr.algorithm('HCClockScan').Noise = True
  appMgr.algorithm('HCClockScan').SplitEvenOdd = True
  appMgr.algorithm('HCClockScan').DigitLocations = ["Raw/HC/Digits","Prev1/Raw/HC/Digits","Next1/Raw/HC/Digits"]
  appMgr.algorithm('HCClockScan').MinBx = options.minBx
  appMgr.algorithm('HCClockscan').MaxBx = options.maxBx
elif options.analysisType == 'Pedestals':
  from Configurables import HCDigitMonitor
  appMgr.addAlgorithm("HCDigitMonitor")
  appMgr.algorithm('HCDigitMonitor').CrateB = 0
  appMgr.algorithm('HCDigitMonitor').CrateF = 1
  appMgr.algorithm('HCDigitMonitor').ChannelsB0 = channelsB0
  appMgr.algorithm('HCDigitMonitor').ChannelsB1 = channelsB1
  appMgr.algorithm('HCDigitMonitor').ChannelsB2 = channelsB2
  appMgr.algorithm('HCDigitMonitor').ChannelsF1 = channelsF1
  appMgr.algorithm('HCDigitMonitor').ChannelsF2 = channelsF2
  appMgr.algorithm('HCDigitMonitor').MinBx = options.minBx
  appMgr.algorithm('HCDigitMonitor').MaxBx = options.maxBx
elif options.analysisType == 'PedestalCorrection':
  from Configurables import HCPedestalCorrection
  appMgr.addAlgorithm("HCPedestalCorrection")
  appMgr.algorithm('HCPedestalCorrection').CrateB = 0
  appMgr.algorithm('HCPedestalCorrection').CrateF = 1
  appMgr.algorithm('HCPedestalCorrection').ChannelsB0 = channelsB0
  appMgr.algorithm('HCPedestalCorrection').ChannelsB1 = channelsB1
  appMgr.algorithm('HCPedestalCorrection').ChannelsB2 = channelsB2
  appMgr.algorithm('HCPedestalCorrection').ChannelsF1 = channelsF1
  appMgr.algorithm('HCPedestalCorrection').ChannelsF2 = channelsF2

# Go!
appMgr.run(options.NumberOfEvents)

