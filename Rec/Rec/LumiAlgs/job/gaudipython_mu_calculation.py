#!/usr/bin/env python
import sys, math, pickle

###############################################################################
# GaudiPython script for lumi 
# Tested with: DaVinci v35r0; 
###############################################################################

from Gaudi.Configuration import importOptions, DEBUG, INFO, appendPostConfigAction
from Configurables import ApplicationMgr, DaVinci
from GaudiConf import IOHelper

import GaudiPython
from GaudiPython.GaudiAlgs import GaudiAlgo, SUCCESS
from Configurables import DaVinci, LHCbApp
	
DaVinci().HistogramFile = "DVHistos.root" # Histogram file
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 100000
DaVinci().TupleFile = "DVnTuples.root" # A file to store nTuples, such as the Lumi ntuple
DaVinci().Lumi = True # integrate the luminosity FSRs and print/store the result

DaVinci().DataType = "2012"
DaVinci().DDDBtag = 'dddb-20130929-1'
DaVinci().CondDBtag = 'cond-20141002'
DaVinci().DQFLAGStag = 'dq-20140822'

ApplicationMgr().AppName = 'LumiTool'
ApplicationMgr().OutputLevel = INFO
ApplicationMgr().HistogramPersistency = 'None'

IOHelper('ROOT').inputFiles([
    'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/swtest/lhcb/swtest/2011_smallfiles_EW/00010654_00000049_1.radiative.dst',
    'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/swtest/lhcb/swtest/2011_smallfiles_EW/00010352_00000014_1.radiative.dst',
    'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/swtest/lhcb/swtest/2011_smallfiles_EW/00010352_00000002_1.radiative.dst',
])

# more verbose lumi output
from Configurables import LumiAlgsConf
LumiAlgsConf().OutputLevel = INFO
def action():
    from Configurables import GetIntegratedLuminosity, LumiIntegrateFSR
    alg = GetIntegratedLuminosity('GetIntegratedLuminosity')
    alg.WriteCountersDetails = False
    alg.OutputLevel = INFO
    LumiIntegrateFSR('IntegrateBeamCrossing').OutputLevel = INFO
    LumiIntegrateFSR('IntegrateBeamCrossing').AccumulateMu = True
    # LumiIntegrateFSR('IntegrateBeamCrossing').MuKeyName = 'PoissonPV3D'

appendPostConfigAction(action)

appMgr = GaudiPython.AppMgr(outputlevel=INFO)
# some short-cuts to services
evtSvc = appMgr.evtSvc()
toolSvc = appMgr.toolsvc()
evCounter = 0

# store
store = {}


class myAnalysisAlgo(GaudiAlgo) :
    """ Algorithm to interrogate the lumi integration tools """
    def __init__(self, name='myAnalysisAlgo', **args):
        GaudiAlgo.__init__(self, name, **args) # essential
    def initialize(self):
        return SUCCESS	
    def execute( self ) :
        globals()['evCounter'] += 1
        return SUCCESS	
    def finalize( self ) :
        return SUCCESS	
    def stop( self ) :
        """ 
        lumi calculation is done in the "stop" state of gaudi 
        since we instantiate some tools, it may be better to execute this code in stop() rather than finalize()
        """
        print '********************* number of events seen: ', globals()['evCounter']

        from Configurables import LumiIntegrator
        lumitool = toolSvc.create('LumiIntegrator', 'IntegrateBeamCrossing', interface='ILumiIntegrator')
        lumirawtool = toolSvc.create('LumiIntegrator', 'RawLumiIntegrator', interface='ILumiIntegrator')
        print '********************* created lumiintegrator tools'

        # store data
        store = {}
        for (text, tool) in ( ('corrected', lumitool), ('raw', lumirawtool) ):
            n_files = tool.muFiles()
            print '********************* raw input  files', text, n_files, tool.lumiValue(), tool.lumiError(), tool.absolute()
            store[text] = {'files':tool.muFiles(), 
                           'lumiValue':tool.lumiValue(), 
                           'lumiError':tool.lumiError(), 
                           'absolute':tool.absolute(),
                           'filedata':[],
                       }
            for i in range(n_files):
                store[text]['filedata'].append(
                    {'sequence' :i, 
                     'run'      :tool.muRun(i), 
                     'guid'     :tool.muGuid(i), 
                     'time0'    :tool.muTime0(i), 
                     'time1'    :tool.muTime1(i), 
                     'deltaLumi':tool.muDeltaLumi(i), 
                     'norm'     :tool.muNorm(i), 
                     'mu'       :tool.muMu(i), 
                     'counters' :{}
                 }
                )
                print 'file', i, tool.muRun(i), tool.muGuid(i), tool.muTime0(i), tool.muTime1(i), tool.muDeltaLumi(i), tool.muNorm(i), tool.muMu(i), 
                print tool.muKeys(i)
                for k in range(tool.muKeys(i)):
                    if tool.muCounter(i,k) <= 50: continue # these are not useful for mu
                    norm = tool.muCounterNorm(i,k)
                    value = 0 if norm == 0 else tool.muCounterValue(i,k)/norm
                    store[text]['filedata'][-1]['counters'][tool.muCounterName(i,k)] = {'key':tool.muCounter(i,k), 'norm':norm, 'value':value}
                    print '      ', tool.muCounter(i,k), tool.muCounterName(i,k), norm, value, 
                print

        # save store
        pfname = 'rawdata.pck'
        pf = open(pfname,'wb')
        print pfname, 'opened'
        pickle.dump(store, pf)
        pf.close()
        print pfname, 'closed'
        
        print store

        return SUCCESS	

  
########################################################################################
#########################    THE ACTUAL JOB EXECUTION    ###############################
########################################################################################
        
# Make sure exit handling is correct
def myActionAtExit():
    from GaudiPython.Bindings import AppMgr
    gaudi = AppMgr()
    gaudi.exit()
import atexit
atexit.register(myActionAtExit)

# user algorithm
appMgr.addAlgorithm( myAnalysisAlgo() )
# all events
appMgr.run(-1)
