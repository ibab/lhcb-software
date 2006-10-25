#!/bin/tcsh

#cmt config
source setup.csh

./prepare_DBs.csh

foreach n ( testXml.opts \
	    fillCondDB.opts testCondDB.opts testCondDB-COLD.opts \
	    testCache.opts testMultiDB.opts testLayers.opts \
	    testUpdateMgr.opts testUMSError.opts \
	    TestTimeOut.opts TestConditionsOverride.opts \
	    )
	    
    ../$CMTCONFIG/DetCondExample.exe ../options/$n |& tee ../logs/${n:r}.ref.log

    if ( $? != 0 ) then
	echo "************************************************************************"
	echo "   Test $n failed\!\!\!"
	echo "************************************************************************"
	exit 1
    endif

end

