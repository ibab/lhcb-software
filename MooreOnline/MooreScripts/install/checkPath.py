import sys
import os

pathcheck=sys.argv[1]

#camelcase better not be missing, the others can be missing
order=[['MooreOnline','MOOREONLINE','Online','ONLINE'],
       ['MOOREONLINE','MOORE','HLT','PHYS','REC','LBCOM','LHCB','GAUDI'],
       ['MOOREONLINE','ONLINE','LHCB']
       ]


ld=os.environ[pathcheck]

def myfind(string,order):
    return [string.find("/"+o+"_") for o in order]

found=[myfind(ld,anorder) for anorder in order]

#print found

ok=True
for anorder in range(len(order)):
    if not ok: break
    for f in range(1,len(found[anorder])):
        if found[anorder][f]<0:
            #camelcase better not be missing, the others can be missing
            if order[anorder][f].upper()!=order[anorder][f]:
                print "# ERROR: missing "+order[anorder][f]
                ok=False
                break
        elif found[anorder][f]>=len(ld):
            print "# ERROR: missing "+order[anorder][f]
            ok = False
            break
        elif found[anorder][f]<found[anorder][f-1]:
            print "# ERROR: wrong order of "+order[anorder][f]+" "+order[anorder][f-1]
            ok=False
            break


if ok:
    print "# INFO: "+pathcheck+" is correct :)"
else:
    print "# ERROR: "+pathcheck+" not correct :("
    print "# ERROR: ",order,found
    ld=[l.replace('/cvmfs/cern.ch/lib/lhcb','/cvmfs...').replace(os.environ['CMTCONFIG'],'...').replace('/afs/cern.ch/lhcb/software/releases','/afs...').replace('/afs/cern.ch/lhcb/software/nightlies','/nightlies...') for l in ld.split(':') if 'InstallArea' in l];
    print '\n# ERROR: \t'.join(['']+ld);
