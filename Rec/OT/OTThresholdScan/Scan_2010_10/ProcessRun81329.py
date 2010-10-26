# =============================================================================
__author__  = "Daan van Eijk"
__date__    = "2010-07-29"
__version__ = "CVS Tag $Name: v14r3 $, version $Revision: 1.1 $ "
# =============================================================================
## import everything from bender 
from   Bender.Awesome      import *
from   LoKiCore.math       import * 
from   ROOT                import Double, GaudiAlg
from   PartProp.Nodes      import * 
from   LoKiPhys.decorators import *
#from   GaudiPython.GaudiAlgs import HistoAlgo
import math

# =============================================================================
class HitEfficiency( Algo ):
    """
    Algorithm to fill NTuple for hit efficiency
    """
    ## initialize the algorithm 
    def initialize ( self ) :
        """
        Initialize the algorithm 
        """

        ## initialize the base 
        sc = Algo.initialize ( self ) ## initialize the base
        if sc.isFailure() : return sc

        #Get the OT
        self._detOT = self.getDet('/dd/Structure/LHCb/AfterMagnetRegion/T/OT')

        #Create the linear extrapolator tool
        self._linearextrapolator = self.tool(cpp.ITrackExtrapolator,'TrackLinearExtrapolator')

        #Create the interpolator tool
        self._interpolator = self.tool(cpp.ITrackInterpolator,'TrackInterpolator')

        #Create the decoder tool
        self._decoder = self.tool(cpp.IOTRawBankDecoder,'OTRawBankDecoder')

        #Define thresholds, book histograms
        self._mydict = {}

        nlayers = 4
        stepoffset = 1
        nstepsperlayer = 8
        nthresholds = 33

        for i in range(0,nthresholds):
            string = 'threshold_'+str(i)
            #Using GaudiAlg.ID('subdir/histo') you can also book histo under subdir...
            self.bookProfile2D(GaudiAlg.ID(string),'Histograms',-3157,3157,74,-2800.,2800.,100)
        
        for i in range(0,nlayers):
            startvalue = nstepsperlayer*i+stepoffset
            for j in range(0,nstepsperlayer):
                self._mydict[startvalue+j]=i

        #calibthresholds:
        caliblist = []
        for l in caliblist:
            self._mydict[l] = 'calib'

        print 'This is the list with thresholds and layers!', self._mydict
        
        return SUCCESS
     
    def uniqueLayer( self,channelID ):
        return  (channelID.station()-1) * 4 + channelID.layer() 

    def uniqueQuarter( self,channelID ):
        return self.uniqueLayer(channelID) * 4 + channelID.quarter()

    def uniqueModule( self,channelID ):
        return self.uniqueQuarter(channelID)*9 + channelID.module() - 1

    def uniqueOtis( self,channelID ):
        return self.uniqueModule(channelID)*4 + (channelID.straw()-1)/32
            
    ## Main method
    def analyse ( self ) :
        """
        Main method
        
        """

        self._tuple  = self.nTuple( 'Efficiency')

        self._odin = self.get( 'DAQ/ODIN' )
        self._tracks = self.get( '/Event/Rec/Track/Best' )

        runNum = self._odin.runNumber()
        evtNum = self._odin.eventNumber()

        #clear the hitmap
        self._minOTHitsPerTrack = 10 
        self._maxChi2PerDoF = 2  
        self._maxDistError = 0.2
        
        NumUniqueStation = 3
        NumUniqueLayer =12
        NumUniqueModule = 432
        NumUniqueOtis = 432*4

        MaxChan = 128

        self._moduleHitMap = []
        
        for i in range(0,NumUniqueModule):
            listOfchannels = []
            for j in range(0,MaxChan):
                listOfchannels.append(0.0)
            
            self._moduleHitMap.append(listOfchannels)

        #decode all OT hits
        ottimes = LHCb.OTLiteTimeContainer() 
        sc = self._decoder.decode( ottimes )

        #fill all the hits in the hit map
        if( sc ):
            for ihit in ottimes:
                self._moduleHitMap[self.uniqueModule(ihit.channel())][ihit.channel().straw()-1] = ihit.channel()

            if self._tracks:
                for track in self._tracks:
                
                    if (( track.fitStatus() == track.Fitted ) and ( track.nDoF()>1 ) and (track.chi2PerDoF() <  self._maxChi2PerDoF) ):
                        nodes = track.nodes()
                        sumn = 0
                        for node in nodes:
                            if ( ( node.type() == node.HitOnTrack ) and ( node.measurement().type() == node.measurement().OT )):
                                sumn += 1                            
                        if sumn >= self._minOTHitsPerTrack:
                            self.fillEfficiency(track)

        return SUCCESS        

    def fillEfficiency(self, track):
           
        modulesOnTrack = {}
    
        nodes = track.nodes()
        for node in nodes:
            if ( ( node.type() == node.HitOnTrack ) and ( node.measurement().type() == node.measurement().OT )):
                meas = node.measurement()
                module = meas.module()

                mono = ( meas.channel().straw()-1 ) / ( module.nChannels()/2 )

                #print 'mono = ', mono
                #print 'T',module.stationID(),'L',module.layerID(),'Q',module.quarterID(),'M',module.moduleID()
                #print 'uniquemodule = ', module.uniqueModule()
            
                #FIXME: Find the node witht he smallest residual if you have 2 nodes in one monolayer!!!
                if module.uniqueModule() in modulesOnTrack:
                    if mono == 0:
                        modulesOnTrack[module.uniqueModule()][0] = module
                        modulesOnTrack[module.uniqueModule()][1][0] = node
                    elif mono == 1:
                        modulesOnTrack[module.uniqueModule()][0] = module
                        modulesOnTrack[module.uniqueModule()][1][1] = node
                else:
                    modulesOnTrack[module.uniqueModule()] = [0,[0,0]]
                    if mono == 0:
                        modulesOnTrack[module.uniqueModule()][0] = module
                        modulesOnTrack[module.uniqueModule()][1][0] = node
                    elif mono == 1:
                        modulesOnTrack[module.uniqueModule()][0] = module
                        modulesOnTrack[module.uniqueModule()][1][1] = node
                             
#FOR THE FIXME:      
##       // only take the closest node in a mono-layer. we can anyway only unbias 1.
##       int mono = (meas->channel().straw()-1) / (module->nChannels()/2) ;
##       const LHCb::Node*& nodeInModule = mono == 0 ? 
## 	modulesOnTrack[ module ].first :  modulesOnTrack[ module ].second ;
##       if( nodeInModule == 0 ||
## 	  std::abs(nodeInModule->residual()) >
## 	  std::abs((*inode)->residual()) ) nodeInModule = *inode ;
      
##       //modulesOnTrack[ module ].push_back( *inode ) ;
##     }


        #Process the efficiency for these modules. Make sure to unbias the state when necessary. keep track of the layers that we have seen, such that we can skip them below...

        layersOnTrack = []
        for mod in modulesOnTrack:
            layersOnTrack.append(self._detOT.findLayer(modulesOnTrack[mod][0].elementID()))
            nodeMonoA = modulesOnTrack[mod][1][0]
            nodeMonoB = modulesOnTrack[mod][1][1]
            self.fillEfficiency2(track, modulesOnTrack[mod][0], nodeMonoA, nodeMonoB)

        #Store the z-position of the layers and find the missing planes
        state = LHCb.State()
        modulesNotOnTrack = []

        for ilay in self._detOT.layers():
            zpos = ilay.geometry().toGlobal(Gaudi.XYZPoint(0.0,0.0,0.0)).z()
            if ilay not in layersOnTrack:
                self._interpolator.interpolate(track,zpos,state).ignore()
                if (math.sqrt(state.covariance()(0,0))< self._maxDistError):
                    tolerance =0.1
                    #to get a more precies answer, to propagate the hit to the xy plane (modules can be tilted out of this plane)
                    self._linearextrapolator.propagate(state,ilay.plane(),tolerance).ignore()
                    module = self._detOT.findModule(state.position())
                    if module:
                        modulesNotOnTrack.append(module)
                        self.fillEfficiency4(track, module, state)
        
    def fillEfficiency2( self, track, module, nodeMonoA, nodeMonoB):
        states = [0,0]
        strawpos = [Double(),Double()]
        yfrac = [Double(),Double()]

        if nodeMonoA:
            states[0] = nodeMonoA.unbiasedState()
        else:
            states[0] = nodeMonoB.state()

        if nodeMonoB:
            states[1] = nodeMonoB.unbiasedState()
        else:
            states[1] = nodeMonoA.state()

        for imono in range(0,2):
            module.monoLayerIntersection(imono,
                                         states[imono].position(),
                                         states[imono].tx(),
                                         states[imono].ty(),
                                         strawpos[imono],
                                         yfrac[imono]
                                         )

        refstate = states[1] #WHY? Because it's only used to calculate the slopes in fillefficiency3, and this change for monolayer A and B very little!
        
        self.fillEfficiency3(track, module, strawpos, yfrac, refstate)

    def fillEfficiency3(self, track, module, strawpos, yfrac, refstate):

        ##make a temporary structure that tells which hits are in this module.
        modid = module.elementID()

        pitch = module.xPitch()
        uniquemodule = self.uniqueModule(modid)

        nstraws = module.nChannels()

        #compute the direction in the local frame.
        localslopes = module.geometry().toLocal(refstate.slopes())
        localTx = localslopes.x()/localslopes.z()
        cosalpha = 1/math.sqrt( 1+localTx*localTx)
    
        #find the hits in a +- 3 straw window
        for imono in range(0,2):
            monooffset = imono * nstraws/2 -1
            minstraw = max(int(strawpos[imono])-2,1)
            maxstraw = min(int(strawpos[imono]) +3, int(nstraws/2))
        
            for istraw in range(minstraw,maxstraw+1):
                channel = self._moduleHitMap[uniquemodule][istraw+monooffset]
                foundhit = False
                foundhot = False
                if channel:
                    foundhit = True
                if (foundhit and track.isOnTrack(LHCb.LHCbID(channel))):
                    foundhot = True

                monocoord = strawpos[imono]-istraw
                dstraw = monocoord*pitch*cosalpha

                pvcontainer  = self.get('Rec/Vertex/Primary')
                
                step = self._odin.calibrationStep()
                layer = module.elementID().uniqueLayer()-4

                print 'step =', step
                print 'layer =', layer

                hitpattern = cpp.LHCb.HitPattern(track.lhcbIDs())
                #Example:
                #'Layer'+str(i)+'/threshold_'+str(startvalue+j)

                if abs(dstraw) < 1.25 and pvcontainer.size() == 1 and self._tracks.size() > 0 and self._tracks.size() < 1000 and hitpattern.numOTHits() > 10 and track.type() == 3:
                    if step in self._mydict:
                        if layer == self._mydict[step]:
                            self.profile2D(refstate.x(), refstate.y(),foundhit,GaudiAlg.ID('threshold_'+str(step)),'Histograms',-3157,3157,-2800,2800,74,100,1)
                        #elif self._mydict[step] == 'calib':
                            #    self.profile2D(refstate.x(), refstate.y(),foundhit,GaudiAlg.ID('threshold_'+str(step)),'Histograms',-3157,3157,-2800,2800,74,100,1)

        self.setFilterPassed ( True )

    

    def fillEfficiency4(self, track, module, state):
        strawpos = [Double(),Double()]
        yfrac = [Double(),Double()]

        for imono in range(0,2):
            module.monoLayerIntersection(imono,
                                         state.position(),
                                         state.tx(),
                                         state.ty(),
                                         strawpos[imono],
                                         yfrac[imono]
                                         )
        self.fillEfficiency3(track, module, strawpos, yfrac, state)
    
# =============================================================================
## configure the job
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-31
def configure ( datafiles , catalogs = [] ) :
    """
    Configure the job
    """
    
    ##
    ## Static Configuration: 
    ##

    ## DaVinci Configurable
    
    from Configurables import Brunel
    brunel = Brunel (
        DataType   = '2010'
        )

    from Configurables import RecMoniConf
    #RecMoniConf().setProp("MoniSequence",["OT"])
    RecMoniConf().setProp("MoniSequence",[])


    importOptions('$APPCONFIGOPTS/DisableLFC.py')

    #Need this to run locally (for testing purposes)
    importOptions('$APPCONFIGOPTS/UseOracle.py')

    from Configurables import HistogramPersistencySvc
    HistogramPersistencySvc ( OutputFile = 'histos.root' ) 
    
    from Configurables import NTupleSvc
    NTupleSvc().Output += [ "ACCEPT DATAFILE='tuples.root' TYPE='ROOT' OPT='NEW'"]
    NTupleSvc().OutputLevel = 1
    
    ## define input data 
    setData  ( datafiles )
    
    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()
    
    ## create local algorithm:
    hitefficiency = HitEfficiency ('HitEfficiency',
                                  ## N-tuple LUN 
                                   NTupleLUN  = "ACCEPT" )
    
    gaudi.addAlgorithm( hitefficiency )

    return SUCCESS 

# =============================================================================
## job steering 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-31
if __name__ == '__main__' :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120  
    
    ## configure the job:
    #files = [
    #  "DATAFILE='castor:/castor/cern.ch/grid/lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/72908/072908_0000000002.raw' SVC='LHCb::MDFSelector'"
    #  ]

    #Run 77555 is AUG 2010 threshold scan
    #files = [
    #    "DATAFILE='castor:/castor/cern.ch/grid/lhcb/data/2010/RAW/FULL/LHCb/TEST/77555/077555_0000000024.raw' SVC='LHCb::MDFSelector'"
    #    ]

    #Run 81329 is part of OKT 2010 threshold scan
    files = [
        "DATAFILE='castor:/castor/cern.ch/grid/lhcb/data/2010/RAW/FULL/LHCb/TEST/81329/081329_0000000001.raw' SVC='LHCb::MDFSelector'"
        ]
    
    configure( files )
    
    ## run the job
    run( 100 )

# =============================================================================
# The END 
# =============================================================================
