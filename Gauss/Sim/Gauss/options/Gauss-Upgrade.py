##############################################################################
# File for running Gauss with Upgrade configuration
#
# Syntax is:
#   gaudirun.py Gauss-Upgrade.py <someInputJobConfiguration>.py
##############################################################################

from Gaudi.Configuration import *

#           tell Gauss these are upgrade simulations
importOptions("$APPCONFIGOPTS/Gauss/Upgrade.py")

#           select beam conditions
importOptions("$APPCONFIGOPTS/Gauss/MC09-b7TeV-md100-nu6,8-25ns.py")
# importOptions("$APPCONFIGOPTS/Gauss/MC09-b5TeV-md100.py")
# 2x10^32 importOptions("$APPCONFIGOPTS/Gauss/MC09-b7TeV-md100-nu0,7-25ns.py")
# 5x10^32 importOptions("$APPCONFIGOPTS/Gauss/MC09-b7TeV-md100-nu1,7-25ns.py")
# 1x10^33 importOptions("$APPCONFIGOPTS/Gauss/MC09-b7TeV-md100-nu3,4-25ns.py")
# 2x10^33 importOptions("$APPCONFIGOPTS/Gauss/MC09-b7TeV-md100-nu6,8-25ns.py")

##############################################################################
# Database tags must be set and are defined in a separate file
##############################################################################
importOptions('$APPCONFIGOPTS/Conditions/Upgrade-20090917-md100-gauss.py')


