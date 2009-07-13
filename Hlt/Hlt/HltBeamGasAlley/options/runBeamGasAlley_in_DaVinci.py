
from   Gaudi.Configuration         import  *
#########################################################################################
#############################  Import Option Files  #####################################
#########################################################################################
importOptions('$STDOPTS/LHCbApplication.opts')
importOptions('$STDOPTS/DC06Conditions.opts')
importOptions('$STDOPTS/DecodeRawEvent.py')
importOptions('$STDOPTS/RootHist.opts')
#importOptions('$L0DUOPTS/L0Sequence.opts')
#importOptions('$L0DUROOT/options/L0OnDemand.opts')
#importOptions('$L0DUROOT/options/ReplaceL0BanksWithEmulated.opts')

#########################################################################################
#########################  Additional Job Configuration  ################################
#########################################################################################
appConf = ApplicationMgr( OutputLevel = INFO, AppName = 'GaudiPython__BGTrig' )
appConf.ExtSvc += ['DataOnDemandSvc']
DataOnDemandSvc().Algorithms += ["DATA='/Event/MC/Particles' TYPE='UnpackMCParticle'"]
DataOnDemandSvc().Algorithms += ["DATA='/Event/MC/Vertices' TYPE='UnpackMCVertex'"]

appConf.HistogramPersistency       = "ROOT"
HistogramPersistencySvc.OutputFile = "testOutput.root"

#########################################################################################
from Configurables import DecodeVeloRawBuffer,   Tf__PatVeloRTracking
from Configurables import BeamGasTrigClusterCut, BeamGasTrigExtractClusters, BeamGasTrigVertexCut

rzTracking1 = Tf__PatVeloRTracking('RZTracking1')
rzTracking1.OutputTracksName = "Hlt/Track/RZVelo"
# What happens with the Number of unused clusters
# if we don't set this ???

mySeqRZTr = GaudiSequencer('mySeqRZTr')
mySeqRZTr.Members  = [ DecodeVeloRawBuffer() ]
mySeqRZTr.Members += [ rzTracking1 ]

clustCut = BeamGasTrigClusterCut()
clustCut.SensorsBegin = 22
clustCut.SensorsEnd   = 39
clustCut.FracUnusedClustersCut = 0.27
print clustCut

extrCl = BeamGasTrigExtractClusters()
extrCl.OutputLocation = "Raw/Velo/UnusedRLiteClusters"
print extrCl

rzTracking2 = Tf__PatVeloRTracking('RZTracking2')
#rzTracking2.HitManagerName = "SecondDefaultVeloRHitManager"
#ToolSvc.SecondDefaultVeloRHitManager.LiteClusterLocation = extrCl.OutputLocation
rzTracking2.OutputTracksName = "Hlt/Track/RZVeloBeamGas"
rzTracking2.ZVertexMin = -1500.
rzTracking2.ZVertexMax = 1500.
print rzTracking2


vtxCut = BeamGasTrigVertexCut()
vtxCut.RZTracksInputLocation = rzTracking2.OutputTracksName
vtxCut.MaxBinValueCut        = 5 #Triggering cut
vtxCut.HistoBinWidth         = 10
vtxCut.ZExclusionRangeLow    = -265.
vtxCut.ZExclusionRangeUp     = 265.
vtxCut.OutputLevel = DEBUG
#HltAlgorithm related
vtxCut.MinCandidates    = 1 #Should be > 0 and <= MaxBinValueCut+1 (by default = 1)
print vtxCut

mySeqBGTrig = GaudiSequencer('mySeqBGTrig')
mySeqBGTrig.Members  = [ clustCut    ]
#mySeqBGTrig.Members += [ extrCl ] # What will change if we do this; change LiteClusterLocation too !!!
mySeqBGTrig.Members += [ rzTracking2 ]
mySeqBGTrig.Members += [ vtxCut      ]


#########################################################################################
from Configurables import DaVinci
DV = DaVinci()
DV.DataType  = 'DC06'
#DaVinci().DDDBtag   = 'head-20090112'
#DaVinci().CondDBtag = 'sim-20090112'
DV.UserAlgorithms = [ mySeqRZTr, mySeqBGTrig ]
DV.InputType = 'DST'
print DV

#########################################################################################
####################  GaudiPython; Other Imports; Type Shortcuts  #######################
#########################################################################################
import GaudiPython
appMgr      = GaudiPython.AppMgr() 
#appMgr.DLLs = [ 'GaudiAlg', 'RootHistCnv' ]

from   GaudiPython                 import  PyAlgorithm
from   ROOT                        import  TH1F, TH1D, TCanvas, gStyle
from   LinkerInstances.eventassoc  import  *
import math  

SUCCESS        = GaudiPython.SUCCESS
LHCb           = GaudiPython.gbl.LHCb

#########################################################################################
#######################    DEFINE THE ANALYSIS ALGORITHM    #############################
#########################################################################################

evCounter = 0
nL0Passed = 0

appMgr.algorithm('RedoMCLinks').Enable             = False
appMgr.algorithm('TestProtoPRecalibration').Enable = False

class myAnalysisAlgo(PyAlgorithm) :
  ## standard constructor 
  def __init__ ( self , name = 'PythonClassAlgorithm' ) :
    """ Constructor """
    PyAlgorithm.__init__( self , name )

  ## the main execution method 
  def execute( self ) :
    """ The major method 'execute', it is invoked for each event """

    globals()['evCounter'] += 1    
    l0=evt['Trig/L0/L0DUReport']
    if l0.decision(): globals()['nL0Passed'] += 1
    
    if not globals()['evCounter']%10:
      print "Processing Event", globals()['evCounter']
    
    #RZTracks = evt['Rec/Track/RZVeloBeamGas']
    #RZTracks = evt['Rec/Track/Best']
    #                Hlt
        
    return SUCCESS	
		
  
########################################################################################
#########################    THE ACTUAL JOB EXECUTION    ###############################
########################################################################################

  
appMgr.addAlgorithm( myAnalysisAlgo() )

evt  = appMgr.evtsvc()  
tsvc = appMgr.toolsvc()
det  = appMgr.detsvc()
sel  = appMgr.evtsel()

#sel.open('PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001888/DST/0000/00001888_00000011_5.dst')
#sel.open('PFN:castor:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003400/0000/00003400_00000001_5.dst')
#sel.open('/castor/cern.ch/user/p/phopchev/BeamGas/extractedEvents/nastyBGr-3M-maxBinMoreThan_4-fsc0-cc0.dst')
sel.open('PFN:$MYAFS/../../vol4/massi/BEAMGAS_MINBIAS/DIGI/pseudol0/beamgas.digi')
#sel.open('PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000007_1.dst')

nEvents = 90

appMgr.run(nEvents)

gStyle.SetOptStat(111111)

print "\nNumber of run events = ", evCounter
print "Number of L0 Passed events = ", nL0Passed

appMgr.stop()
appMgr.finalize()

#######################################################################
#########################    THE END    ###############################
#######################################################################



