#!/usr/bin/python
"""
Script which wraps any GaudiRun options into an 'excised' version, which subsequently runs only a list of specified algorithms.

common options:
    gaudiexcise.py [-h/-p/-d] [--no-run] [--sandbox-file sandbox.dst] <algorithm> ... all options to be passed to gaudirun ...

behaviour:
    will create a local sandbox.dst file if it does not exist (delete to recreate)
    will run on a local sandbox.dst file if it exists (delete to recreate)

    -h: print help and exit
    -p: just print the alg names, but don't sandbox anything
    -d: dump sandbox contents for first event, don't change anything

    --sandbox-file use a different file as output
    --no-run, don't run the excised alg during the sandbox-creation
    --run-all-after: also run all subsequent algorithms
    --rewrite: overwrite even if the sandbox file exists

uncommon options:
    --kill-nodes [list of nodes to remove from the excised file before writing]
                 default is only '/Event/Transient'
                 this needs to be interpretable as a python list, e.g..
                 '[\'/Event/Loc1\',\'/Event/Loc2\']'
    --copy-nodes [list of nodes to copy to the excised file]
                 default is ['/Event#999']

other options:
    This excise script is sensitive to the content of the two sequencers
    
    GaudiSequencer('PreExciseUserAlgs')
    GaudiSequencer('PostExciseUserAlgs')
    
    These sequencers are inserted on running with the sandbox.
    They are up to the user to configure to potentially fix problem locations or
    force the running of certain other sequencers.
    So, if you configure these sequencers in your options they will be respected.
    E.g. it is most often necessary to force re-decoding of XXXLiteClusters since they cannot be persisted

details:
    communicates with postConfigActions in the 'template' script via a dictionary added, Gaudi.Excise
"""
#defaults
outputfile="sandbox.dst"
runAlg=True
runAllAfter=False
copyNodes='[\'/Event#999\']'
killNodes='[\'/Event/Transient\']'
excise_or_run="Excise"
AlgToExcise=''


import os
import sys

mydir=os.path.sep.join(__file__.split(os.path.sep)[:-1])
#print mydir

args=sys.argv[1:]
#print args


if "-h" in args or "--help" in args:
    #print "FOUND HELP"
    print __doc__
    sys.exit(0)

if "--sandbox-file" in args:
    outputfile=args[args.index("--sandbox-file")+1]
    args=args[:args.index("--sandbox-file")]+args[args.index("--sandbox-file")+2:]

#if I've been asked to dump, bail out here
if "-d" in args:
    if not os.path.exists(outputfile):
        raise ValueError("can't dump the file because it does not exist "+outputfile )
    os.system("gaudirun.py --option=\"from Gaudi.Configuration import*; from Configurables import LHCbApp, StoreExplorerAlg; from GaudiConf import IOExtension; LHCbApp().EvtMax=1; StoreExplorerAlg().Load=True; ApplicationMgr().TopAlg=[StoreExplorerAlg()]; IOExtension().inputFiles(['"+outputfile+"']);\"")
    sys.exit(0)

if len(args) < 3:
    print __doc__
    raise ValueError("Insufficient number of arguments")

if os.path.exists(outputfile) and "--rewrite" not in args:
    excise_or_run="Run"

if "--rewrite" in args:
    args=[a for a in args if a!="--rewrite"]

if "--no-run" in args:
    runAlg=False
    args=[a for a in args if a!="--no-run"]

if "--run-all-after" in args:
    runAllAfter=True
    args=[a for a in args if a!="--run-all-after"]

if "-p" in args:
    excise_or_run="Print"
    args=[a for a in args if a!="-p"]

if "--kill-nodes" in args:
    killNodes=args[args.index("--kill-nodes")+1]
    args=args[:args.index("--kill-nodes")]+args[args.index("--kill-nodes")+1:]
    exec("killNodesA="+myarg)
    if type(killNodesA) is not list:
        print __doc__
        raise TypeError("--kill-nodes has to be interpreble as a python list, you sent "+myarg )

if "--copy-nodes" in args:
    copyNodes=args[args.index("--copy-nodes")+1]
    args=args[:args.index("--copy-nodes")]+args[args.index("--copy-nodes")+1:]
    exec("copyNodesA="+myarg)
    if type(copyNodesA) is not list:
        print __doc__
        raise TypeError("--copy-nodes has to be interpreble as a python list, you sent "+myarg )

AlgToExcise=args[0]
args=args[1:]

areplace=[a for a in args if a.startswith("--option=")]
areplacewith=[a.replace("--option=","--option=\"")+'"' for a in areplace]

#print areplace, areplacewith

commandstr="gaudirun.py "+" ".join(args)+" --option=\"import Gaudi; Gaudi.Excise={}; Gaudi.Excise['killAtLeast']="+killNodes+"; Gaudi.Excise['copyNodes']="+copyNodes+"; Gaudi.Excise['AlgToExcise']='"+AlgToExcise+"'; Gaudi.Excise['runAllAfter']="+str(runAllAfter)+"; Gaudi.Excise['runAlg']="+str(runAlg)+"; Gaudi.Excise['excise_or_run']='"+excise_or_run+"'; Gaudi.Excise['outputfile']='"+outputfile+"';\" "+mydir+"/excise_template.py"

for a,b in zip(areplace,areplacewith):
    commandstr=commandstr.replace(a,b)

print commandstr
os.system(commandstr)
