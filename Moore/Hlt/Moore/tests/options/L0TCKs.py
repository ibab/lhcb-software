from TCKUtils.utils import getTCKList

#use TCKsh to find all TCKs

L0TCKs={}
for TCK in getTCKList():
    L0TCKs[hex(int(TCK,16)&0xffff)]=TCK
    
#print L0TCKs
options="\"from Configurables import Moore; Moore().EvtMax=11; from Configurables import L0MuonAlg; L0MuonAlg( 'L0Muon' ).L0DUConfigProviderType = 'L0DUConfigProvider'; from Configurables import L0Conf; L0Conf().TCK = '##TCK##';\""

#L0TCKs={"0x0045":"0x409f0045"}

#set each into L0Conf, try and run ...
import commands
results={}
for l0 in L0TCKs:
    if int(l0,16)==0:
        continue
    while len(l0)<6:
        l0='x'.join([l0.split('x')[0],'0'+l0.split('x')[-1]])
    results[l0]=commands.getstatusoutput('gaudirun.py $APPCONFIGOPTS/Moore/MooreSimProductionWithL0Emulation.py $APPCONFIGOPTS/Moore/DataType-2012.py $APPCONFIGOPTS/Conditions/TCK-0x409f0045.py ../options/default-threshold.py ../options/Moore-Testing-MC.py  --option='+options.replace("##TCK##",l0))
    
#s,o=results["0x0045"]
#print o
#print s

parsing=["Exit code! ","Not all events! ","Not Emulating! "]
parsed={}
w_e_f={}
for l0 in results:
    s,o=results[l0]
    #check simple things, exit code, all events, contains sequencer, no FATAL errors
    parsed[l0]=[s==0,"SUCCESS Reading Event record 11." in o,"L0EmulatorSeq" in o]
    w_e_f[l0]=[]
    for line in o.split('\n'):
        if line.startswith('#'):
            continue
        for msg in ["WARNING","ERROR","FATAL"]:
            if msg in line:
                w_e_f[l0].append(line.strip())

#print result:
for l0 in results:
    print "TEST RESULT", l0,":",
    if False not in parsed[l0] and len(w_e_f)<2:
        print "PASS"
        continue
    print "FAIL"
    print "TEST RESULT", l0,":",
    for p in ranage(len(parsing)):
        if not parsed[p]:
            print parsing[p],
    print ""
    print "============ STDOUT =========="
    s,o=results[l0]
    print o
    print "============ END =========="
    print "============ KEY MESSAGES =========="
    print "\n".join(w_e_f[l0])
    print "============ END =========="
