# Minimal file for running Boole from python prompt
# Syntax is:
#   gaudirun.py ../options/v200601.py
#
from Gaudi.Configuration import *

#--Setup for v200601 geometry
importOptions('$BOOLEOPTS/v200601.opts')

ApplicationMgr( AppName    = 'Boole',
                AppVersion = 'v14r9' )
