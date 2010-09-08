# Generate N (N=numberOfMisalignFile) files with a random misalignment adding to the constants in  xml file given in input (fnamein)
# The N misalign files are written in file called Detectors_nameOfMisalignFileINDEX.xml (wher INDEX is between 0 and N)
# In the output file we write the constants of fnamein adding a random misalignment with a gaussian
# distribution with mean and sigma defined in sigmaOfDof and meanOfDof for the dof (Tx,Ty,Tz,Rx,Ry,Rz) 
# Plot of the misalignment for each degree of freedom is saved in 'MisalignPlots_nameOfMisalignFileINDEX.pdf')
#
# to run
# python generateMisalignDetectorXml.py
# (the parameters should changed by hand in this file, look for 'User Options')
#
import os
import random
from os.path import join, getsize
from ROOT import TFile, TProfile, TF1, TMath, TH1F, TCanvas,TPostScript,TGraphErrors
from ROOT import gDirectory
from math import fabs
from os   import path
import sys
from ROOT import *
from ROOT import gROOT, gRandom, gStyle
from ROOT import TCanvas, TF1, TH1F, TH2F, TGLabel

gROOT.Reset()

gStyle.SetPadColor(10)
gStyle.SetGridColor(16)
gStyle.SetFrameFillColor(10)
gStyle.SetFillColor(10)
gStyle.SetTitleFillColor(10)
gStyle.SetTitleBorderSize(0)
gStyle.SetCanvasColor(10)
gStyle.SetStatColor(10)

gStyle.SetOptStat(2210)
gStyle.SetOptFit(111)

#User Options
numberOfMisalignFile=10
nameOfMisalignFile='misalign'
fnamein='/afs/cern.ch/user/s/siborghi/cmtuser/Alignment_v4r6/Alignment/Escher/scripts/AlignXml/DetectorsMetrology.xml'
# In the output file we write the constants of fnamein adding a random misalignment with a gaussian
# distribution with mean =0 and sigma defined in the following list for the dof (Tx,Ty,Tz,Rx,Ry,Rz) 
sigmaOfDof =[0.010,0.010,0.100,0.0005,0.0005,0.001]
meanOfDof =[0,0,0,0,0,0]




index=0
outputname=[]
for index  in range(0,numberOfMisalignFile):
    name=nameOfMisalignFile+str(index)
    outputname.append(name)
    
c2 = TCanvas( 'c2', 'Example', 200, 10, 1000, 500 )
c2.Divide(3,2)


for index  in range(0,numberOfMisalignFile):
    fnameout='Detectors_'+outputname[index]+'.xml'
    # dof are Tx, Ty, Tz, Rx, Ry, Rz
    xmin =1000.*(-sigmaOfDof[0]*5 + meanOfDof[0])
    xmax = 1000.*(sigmaOfDof[0]*5 + meanOfDof[0])
    hTx = TH1F( 'hTx', 'Tx', 50, xmin, xmax )
    xmin =1000.*(-sigmaOfDof[1]*5 + meanOfDof[1])
    xmax =1000.*( sigmaOfDof[1]*5 + meanOfDof[1])
    hTy = TH1F( 'hTy', 'Ty', 50, xmin, xmax )
    xmin =1000.*(-sigmaOfDof[2]*5 + meanOfDof[2])
    xmax =1000.*( sigmaOfDof[2]*5 + meanOfDof[2])
    hTz = TH1F( 'hTz', 'Rz', 50, xmin, xmax )
    xmin =1000.*(-sigmaOfDof[3]*5 + meanOfDof[3])
    xmax =1000.*( sigmaOfDof[3]*5 + meanOfDof[3])
    hRx = TH1F( 'hRx', 'Rx', 50, xmin, xmax )
    xmin =1000.*(-sigmaOfDof[4]*5 + meanOfDof[4])
    xmax =1000.*( sigmaOfDof[4]*5 + meanOfDof[4])
    hRy = TH1F( 'hRy', 'Ry', 50, xmin, xmax )
    xmin =1000.*(-sigmaOfDof[5]*5 + meanOfDof[5])
    xmax =1000.*( sigmaOfDof[5]*5 + meanOfDof[5])
    hRz = TH1F( 'hRz', 'Rz', 50, xmin, xmax )


    fout=open(fnameout,'w')
    fin=open(fnamein,'r')
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
            #random misalignment
            misalignTx = random.gauss(meanOfDof[0],sigmaOfDof[0])
            misalignTy = random.gauss(meanOfDof[1],sigmaOfDof[1])
            misalignTz = random.gauss(meanOfDof[2],sigmaOfDof[2])
            misalignRx = random.gauss(meanOfDof[3],sigmaOfDof[3])
            misalignRy = random.gauss(meanOfDof[4],sigmaOfDof[4])
            misalignRz = random.gauss(meanOfDof[5],sigmaOfDof[5])
            hTx.Fill(1000.*misalignTx)
            hTy.Fill(1000.*misalignTy)
            hTz.Fill(1000.*misalignTz)
            hRx.Fill(1000.*misalignRx)
            hRy.Fill(1000.*misalignRy)
            hRz.Fill(1000.*misalignRz)
            #new alignment constants
            vd0transx = tx[nsens]+misalignTx
            vd0transy = ty[nsens]+misalignTy
            vd0transz = tz[nsens]+misalignTz
            vd0rotx = rx[nsens]+misalignRx
            vd0roty = ry[nsens]+misalignRy
            vd0rotz = rz[nsens]+misalignRz

            #print splitRot
            lineout=split1[0]+"Detector"+split2[0]+'-0'+str(nsens)+'"><paramVector name="dPosXYZ" type="double">'
            lineout+= str(vd0transx) +" "+ str(vd0transy)+" "  +str(vd0transz)+'</paramVector><paramVector name="dRotXYZ" type="double">'
            lineout+= str(vd0rotx)+" "+str(vd0roty) +" "  +str(vd0rotz) +"</paramVector></condition>\n"
            fout.write(lineout)

        else:
            fout.write(line)

    c2.cd(1)
    c2_1.SetGrid()
    hTx.SetTitle("Tx misalignment")
    hTx.Fit("gaus")
    hTx.GetXaxis().SetTitle("Tx misalignment [#mum]")
    hTx.GetYaxis().SetTitle("Entries")
    c2.cd(2)
    c2_2.SetGrid()
    hTy.SetTitle("Ty misalignment")
    hTy.Fit("gaus")
    hTy.GetXaxis().SetTitle("Ty misalignment [#mum]")
    hTy.GetYaxis().SetTitle("Entries")
    c2.cd(3)
    c2_3.SetGrid()
    hTz.SetTitle("Tz misalignment")
    hTz.Fit("gaus")
    hTz.GetXaxis().SetTitle("Tz misalignment [#mum]")
    hTz.GetYaxis().SetTitle("Entries")
    c2.cd(4)
    c2_4.SetGrid()
    hRx.SetTitle("Rx misalignment")
    hRx.Fit("gaus")
    hRx.GetXaxis().SetTitle("Rx misalignment [mrad]")
    hRx.GetYaxis().SetTitle("Entries")
    c2.cd(5)
    c2_5.SetGrid()
    hRy.SetTitle("Ry misalignment")
    hRy.Fit("gaus")
    hRy.GetXaxis().SetTitle("Ry misalignment [mrad]")
    hRy.GetYaxis().SetTitle("Entries")
    c2.cd(6)
    c2_6.SetGrid()
    hRz.SetTitle("Rz misalignment")
    hRz.Fit("gaus")
    hRz.GetXaxis().SetTitle("Rz misalignment [mrad]")
    hRz.GetYaxis().SetTitle("Entries")
    c2.Update()
    c2.SaveAs('MisalignPlots_'+outputname[index]+'.pdf')
