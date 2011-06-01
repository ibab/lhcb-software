from GaudiConf import IOHelper


def testthisioh(ioh):
    ioh.outStream(filename='out1.dst',writer='InputCopyStream/SomethingWeird'+ioh._outputPersistency)
    ioh.outputAlgs(filename='out2.dst',writer='TagCollectionStream/SomethingElseWeird'+ioh._outputPersistency)
    from Gaudi.Configuration import ApplicationMgr
    print ApplicationMgr().OutStream
    
    print ioh.activeStreams()
    for stream in ioh.activeStreams():
        print stream.Output
    ioh.convertStreams()
    for stream in ioh.activeStreams():
        print stream.Output

for persistency in [None,'POOL','ROOT','MDF']:
    print '============================='
    print persistency
    print '============================='
    
    ioh=IOHelper(persistency, persistency)
    
    testthisioh(ioh)
