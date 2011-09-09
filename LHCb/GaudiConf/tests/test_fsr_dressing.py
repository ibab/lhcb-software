from GaudiConf import IOHelper

def testType(ioh):
    
    from Configurables import RecordStream
    
    RecordStream('FileRecord').Output="DATAFILE='FishyStuff.dst' TYP='NOTYPE' OPT='REC'"
    
    RecordStream('FileRecord2').Output="DATAFILE='FishyStuff.dst' TYP='NOTYPE' OPT='REC'"
    
    RecordStream('FileRecord3').Output="DATAFILE='FishyStuff.dst' TYP='POOL_ROOTTREE' OPT='REC'"
    
    RecordStream('FileRecord4').Output="DATAFILE='FishyStuff.dst' SVC='RootCnvSvc' OPT='REC'"
    
    RecordStream('FileRecord5').Output="DATAFILE='FishyStuff.dst' SVC='FileRecordCnvSvc' OPT='REC'"
    
    RecordStream("RubbishRecord").EvtDataSvc="CrappyNonexistingService"
    
    ioh.debugIO()
    
    ioh.convertStreams()
    
    ioh.debugIO()


for Persistency in ["ROOT", "POOL","MDF"]:
    print "====================="
    print Persistency
    testType(IOHelper(Persistency,Persistency))
    

print "====================="
