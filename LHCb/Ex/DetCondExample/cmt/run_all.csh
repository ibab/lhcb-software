#!/bin/tcsh -f

#cmt config
source setup.csh

./prepare_DBs.csh

set escDETCONDEXAMPLE = `echo $DETCONDEXAMPLEROOT:h:h:h | sed 's/\//\\\//g'`

foreach n ( testXml.opts \
	    fillCondDB.opts testCondDB.opts testCondDB-COLD.opts \
	    testCondDB-XMLCache.opts \
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

    # clean the log file
    mv -f ../logs/${n:r}.ref.log ../logs/${n:r}.ref.log.tmp
    sed "s/$escDETCONDEXAMPLE/~\/cmtuser/;s/0x[0-9a-f]*/0x########/g" ../logs/${n:r}.ref.log.tmp > ../logs/${n:r}.ref.log
    rm -f ../logs/${n:r}.ref.log.tmp
end

