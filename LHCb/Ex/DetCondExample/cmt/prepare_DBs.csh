#!/bin/tcsh -f

#source setup.csh

foreach d ( "$CONDDBCONNECTIONSTRING" "$CONDDBCONNECTIONSTRING1" "$CONDDBCONNECTIONSTRING2" )
    coolDropDB "$d"
    ../python/create_empty_db.py "$d"
end
