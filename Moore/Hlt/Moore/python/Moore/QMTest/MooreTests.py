"""
QMTest extension module for Moore and related tests.

- compareTimingTableEvents, extract the nubmer of events from the timing table
                            along with the alg name, and use that to get
                            a comparison versus some references
                            Categorize and pretty print the results.

"""
from math import fabs

def extractEventsFromTiming(stdout,beginswith=None):
    """
    extract the timing table from the stdout
    returns a tuple, first a list of Alg:events, then a dict of {alg : event}
    """
    timing=[]
    timingdict={}
    for line in stdout.split('\n'):
        if 'TIMER' not in line:
            continue
        if beginswith is not None and beginswith not in line:
            continue
        line=line.replace('\t',' ')
        line=line.replace('  ',' ')
        line=line[line.find('INFO')+4:]
        line=line.strip().rstrip()
        line=line.split('|')
        if len(line)<5:
            continue
        if not line[0].startswith(beginswith): 
            continue
        name=line[0].strip().rstrip().lstrip()
        num=line[4].strip().rstrip().lstrip()
        timing.append(': '.join([name,num]))
        timingdict[name]=int(num)
    return timing, timingdict

#function to add results
def listToResult(testname,resname,thelist,refresultdict,timingdict,toomany,resstr=None,result=None):
    """
    converts a list into a string for adding into a result.
    testname: the name of this test, used for the result name
    resname: the name of this specific result, the result name is made from
              testname+resname.lower()
    thelist: list of whatever to make into a quote
    refresultsdict: dictionary of {referenceAlg : #events} 
    timingdict: dictionary of {testAlg : #events} 
    toomany: flag to allow the full list as a quote, or only the first 10 entries
    resstr: override the resname in the quote, but not the results entry,
            usually the results entry starts with resname.split('_')[-1]
    """
    #global result
    #print "64"
    if not len(thelist):
        return
    #print "67"
    if resstr is None:
        resstr=resname.split('_')[-1]
    prstr=resstr+": "+str(len(thelist))
    #print "71"
    if len(thelist)>10 and toomany:
        thelist=thelist[:10]
    #print "74"
    newlist=[]
    for l in thelist:
        if l in refresultdict and l in timingdict:
            newlist.append(l+' '+str(refresultdict[l])+'->'+str(timingdict[l]))
        elif l in refresultdict:
            newlist.append(l+' '+str(refresultdict[l])+'->?')
        elif l in timingdict:
            newlist.append(l+' ?->'+str(timingdict[l]))
        else:
            newlist.append(l)
    prstr=prstr+'\n\t'+',\n\t'.join(newlist)
    #print "76"
    result[testname+'.'+resname]=result.Quote(prstr)
    #print "78"
    return

def eventTableRef(filename,beginswith=None):
    """
    Reads a timing table ref, returns a tuple:
         firstly list of [alg: num]
         then a dict of {alg:num}
    """
    reftiming=open(filename)
    refresult=reftiming.read()
    reftiming.close()
    refresult.replace('  ',' ')#ignore multiple whitespace
    refresult=refresult.split('\n')
    refdict={}
    refresult=[ref.strip().rstrip().lstrip() for ref in refresult]
    refresult=[ref for ref in refresult if len(ref)>2]#ignore white lines
    if beginswith is not None:
        refresult=[ref for ref in refresult if ref.startswith(beginswith)]
    for ref in refresult:
        refdict[':'.join(ref.split(':')[:-1])]=int(ref.split(':')[-1])
    return refresult, refdict
    
def compareTimingTableEvents(testname,result,causes,stdout,refFile,beginswith=None,max=200):
    """
    compare the timing table with a test timing table
    testname: the name of this qmtest
    result: the result to add to
    causes: the causes to add to
    stdout: the stdout, a string
    refFile: where to find the reference, a string
    beginswith: ignore timing table entris which do not start with this
    max: how many differences would be too many to print? default 200
    """
    #parse the tables
    refresult,refresultdict=eventTableRef(refFile,beginswith)
    timing, timingdict=extractEventsFromTiming(stdout,beginswith)
    
    #compare, write out new refs?
    if refresult==timing or refresultdict==timingdict:
        return
    causes.append('#events in timing tables disagree')
    extranewref=''
    if beginswith is not None:
        extranewref='.'+beginswith
    f=open(refFile+extranewref+'.new','w')
    f.write('\n'.join(timing))
    f.close()
    #categorize the differences
    missing=[r for r in refresultdict if r not in timingdict]
    added=[r for r in timingdict if r not in refresultdict]
    diff=[r for r in refresultdict if ((r in timingdict) and (timingdict[r]!=refresultdict[r]))]
    singleEventDiffs=[r for r in diff if abs(refresultdict[r]-timingdict[r])<2]
    lineFailureDiff=[r for r in diff if ((r not in singleEventDiffs) and (refresultdict[r]!=0) and (timingdict[r]==0))]
    tenPercentDiffs=[r for r in diff if ((r not in singleEventDiffs) and (r not in lineFailureDiff) and (fabs(float(refresultdict[r]-timingdict[r])/float(refresultdict[r]+timingdict[r]))<0.1))]
    rest=[r for r in diff if ((r not in singleEventDiffs) and (r not in lineFailureDiff) and (r not in tenPercentDiffs))]
    
    #get the results
    toomany=False
    if len(missing)+len(added)+len(diff)>max:
        result[testname+'.a_total']=result.Quote('More than 200 differences, summary only: ' +str(len(missing)+len(added)+len(diff)))
        toomany=True
    else:
        result[testname+'.a_total']=result.Quote('Total: ' +str(len(missing)+len(added)+len(diff)))
    listToResult(testname,"b_Missing",missing,refresultdict,timingdict,toomany,result=result)
    listToResult(testname,"c_Added",added,refresultdict,timingdict,toomany,result=result)
    listToResult(testname,"d_Single",singleEventDiffs,refresultdict,timingdict,toomany,"+/- 1 event",result=result)
    listToResult(testname,"e_failure",lineFailureDiff,refresultdict,timingdict,toomany,"zero events",result=result)
    listToResult(testname,"f_10pc",tenPercentDiffs,refresultdict,timingdict,toomany,"+/- <10 %",result=result)
    listToResult(testname,"g_Rest",rest,refresultdict,timingdict,toomany,result=result)
    
