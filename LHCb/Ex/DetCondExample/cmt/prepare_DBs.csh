#!/bin/tcsh -f

#source setup.csh

foreach d ( "$CONDDBCONNECTIONSTRING" "$CONDDBCONNECTIONSTRING1" "$CONDDBCONNECTIONSTRING2" )
    coolDropDB "$d"
end

# create the database for fillCondDB and testCondDB
python $DETCONDEXAMPLEROOT/python/copy_files_to_db.py -c "$CONDDBCONNECTIONSTRING" \
                                                      -s $DETCONDEXAMPLE_XMLDDDB/Conditions -d /Conditions
python $DETCONDEXAMPLEROOT/python/copy_files_to_db.py -c "$CONDDBCONNECTIONSTRING" \
                                                      -s $DETCONDEXAMPLE_XMLDDDB/DTD -d /DTD -k

# create the other 2 databases
$DETCONDEXAMPLEROOT/python/create_DBs.py
