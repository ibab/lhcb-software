#!/bin/tcsh -f

#source setup.csh

foreach d ( "$CONDDBCONNECTIONSTRING" "$CONDDBCONNECTIONSTRING1" "$CONDDBCONNECTIONSTRING2" )
    coolDropDB "$d"
end

$DETCONDEXAMPLEROOT/python/create_DBs.py
