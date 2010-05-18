import Online.PVSS as PVSS
import Online.PVSSSystems as Systems
DP = PVSS.DataPoint

partition = 'LHCb'
mgr=Systems.controlsMgr('ECS')
typ = mgr.typeMgr().type('_FwFsmObject')
rd  = mgr.devReader()

def load_det_sub_item(itm,match):
    actor = PVSS.DpVectorActor(mgr)
    lookup = partition+itm+match+itm+match+itm[:-1]+'.fsm.currentState'
    #print 'Lookup:',lookup
    actor.lookup(DP.original(lookup),typ)
    rd.add(actor.container)
    rd.execute()
    state = {}
    c = actor.container
    for i in xrange(c.size()):
      print i,c.at(i).name(),c.at(i).data
    for i in actor.container:
        n = i.name()
        n = n[n.find('|')+1:n.find('.fsm.currentState:')]
        n = n[:n.find(itm)]
        #print n, i.name(),i.data
        if not state.has_key(n):
            state[n] = []
        state[n].append(i.data)
    return state

def load_det_item(itm):  return load_det_sub_item(itm='_'+itm+'|',match='*')

def hv_status():    return load_det_item('HV')
def dcs_status():   return load_det_item('DCS')
def dai_status():   return load_det_item('DAI')
def daq_status():   return load_det_item('DAQ')
def det_status():   return load_det_sub_item(itm='|',match='*')


def make_det_status():

    hv  = hv_status()
    dcs = dcs_status()
    dai = dai_status()
    daq = daq_status()
    det = det_status()

    print '---> ',len(hv.keys()),'datapoints'

    print '%-16s %-12s %-16s %-12s %-12s'%('Detector','HV status','DCS status','DAI status','DAQ status')
    print '-'*100
    for i in hv.keys():
        print '%-16s %-12s %-16s %-12s %-12s'%(i,hv[i][0],dcs[i][0],dai[i][0],daq[i][0])

    print '-'*100
    for i in det.keys():
        n = i
        if n.find(partition)==0: n = n[len(partition)+1:]
        s = '%-24s '%n
        itm = det[i]
        for j in itm: s = s + '%-16s '%j
        print s

def make_shift_info(match):
    actor = PVSS.DpVectorActor(mgr)
    lookup = partition+'_RunInfoCond.'
    start = len(lookup)
    lookup = lookup+match
    type = mgr.typeMgr().type('RunInfoConditions')
    actor.lookup(DP.original(lookup),type)
    rd.add(actor.container)
    rd.execute()
    #print '---> ',actor.container.size(),'datapoints'
    look = lookup[:start]
    for i in actor.container:
        nam = i.name()
        nam = nam[nam.find(look)+len(look):nam.find(':_original..')]
        print '%-24s %s'%(nam,i.data)
        #nam = nam[nam.find(look):nam.find(':_original..')]
        #print nam

def make_run_info(match):
    actor = PVSS.DpVectorActor(mgr)
    lookup = partition+'_RunInfo.'
    start = len(lookup)
    lookup = lookup+match
    type = mgr.typeMgr().type('RunInfo')
    actor.lookup(DP.original(lookup),type)
    rd.add(actor.container)
    rd.execute()
    look = lookup[:start]
    for i in actor.container:
        nam = i.name()
        nam = nam[nam.find(look)+len(look):nam.find(':_original..')]
        print '%-24s %s'%(nam,i.data)
        #nam = nam[nam.find(look):nam.find(':_original..')]
        #print nam



#import time, os
#print os.getpid()
#time.sleep(20)

def make_all():
    make_det_status()
    make_shift_info('LHC.*')
    make_shift_info('Magnet.*.*')
    make_shift_info('VELO.*.*')
    
    make_run_info('HLTFarm.*')
    make_run_info('SubDetectors.*')
    make_run_info('TFC.*')
    make_run_info('Trigger.*')
    make_run_info('general.TAE')
    make_run_info('general.L0Gap')
    make_run_info('general.activePartId')
    make_run_info('general.dataType')
    make_run_info('general.fillNumber')
    make_run_info('general.partName')
    #make_run_info('general.runInitialStartTime')
    make_run_info('general.runNMinutes')
    make_run_info('general.runNTriggers')
    make_run_info('general.runNumber')
    #make_run_info('general.runStartTime')
    #make_run_info('general.runStopTime')
    make_run_info('general.runType')
    make_run_info('status')


make_all()
