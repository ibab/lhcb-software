from GaudiConf import IOHelper


def testthisioh(ioh):
    print ioh.isRootSupported()
    
    ifi='PFN:itest.txt'
    dressedI=ioh.dressFile(ifi,"I")
    
    print dressedI
    
    undressedI=ioh.undressFile(dressedI)
    
    if ifi!=undressedI:
        print dressedI,"!=",undressedI
    
    ofi='PFN:otest.txt'
    dressedO=ioh.dressFile(ofi,"O")
    
    print dressedO
    
    undressedO=ioh.undressFile(dressedO)
    
    if ofi!=undressedO:
        print dressedO,"!=",undressedO
    
    
    ioh.inputFiles(['test1.blah','test2.blah'])
    from Gaudi.Configuration import EventSelector
    print EventSelector().Input
    
    print ioh.selectorString()
    print ioh.helperString()
    import commands
    print commands.getstatusoutput('python -c "'+ioh.selectorString().replace('"','\\"')+'"')
    print commands.getstatusoutput("python -c '"+ioh.helperString()+"'")

    ioh.changeServices()
    print ioh.activeServices()
    

for persistency in [None,'POOL','ROOT','MDF']:
    print '============================='
    print persistency
    print '============================='
    
    ioh=IOHelper(persistency, persistency)
    ioh.convertSelector()
    
    from Gaudi.Configuration import EventSelector
    print EventSelector().Input
    
    EventSelector().Input=[]
    
    testthisioh(ioh)
