"""
QMTest validator expension module, for checking the split RawEvent file format
"""

def dirThisFile(afile,causes, result, testname):
    """
    interpret a ROOT file, read all locations into a list of directories/names
    """
    import ROOT
    import os
    if not os.path.exists(afile):
        causes.append("File was not found "+afile)
        result[testname]=result.Quote("File was not found "+afile)
        return []
    f=ROOT.TFile.Open(afile)
    if not f:
        causes.append("File was not openable by ROOT "+afile)
        result[testname]=result.Quote("File was not openable by ROOT "+afile)
        return []
    try:
        names = [b.GetName().replace("_","/").lstrip("/").rstrip("/").rstrip(".") for b in f.Event.GetListOfBranches()]
    except:
        names = []
    #print names
    f.Close()
    return names

def checkIsSplit(afile, version, causes, result, testname):
    """
    Check the split version of a file, compare the branches of 'Event' with the
    expected contents from the database
    """
    from RawEventCompat.Configuration import RawEventFormatConf, _checkv, _replaceWrap
    RawEventFormatConf().loadIfRequired()
    from RawEventCompat.Configuration import ReverseDict
    from RawEventFormat import Raw_location_db
    version=_checkv(version)
    required=[_replaceWrap(r).lstrip("/").rstrip("/") for r in ReverseDict(version).keys()]
    shouldbegone=[]
    otherversions=[v for v in Raw_location_db if v!=version]
    for other in otherversions:
        for loc in ReverseDict(other).keys():
            loc=loc.lstrip("/").rstrip("/")
            if loc not in shouldbegone and loc not in required:
                shouldbegone.append(loc)
    required=[("Event/"+r).replace("Event/Event/","Event/") for r in required]
    shouldbegone=[("Event/"+r).replace("Event/Event/","Event/") for r in shouldbegone]
    names=dirThisFile(afile,causes, result, testname)
    if not len(names):
        return
    missing=[r for r in required if r not in names]
    therewrongly=[s for s in shouldbegone if s in names]
    if len(missing)+len(therewrongly)==0:
        return
    causes.append("File was not of the correct split format")
    quote=""
    if len(missing):
        quote=quote+"Expected the following locations: "+missing.__str__()
    if len(therewrongly):
        if len(quote):
            quote=quote+"\n"
        quote=quote+"Locations which should not be there: "+therewrongly.__str__()
    result[testname]=result.Quote(quote)
    return


def checkIsGone(afile, shouldbegone, causes, result, testname):
    """
    Check a given set of locations do not exist on the Root file
    """
    names=dirThisFile(afile,causes, result, testname)
    shouldbegone=[s.lstrip("/").rstrip("/") for s in shouldbegone]
    if not len(names):
        return
    therewrongly=[s for s in shouldbegone if s in names]
    if len(therewrongly)==0:
        return
    causes.append("Extra location discovered")
    quote="Locations which should not be there: "+therewrongly.__str__()
    result[testname]=result.Quote(quote)
    return

