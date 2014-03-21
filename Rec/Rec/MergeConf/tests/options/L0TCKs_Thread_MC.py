from TCKUtils.utils import getTCKList
from LbUtils import LockOnPrintThread
import Queue, threading, thread, commands
#use TCKsh to find all TCKs

numthreads=8
import os
if "LHCB_NIGHTLY_MAX_THREADS" in os.environ:
    if os.environ["LHCB_NIGHTLY_MAX_THREADS"] is not "":
        numthreads=int(os.environ["LHCB_NIGHTLY_MAX_THREADS"])
        if numthreads>8:
            numthreads=8
        if numthreads<1:
            numthreads=1

L0TCKs={}
for TCK in getTCKList():
    if int(TCK,16)&0xffff == 0:
        continue
    L0TCKs[hex(int(TCK,16)&0xffff)]=TCK
    
#print L0TCKs
options="\"from Configurables import EventSelector; EventSelector().PrintFreq=1; from Configurables import L0App; L0App().EvtMax=3; L0App().TCK = '##TCK##';\""

#L0TCKs={"0x0045":"0x409f0045"}

#set each into L0Conf, try and run ...

def runAndParse(self,l0):
    #print l0
    #make sure there are 4 orders of magnitude
    while len(l0)<6:
        l0='x'.join([l0.split('x')[0],'0'+l0.split('x')[-1]])
    #get the result of the test
    s,o=commands.getstatusoutput('gaudirun.py ../options/L0-Testing-MC.py --option='+options.replace("##TCK##",l0))
    #coarsely parse the result.
    parsing=["Exit code! ","Not all events! ","Not Emulating! "]
    w_e_f=[]
    parsed=[s==0,"SUCCESS Reading Event record 3." in o,"L0EmulatorSeq" in o]
    
    for line in o.split('\n'):
        if line.startswith('#'):
            continue
        for msg in ["WARNING","ERROR","FATAL"]:
            if msg in line.split():
                w_e_f.append(line.strip())
    #did it work?
    toprint= "TEST RESULT "+ l0+" : "
    if False not in parsed and len(w_e_f)<2:
        return toprint+"PASS"
    toprint=toprint+"FAIL\n"
    toprint=toprint+"TEST RESULT "+ l0+" : "
    for p in range(len(parsing)):
        if not parsed[p]:
            toprint=toprint+parsing[p]
    if len(w_e_f)>=2:
        toprint=toprint+" Warnings/Errors/Fatal!"
    toprint=toprint+"\n"
    toprint=toprint+ "============ STDOUT ==========\n"
    toprint=toprint+ o
    toprint=toprint+ "\n============ KEY MESSAGES ==========\n"
    toprint=toprint+ "\n".join(w_e_f)
    toprint=toprint+ "\n============ END ==========\n"
    return toprint

l0pool=Queue.Queue()
for l0 in L0TCKs:
    l0pool.put(l0)

from time import sleep
lock=thread.allocate_lock()
for i in range(numthreads):
    sleep(1)
    t=LockOnPrintThread(l0pool,lock)
    t.method=runAndParse
    t.start()
#sleep(5) #don't all start at the same time!
