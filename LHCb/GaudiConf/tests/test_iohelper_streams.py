from GaudiConf import IOHelper


def testthisioh(ioh):
    ioh.outStream(filename='out1.dst',writer='InputCopyStream/SomethingWeird'+ioh._outputPersistency)
    ioh.outputAlgs(filename='out2.dst',writer='TagCollectionStream/SomethingElseWeird'+ioh._outputPersistency)
    from Gaudi.Configuration import ApplicationMgr
    print "- streams"
    print ApplicationMgr().OutStream
    
    print ioh.activeStreams()
    for stream in ioh.activeStreams():
        if hasattr(stream,"Output"):# or (hasattr(stream,"__slots__") and "Output" in stream.__slots__):
            print stream.Output
        elif hasattr(stream,"Connection"):
            print stream.Connection
    
    ioh.convertStreams()
    print "- after conversion"
    for stream in ioh.activeStreams():
        if hasattr(stream,"Output"):# or (hasattr(stream,"__slots__") and "Output" in stream.__slots__):
            print stream.Output
        elif hasattr(stream,"Connection"):
            print stream.Connection

#preload with some MDFs, check they make it through the conversions
ioh=IOHelper("MDF","MDF")
ioh.outputAlgs("file1.mdf",writeFSR=False)
ioh.outputAlgs("file2.mdf","LHCb::MDFWriter",writeFSR=False)

#add a bare InputCopyStream to a GaudiSequencer, check it's OK
from Gaudi.Configuration import ApplicationMgr
from Configurables import GaudiSequencer
ApplicationMgr().TopAlg=["Rubbish",GaudiSequencer("MoreRubbish")]
GaudiSequencer("MoreRubbish").Members=["InputCopyStream"]

persistencies=[None, "ROOT","MDF"]
if IOHelper().isPoolSupported():
    persistencies.append("POOL")

for persistency in persistencies:
    print '============================='
    print persistency
    print '============================='
    
    ioh=IOHelper(persistency, persistency)
    
    testthisioh(ioh)
