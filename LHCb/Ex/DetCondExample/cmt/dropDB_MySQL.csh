#!/usr/local/bin/tcsh
# $Id: dropDB_MySQL.csh,v 1.2 2004-12-08 17:22:37 marcocle Exp $
# Remove the MySQL ConditionsDB
source ${CONDDBROOT}/cmt/condDBMySQL_dropCondDB.csh $CONDDBMYSQL_DBUSER $CONDDBMYSQL_DBPSWD $CONDDBMYSQL_DBHOST $CONDDBMYSQL_DBNAME

