## $Id: DaVinciTestData.py,v 1.1 2008-12-18 09:57:52 pkoppenb Exp $
 #
 # This will always contain some data
 #
 # syntax gaudirun.py options/DaVinci-2008.py options/DaVinciTestData.py
 #
 # @author P. Koppenburg
 #
##
from Gaudi.Configuration import *
from Configurables import DaVinci

dataType = DaVinci().getProp("DataType")

if ( dataType == "DC06" ):
    importOptions("$DAVINCIROOT/options/DC06_stripped_bbincl_lumi2.py")

elif ( dataType == "2008" ):
    importOptions("$DAVINCIROOT/options/2008-InclJpsiDst.py")

else :
    print "#**********************************************************************"    
    print "# Unknown data type "+dataType
    print "#**********************************************************************"
    
