#SetupProject Panoramix --runtime-project LHCb v34r2
#SetupProject Panoramix v21r5 --runtime-project LHCb v35r3


from LHCbConfig          import *
from Gaudi.Configuration import *

#########################################################################################
#############################  Import Option Files  #####################################
#########################################################################################
#importOptions('$STDOPTS/RootHist.opts')
#importOptions('$STDOPTS/DecodeRawEvent.py')

#########################################################################################
#########################  Additional Job Configuration  ################################
#########################################################################################
appConf = ApplicationMgr( OutputLevel = INFO, AppName = 'nTupleAlgoFT' )
appConf.ExtSvc += ['DataOnDemandSvc']
appConf.ExtSvc += ['ToolSvc']

lhcbApp.Simulation = True 
lhcbApp.DataType = '2011'
importOptions('./FT_DDDB.py')

#lhcbApp.DDDBtag = "MC11-20111102"
#lhcbApp.CondDBtag = "sim-20111111-vc-md100"

from Configurables import DeFTTestAlg
ta = DeFTTestAlg("alg","/Event/pSim/FT/Hits")
ta.OutputLevel = 1
appConf.TopAlg += [ ta ]

#from Configurables import DeFTTestTool
#myTestTool = DeFTTestTool("myTestTool")
#myTestTool.OutputLevel = 1
#print myTestTool

#########################################################################################
####################  GaudiPython; Other Imports; Type Shortcuts  #######################
#########################################################################################
import GaudiPython

appMgr = GaudiPython.AppMgr()
appMgr.initialize()

from   GaudiPython                 import  PyAlgorithm
from   GaudiPython.GaudiAlgs       import  TupleAlgo
from   ROOT                        import  TH1F, TCanvas, gStyle
import math  

SUCCESS        = GaudiPython.SUCCESS
vectorDoubles  = GaudiPython.gbl.vector('double')

#particleVector = GaudiPython.gbl.std.vector('const LHCb::Particle *')
LHCb           = GaudiPython.gbl.LHCb
XYZPoint       = GaudiPython.gbl.ROOT.Math.XYZPoint

#from ROOT import TH1F, TBrowser
#import LHCbMath.Types
#LHCbMath = LHCbMath.Types.Gaudi

evCounter = 0

#########################################################################################
#######################    DEFINE THE ANALYSIS ALGORITHM    #############################
#########################################################################################

class myPyAlgo(TupleAlgo) :
    """ Algorithm to Book & Fill N-Tuple """

    ## standard constructor 
    def __init__ ( self , name = 'PythonNTAlg' ) :
        """ Constructor """
        TupleAlgo.__init__( self , name )

    ## the main execution method 
    def execute( self ) :
        """ The major method 'execute', it is invoked for each event """

        globals()['evCounter'] += 1
        print "Event #", globals()['evCounter']
   
        return SUCCESS	

  
########################################################################################
#########################    THE ACTUAL JOB EXECUTION    ###############################
########################################################################################

appMgr.addAlgorithm( myPyAlgo() )


evt  = appMgr.evtsvc()  
sel  = appMgr.evtsel()
tsvc = appMgr.toolsvc()
det  = appMgr.detsvc()

nEvents = 1
appMgr.run(nEvents)

print "\nNumber of run events = ", evCounter

### Checks of the custom FT DDDB
detFT  = det['/dd/Structure/LHCb/AfterMagnetRegion/T/FT']
print "\nFT User Parameters:\n"
print detFT.params()

det.dir('/dd/Structure/LHCb/AfterMagnetRegion/T/FT')

print "\nFT classID = ", detFT.classID()

#### Test calculcateHits method:
layers = detFT.Layers()
l = layers[0]
p = XYZPoint(-32.1, 1000., 7721.)
#l.calculateHits(p,p)

#### Example usage of the FTDet test tool
#testDeFTTool = tsvc.create('DeFTTestTool', 'myTestTool', interface='IDeFTTestTool')
#print "Testing the DeFT Test Tool:"

#testDeFTTool.printDeFTInfo()
#a = testDeFTTool.getDeFT()
#testDeFTTool.FindGlobalPoint()


