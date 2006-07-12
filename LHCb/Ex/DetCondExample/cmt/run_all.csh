#!/bin/tcsh

#cmt config
source setup.csh

./prepare_DBs.csh

foreach n ( testXml.opts \
	    fillCondDB.opts testCondDB.opts testCondDB-COLD.opts \
	    testCache.opts testMultiDB.opts testLayers.opts \
	    testUpdateMgr.opts testUMSError.opts \
	    )
	    
    ../$CMTCONFIG/DetCondExample.exe ../options/$n |& tee ../logs/${n:r}.ref.log

end
 
