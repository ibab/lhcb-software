#!/usr/bin/env python
"""
Script which wraps any GaudiRun options into an 'compare' mode where various pieces of the output are stored locally as a future reference, and then when run again compares the new output with the old output for you.

common options:
    gaudidiff.py [-h] [--initial-run before] [--test-run after] [--reset] [--no-run] ... all options to be passed to gaudirun ...

behaviour:
    Creates the following files:
    - <run>.csv, => the timing table
    - <run>.xml, => the xml summary
    - <run>.opts.py => all configuration options
    - <run>.conf => all print out during configuration stage
    - <run>.warn => warnings/errors/fatal
    - <run>.stderr => stderr printout
    - <run>.log => stdout not including configuration/timing table
    - <run>.sh => the command used to run, for book-keeping
    
    Run first time (baseline):
      - will write out to <--initial-run>.*, if these files do not exist.
    Run second time (comparison):
      - will write out to <--test-run>.*, overwriting if they exist
      - will diff the results versus --initial-run to find your changes
      
    
    -h: print help and exit
    
    --initial-run <name> : change the name of the baseline run, default before.*
    --test-run <name> : changes the name of the result files after.*
    --reset : forces recreation of the baseline files
    --no-run : don't do gaudirun, just compare the local files again
    --ignore-opts : don't include options-differences in the printed diff
    
    communicates with postConfigActions in the 'template' script via a dictionary added, Gaudi.Diff

    note that the value of --initial-run and --test-run can be a full or relative path.

    exit codes:
      0 for minor diffs
      1 for medium or major diffs

    printout:
      PASS to stdout if there are only minor diffs
      FAIL to stderr if there are major diffs
    
"""
#defaults
before="before"
after="after"
before_or_after="before"
alsoOptions=True
gaudirun=True

#######################################
# Parse the arguments, trivial
#######################################

import os
import sys

mydir=os.path.sep.join(__file__.split(os.path.sep)[:-1])
#print mydir

args=sys.argv[1:]
#print args


if "-h" in args or "--help" in args:
    print __doc__
    sys.exit(0)

if "--initial-run" in args:
    before=args[args.index("--initial-run")+1]
    args=args[:args.index("--initial-run")]+args[args.index("--initial-run")+2:]


if "--test-run" in args:
    after=args[args.index("--test-run")+1]
    args=args[:args.index("--test-run")]+args[args.index("--test-run")+2:]


if "--no-run" in args:
    gaudirun=False
    args=args[:args.index("--no-run")]+args[args.index("--no-run")+1:]

if not len(args):
    print __doc__
    raise ValueError("Insufficient number of arguments")

if os.path.exists(before+".opts.py") and "--reset" not in args:
    before_or_after="after"

if "--reset" in args:
    args=[a for a in args if a!="--reset"]


if "--ignore-opts" in args:
    alsoOptions=False
    args=[a for a in args if a!="--ignore-opts"]


#######################################
# Run the test job
#######################################

areplace=[a for a in args if a.startswith("--option=")]
areplacewith=[a.replace("--option=","--option=\"")+'"' for a in areplace]


exec("prefix = "+before_or_after)

commandstr="time gaudirun.py -o "+prefix+".opts.py "+" ".join(args)+" --option=\"import Gaudi; Gaudi.Diff={}; Gaudi.Diff['prefix']='"+prefix+"';\" "+mydir+"/diff_template.py > "+prefix+".stdout 2> "+prefix+".stderr"

for a,b in zip(areplace,areplacewith):
    commandstr=commandstr.replace(a,b)

if gaudirun:
    print "Running test '"+before_or_after+"':", prefix
    for ext in [".csv",".warn",".stdout",".stderr",".xml",".conf",".opts.py",".log",".sh"]:
        path=prefix+ext
        if os.path.exists(path):
            os.system("rm "+path)
    f=open(prefix+".sh","w")
    f.write(commandstr)
    f.close()
    print commandstr
    os.system(commandstr)

#pass to .conf everything which is printed before the first applicationmgr.success
if os.path.exists(prefix+".stdout") and gaudirun:
    f=open(prefix+".stdout")
    stdout=f.readlines()
    f.close()
    endline=0
    for line in range(len(stdout)):
        if "ApplicationMgr" and "SUCCESS" in stdout[line]:
            endline=line
            break
    conf=stdout[:endline]
    f=open(prefix+".conf","w")
    f.write("".join(conf))
    f.close()
    warn=[l for l in stdout[endline:] if not l.startswith("#") and ("WARNING" in l or "ERROR" in l or "FATAL" in l or "Exceptions/Errors" in l)]
    f=open(prefix+".warn","w")
    f.write("".join(warn))
    f.close()
    if len(warn):
        print "Seen warnings in this test:"
        print "".join(warn)
    log=[l for l in stdout[endline:] if "TIMER.TIMER" not in l]
    f=open(prefix+".log","w")
    f.write("".join(log))
    f.close()
    os.system("rm "+prefix+".stdout")

if gaudirun and os.path.exists(prefix+".stderr"):
    if os.path.getsize(prefix+".stderr")>20:
        print "Seen stderr in this test! Printing last 10 lines to stderr:"
        os.system("tail -n 20 "+prefix+".stderr 1>&2")

#bail out if this is the initial run
if before_or_after=="before":
    print "Stored test files for '"+prefix+"' now make your changes and run the comparison"
    os.system("ls "+prefix+".*")
    sys.exit(0)

#######################################
# Run the comparisons
#######################################
diffcount=0

#check that the two stderr are identical
if gaudirun:
    print "New comparison files created, now checking the differences",
else:
    print "Skip running, straight to comparison",
print "between",before,"and",after 
print "--------------------------"
os.system("ls "+prefix+".*")
print "-----------vs-------------"
os.system("ls "+before+".*")
stderrCheck=(True,"")
warnCheck=(True,"")
confCheck=(True,"")
optsCheck=(True,"")
fileCheck=(True,"")
statCheck=(True,"")
eventCheck=(True,"")
counterCheck=(True,"")
timingCheck=(True,"")

#warnings/errors during configuration
if os.path.exists(before+".conf") and os.path.exists(after+".conf"):
    bf=open(before+".conf")
    bfr=bf.readlines()
    bf.close()
    af=open(after+".conf")
    afr=af.readlines()
    af.close()
    bfr=[b for b in bfr if "warning" in b.lower() or "error" in b.lower() or "fatal" in b.lower()]
    afr=[b for b in afr if "warning" in b.lower() or "error" in b.lower() or "fatal" in b.lower()]
    missing=[b for b in bfr if b not in afr]
    added=[a for a in afr if a not in bfr]
    if len(added):
        confCheck=(False,"")
    if len (added+missing):
        print "--------------------------"
        print "CONF: Warnings during configuration"
        print "--------------------------"
        print "".join(["-"+m for m in missing]+["+"+a for a in added]).strip()
    diffcount+=len(added+missing)
    

def cdiff(c1,c2):
    missing=[c for c in c1 if c not in c2]
    added=[c for c in c2 if c not in c1]
    diff=[c for c in c1 if c in c2 and c1[c]!=c2[c]]
    subdiff={}
    for d in diff:
        if type(c1[d]) is dict:
            am,ad,di=cdiff(c1[d],c2[d])
            missing=missing+[d+"."+str(c) for c in am]
            added=added+[d+"."+str(c) for c in ad]
            subdiff[d]=di
    parseddiffs=[]
    for d in diff:
        if type(c1[d]) is not dict:
            if type(c1[d]) is list:
                parseddiffs.append(d+": -"+[a for a in c1[d] if a not in c2[d]].__str__()+" +"+[a for a in c2[d] if a not in c1[d]].__str__())
            elif type(c1[d]) is str:
                if c1[d].replace(before,after)==c2[d]:
                    continue
            elif len(c1[d].__str__())+len(c2[d].__str__())<60:
                parseddiffs.append(d+": "+c1[d].__str__()+" ==> "+c2[d].__str__())
            else:
                 parseddiffs.append(d+": \n\t"+c1[d].__str__()+"\n\t ==> \n\t"+c2[d].__str__())
    for d in subdiff:
        for sd in subdiff[d]:
            parseddiffs.append(d+"."+sd)
    return missing,added,parseddiffs
    

#configuration result
if alsoOptions and os.path.exists(before+".opts.py") and os.path.exists(after+".opts.py"):
    bf=open(before+".opts.py")
    bfr=bf.read()
    bf.close()
    af=open(after+".opts.py")
    afr=af.read()
    af.close()
    exec("beforedict="+bfr.replace("\n",""))
    exec("afterdict="+afr.replace("\n",""))
    #print beforedict.keys()
    #print afterdict.keys()
    missing,added,diffs=cdiff(beforedict,afterdict)
    if len(missing+added+diffs):
        optsCheck=(False,"\n".join([""]+["-"+m for m in missing]+["+"+a for a in added]+["?"+d for d in diffs]))
        print "--------------------------"
        print "OPTS.PY: Options differences:"
        print "--------------------------"
        print optsCheck[1].strip()
    diffcount+=len(missing+added+diffs)
    pass

#timing table
try:
    from LHCbAlgs import TTParser
    from math import fabs
    from LHCbAlgs.TTParser import compare, event_av_dict
    #print "WAAAAAAAAAHHHHH"
    if os.path.exists(before+".csv") and os.path.exists(after+".csv"):
        #print "WAAAAAAAAAHHHHH2"
        miss,diff,added=compare(before+".csv",after+".csv")
        TT1=event_av_dict(before+".csv")
        TT2=event_av_dict(after+".csv")
        #print TT1, TT2
        #ignore timing differences for <10 events seen
        #print diff
        diff=[d for d in diff if TT1[d][0]!=TT2[d][0] or TT1[d][0]>10]
        #ignrore timing differences of <0.1% of total
        diff=[d for d in diff if TT1[d][0]!=TT2[d][0] or fabs(TT1[d][1]-TT2[d][1])>0.0005*(TT1["EVENT LOOP"][1]+TT2["EVENT LOOP"][1])]
        if len(miss+diff+added):
            pr="\n-".join([""]+miss)
            pr=pr+"\n+".join([""]+added)
            pr=pr+"\n?".join([""]+[d+": "+TT1[d].__str__()+" ==> "+TT2[d].__str__() for d in diff])
            checkTiming=(True,pr)
            print "--------------------------"
            print "CSV: Timing differences: (#events, seconds)"
            print "--------------------------"
            print pr.strip()
        diffcount+=len(miss+diff+added)
except ImportError:
    pass

def vtd(vtreedict):
    retdict={}
    for c,v in vtreedict.iteritems():
        if c.replace("PFN:","") in retdict:
            continue
        if "GUID" in v.attrib() and c==v.attrib("GUID"):
            continue
        retdict[c]=v.value()
    return retdict

def ddiff(d1,d2):
    added=["+"+a+": "+str(d2[a]) for a in d2 if a not in d1]
    missing=["-"+m+": "+str(d1[m]) for m in d1 if m not in d2]
    diff=["?"+d+": "+str(d1[d])+" ==> "+str(d2[d]) for d in d1 if d in d2 and d1[d]!=d2[d]]
    if len(added+missing+diff):
        return (False,"\n".join([""]+missing+added+diff))
    return (True,"")

#xml summary
try:
    if os.path.exists(before+".xml") and os.path.exists(after+".xml"):
        from XMLSummaryBase.summary import Summary
        sumbefore=Summary()
        sumbefore.parse(before+".xml")
        sumafter=Summary()
        sumafter.parse(after+".xml")
        if sumbefore.isFailure()!=sumafter.isFailure() or sumbefore.children('step')[0].value()!= sumafter.children('step')[0].value():
            a=sumafter.children('step')[0].value()
            b=sumbefore.children('step')[0].value()
            
            statCheck=(False,
                       "Success: "
                       + str(not sumbefore.isFailure())
                       +" ==> "
                       +str(not sumafter.isFailure())
                       +" \n"+"Step: "
                       +b
                       +" ==> "
                       +a
                       )
        befevents=0
        for f,v in vtd(sumbefore.file_dict()["input"]).iteritems():
            befevents+=v
            afevents=0
        for f,v in vtd(sumafter.file_dict()["input"]).iteritems():
            afevents+=v
    
        if befevents!=afevents:
            eventCheck=(False,"Events: "+str(befevents)+" ==> "+str(afevents))
        fileCheck=ddiff(vtd(sumbefore.file_dict()["input"]),vtd(sumafter.file_dict()["input"]))
        allcountbef={}
        allcountafter={}
        for k,v in sumbefore.counter_dict().iteritems():
            for c,v in v.iteritems():
                allcountbef[k+"."+c]=v
        for k,v in sumafter.counter_dict().iteritems():
            for c,v in v.iteritems():
                allcountafter[k+"."+c]=v
        if vtd(allcountbef)!=vtd(allcountafter):
            counterCheck=(True,ddiff(vtd(allcountbef),vtd(allcountafter))[1])
        if not (counterCheck[0] and fileCheck[0] and eventCheck[0] and statCheck[0]):
            print "--------------------------"
            print "XML: Differences"
            print "--------------------------"
            print (statCheck[1]+"\n"+eventCheck[1]+fileCheck[1]+counterCheck[1]).strip()
            diffcount+=len((statCheck[1]+"\n"+eventCheck[1]+fileCheck[1]+counterCheck[1]).strip().split("\n"))
except ImportError:
    pass

#warnings
if os.path.exists(before+".warn") and os.path.exists(after+".warn"):
    a=open(after+".warn")
    ar=a.readlines()
    a.close()
    b=open(before+".warn")
    br=b.readlines()
    b.close()
    added=[a for a in ar if a not in br]
    removed=[b for b in br if b not in ar]
    if len(added):
        warnCheck=(False,"".join(added))
    if len(added+removed):
        print "--------------------------"
        print "WARN: Warning differences:"
        print "--------------------------"
        print "-".join([""]+removed).strip()
        print "+".join([""]+added).strip()
    diffcount+=len(added+removed)



#stderr
if os.path.exists(before+".stderr") and os.path.exists(after+".stderr") and os.path.getsize(before+".stderr")>20 and os.path.getsize(after+".stderr")>20:
    os.system("diff "+before+".stderr "+after+".stderr > "+after+".diff")
    b=open(after+".diff")
    br=b.read()
    b.close()
    if len(br):
        stderrCheck=(False,br)
        print "--------------------------"
        print "STDERR: Stderr differences:"
        print "--------------------------"
        print br
        diffcount+=len(br.split("\n"))
    os.system("rm "+after+".diff")
elif os.path.exists(after+".stderr") and os.path.getsize(after+".stderr")>20:
    stderrCheck=(False,"br")
    print "--------------------------"
    print "STDERR: New stderr:"
    print "--------------------------"
    os.system("cat "+after+".stderr")
    f=open(after+".stderr")
    l=len(f.readlines())
    f.close()
    diffcount+=l
elif os.path.exists(before+".stderr") and os.path.getsize(before+".stderr")>20:
    stderrCheck=(True,"br")
    print "--------------------------"
    print "STDERR: Removed stderr:"
    print "--------------------------"
    os.system("cat "+before+".stderr")
    f=open(before+".stderr")
    l=len(f.readlines())
    f.close()
    diffcount+=l

exitval=stderrCheck[0] and warnCheck[0] and confCheck[0] and optsCheck[0] and fileCheck[0] and statCheck[0] and eventCheck[0] and counterCheck[0] and timingCheck[0]

print "--------------------------"
print "TOTAL:",diffcount
if diffcount>100:
    print "more than 100 differences, probably you want to pipe the diff output to a file!"
    print "try again with --no-run to just do the diffs without re-running, and pipe to a file with | tee mydiffs.log,  or similar"

if not exitval:
    print >> sys.stderr, "FAIL"
    sys.exit(1)

print "PASS"
