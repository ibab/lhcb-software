# Combine to xml file given in input (fnamein and fnamein2) and write the combined file in fnameout
# Plot the value of the constants in global frame and the differences versus z position of the modules.
# the plots are saved in SensConstDiff_wrt....ps
#
# to run
# python CombineDetectorXml.py
# (the parameters should changed by hand in this file, look for 'User Options')
#
import os
from os.path import join, getsize
from ROOT import TFile, TProfile, TF1, TMath, TH1F, TCanvas,TPostScript,TGraphErrors
from ROOT import gDirectory
from math import fabs
from os   import path
import sys
from ROOT import *
from ROOT import gROOT, gRandom, gStyle
from ROOT import TCanvas, TF1, TH1F, TH2F, TGLabel

#User Options
inputname="metr_ted"
fnamein='/afs/cern.ch/user/s/siborghi/cmtuser/Alignment_v4r6/Alignment/Escher/scripts/AlignXml/DetectorsMetrology.xml'
fnamein2='/afs/cern.ch/user/s/siborghi/cmtuser/Alignment_v4r6/Alignment/Escher/scripts/AlignXml/DetectorsTed.xml'
fnameout='Detectors_'+inputname+'.xml'
# dof are Tx, Ty, Tz, Rx, Ry, Rz
# In the output file we write the constants of fnamein when dofinfile1 is 1 and the constants of fnamein2 when dofinfile1 is 0 
dofinfile1 =[1,1,1,0,0,0]


vd0transx =[]
vd0transy =[]
vd0transz =[]
vd0rotx =[]
vd0roty =[]
vd0rotz =[]
vdtransx =[]
vdtransy =[]
vdtransz =[]
vdrotx =[]
vdroty =[]
vdrotz =[]

for i in range(0,42):
    #print i
    vd0transx.append(" ")
    vd0transy.append(" ")
    vd0transz.append(" ")
    vd0rotx.append(" ")
    vd0roty.append(" ")
    vd0rotz.append(" ")
    vdtransx.append(" ")
    vdtransy.append(" ")
    vdtransz.append(" ")
    vdrotx.append(" ")
    vdroty.append(" ")
    vdrotz.append(" ")

fin=open(fnamein,'r')
fout=open(fnameout,'w')
for line in fin.readlines():
    if ( (line.find('Detector')>0) and (line.find('PU')<0) ):
        tx =[0,0]
        ty =[0,0]
        tz =[0,0]
        rx =[0,0]
        ry =[0,0]
        rz =[0,0]
        #Determine module number and sensor 00 or 01
        split1     = line.split('Detector')
        split2     = split1[1].split('-')
        nmod  = int(split2[0])
        split3     = split2[1].split('"')
        nsens = int(split3[0])
        #print nmod
        #print nsens
        #extract translation and rotation
        split4     = split1[1].split('>')
        split5     = split4[2].split('<')
        splitTrans = split5[0].split()
        #print splitTrans
        tx[nsens] = float(splitTrans[0])
        ty[nsens] = float(splitTrans[1])
        tz[nsens] = float(splitTrans[2])
        split6     = split4[4].split('<')
        splitRot   = split6[0].split()
        rx[nsens] = float(splitRot[0])
        ry[nsens] = float(splitRot[1])
        rz[nsens] = float(splitRot[2])
        if (nsens ==0):
            vd0transx[nmod] = (splitTrans[0])
            vd0transy[nmod] = (splitTrans[1])
            vd0transz[nmod] = (splitTrans[2])
            vd0rotx[nmod] = (splitRot[0])
            vd0roty[nmod] = (splitRot[1])
            vd0rotz[nmod] = (splitRot[2])
        else:
            vdtransx[nmod] = (splitTrans[0])
            vdtransy[nmod] = (splitTrans[1])
            vdtransz[nmod] = (splitTrans[2])
            vdrotx[nmod] = (splitRot[0])
            vdroty[nmod] = (splitRot[1])
            vdrotz[nmod] = (splitRot[2])

fin=open(fnamein2,'r')
for line in fin.readlines():
    if ( (line.find('Detector')>0) and (line.find('PU')<0) ):
        tx =[0,0]
        ty =[0,0]
        tz =[0,0]
        rx =[0,0]
        ry =[0,0]
        rz =[0,0]
        #Determine module number and sensor 00 or 01
        split1     = line.split('Detector')
        split2     = split1[1].split('-')
        nmod  = int(split2[0])
        split3     = split2[1].split('"')
        nsens = int(split3[0])
        #print nmod
        #print nsens
        #extract translation and rotation
        split4     = split1[1].split('>')
        split5     = split4[2].split('<')
        splitTrans = split5[0].split()
        #print splitTrans
        tx[nsens] = float(splitTrans[0])
        ty[nsens] = float(splitTrans[1])
        tz[nsens] = float(splitTrans[2])
        split6     = split4[4].split('<')
        splitRot   = split6[0].split()
        rx[nsens] = float(splitRot[0])
        ry[nsens] = float(splitRot[1])
        rz[nsens] = float(splitRot[2])
        #print splitRot
        if (nsens ==0):
            if (vd0rotx[nmod]=="0") and (vd0rotz[nmod]=="0") and (vd0rotz[nmod]=="0"):
                lineout=split1[0]+"Detector"+split2[0]+'-0'+str(nsens)+'"><paramVector name="dPosXYZ" type="double">'
                lineout+= vd0transx[nmod] +" "+ vd0transy[nmod]+" "  +vd0transz[nmod]+'</paramVector><paramVector name="dRotXYZ" type="double">'
                lineout+= vd0rotx[nmod]+" "+vd0roty[nmod] +" "  +vd0rotz[nmod] +"</paramVector></condition>\n"
                fout.write(lineout)
            else:
                lineout=split1[0]+"Detector"+split2[0]+'-0'+str(nsens)+'"><paramVector name="dPosXYZ" type="double">'
                if (dofinfile1[0]==1):        
                    lineout+=vd0transx[nmod]
                else:
                    lineout+= splitTrans[0] 
                lineout+= " "
                if (dofinfile1[1]==1):        
                    lineout+=vd0transy[nmod]
                else:
                    lineout+= splitTrans[1]
                lineout+= " "  
                if (dofinfile1[2]==1):        
                    lineout+=vd0transz[nmod]
                else:
                    lineout+= splitTrans[2]
                lineout+= '</paramVector><paramVector name="dRotXYZ" type="double">'
                if (dofinfile1[3]==1):        
                    lineout+= vd0rotx[nmod]
                else:
                    lineout+= splitRot[0]  
                lineout+= " "
                if (dofinfile1[4]==1):        
                    lineout+= vd0roty[nmod] 
                else:
                    lineout+= splitRot[1]  
                lineout+= " "  
                if (dofinfile1[5]==1):        
                    lineout+= vd0rotz[nmod] 
                else:
                    lineout+= splitRot[2]
                lineout+= "</paramVector></condition>\n"
                fout.write(lineout)
        else:
            if (vdrotx[nmod]=="0") and (vdrotz[nmod]=="0") and (vdrotz[nmod]=="0"):
                lineout=split1[0]+"Detector"+split2[0]+'-0'+str(nsens)+'"><paramVector name="dPosXYZ" type="double">'
                lineout+= vdtransx[nmod] +" "+ vdtransy[nmod]+" "  +vdtransz[nmod]+'</paramVector><paramVector name="dRotXYZ" type="double">'
                lineout+= vdrotx[nmod]+" "+vdroty[nmod] +" "  +vdrotz[nmod] +"</paramVector></condition>\n"
                fout.write(lineout)
            else:
                lineout=split1[0]+"Detector"+split2[0]+'-0'+str(nsens)+'"><paramVector name="dPosXYZ" type="double">'
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
                if (dofinfile1[3]==1):        
                    lineout+= vdrotx[nmod]
                else:
                    lineout+= splitRot[0]  
                lineout+= " "
                if (dofinfile1[4]==1):        
                    lineout+= vdroty[nmod] 
                else:
                    lineout+= splitRot[1]  
                lineout+= " "  
                if (dofinfile1[5]==1):        
                    lineout+= vdrotz[nmod] 
                else:
                    lineout+= splitRot[2]
                lineout+= "</paramVector></condition>\n"
                fout.write(lineout)
    else:
        fout.write(line)
