#############################################################
##
##  Summary utility for HPD image analysis  
##
##  Author: T. Blake
##
##  
#############################################################


from pyHPDXMLParsingUtils import *
from math import fabs

import os, sys

def appendCondDBOveride( file, det, hpd, x0, y0 ):
    """
    Append x0, y0 silicon sensor offset for HPD 'hpd' in RICH 'det' to CondDB overide file
    """
    conddbpath = 'Conditions/Alignment/Rich' + str(det)
    hpdpath = '/HPD' + str(hpd) + '_Align'
    poscond = 'double_v dPosXYZ = ' + str(x0) + ' ' + str(y0) + ' 0.0' 
    rotcond = 'double_v dRotXYZ = 0.0 0.0 0.0'
    pivcond = 'double_v pivotXYZ = 0.0 0.0 0.0'
    file.write( '\n UpdateManagerSvc().ConditionsOverride += [ \n'
                '\t "' + conddbpath + hpdpath + ' := ' +
                poscond + '; ' +
                rotcond + '; ' +
                pivcond + ';" \n ] \n' )

    
def produceCondDBOverideFromXML( xmlfilelist, condbfilename ):
    """
    Produce a condDB overide file from a XML file list for all valid HPDs
    """
    counterdict = countersFromXMLFileList( xmlfilelist )
    file = open( condbfilename, 'w' )
    file.write('from Configurables import UpdateManagerSvc \n\n')
    for hpd in range(0,484):
        (isvalid, x0,y0) =  hpdLocalOffset( hpd, counterdict )
        if isvalid:
            if ( hpd < 196 ):
                appendCondDBOveride(file, 1, hpd, x0, y0)
            else:
                appendCondDBOveride(file, 2, hpd, x0, y0)
    file.close()


if len(sys.argv) < 3:
    sys.exit(2)
else:
    xmlarr = []
    for i in range(2,len(sys.argv)):
        if not os.path.exists( sys.argv[i]):
            sys.exit(2)
        else:
            xmlarr += [ sys.argv[i] ]
    produceCondDBOverideFromXML( xmlarr, sys.argv[1] )
    
