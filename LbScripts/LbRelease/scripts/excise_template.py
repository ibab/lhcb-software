"""
A very neat little gaudirun options file script, which uses PostConfig actions to make sure the setup is 100% the same.
"""
#################################################
# Part 1: Steering parameters for you to consider,
#         need to exist before this script is called
#################################################

import Gaudi
#print
#print "WAAAAAAAAAAAAAAAAAAAAAAAAAAAAH!!!!!!!!!!!!!!!!!!"
#print
#print Gaudi.Excise

#The main steering parameter, the full name of the alg to be found
#Gaudi.Excise['AlgToExcise']
#that can be any sort of alg, including a sequencer, but can't for the moment be in the middle of a ProcessPhase, since I am unable to insert things into a ProcessPhase.

#choose one of these three options
#Gaudi.Excise['exise_or_run']="Print" #Just print what I have found and do nothing
#Gaudi.Excise['exise_or_run']="Excise" #Create the sandbox
#Gaudi.Excise['exise_or_run']="Run"   #run from the sandbox

#Do you want to run just one alg, or everything after a certain point? You decide
#Gaudi.Excise['runAllAfter']=False

#do you want to run the alg in question during the dumping?
#Gaudi.Excise['runAlg']=True

#name of the output file:
#Gaudi.Excise['outputfile']="sandbox.dst"

#Used only in creating the sandbox, add to killAtLeast if you know a set of event nodes which are not required by your alg or any subsequent alg
#Gaudi.Excise['copyNodes']=["/Event#999"]
#Gaudi.Excise['killAtLeast']=["/Event/Transient"]

#used only when reading from the sandbox, list of algs which must be added

from GaudiConf.Manipulations import configurableInstanceFromString, nameFromConfigurable, postConfigCallable, configurableClassFromString

#######################################################
# Part 2: Functions and methods you don't need to touch
#######################################################

from Configurables import GaudiSequencer
from Configurables import ProcessPhase

def findalg( head=None, find="" ) :
    """ Find a given algoritm, return a tuple
    (found_yet?,before,contained_in,after,contains_stuff)
    """
    from GaudiConf.Manipulations import configurableInstanceFromString, nameFromConfigurable, postConfigCallable, configurableClassFromString
    if type(head) is list:
        raise TypeError("findalg, you need to send me one object at a time!")
    
    if head is None:
        from Gaudi.Configuration import ApplicationMgr
        head=ApplicationMgr()
    
    headname=nameFromConfigurable(head)
    
    itsme=False
    
    if headname is None or head is None:
        return (False,[],[],[])
    if headname==find:
        itsme=True
    
    head=configurableInstanceFromString(headname)
    found=itsme
    before=[]
    contained=[]
    after=[]
    contains_stuff=[]
    
    orderedkids=[]
    
    for p in [ 'Members','Filter0','Filter1','TopAlg','OutStream' ] :
        if not hasattr(head,p) : continue
        orderedkids=orderedkids+getattr(head,p)
    from Configurables import ProcessPhase
    if type(head) is ProcessPhase:
        orderedkids+=["GaudiSequencer/"+headname.split("/")[-1]+d+"Seq" for d in head.getProp("DetectorList")]
    
    if itsme:
        contains_stuff=orderedkids
    
    for s in orderedkids:
        ifound,ibefore,icontained,iafter,istuff=findalg(s,find)
        if (found and ifound):
            print "#WARNING The algorithm you asked for appears twice! I cannot excise it properly like that. I take the first occurance"
        if ifound and itsme:
            raise NameError("Recursion detected, somehow the algorithm is a container which contains itself, that's really a problem.")
        
        if itsme:
            contains_stuff=contains_stuff+ibefore+icontained+iafter+istuff
        elif found:
            after=after+ibefore+icontained+iafter
        elif ifound:
            before=before+ibefore
            contained=icontained
            after=after+iafter
            found=True
            contains_stuff=contains_stuff+istuff
        else:
            before+=ibefore
            if len(iafter) or len(icontained):
                raise ValueError("I didn't expect sequencers when the alg was said not to be found yet")
    if itsme:
        return (True,[],[],[],contains_stuff)
    
    #if I found it, register me as containing the thing
    if found:
        contained=[headname]+contained
    else:
        before=[headname]+before
    
    return (found,before,contained,after,contains_stuff)

def printIfFound(head=None,find=""):
    print findalg(head,find)

def createsandbox(head,find="",outputname="sandbox.dst"):
    from GaudiConf.Manipulations import configurableInstanceFromString, nameFromConfigurable, postConfigCallable, configurableClassFromString
    found,before,contains,after,contained_stuff=findalg(head,find)
    print found,before,contains,after,contained_stuff
    if not found:
        raise NameError("I could not find the thing you asked me to excise!")
    for a in after:
        #needed here for algs/sequencers which appear twice!
        if a not in before and a not in contains and a not in contained_stuff:
            ac=configurableInstanceFromString(a)
            ac.Enable=False
    if not contains[-1].startswith("ProcessPhase"):
        #if not insertEmptyKiller(contains[-1],find):
        #     raise ValueError("Could not insert EmptyNodeKiller")
        if not insertDump(contains[-1],find,outputname):
            raise ValueError("Could not insert OutputStream")
    else:
        raise NameError("#Cannot excise from the middle of a ProcessPhase. Try Excising the ProcessPhase itself, called "+contains[-1])
    if not Gaudi.Excise['runAlg']:
        ac=configurableInstanceFromString(find)
        dc.Enable=False
    return

def runsandbox(head,find=None,outputname="sandbox.dst",runAllAfter=False,findalg=findalg):
    from GaudiConf.Manipulations import configurableInstanceFromString, nameFromConfigurable, postConfigCallable, configurableClassFromString
    if find is None:
        raise ValueError("You didn't ask me to search for anything!")
    found,before,contains,after,contained_stuff=findalg(head,find)
    print found,before,contains,after,contained_stuff
    if not found:
        raise NameError("I could not find the thing you asked me to excise!")
    user_algs=getUserAlgs()
    for a in before:
        if a not in contains and a not in user_algs and a not in contained_stuff:
            ac=configurableInstanceFromString(a)
            ac.Enable=False
    if not insertEmptyKiller(contains[-1],find):
        raise ValueError("Could not insert EmptyNodeKiller")
    if not insertSomething(contains[-1],find,GaudiSequencer('PreExciseUserAlgs')):
        raise ValueError("Could not insert PreAlgs")
    if not insertSomething(contains[-1],find,GaudiSequencer('PostExciseUserAlgs'),before=False):
        raise ValueError("Could not insert PostAlgs")
    if not runAllAfter:
        for a in after:
            if a not in before and a not in contains and a not in user_algs and a not in contained_stuff:
                ac=configurableInstanceFromString(a)
                ac.Enable=False
    from GaudiConf import IOExtension
    IOExtension().inputFiles([outputname],clear=True)
    return

def insertSomething(seq,entry,insertme,before=True):
    from GaudiConf.Manipulations import configurableInstanceFromString, nameFromConfigurable, postConfigCallable, configurableClassFromString
    seqname=nameFromConfigurable(seq)
    seq=configurableInstanceFromString(seqname)
    entryname=nameFromConfigurable(entry)
    entry=configurableInstanceFromString(entry)
    if type(insertme) is not list:
        insertme=[insertme]
    dumpnames=[s.getFullName() for s in insertme]
    for p in [ 'Members','Filter0','Filter1','TopAlg','OutStream' ] :
        if not hasattr(seq,p) : continue
        lists = getattr(seq,p)
        bef=[]
        me=None
        af=[]
        if entryname in lists:
            bef=lists[:lists.index(entryname)]
            me=lists[lists.index(entryname)]
            if len(lists)>lists.index(entryname):
                af=lists[lists.index(entryname)+1:]
            if before:
                seq.setProp(p,bef+dumpnames+[me]+af)
            else:
                seq.setProp(p,bef+[me]+dumpnames+af)
            return True
        if entry in lists:
            bef=lists[:lists.index(entry)]
            me=lists[lists.index(entry)]
            if len(lists)>lists.index(entry):
                af=lists[lists.index(entry)+1:]
            if before:
                seq.setProp(p,bef+insertme+[me]+af)
            else:
                seq.setProp(p,bef+[me]+insertme+af)
            return True
    
    raise ValueError("Failed to insert "+insertme.__str__()+" in "+seqname+" "+" about "+entryname)

def insertDump(seq,entry,outputname):
    dumpAlgs=[]
    from Configurables import EventNodeKiller
    dumpAlgs+=[EventNodeKiller("killForDump")]
    EventNodeKiller("killForDump").Nodes=Gaudi.Excise['killAtLeast']
    from Configurables import OutputStream, InputCopyStream
    dump=OutputStream("DumpEverything")
    dump.ItemList=Gaudi.Excise['copyNodes']
    from GaudiConf import IOHelper
    dumpAlgs+=IOHelper().outputAlgs(outputname,dump,writeFSR=True)
    return insertSomething(seq,entry,dumpAlgs)

def insertEmptyKiller(seq,entry):
    from Configurables import EmptyEventNodeCleaner
    return insertSomething(seq,entry,EmptyEventNodeCleaner("ExciseEmptyKiller"))

def getUserAlgs(findalg=findalg):
    allalgs=[]
    from Configurables import GaudiSequencer
    for alg in [GaudiSequencer("PreExciseUserAlgs"), GaudiSequencer('PostExciseUserAlgs')]:
        found,b,c,a,ci=findalg(alg,find="THisisprobablynotgoingtoexist")
        allalgs=allalgs+b+c+a+ci
    return allalgs
    


#######################################################
# Part 3: do the correct PostConfigAction you requested
#######################################################
if Gaudi.Excise['excise_or_run'].lower()=="excise":
    postConfigCallable(createsandbox,None,find=Gaudi.Excise['AlgToExcise'],outputname=Gaudi.Excise['outputfile'])
elif Gaudi.Excise['excise_or_run'].lower()=="run":
    postConfigCallable(runsandbox,None,find=Gaudi.Excise['AlgToExcise'],outputname=Gaudi.Excise['outputfile'],runAllAfter=Gaudi.Excise['runAllAfter'])
elif Gaudi.Excise['excise_or_run'].lower()=="print":
    postConfigCallable(printIfFound,None,find=Gaudi.Excise['AlgToExcise'])
else:
    raise ValueError("You have no chosen a valid string for Gaudi.Excise.['ecsise_or_run'], "+Gaudi.Excise['excise_or_run'])
