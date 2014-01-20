from GaudiConf import (IOHelper, IOExtension)

exts=['.RAW','.mdf','.digi','.sim','.sdst','.xdst','.mdst','.dst']

def testthisiox(iox):
    print "- test file detection and dressing"
    inputs=[]
    basename='eggs'
    for ext in exts:
        basename+='.spam_'
        file = basename+ext
        inputs.append(file)
        print iox.detectFileType(file)
        print iox.detectMinType(inputs)
        for IO in ['I','O']:
            print iox.dressFile(file,IO)

    print '- check you take the last extension'
    print iox.detectFileType(''.join(exts))
    print iox.detectFileType(''.join(exts+[exts[0]]))
    
    print "- test getting ioh"
    
    for ext in exts:
        ioh=iox.getIOHelper(basename+ext)
        print ioh
        for oext in exts:
            ioh=iox.getCompleteIOHelper([basename+ext],basename+oext)
            print ioh
    
    print "- test inputting files"

    print inputs
    iox.inputFiles(inputs)
    
    from Gaudi.Configuration import EventSelector
    print EventSelector().Input
    
    print "- test outputting files"
    
    for ext in exts:
        iox.outputAlgs(basename+ext,writer='OutputStream/Alg'+ext.replace('.',''))
        iox.outStream(basename+ext,writer='OutputStream/Str'+ext.replace('.',''))
    
    streams=IOHelper().activeStreams()
    print streams
    for stream in streams:
        print stream.Output
    
    print "- test file extensions which should not be known"
    print [iox.detectFileType('ni'+file) for file in ext]

    print "- input card printing test"
    
    for setPersistency in [True, False]:
        print iox.extensionString(setPersistency=setPersistency)
    
    import commands
    
    
    for setPersistency in [True, False]:
        print commands.getstatusoutput("python -c '"+iox.extensionString(setPersistency=setPersistency)+"'")


persistencies=[None, "ROOT"]

for persistency in persistencies:
    print '============================='
    print persistency
    print '============================='

    print "- input conversion tests "
    #preload with MDF
    
    iox=IOExtension(persistency)
    
    from Gaudi.Configuration import EventSelector
    EventSelector().Input=[]
    
    testthisiox(iox)
