# -*- coding: utf-8 -*-
###################################################
# Helper functions, shared code, used in the tests
###################################################
PrintParseLine="#######################  Printing Configurables:"

def generateList(packages=[]):
    """
    Return a dictionary of all configurables, {package: {configurable: True/False/None} }
    input a list of packages to restrict the packages examined
    True: exists in Configurables and is instantiated OK
    False: exists in Configurables but fails to instantiate
    None: Exists in confDB but not in Configurables?
    """
    from GaudiKernel.ConfigurableDb import cfgDb, loadConfigurableDb
    import Configurables
    #package : conf : Pass/Fail (True/False)
    retdict={}
    loadConfigurableDb()
    for name,conf in cfgDb.iteritems():
        mypack=conf['package']
        if len(packages) and mypack not in packages:
            continue
        if mypack not in retdict:
            retdict[mypack]={}
        retdict[mypack][name]=None
        
        if hasattr(Configurables, name):
            try:
                aConf=getattr(Configurables,name)
                if callable(aConf):
                    aConf()
                else:
                    raise TypeError(name+" is not callable, Configurables."+name+" evaluates as type "+str(type(aConf)))
                retdict[mypack][name]=True
            except Exception,e:
                import sys, traceback
                retdict[mypack][name]=False
                print >> sys.stderr, 'ERROR, cannot import/instantiate configurable', name, '\n-------\n', e.__class__, '\n-------'
                traceback.print_exc()
    return retdict

def saveTo(ofile,thingy):
    f=None
    try:
        import os
        f=open(os.path.expandvars(ofile),"w")
    except IOError:
        pass
    if not f:
        import sys
        print >> sys.stderr, "# Warning, could not save file "+ofile
        return
    f.write(thingy.__str__())
    f.close()

def evalProject():
    """
    guess project name from directory
    """
    import os
    cwd=os.getcwd()
    s=[s for s in cwd.split(os.sep) if "_" in s][-1]
    proj=s.split("_")[0]
    if proj.endswith('Dev'):
        proj = proj[:-3]
    ver=s.split("_")[-1]
    return proj,ver

def moduleList():
    """
    Determine a list of modules I expected to see
    """
    proj,ver=evalProject()
    import commands
    stat,out=commands.getstatusoutput("svnProjectDeps "+proj+" "+ver)
    if stat:
        print out
        raise ValueError("Couldn't call svnProjectDeps :S ")
    out=out.replace("\t"," ")
    while "  " in out:
        out=out.replace("  "," ")
    proj_pack_list=[l.strip().split()[:-1] for l in out.split("\n")]
    proj_pack={}
    for proj_pack_i in proj_pack_list:
        if proj_pack_i[0] not in proj_pack:
            proj_pack[proj_pack_i[0]]=[proj_pack_i[2].split('/')[-1]]
        else:
            proj_pack[proj_pack_i[0]].append(proj_pack_i[2].split('/')[-1])
    return proj_pack

def dict_from_file(fname):
    """
    read a dictionary from a file into a variable to return
    """
    import os
    if not os.path.exists(os.path.expandvars(fname)):
        raise IOError(fname + " does not exist, so I cannot read from it")
    f=open(os.path.expandvars(fname))
    fr=f.read()
    f.close()
    exec("retdict = "+fr.replace("\n"," "))
    if type(retdict) is not dict:
        raise TypeError(fname+" did not contain a dictionary!")
    return retdict

#########################################
# The test functions, run one of these!
#   python -c "from GaudiConf.QMTest.ConfigurablesTest import test...; test...(outputfile);"
#########################################


def testThisProjectOnly(output=None,project=None):
    if project is None:
        project,dummy=evalProject()
    if project is None:
        raise ValueError("could not determine project automatically, please supply a string")
    proj_pack=moduleList()
    findproj=[p for p in proj_pack if p==project or p.lower()==project.lower()][0]
    clist=generateList(proj_pack[findproj])
    if output:
        saveTo(output,clist)
    else:
        print PrintParseLine
        print clist
    return

def testThisPackOnly(package, output=None):
    clist=generateList([package.split('/')[-1]])
    if output:
        saveTo(output,clist)
    else:
        print PrintParseLine
        print clist
    return

def testAllConfigurables(output=None):
    clist=generateList()
    if output:
        saveTo(output,clist)
    else:
        print PrintParseLine
        print clist
    return


##############################################
# Evaluate the results, comparing a ref file dictionary with a test output
##############################################

def compareConfigurables(reference, testoutput, causes, result, testname, packages=[], failIfMissing=[], threshold=50):
    """
    Compare two sets of configurables
    reference: compare against these configurables, a filename
    testoutput: the result of the current list, a dictionary or filename
    packages: a list of packages to check, empty means everything
    failIfMissing: a smaller list of packages to trigger a failure if they are missing.
                   Can be used to check all configurables for consistency but only demand that a certain list must exist.
                   If empty, is initialized to packages, which if empty is initialized to everything.
    threshold: how many missing configurables to tolerate before an error?
    causes: the usual QMTest causes
    result: the usual QMTest result
    testname: the usual QMTestname
    """
    rfile=reference
    reference=dict_from_file(reference)
    if type(testoutput) is str:
        import os
        if os.path.exists(testoutput):
            testoutput=dict_from_file(testoutput)
        elif PrintParseLine in testoutput:
            testoutput=testoutput[testoutput.find(PrintParseLine)+len(PrintParseLine):].strip().replace("\n"," ")
            exec("testoutput="+testoutput)
        else:
            raise ValueError("Failed to interpret testoutput as a file or stdout")
    if type(reference) is not dict:
        raise TypeError("expected reference dictionary, got instead "+str(type(reference)))
    if type(testoutput) is not dict:
        raise TypeError("expected testoutput dictionary, got instead "+str(type(testoutput)))
    if reference==testoutput:
        return
    saveTo(rfile+".new",testoutput)
    missingpackages=[]
    addedpackages=[]
    failedpackages=[]
    brokenpackages=[]
    mixedpackages=[]
    extraconfigurables=[]
    missingconfigurables=[]
    failedconfigurables=[]
    brokenconfigurables=[]
    if not len(packages):
        packages=reference.keys()+testoutput.keys()
    else:
        packages=[p.split("/")[-1] for p in packages]
    if not len(failIfMissing):
        failIfMissing=packages
    if len(packages):
        missingpackages=[m for m in reference if m not in testoutput and m in packages]
        addedpackages=[m for m in testoutput if m not in reference and m in packages]
        #all None
        brokenpackages=[m for m in testoutput if  m in packages and [None]*len(testoutput[m])==[testoutput[m][v] for v in testoutput[m]] ]
        #all False
        failedpackages=[m for m in testoutput if  m in packages and [False]*len(testoutput[m])==[testoutput[m][v] for v in testoutput[m]] ]
        #Nothing True
        mixedpackages=[m for m in testoutput if  m in packages and True not in [testoutput[m][v] for v in testoutput[m]] ]
        #misc not all False/None
        for p in packages:
            if p not in missingpackages and p not in brokenpackages and p not in failedpackages and p not in mixedpackages:
                if p in testoutput:
                    cs=testoutput[p]
                    for c,v in cs.iteritems():
                        if v is False:
                            failedconfigurables.append((c,p))
                        elif v is None:
                            brokenconfigurables.append((c,p))
                        elif p in reference and (c not in reference[p]):
                            extraconfigurables.append((c,p))
                if p in reference:
                    cs=reference[p]
                    for c,v in cs.iteritems():
                        if c not in testoutput[p]:
                            missingconfigurables.append((c,p))
    
    #if there is a missing package, test if I expected it to be gone!
    movedpackages=[]
    if len(missingpackages):
        expected=[]
        proj_pack=moduleList()
        for k,v in proj_pack.iteritems():
            expected+=v
        movedpackages=[m for m in missingpackages if m not in expected]
        missingpackages=[m for m in missingpackages if m in expected]
    #reasons to fail
    missingtofail=[m for m in missingpackages if m in failIfMissing]
    if len(missingtofail):
        causes.append("Entire Package Missing")
        result[testname+".missingpack"]=result.Quote("Completely cannot see the following packages, even though they were there in svnProjectDeps, indicates some major problem with svnProjectDeps or that the package failed to build: "+missingtofail.__str__())
    if len(brokenpackages):
        causes.append("Entire package broken")
        result[testname+".brokenpack"]=result.Quote("Package existed in confDB, but had no entries in Configurables module? Perhaps the python code was broken in this package : "+brokenpackages.__str__())
    if len(failedpackages):
        causes.append("Entire package failed")
        result[testname+".failedpack"]=result.Quote("All configurables in a package failed to instantiate. Check stderr, definitely a problem with the configurables themselves. : "+failedpackages.__str__())
    if len(mixedpackages):
        causes.append("Entire package broken or failed")
        result[testname+".mixedpack"]=result.Quote("All configurables either did not make it to Configurables module or failed to instantiate, probably the module is broken : "+mixedpackages.__str__())
    if len(failedconfigurables):
        causes.append("Configurables failed to instantiate")
        result[testname+".failed"]=result.Quote("Failed to instantiate : "+mixedpackages.__str__())
    if len(brokenconfigurables):
        causes.append("Configurables which didn't make it to Configurables module")
        result[testname+".broken"]=result.Quote("Failed to make it to Configurables module : "+mixedpackages.__str__())
    #only fail if there are a lot of modifications
    if len(movedpackages+addedpackages)>20:
        causes.append("More than 20 svn moves have taken place, please update the reference! ")
    if len(extraconfigurables)>50:
        causes.append("More than 50 extra configurables, this is suspicious!")
    if len(missingconfigurables)>threshold:
        causes.append("More than "+str(threshold)+" missing configurable(s)")
    missingtopass=[m for m in missingpackages if m not in failIfMissing]
    if len(missingtopass)>20:
        causes.append("More than 20 packages are missing and ignored, check and update the reference! ")
    #not of themselves a problem
    if len(movedpackages):
        result[testname+".movedpack"]=result.Quote("Moved packages in svn : "+movedpackages.__str__())
    #not of themselves a problem
    if len(missingtopass):
        result[testname+".ignoremissingpack"]=result.Quote("Missing packages which were ignored : "+missingtopass.__str__())
    #not of themselves a problem
    if len(addedpackages):
        result[testname+".addedpack"]=result.Quote("Added packages in svn : "+addedpackages.__str__())
    #not of themselves a problem
    if len(missingconfigurables):
        result[testname+".missing"]=result.Quote("Missing configurables : "+missingconfigurables.__str__())
    if len(extraconfigurables):
        result[testname+".added"]=result.Quote("Added configurables : "+extraconfigurables.__str__())
    
