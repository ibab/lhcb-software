"""
Parse the output of the timing table as either a .dat or .csv into a list of python dictionaries
"""

def parse(afile):
    """
    returns a list of python dictionaries which can be used to validate tests
    """
    sep="\t"
    if afile.endswith(".csv"):
        sep=","
    elif not afile.endswith(".dat"):
        raise TypeError("I do not understand files of the type "+afile.split('.')[-1])
    f=open(afile)
    if not f:
        raise IOError("Could not open "+afile)
    result=[]
    headerpos={}
    for line in f.readlines():
        if not line:
            break
        spl=[r.strip() for r in line.strip().split(sep)]
        if not len(headerpos):
            for i,n in enumerate(spl):
                headerpos[n]=i
        else:
            thed={}
            for k,v in headerpos.iteritems():
                av=spl[v]
                rav=None
                try:
                    if "." in av:
                        rav=float(av)
                except:
                    pass

                if rav is None:
                    try:
                        rav=int(av)
                    except:
                        pass
                
                if rav is not None:
                    av=rav
                thed[k]=av
            result.append(thed)
    
    f.close()
    return result

def event_av_dict(TT):
    """
    Takes a timing table list of dictionaries and simply converts it to a dictionary of:
    name : (#events , #sum of time)
    """
    if type(TT) is str:
        TT=parse(TT)
    elif type(TT) is not list:
        raise TypeError("You must send me a list of timing-table-like objects, or a string with the filename, but you sent me a "+str(type(TT)))
    ret={}
    for f in TT:
        ret[f["Algorithm"]]=(f["entries"], f["total (s)"])
    return ret

def compare(TT1,TT2,threshold=0.9):
    """
    Compare timing table entries, threshold applies on comparison of all float values.
    TT1: Reference Timing table to compare against
    TT2: New timing table
    threshold: how large a difference to allow on floating point numbers
    returns a tumple of lists:
    missing, different, added
    """
    refalgs=event_av_dict(TT1)
    compalgs=event_av_dict(TT2)
    missing=[f for f in refalgs if f not in compalgs]
    different=[]
    added=[f for f in compalgs if f not in refalgs]
    for f,t in refalgs.iteritems():
        if f not in compalgs:
            continue
        refevents,refsum=t
        compevents,compsum=compalgs[f]
        if refevents!=compevents:
            #print "Diff events", f
            different.append(f)
            continue
        
        if refsum*(1.+threshold)<compsum:
            #print "Diff high", f
            different.append(f)
            continue
        
        if threshold<1. and refsum*(1.-threshold)>compsum:
            #print "Diff low", f
            different.append(f)
            continue
        
        if threshold>1. and refsum*(1./threshold)>compsum:
            #print "Diff low", f
            different.append(f)
            continue
    
    return missing, different, added
