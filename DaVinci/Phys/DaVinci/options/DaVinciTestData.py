## $Id: DaVinciTestData.py,v 1.3 2009-06-23 13:06:35 pkoppenb Exp $
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

elif ( dataType == "MC09" ):
    importOptions("$DAVINCIROOT/options/MC09-Bs2JpsiPhiDst.py")
 
else :
    print "#**********************************************************************"    
    print "# Unknown data type "+dataType
    print "#**********************************************************************"
    
