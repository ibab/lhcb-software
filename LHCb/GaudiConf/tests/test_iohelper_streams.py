from GaudiConf import IOHelper


def testthisioh(ioh):
    ioh.outStream(filename='out1.dst',writer='InputCopyStream/SomethingWeird'+ioh._outputPersistency)
    ioh.outputAlgs(filename='out2.dst',writer='TagCollectionStream/SomethingElseWeird'+ioh._outputPersistency)
    from Gaudi.Configuration import ApplicationMgr
    print "- streams"
    print ApplicationMgr().OutStream
    
    print ioh.activeStreams()
    for stream in ioh.activeStreams():
        print stream.Output
    
    ioh.convertStreams()
    print "- after conversion"
    for stream in ioh.activeStreams():
        print stream.Output

#preload with some MDFs, check they make it through the conversions
ioh=IOHelper("MDF","MDF")
ioh.outputAlgs("file1.mdf",writeFSR=False)

for persistency in [None,'POOL','ROOT','MDF']:
    print '============================='
    print persistency
    print '============================='
    
    ioh=IOHelper(persistency, persistency)
    
    testthisioh(ioh)
