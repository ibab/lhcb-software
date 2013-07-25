
##############################################################################
#
# Tagging options
#
##############################################################################

print " "
print "==========================================================================="
print "  WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING"
print " "
print "    The syntax :-"
print " "
print """      importOptions("$FLAVOURTAGGINGROOT/options/BTaggingTool.py")"""
print " "
print "    is OBSOLETE. Please instead use :-"
print " "
print "      from Configurables import FlavourTaggingConf"
print "      ftConf = FlavourTaggingConf()"
print " "
print "  WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING"
print "==========================================================================="
print " "

from Configurables import FlavourTaggingConf
# Just create an instance. Nothing to configure as of yet
ftConf = FlavourTaggingConf()

##############################################################################
