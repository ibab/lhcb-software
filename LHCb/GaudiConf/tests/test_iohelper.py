from GaudiConf import IOHelper


def testthisioh(ioh):
    print 'isRootSupported? ', ioh.isRootSupported()

    print '- filedressing tests'
    
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

    print "- input assignment tests"
    
    ioh.inputFiles(['test1.blah','test2.blah'])
    from Gaudi.Configuration import EventSelector
    print EventSelector().Input
    
    print ioh.selectorString()
    for setPersistency in [True, False]:
        print ioh.helperString(setPersistency=setPersistency)

    import commands
    
    print commands.getstatusoutput('python -c "'+ioh.selectorString().replace('"','\\"')+'"')
    
    for setPersistency in [True, False]:
        print commands.getstatusoutput("python -c '"+ioh.helperString(setPersistency=setPersistency)+"'")
    
    print "- change service tests"
    
    ioh.changeServices()
    print ioh.activeServices()
    

for persistency in [None,'POOL','ROOT','MDF']:
    print '============================='
    print persistency
    print '============================='

    print "- input conversion tests "
    #preload with MDF
    iohm=IOHelper("MDF","MDF")
    iohm.inputFiles(['IAMdf.mdf','IALSOAMdf.blah'])
    
    ioh=IOHelper(persistency, persistency)
    ioh.convertSelector()
    
    from Gaudi.Configuration import EventSelector
    print EventSelector().Input
    print ioh.selectorString()
    print ioh.helperString()
    
    EventSelector().Input=[]
    
    testthisioh(ioh)
