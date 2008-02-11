# Minimal file for running Brunel from python prompt
# Syntax is:
#   gaudirun.py ../options/v200601.py
#
from Gaudi.Configuration import *

#--Setup for v200601 geometry
importOptions('$BRUNELOPTS/v200601.opts')

ApplicationMgr( AppName    = 'Brunel',
                AppVersion = 'v32r3p1' )
