#
# analyse the Lumi counters
#
import sys 
import math
from string import split

########### specific ############
import GaudiPython


########################################
class LumiAlg(GaudiPython.PyAlgorithm):
########################################
  def __init__ ( self , name ) :
    """ Constructor """
    GaudiPython.PyAlgorithm.__init__( self , name )
    self.name=name
    ## does not work: GaudiPython.declareProperty( "String", m_string = "hundred")
    self.PreScale=40

    print 'python algorithm',self.name,': __init__'

#----------------------------------
  def initialize(self):
    self.nevents=0
    print 'python algorithm',self.name,'initialises'

    # set up the naming in the TES etc. - would be nice to get from configurables
    self.histbase='/stat/'
    self.histbaselumi='/stat/'#HltLumi/'
    self.histbaselumisumsdir='HltLumiSums'
    self.histbaselumisums=self.histbaselumi+self.histbaselumisumsdir+'/'
    self.histbaselumiprevdir='HltLumiPrev'
    self.histbaselumiprev=self.histbaselumi+self.histbaselumiprevdir+'/'
    self.histbaselumitrenddir='HltLumiTrend'
    self.histbaselumitrend=self.histbaselumi+self.histbaselumitrenddir+'/'

    # set up the local pointers to the histos - would be nice to get from configurables
    self.hStore={}
    self.btype={}
    self.btype['BB']='HistoBeamCrossing'
    self.btype['EE']='HistoNoBeam'
    self.btype['BL']='HistoSingleBeamRight'
    self.btype['BR']='HistoSingleBeamLeft'

    self.analysisDefined=False
    self.allCounters=[]
    
    # the results
    self.rValue={}

    return True

#----------------------------------
  def restart(self):
    self.analysisDefined=False
    self.allCounters=[]
    return True
  
#----------------------------------
  def finalize(self):
    print 'python algorithm',self.name,'finishes after',self.nevents,'events'
    return True
        
#----------------------------------
  def execute(self):
    """ Main execution """
    prflag=(self.nevents<5)
    self.nevents+=1
    if prflag:
      print 'python algorithm',self.name,'executes event',self.nevents

    if not self.analysisDefined:
      # collect the counter types from the histos
      self.defineAnalysis()
      self.analysisDefined=True
      
    if prflag and False:
      self.testingHistos(prflag)
    
    ## store and subtract each "N" events >>> configure "N"
    if self.nevents%self.PreScale==0:
      for cn in self.allCounters:
        self.storeAndSubtract(cn)
    
    return True
  
#----------------------------------
  def defineAnalysis(self):
    '''
    find out from the existing histograms what to do
    '''
    allHistos=self.getHistSvcDump()
    self.allCounters=self.getCounterList(allHistos,self.histbaselumi + self.btype['BB'] +'/')
    self.bookTrendHistos()
  
#----------------------------------
  def bookTrendHistos(self):
    '''
    book the histograms for the trends
    '''
    trendTypes=['mean','logZero']
    for TrT in trendTypes:
      for cn in self.allCounters:
        cntrend=TrT+'_'+cn
        self.HistSvc.book(self.histbaselumitrenddir,cntrend,cntrend,100,0.,100.)
                                    
#----------------------------------
  def getCounterList(self, ls, prefix):
    '''
    analyses the list of all histos and picks out the counter names only
    '''
    cl=[]
    for hn in ls:
      if hn.startswith(prefix):
        cn=hn.replace(prefix,'')
        if len(cn):
          cl.append(cn)
          print 'python algorithm',self.name,': found counter:',cn

    return cl
    
#----------------------------------
  def getHistSvcDump(self):
    '''
    trick to get a list of histos -- is there a clean way?
    '''
    from StringIO import StringIO
    sio = StringIO()
    sys.stdout=sio
    self.HistSvc.dump()
    dumpResult=sio.getvalue()
    sys.stdout=sys.__stdout__
    sio.close()
    ls = dumpResult.split('\n')
    return ls
    
    
#----------------------------------
  def storeAndSubtract(self, ct):
    '''
    calculates "R" for one counter
    '''
    # check if first time around
    try:
      hs=self.hStore[ct]
      makeNew=False
    except KeyError:
      makeNew=True
      self.hStore[ct]={}

    bx_mean={}
    bx_frac={}
    # calculate the basic values per crossing type
    for BT in self.btype.keys():
      BTV=self.btype[BT]
      # get previous
      h1=self.HistSvc[self.histbaselumi+BTV+'/'+ct]
      if makeNew:
        hP=self.cloneHisto1D(h1, self.histbaselumiprev+BT+'/', ct, reset=True)
      else:
        hP=self.hStore[ct][BT]

      # subtract previous from present
      hP.scale(-1.)
      hP.add(h1)
      ## print 'delta   ',ct,BT,BTV,hP.entries(),hP.sumAllBinHeights(),hP.mean()
      mean=hP.mean()
      # count number of entries above the cut (by subtracting below)
      cutBins=7
      nBelow=0
      for b in range(cutBins):
        nBelow+=hP.binHeight(b)
      nTot=hP.sumAllBinHeights()
      nAbove=nTot-nBelow
      if nTot:
        fraction=nAbove/nTot
      else:
        fraction=0
      ## print 'R-Results',ct,BT,BTV,nBelow, nAbove, nTot, 'mean',mean,'fraction',fraction
      bx_mean[BT]=mean
      bx_frac[BT]=fraction
      
      # copy present status to place for previous status for the next time
      self.hStore[ct][BT]=self.cloneHisto1D(h1, self.histbaselumiprev+BT+'/', ct)
      # close the loop
      pass

    # at this stage we should make the subtractions to get corrected "R"
    corr_mean=bx_mean['BB']-bx_mean['BL']-bx_mean['BR']+bx_mean['EE']
    corr_frac=bx_frac['BB']-bx_frac['BL']-bx_frac['BR']+bx_frac['EE']
    try:
      corr_logs = math.log(1.-bx_frac['BB']) + \
                  math.log(1.-bx_frac['EE']) - \
                  math.log(1.-bx_frac['BL']) - \
                  math.log(1.-bx_frac['BR'])
    except:
      corr_logs=0
    print 'python algorithm',self.name,': R-Results background corrected',ct,\
          'mean',corr_mean,'fraction',corr_frac,'log-fraction',corr_logs,'at',self.nevents

    # store trends for ['mean','logZero']
    hM=self.HistSvc[self.histbaselumitrend+'mean_'+ct]
    hZ=self.HistSvc[self.histbaselumitrend+'logZero_'+ct]
    axis=hM.axis()
    bins=axis.bins()
    for b in range(bins):
      valM=hM.binHeight(b)
      valZ=hZ.binHeight(b)
      if b<bins-1:
        valNextM=hM.binHeight(b+1)
        valNextZ=hZ.binHeight(b+1)
      else:
        valNextM=corr_mean
        valNextZ=-corr_logs
      x=0.5*(axis.binUpperEdge(b)+axis.binLowerEdge(b))
      hM.fill(x,valNextM-valM)
      hZ.fill(x,valNextZ-valZ)
    
    

#----------------------------------
  def cloneHisto1D(self, h1, s1, ct, reset=False):
    axis=h1.axis()
    bins=axis.bins()
    x0=axis.lowerEdge()
    x1=axis.upperEdge()
    h2=self.HistSvc[s1+ct]
    if h2!=None:
      h2.reset()
      h2.setTitle(ct)
    else:
      h2=self.HistSvc.book(s1,ct,ct,bins,x0,x1)
    if not reset: h2.add(h1)
    return h2

#----------------------------------
  def testingHistos(self, prflag):
    # histograms
    if prflag:
      print '.... dump histsvc'
      ##self.HistSvc.dump()
      print '.... end dump histsvc'

    histSPD={}
    histSPD['BC']=self.HistSvc[self.histbaselumi+self.btype['BB']+'/SPDMult']
    print histSPD['BC']
    histSPD['NB']=self.HistSvc[self.histbaselumi+self.btype['EE']+'/SPDMult']
    print histSPD['NB']

    print '.... statistics:'
    if histSPD['BC'] !=None:
      print '.... BC', histSPD['BC'].sumBinHeights(), histSPD['BC'].axis().bins()
    if histSPD['NB'] !=None:
      print '.... NB', histSPD['NB'].sumBinHeights(), histSPD['NB'].axis().bins()

    # create new histos for summing
    if self.HistSvc[self.histbaselumisums+'SPD'] == None :
      XXX=self.HistSvc.book(self.histbaselumisumsdir,'SPD','SPD',100,0.,100.)
      print 'XXX',XXX
      axis=histSPD['NB'].axis()
      YYY=histSPD['s1']=self.HistSvc.book(self.histbaselumisumsdir,'SPD1','SPD1',
                                          axis.bins(),axis.lowerEdge(),axis.upperEdge())
      print 'YYY',YYY
      ##if prflag: self.HistSvc.dump()
    else:
      XXX=self.HistSvc[self.histbaselumisums+'SPD']
      YYY=self.HistSvc[self.histbaselumisums+'SPD1']
    histSPD['SUM']=self.HistSvc[self.histbaselumisums+'SPD']
    histSPD['SUM'].fill(5.,11.)
    print '.... SUM', histSPD['SUM'].sumBinHeights()

    ## h1=histSPD['NB'].scale(-1.)  ## modifies the histogram in place
    YYY.reset()
    YYY.add(histSPD['NB'])
    YYY.scale(-1.)
    YYY.add(histSPD['BC'])
    print '.... SM', YYY.sumBinHeights()


    ## need a "clone" algorithm for histograms
    histSPD['clone']=self.cloneHisto1D(histSPD['NB'], self.histbaselumisums, 'clone', 'clone')
    print '.... CL', histSPD['clone'].sumBinHeights(),histSPD['clone'].entries(),\
          histSPD['clone'].sumExtraBinHeights(),histSPD['clone'].extraEntries()
    

###########################################################################################

    ## histo properties:
    ## 'add', 'allEntries', 'annotation', 'axis',
    ## 'binEntries', 'binError', 'binHeight', 'binMean',
    ## 'cast', 'contents', 'coordToIndex',
    ## 'dimension', 'entries', 'equivalentBinEntries', 'extraEntries',
    ## 'fill', 'maxBinHeight', 'mean', 'minBinHeight',
    ## 'reset', 'rms', 'scale', 'setTitle',
    ## 'sumAllBinHeights', 'sumBinHeights', 'sumExtraBinHeights', 'title'

    ## histsvc:
    ## 'book', 'bookProf', 'clearStore', 'dump', 'finalize',
    ## 'getAsAIDA', 'getAsROOT', 'getInterface', 'initialize', 'isValid', 'leaves',
    ## 'name', 'properties', 'registerObject', 'reinitialize', 'restart',
    ## 'retrieve', 'retrieve1D', 'retrieve2D', 'retrieve3D',
    ## 'retrieveInterface', 'retrieveObject', 'retrieveProfile1D', 'retrieveProfile2D',
    ## 'setRoot', 'start', 'stop', 'unregisterObject']

