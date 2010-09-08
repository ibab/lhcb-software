# Combine to xml file given in input (fnamein and fnamein2) and write the combined file in fnameout
# Plot the value of the constants in global frame and the differences versus z position of the modules.
# the plots are saved in SensConstDiff_wrt....ps
#
# to run
# python CombineModuleXml.py
# (the parameters should changed by hand in this file, look for 'User Options')
#

import os
from os.path import join, getsize
from math import fabs
from os   import path
import sys

#User Options
inputname="metr_ted"
fnamein='/afs/cern.ch/user/s/siborghi/cmtuser/Alignment_v4r6/Alignment/Escher/scripts/AlignXml/ModulesMetrology.xml'
fnamein2='/afs/cern.ch/user/s/siborghi/cmtuser/Alignment_v4r6/Alignment/Escher/scripts/AlignXml/ModulesTed.xml'
fnameout='Modules_'+inputname+'.xml'
# dof are Tx, Ty, Tz, Rx, Ry, Rz
# In the output file we write the constants of fnamein when dofinfile1 is 1 and the constants of fnamein2 when dofinfile1 is 0 
dofinfile1 =[1,1,1,0,0,0]



vdtransx =[]
vdtransy =[]
vdtransz =[]
vdrotx =[]
vdroty =[]
vdrotz =[]

for i in range(0,42):
    #print i
    vdtransx.append(" ")
    vdtransy.append(" ")
    vdtransz.append(" ")
    vdrotx.append(" ")
    vdroty.append(" ")
    vdrotz.append(" ")


# read the first file fnamein 
fin=open(fnamein,'r')
for line in fin.readlines():
    if ( (line.find('Module')>0) and (line.find('PU')<0) ):
        tx =[0,0]
        ty =[0,0]
        tz =[0,0]
        rx =[0,0]
        ry =[0,0]
        rz =[0,0]
        #Determine module number and sensor 00 or 01
        split1     = line.split('Module')
        split2     = split1[1].split('"')
        nmod  = int(split2[0])
        #print nmod
        #extract translation and rotation
        split4     = split1[1].split('>')
        #print split4
        split5     = split4[2].split('<')
        splitTrans = split5[0].split()
        #print splitTrans
        nsens = 0
        tx[nsens] = float(splitTrans[0])
        ty[nsens] = float(splitTrans[1])
        tz[nsens] = float(splitTrans[2])
        split6     = split4[4].split('<')
        splitRot   = split6[0].split()
        rx[nsens] = float(splitRot[0])
        ry[nsens] = float(splitRot[1])
        rz[nsens] = float(splitRot[2])
        vdtransx[nmod] = (splitTrans[0])
        vdtransy[nmod] = (splitTrans[1])
        vdtransz[nmod] = (splitTrans[2])
        vdrotx[nmod] = (splitRot[0])
        vdroty[nmod] = (splitRot[1])
        vdrotz[nmod] = (splitRot[2])

# read the second file fnamein2  and write the output file
fin=open(fnamein2,'r')
fout=open(fnameout,'w')
for line in fin.readlines():
    if ( (line.find('Module')>0) and (line.find('PU')<0) ):
        tx =[0,0]
        ty =[0,0]
        tz =[0,0]
        rx =[0,0]
        ry =[0,0]
        rz =[0,0]
        #Determine module number and sensor 00 or 01
        split1     = line.split('Module')
        split2     = split1[1].split('"')
        nmod  = int(split2[0])
        #print nmod
        #extract translation and rotation
        split4     = split1[1].split('>')
        split5     = split4[2].split('<')
        splitTrans = split5[0].split()
        #print splitTrans
        nsens = 0
        tx[nsens] = float(splitTrans[0])
        ty[nsens] = float(splitTrans[1])
        tz[nsens] = float(splitTrans[2])
        #print splitRot
        split6     = split4[4].split('<')
        splitRot   = split6[0].split()
        rx[nsens] = float(splitRot[0])
        ry[nsens] = float(splitRot[1])
        rz[nsens] = float(splitRot[2])
        lineout=split1[0]+"Module"+split4[0]+'><paramVector name="dPosXYZ" type="double">'
        #Translation
        if (dofinfile1[0]==1):
            lineout+=vdtransx[nmod]
        else:
            lineout+= splitTrans[0]
        lineout+= " "
        if (dofinfile1[1]==1):        
            lineout+=vdtransy[nmod]
        else:
            lineout+= splitTrans[1]
        lineout+= " "
        if (dofinfile1[2]==1):        
            lineout+=vdtransz[nmod]
        else:
            lineout+= splitTrans[2]
            
        lineout+= '</paramVector><paramVector name="dRotXYZ" type="double">'
        #Rotation
        if (dofinfile1[3]==1):        
            lineout+=vdrotx[nmod]
        else:
            lineout+= splitRot[0]
        lineout+= " "
        if (dofinfile1[4]==1):        
            lineout+=vdroty[nmod]
        else:
            lineout+= splitRot[1]
        lineout+= " "
        if (dofinfile1[5]==1):        
            lineout+=vdrotz[nmod]
        else:
            lineout+= splitRot[2]
        lineout+= "</paramVector></condition>\n"
        
        fout.write(lineout)
    else:
        fout.write(line)
