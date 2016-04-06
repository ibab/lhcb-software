"""
QMTest extension module for Moore and related tests.

- compareTimingTableFiles, extract the number of events from the timing table
                           along with the alg name, and use that to get
                           a comparison versus some references
                           Categorize and pretty print the results.

"""
from math import fabs

def compareTimingTableFiles(testname,result,causes,myfile,refFile,beginswith=None,max=200,extranewref='',timingthreshold=10.):
    """
    compare the timing table with a test timing table
    testname: the name of this qmtest
    result: the result to add to
    causes: the causes to add to
    stdout: the stdout, a string
    refFile: where to find the reference, a string
    beginswith: ignore timing table entries which do not start with this
    max: how many differences would be too many to print? default 200
    extranewref: whether to append anything extra to the newly created ref file
    """
    from LHCbAlgs import TTParser
    #parse the tables
    import os
    if not os.path.exists(myfile):
        causes.append("Cannot compare timing, timing table not created "+myfile)
        return
    
    if not os.path.exists(refFile):
        causes.append("Cannot compare timing, reference table does not exit "+refFile)
        return
    
    TTref=TTParser.parse(refFile)
    TTthis=TTParser.parse(myfile)
    #
    missing,different,added=TTParser.compare(TTref,TTthis,threshold=timingthreshold)
    if beginswith:
        missing=[m for m in missing if m.startswith(beginswith)]
        different=[d for d in different if d.startswith(beginswith)]
        added=[a for a in added if a.startswith(beginswith)]
    if len(missing+different+added)==0:
        return
    
    #work out if it was the number of events or the timing that caused this
    refdict=TTParser.event_av_dict(TTref)
    thisdict=TTParser.event_av_dict(TTthis)
    events=[]
    timings=[]
    for d in different:
        if refdict[d][0]!=thisdict[d][0]:
            events.append(d)
        elif refdict[d][1]<thisdict[d][1] and refdict[d][1]>0.0001:
            #allow faster, but not slower, and ignore <10 events for noise
            if refdict[d][0]>10:
                timings.append(d)
    
    different=events+timings
    if len(events):
        causes.append('#timing tables disagree')
        
        if len(extranewref) and not extranewref.startswith('.'):
            extranewref='.'+extranewref
        if beginswith is not None:
            extranewref=extranewref+'.'+beginswith
        import os
        os.system("cp "+myfile+" "+refFile+extranewref+'.new')
    
    #categorize the differences
    singleEventDiffs=[r for r in events if abs(refdict[r][0]-thisdict[r][0])<2]
    lineFailureDiff=[r for r in events if ((r not in singleEventDiffs) and (refdict[r][0]!=0) and (thisdict[r][0]==0))]
    tenPercentDiffs=[r for r in events if ((r not in singleEventDiffs) and (r not in lineFailureDiff) and (fabs(float(refdict[r][0]-thisdict[r][0])/float(refdict[r][0]+thisdict[r][0]))<0.1))]
    rest=[r for r in events if ((r not in singleEventDiffs) and (r not in lineFailureDiff) and (r not in tenPercentDiffs))]
    
    #get the results
    toomany=False
    if len(missing)+len(added)+len(different)>max:
        result[testname+'.a_total']=result.Quote('More than 200 differences, summary only: ' +str(len(missing)+len(added)+len(different)))
        toomany=True
    else:
        result[testname+'.a_total']=result.Quote('Total: ' +str(len(missing)+len(added)+len(different)))
    refresultdict={}
    timingdict={}
    for r,v in refdict.iteritems():
        refresultdict[r]=v[0]
    for r,v in thisdict.iteritems():
        timingdict[r]=v[0]
    
    listToResult(testname,"b_Missing",missing,refresultdict,timingdict,toomany,result=result)
    listToResult(testname,"c_Added",added,refresultdict,timingdict,toomany,result=result)
    listToResult(testname,"d_Single",singleEventDiffs,refresultdict,timingdict,toomany,"+/- 1 event",result=result)
    listToResult(testname,"e_failure",lineFailureDiff,refresultdict,timingdict,toomany,"zero events",result=result)
    listToResult(testname,"f_10pc",tenPercentDiffs,refresultdict,timingdict,toomany,"+/- <10 %",result=result)
    listToResult(testname,"g_Rest",rest,refresultdict,timingdict,toomany,result=result)
    
    for r,v in refdict.iteritems():
        refresultdict[r]=v[1]
    for r,v in thisdict.iteritems():
        timingdict[r]=v[1]
    
    listToResult(testname,"h_Timing",timings,refresultdict,timingdict,toomany,result=result)

