#check the import
from GaudiConf.QMTest.ConfigurablesTest import *

#check the methods
proj,ver=evalProject()
if None in [proj,ver] :
    raise ValueError("Couldn't work out what project I'm in...")



mods=moduleList()
mymods=[]
for proj in mods:
    mymods+=mods[proj]

if "GaudiConf" not in mymods:
    raise KeyError("Could not find my own module in the module list, this is bad, maybe you getpacked this into some project not depending on LHCb??")


confs=generateList()
if "GaudiConf" not in confs:
    raise KeyError("Could not find my own module in the configurable list, did building GaudiConf fail?")

confs2=generateList(mymods)

if confs2!=confs:
    print "missing", [m for m in confs if m not in confs2]
    print "added", [m for m in confs2 if m not in confs]
    raise KeyError("The content of the two dictionaries, one obtained from the list of all modules, and one obtained without sending a list don't agree")

#test saving
saveTo("conftest.conf",confs)

dummyconf={}
for i in range(42):
    dummyconf["pack"+str(i)]={}
    for j in range(7):
        dummyconf["pack"+str(i)]["conf"+str(i*7+j+1)]=True

dummyconf["GaudiConf"]=confs2["GaudiConf"]

saveTo("dummy.conf",dummyconf)

#five missing packages, five added packages, pack0-4, ~20 missing configurables, ~10 extra configurables

for i in range(5):
    del dummyconf["pack"+str(i)]
    dummyconf["addpack"+str(i)]={"Added1"+str(i):True,"Added2"+str(i):True}

for i in range(5,9):
    dummyconf["pack"+str(i)]["addconf"+str(i*7)]=True
    dummyconf["pack"+str(i)]["addconf2"+str(i*7)]=True
    for j in range(4):
        del dummyconf["pack"+str(i)]["conf"+str(i*7+j+1)]

saveTo("dummy.edit.conf",dummyconf)

#10 failed configurables, 10 broken configurables

for i in range(10,20):
    dummyconf["pack"+str(i)]["conf"+str(i*7+1)]=False
    dummyconf["pack"+str(i)]["conf"+str(i*7+2)]=None

del dummyconf["GaudiConf"]

saveTo("dummy.fails.conf",dummyconf)

class myresults(dict):
    Quote=str

result=myresults()

causes=[]

compareConfigurables("dummy.conf","dummy.conf",causes,result,"againstself")

if len(causes) or len(result):
    print result, causes
    raise KeyError("Testing identical configurations threw an error")

compareConfigurables("dummy.conf","dummy.edit.conf",causes,result,"againstedit")

if len(causes):
    print result,causes
    raise KeyError("Testing missing configurables which should not exist threw an error")

print result.keys()

if result.keys()!=['againstedit.missing', 'againstedit.added', 'againstedit.addedpack', 'againstedit.movedpack']:
    print causes, result
    raise KeyError("Did not detect the correct differences in the 'passing' test case")

compareConfigurables("dummy.conf","dummy.fails.conf",causes,result,"againstfail")

if result.keys()!=['againstedit.missing', 'againstfail.missing', 'againstfail.missingpack', 'againstfail.movedpack', 'againstfail.broken', 'againstfail.failed', 'againstedit.added', 'againstfail.addedpack', 'againstfail.added', 'againstedit.addedpack', 'againstedit.movedpack'] or causes!=['Entire Package Missing', 'Configurables failed to instantiate', "Configurables which didn't make it to Configurables module"]:
    print causes, result
    raise KeyError("Did not detect the correct differences in the 'failing' test case")


compareConfigurables("dummy.conf","dummy.fails.conf",causes,result,"forcefail",threshold=0)

if 'More than 0 missing configurable(s)' not in causes:
    print causes, result
    raise KeyError("Did not adjust threshold correctly in force fail test")


print "PASS"
