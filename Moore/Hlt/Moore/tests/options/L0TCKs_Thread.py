from TCKUtils.utils import getTCKList
from LbUtils import LockOnPrintThread
import Queue, threading, thread, commands
#use TCKsh to find all TCKs

L0TCKs={}
for TCK in getTCKList():
    if int(TCK,16)&0xffff == 0:
        continue
    L0TCKs[hex(int(TCK,16)&0xffff)]=TCK
    
#print L0TCKs
#takes far too long... perhaps use L0 only? tried passthrough, doesn't configure!
options="\"from Configurables import Moore; Moore().EvtMax=11; from Configurables import L0MuonAlg; L0MuonAlg( 'L0Muon' ).L0DUConfigProviderType = 'L0DUConfigProvider'; from Configurables import MessageSvc; MessageSvc().OutputLevel=WARNING; from Configurables import L0Conf; L0Conf().TCK = '##TCK##';\""

#L0TCKs={"0x0045":"0x409f0045"}

#set each into L0Conf, try and run ...

def runAndParse(self,l0):
    #print l0
    #make sure there are 4 orders of magnitude
    while len(l0)<6:
        l0='x'.join([l0.split('x')[0],'0'+l0.split('x')[-1]])
    #get the result of the test
    s,o=commands.getstatusoutput('gaudirun.py $APPCONFIGOPTS/Moore/MooreSimProductionWithL0Emulation.py $APPCONFIGOPTS/Moore/DataType-2012.py $APPCONFIGOPTS/Conditions/TCK-0x409f0045.py ../options/default-threshold.py ../options/Moore-Testing-MC.py --option='+options.replace("##TCK##",l0))
    #coarsely parse the result.
    parsing=["Exit code! ","Not all events! ","Not Emulating! "]
    w_e_f=[]
    parsed=[s==0,"SUCCESS Reading Event record 11." in o,"L0EmulatorSeq" in o]
    
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

lock=thread.allocate_lock()
for i in range(8):
    t=LockOnPrintThread(l0pool,lock)
    t.method=runAndParse
    t.start()
