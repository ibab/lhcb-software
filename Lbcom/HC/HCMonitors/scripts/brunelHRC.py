#!/usr/bin/env python
import sys 
from ROOT import *
from Gaudi.Configuration import *
from Configurables import LHCbApp
from array import array
import commands
from optparse import OptionParser
import xml.etree.cElementTree as ET
import xml.dom.minidom as mindom
lhcbApp = LHCbApp()


print 'enter brunel'
# Checking out the configuration

parser = OptionParser()
parser.add_option("-a", "--analysisType", type="string",
                  help="type of analyses (NTuple, Pedestals, CommonMode)",
                  dest="analysisType")

parser.add_option("-r", "--runNumber", type="string",
                  help="Run number to be analysed",
                  dest="runNumber", default = -1)

parser.add_option("", "--refRunNumber", type="string",
                  help="reference run number for common mode suppression",
                  dest="refRunNumber",default = "-1")

parser.add_option("-n", "--NumberOfEvents", type="int",
                  help="Number of events to be analysed",
                  dest="NumberOfEvents" , default = -1)

parser.add_option( "--minBx", type="int",
                  help="minimum bxID to consider",
                  dest="minBx" , default = 0)

parser.add_option( "--maxBx", type="int",
                  help="maximum bxID to consider",
                  dest="maxBx" , default = 10000)

parser.add_option( "--BxCentral", type="int",
                  help="",
                  dest="BxCentral" , default = 10000)
parser.add_option( "--BxNext", type="int",
                  help="",
                  dest="BxNext" , default = 10000)
parser.add_option( "--BxPrev", type="int",
                  help="",
                  dest="BxPrev" , default = 10000)

parser.add_option("-d", "--DataDirectory", type="string",
                  help="Top directory for data",
                  dest="DataDirectory", default = "/daqarea/lhcb/data/2015/RAW/FULL/HRC/TEST")
### /castor/cern.ch/grid/lhcb/data/2015/RAW/FULL/HRC/TEST

parser.add_option("-f", "--filename", type="string",
                  help="RAW file name",
                  dest="FileName", default = "")

parser.add_option("-o", "--OutputDirectory", type="string",
                  help="Ouptut Directory",
                  dest="OutputDirectory", default = "/home/herschel/AnalysedRuns")

parser.add_option("-t", "--TAE", type="int",
                  help="TAE mode, number of prev and next",
                  dest="TAE", default = -1 )

parser.add_option("-s", "--stepList", type="string",
                  help="list of step to consider",
                  dest="stepList", default = "" )


options, arguments = parser.parse_args()

print 'Run Brunel on',options.runNumber,'with analysis:',options.analysisType

listOfFiles = []


for runNumber in  options.runNumber.split(','):
  dirname =options.DataDirectory+'/'+runNumber
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

EventSelector().PrintFreq = 100000
EventSelector().Input = listOfFiles

from DAQSys.DecoderClass import Decoder
DecoderDB = {}

if options.TAE > 0:
  for k in ['Prev','Next']:
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
appMgr.HistogramPersistency = "ROOT"
hpSvc = appMgr.service('HistogramPersistencySvc')
hpSvc.OutputFile =options.OutputDirectory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+'.root'

ntSvc = appMgr.ntupleSvc()
ntSvc.Output     = [ "FILE1 DATAFILE='"+options.OutputDirectory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+"Tuple.root'  TYP='ROOT'  OPT='NEW'" ]

eventTimeDecoder = appMgr.toolsvc().create("OdinTimeDecoder", interface = "IEventTimeDecoder")

appMgr.addAlgorithm('LbAppInit')

from Configurables import HCRawBankDecoder

if options.TAE > 0:
  for k in ['Prev','Next']:
    for i in range(options.TAE):
      location = k + repr(options.TAE - i)
      decoderName = "HCRawBankDecoder/HCRawBankDecoder" + location
      appMgr.addAlgorithm(decoderName)

appMgr.addAlgorithm("HCRawBankDecoder/HCRawBankDecoder")

mainAlgorithm = None

if options.analysisType == 'NTuple':
  from Configurables import HCDigitTuple
  appMgr.addAlgorithm("HCDigitTuple")
  mainAlgorithm = appMgr.algorithm('HCDigitTuple')
elif  options.analysisType == 'Pedestals':
  from Configurables import HCDigitMonitor
  appMgr.addAlgorithm("HCDigitMonitor")
  mainAlgorithm = appMgr.algorithm('HCDigitMonitor')
  mainAlgorithm.MinBx = options.minBx
  mainAlgorithm.MaxBx = options.maxBx
elif  options.analysisType == 'CommonMode':
  from Configurables import HCPedestalCorrection
  appMgr.addAlgorithm("HCPedestalCorrection")
  mainAlgorithm = appMgr.algorithm('HCPedestalCorrection')
  mainAlgorithm.MinBx = 500
  mainAlgorithm.MaxBx = options.maxBx
elif options.analysisType == 'DelayScan':
  from Configurables import HCDelayScan
  appMgr.addAlgorithm("HCDelayScan")
  mainAlgorithm = appMgr.algorithm('HCDelayScan')
  mainAlgorithm.BxCentral = options.BxCentral
  mainAlgorithm.BxPrev = options.BxPrev
  mainAlgorithm.BxNext = options.BxNext
  mainAlgorithm.MinimumStepNr = 0
  mainAlgorithm.MaximumStepNr = 512
  mainAlgorithm.VFEClkPitch = 2
  mainAlgorithm.ADCClkPitch = 2
  mainAlgorithm.BxRange = [options.minBx,options.maxBx]


### Common config for all algorithm deriving from HCMonitorBase
mainAlgorithm.CrateB = 0
mainAlgorithm.CrateF = 1
mainAlgorithm.ChannelsB0 = [47, 46, 45, 44]
mainAlgorithm.ChannelsB1 = [23, 22, 21, 20]
mainAlgorithm.ChannelsB2 = [11, 10, 9, 8]
mainAlgorithm.MasksB0 = [1,1,1,1]
mainAlgorithm.MasksB1 = [1,1,1,1]
mainAlgorithm.MasksB2 = [1,1,1,1]
mainAlgorithm.MasksF1 = [1,1,1,1]
mainAlgorithm.MasksF2 = [1,1,1,1]
mainAlgorithm.SpareChannelsB0 = [12,12,12,12]
mainAlgorithm.SpareChannelsB1 = [51,51,51,51]
mainAlgorithm.SpareChannelsB2 = [43,43,43,43]
if options.runNumber > 163708: 
  ## When F13 was put to the other side
  mainAlgorithm.ChannelsF1 = [23, 22, 21, 46]
  mainAlgorithm.ChannelsF2 = [11, 10, 9, 8]
  mainAlgorithm.SpareChannelsF1 = [ 51,51,51,45]
  mainAlgorithm.SpareChannelsF2 = [43,43,43,43]
elif  options.runNumber < 163707 and options.runNumber > 161696: 
  ## `After TS2 mapping same than B side
  mainAlgorithm.ChannelsF1 = [23, 22, 21, 20]
  mainAlgorithm.ChannelsF2 = [11, 10, 9, 8]
  mainAlgorithm.SpareChannelsF1 = [51,51,51,51]
  mainAlgorithm.SpareChannelsF2 = [43,43,43,43]
elif  options.runNumber < 161697: 
  ## Bwetwen adaptor board installation and TS2
  mainAlgorithm.ChannelsF1 = [23, 22, 21, 20]
  mainAlgorithm.ChannelsF2 = [47, 46, 45, 44]
  mainAlgorithm.SpareChannelsF1 = [ 51,51,51,51]
  mainAlgorithm.SpareChannelsF2 = [43,43,43,43 ]

### If corrected adc are required load the correction from the xml of the reference file
if int(options.refRunNumber)>0:
  quads = ['B00','B01','B02','B03','B10','B11','B12','B13','B20','B21','B22','B23','F10','F11','F12','F13','F20','F21','F22','F23']
  mainAlgorithm.ChannelCalib = quads
  tree = ET.parse(options.OutputDirectory+'/'+options.refRunNumber+'/CommonMode_'+options.refRunNumber+".xml")
  commonmode = tree.getroot()
  Corrections = {'Theta':[],'x0':[],'y0':[]}
  for q in quads:
    for p in ['Odd','Even']:
      Corrections['Theta'].append( float(commonmode.find(q+p).find('theta').text))
      Corrections['x0'].append( float(commonmode.find(q+p).find('X0').text))
      Corrections['y0'].append( float(commonmode.find(q+p).find('Y0').text))
  mainAlgorithm.Thetas = Corrections['Theta']
  mainAlgorithm.X0 = Corrections['x0']
  mainAlgorithm.Y0 = Corrections['y0']

'''
locations =  ["Raw/HC/Digits"]  
if  options.analysisType == 'TAE':
  for k in ['Prev','Next']:
    for i in range(options.TAE):
      location = k + repr(options.TAE - i)
      locations.append(location+"/Raw/HC/Digits")
mainAlgorithm.DigitLocation = locations
'''
appMgr.run(options.NumberOfEvents)

