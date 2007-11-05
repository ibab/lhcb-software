# Minimal file for running DaVinci from python prompt
# Syntax is:
#   gaudirun.py ../options/DaVinci.py
#
from Gaudi.Configuration import *

importOptions('../options/DaVinci.opts')

ApplicationMgr( AppName    = 'DaVinci',
                AppVersion = 'v19r7' )
