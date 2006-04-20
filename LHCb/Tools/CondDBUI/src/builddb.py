from PyCool import cool, pool
import sys, random

connectString = "sqlite://;schema=../db/test_db;user=none;dbname=TEST"

dbSvc = cool.DatabaseSvcFactory.databaseService()
dbSvc.dropDatabase(connectString)

db = dbSvc.createDatabase(connectString)

stringPayloadSpecs = pool.AttributeListSpecification()
stringPayloadSpecs.push_back("data","string")

# Tree structure construction
multiFolder  = db.createFolder('/MultiFolder', stringPayloadSpecs, 
                               'Multi version folder', cool.FolderVersioning.MULTI_VERSION, 
                               createParents = True )
singleFolder = db.createFolder('/SingleFolder', stringPayloadSpecs, 
                               'Single version folder', cool.FolderVersioning.SINGLE_VERSION, 
                               createParents = True )
                                 
# Fill the folders
stringPayload = pool.AttributeList(stringPayloadSpecs)

# Multi version folder
print "Filling the multi version folder..."
stringPayload["data"] = "Default Value"
for channelID in range(20):
    multiFolder.storeObject(cool.ValidityKeyMin, cool.ValidityKeyMax, stringPayload, channelID)
    
for tag in ['v1', 'v2', 'v3', 'v4']:
    for channelID in range(20):
        multiFolder.setupStorageBuffer()
        lastTill = 0
        for objectID in range(random.randint(5,30)):
            stringPayload['data'] = 'Value %s, version %s'%(str(objectID),tag)
            since = cool.ValidityKey(random.randint(0,100)   + lastTill)
            till  = cool.ValidityKey(random.randint(10,1000) + since.value())
            
            multiFolder.storeObject(since, till, stringPayload, channelID)
            lastTill = till.value()
        multiFolder.flushStorageBuffer()
    multiFolder.tagCurrentHead(tag, "test version %s"%tag)

# Single version folder
print "Filling the single version folder..."
for channelID in range(50):
    singleFolder.setupStorageBuffer()

    stringPayload['data'] = 'Initial Value'
    singleFolder.storeObject(cool.ValidityKeyMin, cool.ValidityKeyMax, stringPayload, channelID)
    lastSince = cool.ValidityKeyMin.value()
    for objectID in range(random.randint(5,30)):
        stringPayload['data'] = 'Value ' + str(objectID)
        since = cool.ValidityKey(random.randint(10,1000) + lastSince)
        
        singleFolder.storeObject(since, cool.ValidityKeyMax, stringPayload, channelID)
        lastSince = since.value()
    singleFolder.flushStorageBuffer()

print "Done :-)"
    

