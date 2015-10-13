#!/usr/bin/env python

# Check out the configuration.
from optparse import OptionParser
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

print 'Run', options.runNumber, 'with analysis:', options.analysisType

# Get the list of input files.
import commands
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

from Gaudi.Configuration import *

from Configurables import DDDBConf
DDDBConf(InitialTime = 'now')
from Configurables import LHCbApp
app = LHCbApp()
app.DataType = "2015"
app.CondDBtag = "cond-20150828"

EventSelector().PrintFreq = 100000
EventSelector().Input = listOfFiles

# Set up the sequence of algorithms to be run.
mainSeq = GaudiSequencer("MainSeq")

from Configurables import DecodeRawEvent
DecodeRawEvent()

#from Configurables import createODIN
#mainSeq.Members += [createODIN()]

from Configurables import HltRoutingBitsFilter
physFilter = HltRoutingBitsFilter("PhysFilter")
physFilter.RequireMask = [0x0, 0x4, 0x0]
mainSeq.Members += [physFilter]

from Configurables import HCRawBankDecoder
decoder = HCRawBankDecoder()
from Configurables import GaudiSequencer
mainSeq.Members += [decoder]

# Common config for all algorithms deriving from HCMonitorBase
def configureMapping(algorithm):
  algorithm.CrateB = 0
  algorithm.CrateF = 1
  algorithm.ChannelsB0 = [47, 46, 45, 44]
  algorithm.ChannelsB1 = [23, 22, 21, 20]
  algorithm.ChannelsB2 = [11, 10, 9, 8]
  # Default: no masked channels
  #algorithm.MasksB0 = [1,1,1,1]
  #algorithm.MasksB1 = [1,1,1,1]
  #algorithm.MasksB2 = [1,1,1,1]
  #algorithm.MasksF1 = [1,1,1,1]
  #algorithm.MasksF2 = [1,1,1,1]
  algorithm.SpareChannelsB0 = [12,12,12,12]
  algorithm.SpareChannelsB1 = [51,51,51,51]
  algorithm.SpareChannelsB2 = [43,43,43,43]
  if options.runNumber > 163708: 
    ## When F13 was put to the other side
    algorithm.ChannelsF1 = [23, 22, 21, 46]
    algorithm.ChannelsF2 = [11, 10, 9, 8]
    algorithm.SpareChannelsF1 = [ 51,51,51,45]
    algorithm.SpareChannelsF2 = [43,43,43,43]
  elif options.runNumber < 163707 and options.runNumber > 161696: 
    ## `After TS2 mapping same than B side
    algorithm.ChannelsF1 = [23, 22, 21, 20]
    algorithm.ChannelsF2 = [11, 10, 9, 8]
    algorithm.SpareChannelsF1 = [51,51,51,51]
    algorithm.SpareChannelsF2 = [43,43,43,43]
  elif options.runNumber < 161697: 
    ## Between adaptor board installation and TS2
    algorithm.ChannelsF1 = [23, 22, 21, 20]
    algorithm.ChannelsF2 = [47, 46, 45, 44]
    algorithm.SpareChannelsF1 = [ 51,51,51,51]
    algorithm.SpareChannelsF2 = [43,43,43,43 ]

if int(options.refRunNumber) > 0:
  from Configurables import HCDigitCorrector
  digitCorrector = HCDigitCorrector()
  configureMapping(digitCorrector)
  # Load the correction from the xml of the reference file
  quads = ['B00','B01','B02','B03','B10','B11','B12','B13','B20','B21','B22','B23','F10','F11','F12','F13','F20','F21','F22','F23']
  import xml.etree.cElementTree as ET
  import xml.dom.minidom as mindom
  tree = ET.parse(options.OutputDirectory+'/'+options.refRunNumber+'/CommonMode_'+options.refRunNumber+".xml")
  commonmode = tree.getroot()
  Corrections = {'Theta':[],'x0':[],'y0':[]}
  for q in quads:
    for p in ['Odd','Even']:
      Corrections['Theta'].append( float(commonmode.find(q+p).find('theta').text))
      Corrections['x0'].append( float(commonmode.find(q+p).find('X0').text))
      Corrections['y0'].append( float(commonmode.find(q+p).find('Y0').text))
  digitCorrector.Thetas = Corrections['Theta']
  digitCorrector.X0 = Corrections['x0']
  digitCorrector.Y0 = Corrections['y0']
  mainSeq.Members += [digitCorrector]

if options.analysisType == 'NTuple':
  from Configurables import HCDigitTuple
  digitTuple = HCDigitTuple()
  if int(options.refRunNumber) > 0:
    digitTuple.CorrectedDigitLocation = "Raw/HC/CorrectedDigits"
  configureMapping(digitTuple)
  mainSeq.Members += [digitTuple]
elif options.analysisType == 'Pedestals':
  from Configurables import HCDigitMonitor
  digitMonitor = HCDigitMonitor()
  configureMapping(digitMonitor)
  digitMonitor.MinBX = options.minBx
  digitMonitor.MaxBX = options.maxBx
  mainSeq.Members += [digitMonitor]
  if int(options.refRunNumber) > 0:
    correctedDigitMonitor = HCDigitMonitor("HCCorrectedDigitMonitor")
    configureMapping(correctedDigitMonitor)
    correctedDigitMonitor.Edges = [-300.5,1200]
    correctedDigitMonitor.DigitLocation = "Raw/HC/CorrectedDigits"
    correctedDigitMonitor.MinBX = options.minBx
    correctedDigitMonitor.MaxBX = options.maxBx
    mainSeq.Members += [correctedDigitMonitor]
elif options.analysisType == 'CommonMode':
  from Configurables import HCPedestalCorrection
  pedestalCorrection = HCPedestalCorrection()
  configureMapping(pedestalCorrection)
  pedestalCorrection.MinBX = 500
  pedestalCorrection.MaxBX = options.maxBx
  mainSeq.Members += [pedestalCorrection]
elif options.analysisType == 'DelayScan':
  from Configurables import HCDelayScan
  delayScan = HCDelayScan()
  configureMapping(delayScan)
  delayScan.BxCentral = options.BxCentral
  delayScan.BxPrev = options.BxPrev
  delayScan.BxNext = options.BxNext
  delayScan.MinimumStepNr = 0
  delayScan.MaximumStepNr = 512
  delayScan.MinBX = options.minBx
  delayScan.MaxBX = options.maxBx
  mainSeq.Members += [delayScan]

appConf = ApplicationMgr(OutputLevel = INFO, AppName = options.analysisType)
appConf.TopAlg += [mainSeq]

'''
locations =  ["Raw/HC/Digits"]  
if  options.analysisType == 'TAE':
  for k in ['Prev','Next']:
    for i in range(options.TAE):
      location = k + repr(options.TAE - i)
      locations.append(location+"/Raw/HC/Digits")
mainAlgorithm.DigitLocation = locations
'''

from GaudiPython.Bindings import AppMgr
appMgr = AppMgr()
appMgr.HistogramPersistency = "ROOT"
hpSvc = appMgr.service('HistogramPersistencySvc')
hpSvc.OutputFile = options.OutputDirectory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+'.root'

ntSvc = appMgr.ntupleSvc()
ntSvc.Output = [ "FILE1 DATAFILE='"+options.OutputDirectory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+"Tuple.root'  TYP='ROOT'  OPT='NEW'" ]

appMgr.run(options.NumberOfEvents)

