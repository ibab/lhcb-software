#!/usr/local/bin/tcsh
# $Id: dropDB_Oracle.csh,v 1.2 2004-12-08 17:22:37 marcocle Exp $
# Remove the Oracle ConditionsDB
source ${CONDDBROOT}/cmt/condDBOracle_dropCondDB.csh $CONDDBORACLE_DBUSER $CONDDBORACLE_DBPSWD $CONDDBORACLE_DBHOST $CONDDBORACLE_DBNAME
